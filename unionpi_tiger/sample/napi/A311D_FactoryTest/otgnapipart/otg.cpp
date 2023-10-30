/*
 * Copyright 2023 Unionman Technology Co., Ltd.
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
#include "napi/native_node_api.h"

#ifdef __cplusplus
#include "um_otg.h"
extern "C" {
#endif

struct otgOnData {
    napi_async_work asyncWork = nullptr; // 异步工作项
    napi_deferred deferred = nullptr; // 用于Promise的resolve、reject处理
    napi_ref callback = nullptr; // 回调函数
    int result = 0; // 业务逻辑处理结果（返回值）
};

// 业务逻辑处理函数，由worker线程池调度执行。
static void executeCB(napi_env env, void* data)
{
    otgOnData* otgData = (otgOnData*)data;
    int ret = 0;
    ret = otg_test();
    otgData->result = ret;
}

// 业务逻辑处理完成回调函数，在业务逻辑处理函数执行完成或取消后触发，由EventLoop线程中执行。
static void completeCBForPromise(napi_env env, napi_status status, void* data)
{
    otgOnData* otgData = (otgOnData*)data;
    napi_value result = nullptr;
    napi_create_int32(env, otgData->result, &result);
    napi_resolve_deferred(env, otgData->deferred, result);

    // 删除napi_ref对象
    if (otgData->callback != nullptr) {
        napi_delete_reference(env, otgData->callback);
    }

    // 删除异步工作项
    napi_delete_async_work(env, otgData->asyncWork);
    delete otgData;
}

static napi_value UM_otg_test(napi_env env, napi_callback_info info)
{
    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto otgData = new otgOnData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };
    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "UM_otg_test", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        env, nullptr, resourceName, executeCB, completeCBForPromise, (void*)otgData, &otgData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, otgData->asyncWork);

    // 返回promise
    return promise;
}

/*
 * 注册接口
 */
static napi_value registerotgnapiApis(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("UM_otg_test", UM_otg_test),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

/*
 * 模块定义
 */
static napi_module otgnapiModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registerotgnapiApis,
    .nm_modname = "otgtest", // 模块名
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

#ifdef __cplusplus
}
#endif
/*
 * 注册模块
 */
extern "C" __attribute__((constructor)) void RegisterotgnapiModule(void)
{
    napi_module_register(&otgnapiModule); // 接口注册函数
}