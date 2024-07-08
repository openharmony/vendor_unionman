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
#include <bits/alltypes.h>

#ifdef __cplusplus
#include "um_gpio.h"
extern "C" {
#endif

struct LedData {
    napi_async_work asyncWork = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callback = nullptr;
    int args[2] = {0};
    int result = 0;
};

static void SetLedValueExecute(napi_env env, void *data)
{
    //获取上下文数据
    LedData *ledData = (LedData *)data;
    int gpioNum = ledData->args[0];
    int gpioValue = ledData->args[1];
    // 判断gpio口是否已经导出
    int bExport = UM_GPIO_EXPORTED;
    int getValue = -1;
    UM_GPIO_IsExport(gpioNum, &getValue);
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpioNum, bExport);
    }
    //设置gpio口为输出模式
    UM_GPIO_SetDirection(gpioNum, UM_GPIO_DIRECTION_OUT);
    //设置gpio口值, 并将返回值赋值给result
    ledData->result = UM_GPIO_SetValue(gpioNum, gpioValue);
}

static void GetLedValueExecute(napi_env env, void *data)
{
    //获取上下文数据
    LedData *ledData = (LedData *)data;
    int gpioNum = ledData->args[0];
    int getValue = -1;
    //读取gpio口的值, 并将其赋值给result
    UM_GPIO_GetValue(gpioNum, &getValue);
    ledData->result = getValue;
}

static void SetLedValueComplete(napi_env env, napi_status status, void *data)
{
    LedData *ledData = (LedData *)data;
    
    napi_value result = nullptr;
    napi_create_int32(env, ledData->result, &result);
    
    napi_resolve_deferred(env, ledData->deferred, result);
    
    if (ledData->callback != nullptr) {
        napi_delete_reference(env, ledData->callback);
    }
    napi_delete_async_work(env, ledData->asyncWork);
    delete ledData;
}

static void GetLedValueComplete(napi_env env, napi_status status, void *data)
{
    LedData *ledData = (LedData *)data;
    napi_value result = nullptr;
    napi_create_int32(env, ledData->result, &result);
    
    napi_resolve_deferred(env, ledData->deferred, result);
    
    if (ledData->callback != nullptr) {
        napi_delete_reference(env, ledData->callback);
    }
    napi_delete_async_work(env, ledData->asyncWork);
    delete ledData;
}


static napi_value SetLedValue(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    napi_value thisArg = nullptr;
    napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr);
    
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    napi_create_promise(env, &deferred, &promise);
    
    auto ledData = new LedData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };
    //传入收到的数据
    napi_get_value_int32(env, args[0], &ledData->args[0]);
    napi_get_value_int32(env, args[1], &ledData->args[1]);

    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "SetLedValue", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, SetLedValueExecute, SetLedValueComplete, (void *)ledData,
                           &ledData->asyncWork);
    napi_queue_async_work(env, ledData->asyncWork);
    // 原生方法返回promise
    return promise;
}

static napi_value GetLedValue(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    napi_value thisArg = nullptr;
    napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr);
    
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    napi_create_promise(env, &deferred, &promise);
    
    auto ledData = new LedData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };
    
    napi_get_value_int32(env, args[0], &ledData->args[0]);

    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "GetLedValue", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, GetLedValueExecute, GetLedValueComplete, (void *)ledData,
                           &ledData->asyncWork);
    napi_queue_async_work(env, ledData->asyncWork);
    return promise;
}

EXTERN_C_START
static napi_value GpioLedInit(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        {"SetLedValue", nullptr, SetLedValue, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"GetLedValue", nullptr, GetLedValue, nullptr, nullptr, nullptr, napi_default, nullptr}
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module GpioLedModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = GpioLedInit,
    .nm_modname = "gpioled",
    .nm_priv = ((void *)0),
    .reserved = {0},
};


#ifdef __cplusplus
}
#endif


extern "C" __attribute__((constructor)) void RegisterGpioLedModule(void)
{
    napi_module_register(&GpioLedModule);
}