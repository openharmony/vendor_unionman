#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "um_gpio.h"

#define INFRARED_TRANSMIT_GPIO UM_GPIO_07
#define INFRARED_RECEIVE_GPIO UM_GPIO_08

struct infrared_Data {
    napi_async_work asyncWork = nullptr; // 异步工作项
    napi_deferred deferred = nullptr;    // 用于Promise的resolve、reject处理
    int gpioNum = 0;                     // GPIO引脚编号
    int signal = 0;                      // 要设置的信号电平
};

// 发送红外信号
static void sendInfraredSignalExecuteCB(napi_env env, void *data) {
    infrared_Data *addonData = (infrared_Data *)data;
    UM_GPIO_SetValue(addonData->gpioNum, addonData->signal);
}

// 接收红外信号
static void receiveInfraredSignalExecuteCB(napi_env env, void *data) {
    infrared_Data *addonData = (infrared_Data *)data;
    int getValue = -1;
    UM_GPIO_GetValue(addonData->gpioNum, &getValue);
    addonData->signal = getValue;
}

// 完成回调函数
static void completeCBForPromise(napi_env env, napi_status status, void *data) {
    infrared_Data *addonData = (infrared_Data *)data;
    napi_value result = nullptr;
    napi_create_int32(env, addonData->signal, &result);
    napi_resolve_deferred(env, addonData->deferred, result);

    // 删除异步工作项
    napi_delete_async_work(env, addonData->asyncWork);
    delete addonData;
}

// 发送红外信号函数
static napi_value sendInfraredSignalWithPromise(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    auto addonData = new infrared_Data {
        .asyncWork = nullptr,
        .deferred = deferred,
        .gpioNum = INFRARED_TRANSMIT_GPIO,
    };

    NAPI_CALL(env, napi_get_value_int32(env, args[0], &addonData->signal));

    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "sendInfraredSignalWithPromise", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, sendInfraredSignalExecuteCB, completeCBForPromise, (void *)addonData, &addonData->asyncWork);
    napi_queue_async_work(env, addonData->asyncWork);

    return promise;
}

// 接收红外信号函数
static napi_value receiveInfraredSignalWithPromise(napi_env env, napi_callback_info info) {
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    auto addonData = new infrared_Data {
        .asyncWork = nullptr,
        .deferred = deferred,
        .gpioNum = INFRARED_RECEIVE_GPIO,
    };

    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "receiveInfraredSignalWithPromise", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, receiveInfraredSignalExecuteCB, completeCBForPromise, (void *)addonData, &addonData->asyncWork);
    napi_queue_async_work(env, addonData->asyncWork);

    return promise;
}

// 注册接口
static napi_value registerMyNapi_InfraredApis(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("sendInfraredSignalWithPromise", sendInfraredSignalWithPromise),
        DECLARE_NAPI_FUNCTION("receiveInfraredSignalWithPromise", receiveInfraredSignalWithPromise),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

// 模块定义
static napi_module mynapiinfraredModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registerMyNapi_InfraredApis,
    .nm_modname = "mynapiinfrared", // 模块名
    .nm_priv = ((void *)0),
    .reserved = {0},
};

// 注册模块
extern "C" __attribute__((constructor)) void RegisterMyNapiInfraredModule(void) {
    napi_module_register(&mynapiinfraredModule);
}