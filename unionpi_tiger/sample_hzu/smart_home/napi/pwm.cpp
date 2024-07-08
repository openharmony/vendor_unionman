/*
 * Copyright (c) 2022 Unionman Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "napi/native_api.h"
#include <asm-generic/stat.h>
#include <bits/alltypes.h>

#ifdef __cplusplus
#include "um_pwm.h"
extern "C" {
#endif

struct PwmData {
    napi_async_work asyncWork = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callback = nullptr;
    int args[2] = {0};
    int result = 0;
};

static void PwmExecute(napi_env env, void *data)
{
    // 获取上下文数据
    PwmData *pwmData = (PwmData *)data;
    int initDuty = 500000;
    int maxDuty = 2500000;
    int isEnable = PWM_IS_ENABLED;
    // 提取引脚值与pwm旋转角度
    int pwmPin = pwmData->args[0];
    int pwmAngle = pwmData->args[1];
    // 设置溢出值
    set_pwm_period(pwmPin, maxDuty);
    // 根据旋转角度计算高电平时间
    double pwmDuty = 1.0 * pwmAngle * initDuty / 45 + initDuty;
    // 设置pwm一个周期高电平时间并使引脚使能有效
    set_pwm_dutyCycle(pwmPin, static_cast<int>(pwmDuty));
    set_pwm_enable(pwmPin, isEnable);
    // 将引脚使能值作为业务逻辑处理结果
    pwmData->result = is_pwm_enabled(pwmPin);
}

static void PwmComplete(napi_env env, napi_status status, void *data)
{
    PwmData *pwmData = (PwmData *)data;
    napi_value result = nullptr;
    napi_create_int32(env, pwmData->result, &result);
    napi_resolve_deferred(env, pwmData->deferred, result);
    if (pwmData->callback != nullptr) {
        napi_delete_reference(env, pwmData->callback);
    }
    napi_delete_async_work(env, pwmData->asyncWork);
    delete pwmData;
}

static napi_value SetPwmAsync(napi_env env, napi_callback_info info)
{
    // 获取两个参数
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    napi_value thisArg = nullptr;
    napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr);
    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    napi_create_promise(env, &deferred, &promise);
    // 异步工作项获取上下文数据
    auto pwmData = new PwmData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };
    // 传入收到的数据
    napi_get_value_int32(env, args[0], &pwmData->args[0]);
    napi_get_value_int32(env, args[1], &pwmData->args[1]);
    // 创建async work
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "SetPwmAsync", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, PwmExecute, PwmComplete, (void *)pwmData, &pwmData->asyncWork);
    // 将创建的async work加到队列
    napi_queue_async_work(env, pwmData->asyncWork);
    return promise;
}

EXTERN_C_START
static napi_value PwmInit(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        {"SetPwmAsync", nullptr, SetPwmAsync, nullptr, nullptr, nullptr, napi_default, nullptr}};
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module PwmModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = PwmInit,
    .nm_modname = "pwm",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

#ifdef __cplusplus
}
#endif

extern "C" __attribute__((constructor)) void RegisterPwmModule(void)
{
    napi_module_register(&PwmModule);
}