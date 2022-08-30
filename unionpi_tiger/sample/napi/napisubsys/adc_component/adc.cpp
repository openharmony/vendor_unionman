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

#include <cstdio>
#include <cstring>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "um_adc.h"
#include "sample_client.h"

// 同步方法实现获取adc值
static napi_value sync_get_adc_value(napi_env env, napi_callback_info info)
{
    constexpr int adc_channel = 1;
    int value = 0;
    if (get_adc_data(adc_channel, &value) < 0) {
        value = -1;
    }

    napi_value ret;
    NAPI_CALL(env, napi_create_int32(env, value, &ret));
    return ret;
}
// 同步方法实现获取权限
static napi_value sync_get_permission(napi_env env, napi_callback_info info)
{
    int result = 0;
    constexpr char path[128] = "/sys/bus/iio/devices/iio:device0/in_voltage2_raw";
    result = open_permission(path);

    napi_value ret;
    NAPI_CALL(env, napi_create_int32(env, result, &ret));
    return ret;
}
/*
 * 注册接口
 */
static napi_value registeradc_napiApis(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // 声明函数供ets调用
        DECLARE_NAPI_FUNCTION("sync_get_adc_value", sync_get_adc_value),
        DECLARE_NAPI_FUNCTION("sync_get_permission", sync_get_permission),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}
/*
 * 模块定义
 */
static napi_module adc_napiModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registeradc_napiApis,
    .nm_modname = "adc_napi", // 模块名
    .nm_priv = ((void *)0),
    .reserved = {0},
};

/*
 * 注册模块
 */
extern "C" __attribute__((constructor)) void Registeradc_napiModule(void)
{
    napi_module_register(&adc_napiModule); // 接口注册函数
}