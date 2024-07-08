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

#ifdef __cplusplus
#include "um_adc.h"
extern "C" {
#endif
// 保存光线强度
static int g_light;

struct LightData {
    napi_async_work asyncWork = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callback = nullptr;
    int args[1] = {0};
    int result = 0;
};

static void LightIntensityExecute(napi_env env, void *data)
{
    LightData *lightData = (LightData *)data;
    int pin = lightData->args[0];
    lightData->result = get_adc_data(pin, &g_light);
}

static void LightIntensityComplete(napi_env env, napi_status status, void *data)
{
    LightData *lightData = (LightData *)data;
    napi_value result = nullptr;
    napi_create_int32(env, lightData->result, &result);
    napi_resolve_deferred(env, lightData->deferred, result);
    if (lightData->callback != nullptr) {
        napi_delete_reference(env, lightData->callback);
    }
    napi_delete_async_work(env, lightData->asyncWork);
    delete lightData;
}
// 异步读取传感器数据
static napi_value LightIntensity(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    napi_create_promise(env, &deferred, &promise);
    auto lightData = new LightData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };
    napi_get_value_int32(env, args[0], &lightData->args[0]);
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "LightIntensity", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, LightIntensityExecute, LightIntensityComplete, (void *)lightData,
                           &lightData->asyncWork);
    napi_queue_async_work(env, lightData->asyncWork);
    return promise;
}
// 获取光线强度
static napi_value GetLightIntensity(napi_env env, napi_callback_info info)
{
    napi_value result;
    napi_create_int32(env, g_light, &result);
    return result;
}

EXTERN_C_START
static napi_value LightInit(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        {"LightIntensity", nullptr, LightIntensity, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"GetLightIntensity", nullptr, GetLightIntensity, nullptr, nullptr, nullptr, napi_default, nullptr}};
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module LightModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = LightInit,
    .nm_modname = "light",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

#ifdef __cplusplus
}
#endif

extern "C" __attribute__((constructor)) void RegisterLightModule(void)
{
    napi_module_register(&LightModule);
}