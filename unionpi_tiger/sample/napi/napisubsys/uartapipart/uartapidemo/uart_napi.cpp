/*
 * Copyright (c) 2023 Unionman Technology Co., Ltd.
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

#include <cstring>
#include <mutex>
#include <thread>
#define NAPI_TEST 1
#include <napi/native_api.h>
#include <napi/native_node_api.h>
#include "uart/uart_ctl.h"

static napi_threadsafe_function touchCallback = nullptr;
static std::mutex callbackMutex;
static Uart::UartCtl* uart1;

static void readTouchLoop()
{
    while (true) {
        int32_t res = uart1->readTouch();
        if (res != -1) {
            uint8_t* codePtr = new uint8_t;
            *codePtr = static_cast<uint8_t>(res);
            callbackMutex.lock();
            if (touchCallback != nullptr) {
                napi_call_threadsafe_function(touchCallback, codePtr, napi_tsfn_nonblocking);
            }
            callbackMutex.unlock();
        }
    }
}

static void callfunc(napi_env env, napi_value js_callback, void* context, void* data)
{
    // 将*data转换为对应的类型
    uint8_t* codePtr = reinterpret_cast<uint8_t*>(data);
    // 从content中取出指向js回调函数的引用
    napi_ref funRef = reinterpret_cast<napi_ref>(context);
    napi_value code, result, global, fun;
    napi_get_global(env, &global);
    // 将事件码转换为js变量
    napi_create_uint32(env, *codePtr, &code);
    // 取出引用指向的js回调函数
    napi_get_reference_value(env, funRef, &fun);
    delete codePtr;
    // 调用js回调函数
    napi_call_function(env, global, fun, 1, &code, &result);
}

static napi_value onTouch(napi_env env, napi_callback_info cb)
{
    napi_value ret;
    napi_get_undefined(env, &ret);
    // 获取js传来的参数
    size_t argc = 1;
    napi_value argv;
    napi_get_cb_info(env, cb, &argc, &argv, nullptr, nullptr);
    // 校验参数数量
    NAPI_ASSERT(env, argc == 1, "requires 1 parameter");
    // 校验参数类型
    napi_valuetype type;
    napi_typeof(env, argv, &type);
    NAPI_ASSERT(env, type == napi_function, "frist parameter type requires callback");
    // 创建传来的回调函数的引用
    napi_ref funRef;
    napi_create_reference(env, argv, 1, &funRef);
    // 创建资源和资源名
    napi_value resource, resourceName;
    napi_get_null(env, &resource);
    napi_create_string_latin1(env, "touchCallback", NAPI_AUTO_LENGTH, &resourceName);
    callbackMutex.lock();
    if (touchCallback != nullptr) {
        // 如果先前设置过回调函数，释放先前回调函数的相关资源
        napi_ref funRef;
        napi_get_threadsafe_function_context(touchCallback, (void**)&funRef);
        napi_delete_reference(env, funRef);
        napi_release_threadsafe_function(touchCallback, napi_tsfn_release);
    }
    // 创建threadsafe_function
    napi_create_threadsafe_function(env, argv, resource, resourceName, 0, 1,
        nullptr, nullptr, funRef, callfunc, &touchCallback);
    callbackMutex.unlock();
    return ret;
}

static napi_value closeTouch(napi_env env, napi_callback_info cb)
{
    napi_value ret;
    napi_get_undefined(env, &ret);
    callbackMutex.lock();
    if (touchCallback != nullptr) {
        napi_ref funRef;
        napi_get_threadsafe_function_context(touchCallback, reinterpret_cast<void**>(&funRef));
        napi_delete_reference(env, funRef);
        napi_release_threadsafe_function(touchCallback, napi_tsfn_release);
    }
    touchCallback = nullptr;
    callbackMutex.unlock();
    return ret;
}

static napi_value defineEventCode(napi_env env)
{
    using namespace Uart;
    napi_value eventCode, right, left, back, forward, pullUp, pullDown, pullMove,
        touch1, touch2, touch3, touch4, touch5;
    // 创建EventCode对象
    napi_create_object(env, &eventCode);
    // 用c++的常量创建对应的js变量
    napi_create_uint32(env, static_cast<uint32_t>(eventCode::right), &right);
    napi_create_uint32(env, static_cast<uint32_t>(eventCode::left), &left);
    napi_create_uint32(env, static_cast<uint32_t>(eventCode::back), &back);
    napi_create_uint32(env, static_cast<uint32_t>(eventCode::forward), &forward);
    napi_create_uint32(env, static_cast<uint32_t>(eventCode::pullUp), &pullUp);
    napi_create_uint32(env, static_cast<uint32_t>(eventCode::pullDown), &pullDown);
    napi_create_uint32(env, static_cast<uint32_t>(eventCode::pullMove), &pullMove);
    napi_create_uint32(env, static_cast<uint32_t>(eventCode::touch1), &touch1);
    napi_create_uint32(env, static_cast<uint32_t>(eventCode::touch2), &touch2);
    napi_create_uint32(env, static_cast<uint32_t>(eventCode::touch3), &touch3);
    napi_create_uint32(env, static_cast<uint32_t>(eventCode::touch4), &touch4);
    napi_create_uint32(env, static_cast<uint32_t>(eventCode::touch5), &touch5);
    // 将js变量设置为对象的静态成员
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("right", right),
        DECLARE_NAPI_STATIC_PROPERTY("left", left),
        DECLARE_NAPI_STATIC_PROPERTY("back", back),
        DECLARE_NAPI_STATIC_PROPERTY("forward", forward),
        DECLARE_NAPI_STATIC_PROPERTY("pullUp", pullUp),
        DECLARE_NAPI_STATIC_PROPERTY("pullDown", pullDown),
        DECLARE_NAPI_STATIC_PROPERTY("pullMove", pullMove),
        DECLARE_NAPI_STATIC_PROPERTY("touch1", touch1),
        DECLARE_NAPI_STATIC_PROPERTY("touch2", touch2),
        DECLARE_NAPI_STATIC_PROPERTY("touch3", touch3),
        DECLARE_NAPI_STATIC_PROPERTY("touch4", touch4),
        DECLARE_NAPI_STATIC_PROPERTY("touch5", touch5),
    };
    napi_define_properties(env, eventCode, sizeof(desc) / sizeof(desc[0]), desc);
    // 返回枚举对象
    return eventCode;
}

static napi_value registerUartCtl_Apis(napi_env env, napi_value exports)
{
    uart1 = new Uart::UartCtl(1);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("EventCode", defineEventCode(env)),
        DECLARE_NAPI_FUNCTION("onTouch", onTouch),
        DECLARE_NAPI_FUNCTION("closeTouch", closeTouch)
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    // 注册清理回调，napi退出时删除所有资源
    NAPI_CALL(env, napi_add_env_cleanup_hook(env, [](void* arg) {
        delete uart1;
        uart1 = nullptr;
    }, nullptr));
    std::thread thread1(readTouchLoop);
    thread1.detach();
    return exports;
}

NAPI_MODULE(uartapipart, registerUartCtl_Apis)