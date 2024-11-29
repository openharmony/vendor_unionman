#include <cstdio>
#include <cstring>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "um_adc.h"
#include "um_gpio.h"

struct LedAddOnData {
    napi_async_work asyncWork = nullptr; // 异步工作项
    napi_deferred deferred = nullptr;    // 用于Promise的resolve、reject处理
    napi_ref callback = nullptr;         // 回调函数
    //0是led灯的哪个接口，1是开关状态
    int args[2] = {0};                   // 2个输入参数
    int result = 0;                      // 业务逻辑处理结果（返回值）
};

// 业务逻辑处理函数，由worker线程池调度执行。
static void setLedStatusExecuteCB(napi_env env, void *data)
{
    LedAddOnData *addOnData = (LedAddOnData *)data;
    int gpioNum = addOnData->args[0];
    int bExport = UM_GPIO_EXPORTED;
    int direction = UM_GPIO_DIRECTION_OUT;
    int getValue = -1;

    UM_GPIO_IsExport(gpioNum, &getValue);
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpioNum, bExport);
    }

    UM_GPIO_SetDirection(gpioNum, direction);
    addOnData->result = UM_GPIO_SetValue(gpioNum, addOnData->args[1]);
}

static void getLedStatusExecuteCB(napi_env env, void *data)
{
    LedAddOnData *addOnData = (LedAddOnData *)data;
    int gpioNum = addOnData->args[0];
    int getValue = -1;

    UM_GPIO_GetValue(gpioNum, &getValue);
    addOnData->result = getValue;
}

// 业务逻辑处理完成回调函数，在业务逻辑处理函数执行完成或取消后触发，由EventLoop线程中执行。
static void completeCBForCallback(napi_env env, napi_status status, void *data)
{
    LedAddOnData *addOnData = (LedAddOnData *)data;
    napi_value callback = nullptr;
    napi_get_reference_value(env, addOnData->callback, &callback);
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);
    napi_value result = nullptr;
    napi_create_int32(env, addOnData->result, &result);

    // 执行回调函数
    napi_value returnVal = nullptr;
    napi_call_function(env, undefined, callback, 1, &result, &returnVal);

    // 删除napi_ref对象
    if (addOnData->callback != nullptr) {
        napi_delete_reference(env, addOnData->callback);
    }

    // 删除异步工作项
    napi_delete_async_work(env, addOnData->asyncWork);
    delete addOnData;
}

static napi_value setLedStatusWithCallback(napi_env env, napi_callback_info info)
{
    // 获取3个参数，值的类型是js类型（napi_value）
    size_t argc = 3;
    napi_value args[3];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete中传递数据
    auto addonData = new LedAddOnData {
        .asyncWork = nullptr,
    };

    // 将接收到的参数传入用户自定义的上下文数据
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &addonData->args[0]));
    NAPI_CALL(env, napi_get_value_int32(env, args[1], &addonData->args[1]));
    NAPI_CALL(env, napi_create_reference(env, args[2L], 1, &addonData->callback));

    // 创建async work，创建成功后通过最后一个参数接受async work的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "setLedStatusWithCallback", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, setLedStatusExecuteCB, completeCBForCallback, (void *)addonData,
                           &addonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    // 原生方法返回空对象
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

static napi_value getLedStatusWithCallback(napi_env env, napi_callback_info info)
{
    // 获取2个参数，值的类型是js类型（napi_value）
    size_t argc = 2;
    napi_value args[2];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete中传递数据
    auto addonData = new LedAddOnData {
        .asyncWork = nullptr,
    };

    // 将接收到的参数传入用户自定义的上下文数据
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &addonData->args[0]));
    NAPI_CALL(env, napi_create_reference(env, args[1], 1, &addonData->callback));

    // 创建async work，创建成功后通过最后一个参数接受async work的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "getLedStatusWithCallback", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, getLedStatusExecuteCB, completeCBForCallback, (void *)addonData,
                           &addonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    // 原生方法返回空对象
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

static void completeCBForPromise(napi_env env, napi_status status, void *data)
{
    LedAddOnData *addOnData = (LedAddOnData *)data;
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

static napi_value setLedStatusWithPromise(napi_env env, napi_callback_info info)
{
    // 获取2个参数，值的类型是js类型(napi_value)
    size_t argc = 2;
    napi_value args[2];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto addonData = new LedAddOnData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };

    // 将被收到的参数传入
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &addonData->args[0]));
    NAPI_CALL(env, napi_get_value_int32(env, args[1], &addonData->args[1]));

    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "setLedStatusWithPromise", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, setLedStatusExecuteCB, completeCBForPromise, (void *)addonData,
                           &addonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    // 返回promise
    return promise;
}

static napi_value getLedStatusWithPromise(napi_env env, napi_callback_info info)
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
    auto addonData = new LedAddOnData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };

    // 将被收到的参数传入
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &addonData->args[0]));

    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "getLedStatusWithPromise", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, getLedStatusExecuteCB, completeCBForPromise, (void *)addonData,
                           &addonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    // 返回promise
    return promise;
}

static napi_value setLedStatus(napi_env env, napi_callback_info info)
{
    // 获取3个参数，值的类型是js类型（napi_value）
    size_t argc = 3;
    napi_value args[3];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    if (argc == 2L) {
        // 创建promise
        napi_value promise = nullptr;
        napi_deferred deferred = nullptr;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

        // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
        auto addonData = new LedAddOnData {
            .asyncWork = nullptr,
            .deferred = deferred,
        };

        // 将被收到的参数传入
        NAPI_CALL(env, napi_get_value_int32(env, args[0], &addonData->args[0]));
        NAPI_CALL(env, napi_get_value_int32(env, args[1], &addonData->args[1]));

        // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
        napi_value resourceName = nullptr;
        napi_create_string_utf8(env, "setLedStatusWithPromise", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(env, nullptr, resourceName, setLedStatusExecuteCB, completeCBForPromise,
                               (void *)addonData, &addonData->asyncWork);

        // 将刚创建的async work加到队列，由底层去调度执行
        napi_queue_async_work(env, addonData->asyncWork);

        // 返回promise
        return promise;
    } else {
        // 异步工作项上下文用户数据，传递到异步工作项的execute、complete中传递数据
        auto addonData = new LedAddOnData {
            .asyncWork = nullptr,
        };

        // 创建async work，创建成功后通过最后一个参数接受async work的handle
        napi_value resourceName = nullptr;
        napi_create_string_utf8(env, "setLedStatusWithCallback", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(env, nullptr, resourceName, setLedStatusExecuteCB, completeCBForCallback,
                               (void *)addonData, &addonData->asyncWork);

        // 将刚创建的async work加到队列，由底层去调度执行
        napi_queue_async_work(env, addonData->asyncWork);

        // 原生方法返回空对象
        napi_value result = 0;
        NAPI_CALL(env, napi_get_null(env, &result));
        return result;
    }
}

static napi_value getLedStatus(napi_env env, napi_callback_info info)
{
    // 获取2个参数，值的类型是js类型（napi_value）
    size_t argc = 2;
    napi_value args[2];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    if (argc == 1) {
        // 创建promise
        napi_value promise = nullptr;
        napi_deferred deferred = nullptr;
        NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

        // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
        auto addonData = new LedAddOnData {
            .asyncWork = nullptr,
            .deferred = deferred,
        };

        // 将被收到的参数传入
        NAPI_CALL(env, napi_get_value_int32(env, args[0], &addonData->args[0]));

        // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
        napi_value resourceName = nullptr;
        napi_create_string_utf8(env, "getLedStatusWithPromise", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(env, nullptr, resourceName, getLedStatusExecuteCB, completeCBForPromise,
                               (void *)addonData, &addonData->asyncWork);

        // 将刚创建的async work加到队列，由底层去调度执行
        napi_queue_async_work(env, addonData->asyncWork);

        // 返回promise
        return promise;
    } else {
        // 异步工作项上下文用户数据，传递到异步工作项的execute、complete中传递数据
        auto addonData = new LedAddOnData {
            .asyncWork = nullptr,
        };

        // 创建async work，创建成功后通过最后一个参数接受async work的handle
        napi_value resourceName = nullptr;
        napi_create_string_utf8(env, "getLedStatusWithCallback", NAPI_AUTO_LENGTH, &resourceName);
        napi_create_async_work(env, nullptr, resourceName, getLedStatusExecuteCB, completeCBForCallback,
                               (void *)addonData, &addonData->asyncWork);

        // 将刚创建的async work加到队列，由底层去调度执行
        napi_queue_async_work(env, addonData->asyncWork);

        // 原生方法返回空对象
        napi_value result = 0;
        NAPI_CALL(env, napi_get_null(env, &result));
        return result;
    }
}

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

//同步方法获取温度值
static napi_value sync_get_temperature_value(napi_env env, napi_callback_info info)
{
    constexpr int adc_channel = 1;
    double temperature = 0;
    int value = 0;
    if (get_adc_data(adc_channel, &value) >= 0) {
        temperature = ((double)value) * TEMP_CONST;
        printf("adc:[%d]\n", value);
        printf("temperature:[%.1f]C\n", temperature);
    }

    napi_value ret;
    NAPI_CALL(env, napi_create_int32(env, temperature, &ret));
    return ret;
}


/*
 * 注册接口
 */
static napi_value registertemp_ledApis(napi_env env, napi_value exports)
{
    napi_value gpioValHigh = gpioValHigh;
    napi_value gpioValLow = nullptr;
    napi_create_int32(env, UM_GPIO_HIGH_LEVE, &gpioValHigh);
    napi_create_int32(env, UM_GPIO_LOW_LEVE, &gpioValLow);
    napi_property_descriptor desc[] = {
        // 声明函数供ets调用
        DECLARE_NAPI_FUNCTION("sync_get_temperature_value", sync_get_temperature_value),
        DECLARE_NAPI_FUNCTION("sync_get_adc_value", sync_get_adc_value),
        DECLARE_NAPI_FUNCTION("setLedStatusWithCallback", setLedStatusWithCallback),
        DECLARE_NAPI_FUNCTION("getLedStatusWithCallback", getLedStatusWithCallback),
        DECLARE_NAPI_FUNCTION("setLedStatusWithPromise", setLedStatusWithPromise),
        DECLARE_NAPI_FUNCTION("getLedStatusWithPromise", getLedStatusWithPromise),
        DECLARE_NAPI_FUNCTION("setLedStatus", setLedStatus),
        DECLARE_NAPI_FUNCTION("getLedStatus", getLedStatus),
        DECLARE_NAPI_STATIC_PROPERTY("LED_ON", gpioValHigh),
        DECLARE_NAPI_STATIC_PROPERTY("LED_OFF", gpioValLow),

    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}


/*
 * 模块定义
 */
static napi_module templedModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registertemp_ledApis,
    .nm_modname = "templed", // 模块名
    .nm_priv = ((void *)0),
    .reserved = {0},
};

/*
 * 注册模块
 */
extern "C" __attribute__((constructor)) void RegisterTempled_DemoModule(void)
{
    napi_module_register(&templedModule); // 接口注册函数
}