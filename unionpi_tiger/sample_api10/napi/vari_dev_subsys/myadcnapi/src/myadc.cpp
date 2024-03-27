/* Copyright 2024 Unionman Technology Co., Ltd.
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
#include <cstdio>
#include <cstring>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#define ADC_CHANNEL_1 "/sys/bus/iio/devices/iio:device0/in_voltage2_raw" // ADC1的文件路径
#define ADC_CHANNEL_2 "/sys/bus/iio/devices/iio:device0/in_voltage3_raw" // ADC1的文件路径

#define TEMP_CONST 0.042

#ifdef __cplusplus
extern "C"
{
#endif

    int GetAdcData(int channel, int *value)
    {
        char adcPath[128];
        memset_s(adcPath, 0, sizeof(adcPath)); // 数组清空
        if (channel == 1) {
            sprintf_s(adcPath, "%s", ADC_CHANNEL_1);
        } else {
            sprintf_s(adcPath, "%s", ADC_CHANNEL_2);
        }

        FILE *fp = fopen(adcPath, "r"); // 只读打开文件
        if (!fp) {
            perror("fopen"); // 提示报错信息
            return -1;
        }

        char buffer[sizeof(int)];             // 从文件中读取到的数据
        fread(buffer, sizeof(buffer), 1, fp); // 读取数据的个数为1
        if (fclose(fp) == 0) {
            return 0;                         // 文件成功关闭，返回成功状态
        } else {
            perror("fclose");                 // 提示关闭文件失败
            return -1;                        // 返回失败状态
        }

        *value = atoi(buffer);
        return 0;
    }

    // 4.同步方法实现获取权限
    static napi_value GetAdcValue(napi_env env, napi_callback_info info)
    {
        int channel = 0;
        // 获取1个参数，值的类型是js类型转化成napi类型
        size_t argc = 1L;
        int32_t adcValue = 0;
        napi_value args[1];
        napi_value thisArg = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

        // 将转化成的napi类型参数在转化成C/C++类型
        NAPI_CALL(env, napi_get_value_int32(env, args[0], &channel));

        GetAdcData(channel, &adcValue);

        napi_value ret;
        NAPI_CALL(env, napi_create_int32(env, adcValue, &ret));
        return ret;
    }
    /*
     * 3.注册接口
     */
    static napi_value register_adc_napi_apis(napi_env env, napi_value exports)
    {
        napi_property_descriptor desc[] = {
            // 声明函数供ets调用
            DECLARE_NAPI_FUNCTION("GetAdcValue", GetAdcValue),
        };
        NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
        return exports;
    }
    /*
     * 1.模块定义
     */
    static napi_module adc_napiModule = {
        .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = register_adc_napi_apis,
        .nm_modname = "adcnapi", // 模块名
        .nm_priv = ((void *)0),
        .reserved = {0},
    };

#ifdef __cplusplus
}
#endif
/*
 * 2.注册模块
 */
extern "C" __attribute__((constructor)) void RegisteradcNapiModule(void)
{
    napi_module_register(&adc_napiModule); // 接口注册函数
}