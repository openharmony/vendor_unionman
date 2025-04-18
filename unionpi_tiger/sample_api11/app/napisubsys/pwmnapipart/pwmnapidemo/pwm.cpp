/* Copyright 2022 Unionman Technology Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "napi/native_api.h"
#include "napi/native_node_api.h"

static const int INIT_DUTY = 500000;
static const int MAX_DUTY = 2500000;

#ifdef __cplusplus
#include "um_pwm.h"
extern "C" {
#endif

struct PwmOnData {
    napi_async_work asyncWork = nullptr; // 异步工作项
    napi_deferred deferred = nullptr;    // 用于Promise的resolve、reject处理
    napi_ref callback = nullptr;         // 回调函数
    int args[2] = {0};                   // 2个输入参数
    int result = 0;                      // 业务逻辑处理结果（返回值）
};

// 业务逻辑处理函数，由worker线程池调度执行。
static void executeCB(napi_env env, void *data)
{
    PwmOnData *pwmData = (PwmOnData *)data;

    int pwmPex = pwmData->args[0];
    int pwmAngle = pwmData->args[1];

    set_pwm_period(pwmPex, MAX_DUTY);
    double pwmDuty = 1.0 * INIT_DUTY * pwmAngle / 45L + INIT_DUTY;
    set_pwm_dutyCycle(pwmPex, static_cast<int>(pwmDuty));
    set_pwm_enable(pwmPex, 1);

    pwmData->result = is_pwm_enabled(pwmPex);
}

// 业务逻辑处理完成回调函数，在业务逻辑处理函数执行完成或取消后触发，由EventLoop线程中执行。
static void completeCBForPromise(napi_env env, napi_status status, void *data)
{
    PwmOnData *pwmData = (PwmOnData *)data;
    napi_value result = nullptr;
    napi_create_int32(env, pwmData->result, &result);
    napi_resolve_deferred(env, pwmData->deferred, result);

    // 删除napi_ref对象
    if (pwmData->callback != nullptr) {
        napi_delete_reference(env, pwmData->callback);
    }

    // 删除异步工作项
    napi_delete_async_work(env, pwmData->asyncWork);
    delete pwmData;
}

static napi_value setPwmStatus(napi_env env, napi_callback_info info)
{
    // 获取2个参数，值的类型是js类型(napi_value)
    size_t argc = 2L;
    napi_value args[2];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto pwmData = new PwmOnData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };

    // 将被收到的参数传入
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &pwmData->args[0]));
    NAPI_CALL(env, napi_get_value_int32(env, args[1], &pwmData->args[1]));

    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "setPwmStatus", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, executeCB, completeCBForPromise, (void *)pwmData,
                           &pwmData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, pwmData->asyncWork);

    // 返回promise
    return promise;
}

/*
 * 注册接口
 */
static napi_value registerpwmnapiApis(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("setPwmStatus", setPwmStatus),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

/*
 * 模块定义
 */
static napi_module pwmnapiModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registerpwmnapiApis,
    .nm_modname = "pwmtest", // 模块名
    .nm_priv = ((void *)0),
    .reserved = {0},
};

#ifdef __cplusplus
}
#endif
/*
 * 注册模块
 */
extern "C" __attribute__((constructor)) void RegisterpwmnapiModule(void)
{
    napi_module_register(&pwmnapiModule); // 接口注册函数
}
