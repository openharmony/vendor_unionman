#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "um_gpio.h"

// 用于存储与传感器和蜂鸣器操作相关的异步任务数据
struct SensorAddOnData {
    napi_async_work asyncWork = nullptr; // 异步工作项
    napi_deferred deferred = nullptr;    // 用于Promise的resolve、reject处理
    napi_ref callback = nullptr;         // 回调函数
    int gpioFire = 0;                    // 火焰传感器对应的GPIO口编号
    int gpioCihuan = 0;                  // 磁簧传感器对应的GPIO口编号
    int gpioFmq = 0;                     // 蜂鸣器对应的GPIO口编号
    int getFireValue = -1;               // 火焰传感器获取到的值
    int getCihuanValue = -1;             // 磁簧传感器获取到的值
    int fmqValue = -1;                   // 蜂鸣器当前状态值
};

// 检查并导出指定的GPIO口
static void exportGpioIfNeeded(napi_env env, int gpioNum) {
    int bExport = UM_GPIO_EXPORTED;
    int getValue = -1;

    UM_GPIO_IsExport(gpioNum, &getValue);
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpioNum, bExport);
    }
}

// 设置指定GPIO口为输入或输出模式
static void setGpioDirection(napi_env env, int gpioNum, int direction) {
    UM_GPIO_SetDirection(gpioNum, direction);
}

// 获取火焰传感器和磁簧传感器值的异步执行回调函数
static void getSensorValuesExecuteCB(napi_env env, void *data) {
    SensorAddOnData *addOnData = (SensorAddOnData *)data;

    // 先确保火焰传感器和磁簧传感器的GPIO口已导出
    exportGpioIfNeeded(env, addOnData->gpioFire);
    exportGpioIfNeeded(env, addOnData->gpioCihuan);

    // 设置火焰传感器和磁簧传感器的GPIO口为输入模式
    setGpioDirection(env, addOnData->gpioFire, UM_GPIO_DIRECTION_IN);
    setGpioDirection(env, addOnData->gpioCihuan, UM_GPIO_DIRECTION_IN);

    // 获取火焰传感器的值
    UM_GPIO_GetValue(addOnData->gpioFire, &addOnData->getFireValue);

    // 获取磁簧传感器的值
    UM_GPIO_GetValue(addOnData->gpioCihuan, &addOnData->getCihuanValue);
}

// 根据传感器状态设置蜂鸣器值的异步执行回调函数
static void setFmqValueExecuteCB(napi_env env, void *data) {
    SensorAddOnData *addOnData = (SensorAddOnData *)data;

    // 先确保蜂鸣器的GPIO口已导出
    exportGpioIfNeeded(env, addOnData->gpioFmq);

    // 设置蜂鸣器的GPIO口为输出模式
    setGpioDirection(env, addOnData->gpioFmq, UM_GPIO_DIRECTION_OUT);

    // 根据火焰传感器和磁簧传感器的值来设置蜂鸣器的值
    if (addOnData->getFireValue ^ addOnData->getCihuanValue) {
        // 有火1有磁0  正常蜂鸣器为1
        // 有火1无磁1  着火蜂鸣器为0
        // 无火0有磁0  燃气泄漏蜂鸣器为0
        // 无火0无磁1  正常蜂鸣器为1

        if ((addOnData->getFireValue == 1 && addOnData->getCihuanValue == 0) ||
            (addOnData->getFireValue == 0 && addOnData->getCihuanValue == 1)) {
            UM_GPIO_SetValue(addOnData->gpioFmq, UM_GPIO_LOW_LEVE);
            addOnData->fmqValue = 0;
        } else {
            UM_GPIO_SetValue(addOnData->gpioFmq, UM_GPIO_HIGH_LEVE);
            addOnData->fmqValue = 1;
        }
    } else {
        UM_GPIO_SetValue(addOnData->gpioFmq, UM_GPIO_HIGH_LEVE);
        addOnData->fmqValue = 1;
    }
}

// 异步操作完成后的回调函数（用于回调函数方式）
static void completeCBForCallback(napi_env env, napi_status status, void *data) {
    SensorAddOnData *addOnData = (SensorAddOnData *)data;
    napi_value callback = nullptr;
    napi_get_reference_value(env, addOnData->callback, &callback);
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);
    napi_value result = nullptr;
    napi_create_object(env, &result);

    // 设置返回结果对象的属性，包含火焰传感器、磁簧传感器和蜂鸣器的值

    //1111
    napi_value int32Value1;
    napi_create_int32(env, addOnData->getFireValue, &int32Value1);
    napi_set_named_property(env, result, "flameValue", int32Value1);
    napi_value int32Value2;
    napi_create_int32(env, addOnData->getCihuanValue, &int32Value2);
    napi_set_named_property(env, result, "cihuanValue", int32Value2);
    napi_value int32Value3;
    napi_create_int32(env, addOnData->fmqValue, &int32Value3);
    napi_set_named_property(env, result, "fmqValue", int32Value3);
    //1111
   // napi_set_named_property(env, result, "flameValue", napi_create_int32(env, addOnData->getFireValue));
  //  napi_set_named_property(env, result, "cihuanValue", napi_create_int32(env, addOnData->getCihuanValue));
   // napi_set_named_property(env, result, "fmqValue", napi_create_int32(env, addOnData->fmqValue));

    // 执行回调函数
    napi_value returnVal = nullptr;
    napi_call_function(env, undefined, callback, 1, &result, &returnVal);

    // 删除napi_ref对象
    if (addOnData->callback!= nullptr) {
        napi_delete_reference(env, addOnData->callback);
    }

    // 删除异步工作项
    napi_delete_async_work(env, addOnData->asyncWork);
    delete addOnData;
}

// 异步操作完成后的回调函数（用于Promise方式）
static void completeCBForPromise(napi_env env, napi_status status, void *data) {
    SensorAddOnData *addOnData = (SensorAddOnData *)data;
    napi_value result = nullptr;
    napi_create_object(env, &result);

    // 设置返回结果对象的属性，包含火焰传感器、磁簧传感器和蜂鸣器的值
   // napi_set_named_property(env, result, "flameValue", napi_create_int32(env, addOnData->getFireValue));
  //  napi_set_named_property(env, result, "cihuanValue", napi_create_int32(env, addOnData->getCihuanValue));
   // napi_set_named_property(env, result, "fmqValue", napi_create_int32(env, addOnData->fmqValue));
    //1111
    napi_value int32Value1;
    napi_create_int32(env, addOnData->getFireValue, &int32Value1);
    napi_set_named_property(env, result, "flameValue", int32Value1);
    napi_value int32Value2;
    napi_create_int32(env, addOnData->getCihuanValue, &int32Value2);
    napi_set_named_property(env, result, "cihuanValue", int32Value2);
    napi_value int32Value3;
    napi_create_int32(env, addOnData->fmqValue, &int32Value3);
    napi_set_named_property(env, result, "fmqValue", int32Value3);

    //1111

    napi_resolve_deferred(env, addOnData->deferred, result);

    // 删除napi_ref对象
    if (addOnData->callback!= nullptr) {
        napi_delete_reference(env, addOnData->callback);
    }

    // 删除异步工作项
    napi_delete_async_work(env, addOnData->asyncWork);
    delete addOnData;
}

// 获取火焰传感器和磁簧传感器的值，并通过回调函数异步处理
static napi_value getSensorValuesWithCallback(napi_env env, napi_callback_info info) {
    // 获取无参数，因为传感器的GPIO口编号在函数内部固定设置
    size_t argc = 0;
    napi_value args[1];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

   // 异步工作项上下文用户数据，传递到异步工作项的execute、complete中传递数据
    // auto addonData = new SensorAddOnData {
    //    .asyncWork = nullptr,
   // };
    //1111
        auto addonData = new SensorAddOnData();
    addonData->asyncWork = nullptr;
    
    //1111
    // 设置火焰传感器和磁簧传感器对应的GPIO口编号
    addonData->gpioFire = UM_GPIO_12;
    addonData->gpioCihuan = UM_GPIO_01;

    // 创建async work，创建成功后通过最后一个参数接受async work的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "getSensorValuesWithCallback", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, getSensorValuesExecuteCB, completeCBForCallback, (void *)addonData,
                           &addonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    // 原生方法返回空对象
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

// 根据传感器状态设置蜂鸣器的值，并通过回调函数异步处理
static napi_value setFmqValueWithCallback(napi_env env, napi_callback_info info) {
    // 获取无参数，因为传感器和蜂鸣器的GPIO口编号在函数内部固定设置
    size_t argc = 0;
    napi_value args[1];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete中传递数据
    //auto addonData = new SensorAddOnData {
    //  .asyncWork = nullptr,
   // };
    //1111
    auto addonData = new SensorAddOnData();
    addonData->asyncWork = nullptr;
    
   // addonData->deferred = deferred;

    //1111
    // 设置火焰传感器、磁簧传感器和蜂鸣器对应的GPIO口编号
    addonData->gpioFire = UM_GPIO_12;
    addonData->gpioCihuan = UM_GPIO_01;
    addonData->gpioFmq = UM_GPIO_16;

    // 创建async work，创建成功后通过最后一个参数接受async work的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "setFmqValueWithCallback", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, setFmqValueExecuteCB, completeCBForCallback, (void *)addonData,
                           &addonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    // 原生方法返回空对象
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

// 获取火焰传感器和磁簧传感器的值，并通过Promise异步处理
static napi_value getSensorValuesWithPromise(napi_env env, napi_callback_info info) {
    // 获取无参数，因为传感器的GPIO口编号在函数内部固定设置
    size_t argc = 0;
    napi_value args[1];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
  //  auto addonData = new SensorAddOnData {
  //    .asyncWork = nullptr;
  //    .deferred = deferred;
 //   };
    //1111
        auto addonData = new SensorAddOnData();
    addonData->asyncWork = nullptr;
    addonData->deferred = deferred;
    //1111
    // 设置火焰传感器和磁簧传感器对应的GPIO口编号
    addonData->gpioFire = UM_GPIO_12;
    addonData->gpioCihuan = UM_GPIO_01;

    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "getSensorValuesWithPromise", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, getSensorValuesExecuteCB, completeCBForPromise, (void *)addonData,
                           &addonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    // 返回promise
    return promise;
}

// 根据传感器状态设置蜂鸣器的值，并通过Promise异步处理
static napi_value setFmqValueWithPromise(napi_env env, napi_callback_info info) {
    // 获取无参数，因为传感器和蜂鸣器的GPIO口编号在函数内部固定设置
    size_t argc = 0;
    napi_value args[1];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的.execute、complete之间传递数据
  //  auto addonData = new SensorAddOnData {
 //     .asyncWork = nullptr;
 //     .deferred = deferred;
 //   };
       //1111
        auto addonData = new SensorAddOnData();
    addonData->asyncWork = nullptr;
    addonData->deferred = deferred;
    //1111
    // 设置火焰传感器、磁簧传感器和蜂鸣器对应的GPIO口编号
    addonData->gpioFire = UM_GPIO_12;
    addonData->gpioCihuan = UM_GPIO_01;
    addonData->gpioFmq = UM_GPIO_16;

    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "setFmqValueWithPromise", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, setFmqValueExecuteCB, completeCBForPromise, (void *)addonData,
                           &addonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    // 返回promise
    return promise;
}

// 注册接口函数
static napi_value registerSensorNapiApis(napi_env env, napi_value exports) {
    napi_value gpioValHigh = nullptr;
    napi_value gpioValLow = nullptr;
    NAPI_CALL(env, napi_create_int32(env, UM_GPIO_HIGH_LEVE, &gpioValHigh));
    NAPI_CALL(env, napi_create_int32(env, UM_GPIO_LOW_LEVE, &gpioValLow));
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getSensorValuesWithCallback", getSensorValuesWithCallback),
        DECLARE_NAPI_FUNCTION("setFmqValueWithCallback", setFmqValueWithCallback),
        DECLARE_NAPI_FUNCTION("getSensorValuesWithPromise", getSensorValuesWithPromise),
        DECLARE_NAPI_FUNCTION("setFmqValueWithPromise", setFmqValueWithPromise),
        DECLARE_NAPI_STATIC_PROPERTY("GPIO_ON", gpioValHigh),
        DECLARE_NAPI_STATIC_PROPERTY("GPIO_OFF", gpioValLow),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

// 模块定义
static napi_module sensorNapiModule = {
  .nm_version = 1,
  .nm_flags = 0,
  .nm_filename = nullptr,
  .nm_register_func = registerSensorNapiApis,
  .nm_modname = "sensorNapi", // 模块名
  .nm_priv = ((void *)0),
  .reserved = {0},
};

// 注册模块
extern "C" __attribute__((constructor)) void RegisterSensorNapiModule(void) {
    napi_module_register(&sensorNapiModule); // 接口注册函数
}