#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "um_gpio.h"

struct LedAddOnData {
    napi_async_work asyncWork = nullptr; // 异步工作项
    napi_deferred deferred = nullptr;    // 用于Promise的resolve、reject处理
    napi_ref callback = nullptr;         // 回调函数
    int args[2] = {0};                   // 2个输入参数
    int result = 0;                      // 业务逻辑处理结果（返回值）
};

static napi_value add(napi_env env, napi_callback_info info)
{
    //1、获得js传入参数
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env,info,&argc,args,nullptr,nullptr));

    //2、验证参数
    //2.1 验证参数个数
    NAPI_ASSERT(env,argc>=2,"wrong number of arguments");

    //2.2 验证参数类型
    napi_valuetype valuetype0;
    NAPI_CALL(env, napi_typeof(env, args[0], &valuetype0));
    napi_valuetype valuetype1;
    NAPI_CALL(env, napi_typeof(env, args[1], &valuetype1));
    NAPI_ASSERT(env,valuetype0 == napi_number && valuetype1 == napi_number,"wrong arguments type.");

    //3 napi数据类型转换为C/C++类型
    double value0;
    NAPI_CALL(env,napi_get_value_double(env, args[0], &value0));

    double value1;
    NAPI_CALL(env,napi_get_value_double(env, args[1], &value1));

    //4 业务逻辑
    double sum = value0 + value1;

    //5 将结果数据类型从C/C++转换为napi类型

    napi_value ret;
    NAPI_CALL(env, napi_create_double(env, sum,&ret));
    return ret;
}

// 业务逻辑处理函数，由worker线程池调度执行。-----------------3--------------------Promise和callback都用的是这个
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

// 业务逻辑处理完成回调函数，在业务逻辑处理函数执行完成或取消后触发，由EventLoop线程中执行。------------2---------------
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
    napi_call_function(env, undefined, callback, 1, &result, &returnVal);//根据ArkTs的调用方法进行返回

    // 删除napi_ref对象，释放内存
    if (addOnData->callback != nullptr) {
        napi_delete_reference(env, addOnData->callback);
    }

    // 删除异步工作项，释放内存
    napi_delete_async_work(env, addOnData->asyncWork);
    delete addOnData;
}
//-----------------1--------------------------
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
//------------------------Promise---------------------------
static void completeCBForPromise(napi_env env, napi_status status, void *data)//------------------2----------------删除promise对象
{
    LedAddOnData *addOnData = (LedAddOnData *)data;
    napi_value result = nullptr;
    napi_create_int32(env, addOnData->result, &result);
    napi_resolve_deferred(env, addOnData->deferred, result);//promise独有的deferred

    // 删除napi_ref对象
    if (addOnData->callback != nullptr) {
        napi_delete_reference(env, addOnData->callback);
    }

    // 删除异步工作项
    napi_delete_async_work(env, addOnData->asyncWork);
    delete addOnData;
}

static napi_value setLedStatusWithPromise(napi_env env, napi_callback_info info)//---------------1------------------
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

    // 将被收到的参数传入，此处与callback方式不同，少了一行
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

static napi_value setLedStatus(napi_env env, napi_callback_info info)//------------callback和promisee归一
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


/*
 * 注册接口
 */
static napi_value registerMyNapi_DemoApis(napi_env env, napi_value exports)
{
    napi_value gpioValHigh = gpioValHigh;
    napi_value gpioValLow = nullptr;
    napi_create_int32(env, UM_GPIO_HIGH_LEVE, &gpioValHigh);
    napi_create_int32(env, UM_GPIO_LOW_LEVE, &gpioValLow);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("add", add),
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
static napi_module mynapidemoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registerMyNapi_DemoApis,
    .nm_modname = "mynapidemo", // 模块名
    .nm_priv = ((void *)0),
    .reserved = {0},
};

/*
 * 注册模块
 */
extern "C" __attribute__((constructor)) void RegisterMyNapiDemoModule(void)
{
    napi_module_register(&mynapidemoModule); // 接口注册函数
}