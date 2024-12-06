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

#include <unistd.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#ifdef __cplusplus
#include "dacihuanginterface.h"
extern "C" {
#endif

struct dacihuangAddOnData {
    napi_async_work asyncWork = nullptr; // 异步工作项
    napi_deferred deferred = nullptr;    // 用于Promise的resolve、reject处理
    napi_ref callback = nullptr;         // 回调函数
    int args[1] = {0};                   // 2个输入参数
    int result = 0;                      // 业务逻辑处理结果（返回值）
};

// 业务逻辑处理函数，由worker线程池调度执行。
static void dacihuangshuziExecuteCB(napi_env env, dacihuangAddOnData *data)
{
    dacihuangAddOnData *addOnData = data;
    int gpio = addOnData->args[0];
    int getvalue = -1;
    getvalue = Read_dacihuang_shuzivalue(gpio);
    addOnData->result = getvalue;
}

// 业务逻辑处理函数，由worker线程池调度执行。
static void dacihuangmoniExecuteCB(napi_env env, dacihuangAddOnData *data)
{
    dacihuangAddOnData *addOnData = data;
    int adc = addOnData->args[0];
    int getvalue = -1;
    getvalue = Read_dacihuang_monivalue(adc);
    addOnData->result = getvalue;
}

// 业务逻辑处理完成回调函数，在业务逻辑处理函数执行完成或取消后触发，由EventLoop线程中执行。
static void completeCBForPromise(napi_env env, napi_status status, LedAddOnData *data)
{
    dacihuangAddOnData *addOnData = data;
    napi_value result = nullptr;
    napi_create_int32(env, addOnData->result, &result);
    napi_resolve_deferred(env, addOnData->deferred, result);

    // 删除napi_ref对象
    if (addOnData->callback != nullptr) {
        napi_delete_reference(env, addOnData->callback);
    }

    // 删除异步工作项
    napi_delete_async_work(env, addOnData->asyncWork);
    delete addOnData;
}


static napi_value dacihuangshuziWithPromise(napi_env env, napi_callback_info info)
{
    // 获取1个参数，值的类型是js类型(napi_value)
    size_t argc = 1;
    napi_value args[1];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto dacihuangonData = new dacihuangAddOnData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };

    // 将被收到的参数传入
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &dacihuangonData->args[0]));

    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "dacihuangshuziWithPromise", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, dacihuangshuziExecuteCB, completeCBForPromise, (void *)dacihuangonData,
                           &dacihuangonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, dacihuangonData->asyncWork);

    // 返回promise
    return promise;
}

static napi_value dacihuangmoniWithPromise(napi_env env, napi_callback_info info)
{
    // 获取1个参数，值的类型是js类型(napi_value)
    size_t argc = 1;
    napi_value args[1];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto dacihuangonData = new dacihuangAddOnData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };

    // 将被收到的参数传入
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &dacihuangonData->args[0]));

    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "dacihuangmoniWithPromise", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, dacihuangmoniExecuteCB, completeCBForPromise, (void *)dacihuangonData,
                           &dacihuangonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, dacihuangonData->asyncWork);

    // 返回promise
    return promise;
}

// 注册接口
static napi_value registerdacihuangApis(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("dacihuangshuziWithPromise", dacihuangshuziWithPromise),
        DECLARE_NAPI_FUNCTION("dacihuangmoniWithPromise", dacihuangmoniWithPromise),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

// 模块定义
static napi_module dacihuangModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registerdacihuangApis,
    .nm_modname = "dacihuang",
    .nm_priv = ((void *)0),
    .reserved = {0},
};
#ifdef __cplusplus
}
#endif
// 注册模块
extern "C" __attribute__((constructor)) void RegisterdacihuangModule(void)
{
    napi_module_register(&dacihuangModule); // 接口注册函数
}
