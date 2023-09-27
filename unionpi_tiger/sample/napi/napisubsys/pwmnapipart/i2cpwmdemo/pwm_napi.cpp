/*
 * Copyright (c) 2023 Unionman Technology Co., Ltd.
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
#define NAPI_TEST 1
#include <napi/native_api.h>
#include <napi/native_node_api.h>
#include "pwm/pwm_ctl.h"

static Pwm::PwmCtl* pwm1;

struct asyncPromiseData {
    napi_async_work asyncWork;
    napi_deferred deferred;
    uint32_t angle;
};

static napi_value setAngleAsync(napi_env env, napi_callback_info cb)
{
    size_t argc = 1;
    napi_value argv;
    napi_get_cb_info(env, cb, &argc, &argv, nullptr, nullptr); // 获取参数
    // 校验参数数量
    NAPI_ASSERT(env, argc == 1, "requires 1 parameter");
    napi_valuetype parmType;
    // 校验参数类型
    napi_typeof(env, argv, &parmType);
    NAPI_ASSERT(env, parmType == napi_number, "frist parameter type requires number");
    // 创建异步数据结构体
    asyncPromiseData* pd = new asyncPromiseData;
    napi_get_value_uint32(env, argv, &pd->angle);
    // 创建异步资源名称
    napi_value resourceName;
    napi_create_string_utf8(env, "setAngleAsync", NAPI_AUTO_LENGTH, &resourceName);
    // 创建proimse
    napi_value proimse;
    napi_create_promise(env, &pd->deferred, &proimse);
    // 创建异步工作项
    napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void* data) {
            // 执行耗时操作
            asyncPromiseData* pd = reinterpret_cast<asyncPromiseData*>(data);
            // 根据角度值计算高电平时间,公式2000000 * (angle / 180) + 500000;
            uint32_t dutyCycle = 2000000 * (pd->angle / 180.0f) + 500000;
            pwm1->setDutyCycle(dutyCycle);
        },
        [](napi_env env, napi_status status, void* data) {
            asyncPromiseData* pd = reinterpret_cast<asyncPromiseData*>(data);
            napi_value result;
            napi_get_undefined(env, &result);
            // 将promise的结果设置为完成
            napi_resolve_deferred(env, pd->deferred, result);
            // 删除异步工作项
            napi_delete_async_work(env, pd->asyncWork);
            // 释放异步数据结构体
            delete pd;
        }, pd, &pd->asyncWork);
    // 将异步工作项放入队列
    napi_queue_async_work(env, pd->asyncWork);
    // 返回刚刚的proimse对象
    return proimse;
}

static napi_value Register_Pwm_Ctl_Apis(napi_env env, napi_value exports)
{
    pwm1 = new Pwm::PwmCtl(Pwm::Channel::pwm1);
    napi_property_descriptor desc[] = { DECLARE_NAPI_FUNCTION("setAngleAsync", setAngleAsync) };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    NAPI_CALL(env, napi_add_env_cleanup_hook(env, [](void* arg) {
        delete pwm1;
        pwm1=nullptr;
    }, nullptr));
    return exports;
}

NAPI_MODULE(pwm_ctl, Register_Pwm_Ctl_Apis)