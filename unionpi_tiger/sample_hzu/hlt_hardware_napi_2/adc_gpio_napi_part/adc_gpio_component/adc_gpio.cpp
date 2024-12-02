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
#include "um_gpio.h"
#include "um_adc.h"
#include "sample_client.h"

// 获取摇杆数据：X 和 Y 轴的模拟值，Z 轴的 GPIO 值
int get_joystick_data(int *x_value, int *y_value, int *z_value)
{
    int adc_value = 0;

    // 获取 X 轴值 (ADC_1)
    if (get_adc_data(ADC_1, &adc_value) < 0) {
        return ADC_PATH_ERROR;
    }
    *x_value = adc_value;

    // 获取 Y 轴值 (ADC_2)
    if (get_adc_data(ADC_2, &adc_value) < 0) {
        return ADC_PATH_ERROR;
    }
    *y_value = adc_value;

    // 配置 GPIO（假设 Z 按钮连接到 UM_GPIO_10）
    if (UM_GPIO_Export(UM_GPIO_10, 1) < 0 || UM_GPIO_SetDirection(UM_GPIO_10, UM_GPIO_DIRECTION_IN) < 0) {
        return UM_GPIO_ERR;
    }

    // 获取 Z 轴按键状态
    int gpio_value = 0;
    if (UM_GPIO_GetValue(UM_GPIO_10, &gpio_value) < 0) {
        return UM_GPIO_ERR;
    }

    // 按键按下为 1，未按下为 0
    *z_value = (gpio_value == UM_GPIO_HIGH_LEVE) ? 1 : 0;
    return 0;
}

// 同步方法：获取 X, Y, Z 的值
static napi_value sync_get_joystick_values(napi_env env, napi_callback_info info)
{
    int x_value = 0, y_value = 0, z_value = 0;

    // 调用获取数据的函数
    if (get_joystick_data(&x_value, &y_value, &z_value) < 0) {
        napi_value error;
        NAPI_CALL(env, napi_create_int32(env, -1, &error));
        return error;
    }

    // 创建返回对象
    napi_value result, x_val, y_val, z_val;
    NAPI_CALL(env, napi_create_object(env, &result));

    // 将 X 值添加到结果对象
    NAPI_CALL(env, napi_create_int32(env, x_value, &x_val));
    NAPI_CALL(env, napi_set_named_property(env, result, "x", x_val));

    // 将 Y 值添加到结果对象
    NAPI_CALL(env, napi_create_int32(env, y_value, &y_val));
    NAPI_CALL(env, napi_set_named_property(env, result, "y", y_val));

    // 将 Z 值添加到结果对象
    NAPI_CALL(env, napi_create_int32(env, z_value, &z_val));
    NAPI_CALL(env, napi_set_named_property(env, result, "z", z_val));

    return result;
}

// 同步方法：获取权限
static napi_value sync_get_permission(napi_env env, napi_callback_info info)
{
    constexpr char path[128] = "/sys/bus/iio/devices/iio:device0/in_voltage2_raw";
    int result = open_permission(path);

    napi_value ret;
    NAPI_CALL(env, napi_create_int32(env, result, &ret));
    return ret;
}

// 注册 NAPI 方法
static napi_value register_adc_napiApis(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("sync_get_joystick_values", sync_get_joystick_values),
        DECLARE_NAPI_FUNCTION("sync_get_permission", sync_get_permission),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

// 模块定义
static napi_module adc_napiModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = register_adc_napiApis,
    .nm_modname = "adc_gpio_napi", // 模块名
    .nm_priv = nullptr,
    .reserved = {0},
};

// 注册模块
extern "C" __attribute__((constructor)) void Register_adc_napiModule(void)
{
    napi_module_register(&adc_napiModule);
}
