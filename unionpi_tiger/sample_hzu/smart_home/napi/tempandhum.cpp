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
#include <unistd.h>

#ifdef __cplusplus
#include "i2cinterface.h"
extern "C" {
#endif

static sRHAndTemp_t g_tempRhData;
static double  g_temperatureC;
static double  g_temperatureF;
static double  g_tumidity;
static const uint16_t DEVICE_ADDER = 0x44;
static char g_devName[] = "/dev/i2c-5";

static napi_value Soft_Reset(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    int value = SoftReset(g_devName, DEVICE_ADDER);
    usleep(50L * 1000L);
    napi_create_int32(env, value, &result);
    return result;
}

static napi_value SetMode(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    napi_value thisArg = nullptr;
    napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr);
    int mps;
    int repeatability;
    napi_get_value_int32(env, args[0], &mps);
    napi_get_value_int32(env, args[1], &repeatability);
    napi_value result = nullptr;
    int value = ModeSet(g_devName, DEVICE_ADDER, mps, repeatability);
    usleep(20L * 1000L);
    napi_create_int32(env, value, &result);
    return result;
}

static napi_value ReadTemp_Hum(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    int value = ReadTempAndHum(g_devName, DEVICE_ADDER, &g_tempRhData);
    usleep(20L * 1000L);
    napi_create_int32(env, value, &result);
    g_TemperatureC = g_tempRhData.TemperatureC;
    g_TemperatureF = g_tempRhData.TemperatureF;
    g_Humidity = g_tempRhData.Humidity;
    return result;
}

static napi_value GetTemperatureC(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_create_double(env, g_temperatureC, &result);
    return result;
}

static napi_value GetTemperatureF(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_create_double(env, g_temperatureF, &result);
    return result;
}

static napi_value GetHumidity(napi_env env, napi_callback_info info)
{
    napi_value result = nullptr;
    napi_create_double(env, g_humidity, &result);
    return result;
}

EXTERN_C_START
static napi_value TempAndHumInit(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        {"Soft_Reset", nullptr, Soft_Reset, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"SetMode", nullptr, SetMode, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"ReadTemp_Hum", nullptr, ReadTemp_Hum, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"GetTemperatureC", nullptr, GetTemperatureC, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"GetTemperatureF", nullptr, GetTemperatureF, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"GetHumidity", nullptr, GetHumidity, nullptr, nullptr, nullptr, napi_default, nullptr},
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module TempAndHumModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = TempAndHumInit,
    .nm_modname = "TempAndHum",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

#ifdef __cplusplus
}
#endif

extern "C" __attribute__((constructor)) void RegisterTempAndHumModule(void)
{
    napi_module_register(&TempAndHumModule);
}
