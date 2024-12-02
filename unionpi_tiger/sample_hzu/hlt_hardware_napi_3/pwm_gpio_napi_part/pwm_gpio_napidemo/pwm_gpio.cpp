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
#include <unistd.h> // 用于 usleep

#ifdef __cplusplus
#include "um_pwm.h"
#include "um_gpio.h"
extern "C" {
#endif

// PWM相关常量
static const int PWM_PERIOD = 20000; // 20ms周期

struct PwmGpioData {
    napi_async_work asyncWork = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callback = nullptr;
    int args[3] = {0}; // 红、蓝灯占空比 + 绿色灯控制
    int result = 0;
};

// 业务逻辑处理函数（PWM控制红、蓝灯，GPIO控制绿灯）
static void executePwmGpioCB(napi_env env, void *data)
{
    PwmGpioData *pwmData = (PwmGpioData *)data;

    int redDuty = pwmData->args[0];
    int blueDuty = pwmData->args[1];
    int greenLedOn = pwmData->args[2]; // 绿色灯控制（1:亮, 0:灭）
    
    // 初始化PWM通道
    init_pmw(PWM1); // 红灯
    init_pmw(PWM2); // 蓝灯
    set_pwm_period(PWM1, PWM_PERIOD);
    set_pwm_period(PWM2, PWM_PERIOD);

    // 初始化GPIO控制绿灯
    int greenGpioPin = 12; // 假定绿色灯对应GPIO引脚12
    UM_GPIO_Export(greenGpioPin, 1);  // 导出GPIO
    UM_GPIO_SetDirection(greenGpioPin, UM_GPIO_DIRECTION_OUT); // 设置为输出

    // 混色效果
    for (int i = 0; i < 100; ++i) {
        // 控制红色（PWM1）和蓝色（PWM2）
        set_pwm_dutyCycle(PWM1, redDuty);
        set_pwm_enable(PWM1, 1);
        set_pwm_dutyCycle(PWM2, blueDuty);
        set_pwm_enable(PWM2, 1);
        usleep(5000);

        // 控制绿色灯（通过GPIO）
        if (greenLedOn == 1) {
            UM_GPIO_SetValue(greenGpioPin, UM_GPIO_HIGH_LEVE); // 绿色灯亮
        } else {
            UM_GPIO_SetValue(greenGpioPin, UM_GPIO_LOW_LEVE); // 绿色灯灭
        }
        usleep(5000);
    }

    // 停止所有控制
    set_pwm_enable(PWM1, 0);
    set_pwm_enable(PWM2, 0);
    UM_GPIO_SetValue(greenGpioPin, UM_GPIO_LOW_LEVE);

    pwmData->result = 1; // 成功
}

// 异步工作完成回调函数
static void completePwmGpioCBForPromise(napi_env env, napi_status status, void *data)
{
    PwmGpioData *pwmData = (PwmGpioData *)data;
    napi_value result = nullptr;
    napi_create_int32(env, pwmData->result, &result);
    napi_resolve_deferred(env, pwmData->deferred, result);

    if (pwmData->callback != nullptr) {
        napi_delete_reference(env, pwmData->callback);
    }
    napi_delete_async_work(env, pwmData->asyncWork);
    delete pwmData;
}

// NAPI接口：设置PWM和GPIO控制
static napi_value setPwmGpioStatus(napi_env env, napi_callback_info info)
{
    size_t argc = 4;
    napi_value args[4];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    auto pwmData = new PwmGpioData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };

    NAPI_CALL(env, napi_get_value_int32(env, args[0], &pwmData->args[0])); // 红灯占空比
    NAPI_CALL(env, napi_get_value_int32(env, args[1], &pwmData->args[1])); // 蓝灯占空比
    NAPI_CALL(env, napi_get_value_int32(env, args[2], &pwmData->args[2])); // 绿色灯亮灭控制（1:亮，0:灭）
    
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "setPwmGpioStatus", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, executePwmGpioCB, completePwmGpioCBForPromise,
                           (void *)pwmData, &pwmData->asyncWork);

    napi_queue_async_work(env, pwmData->asyncWork);
    return promise;
}

// 注册接口
static napi_value registerPwmGpioNapiApis(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("setPwmGpioStatus", setPwmGpioStatus),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

// 模块定义
static napi_module pwmGpioNapiModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registerPwmGpioNapiApis,
    .nm_modname = "pwm_gpio_napi",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

#ifdef __cplusplus
}
#endif

// 注册模块
extern "C" __attribute__((constructor)) void RegisterPwmGpioNapiModule(void)
{
    napi_module_register(&pwmGpioNapiModule);
}
