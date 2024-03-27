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
#include <unistd.h>
#include <cstring>
#include <string>

#include <sys/epoll.h>
#include <sys/select.h>
#include "serial_uart.h"

#include "hilog/log.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

static int g_fd = -1;
static int g_initFlag = 0;
constexpr int OPENERR = -1;
constexpr int INITERR = -2;
constexpr int FRAME_LEN = 4;
const char UART_TTL_NAME[] = "/dev/ttyS1";
/*
 *0. 定义异步线程执行中需要的上下文环境
 */
struct UartData
{
    // 异步 worker
    napi_async_work asyncWork = nullptr;
    // 对应JS端的callback函数
    napi_ref callback = nullptr;
    // 返回的结果
    std::string result = "";
};

/*
 * 4.UartInit具体方法实现，同步方法
 */
static napi_value UartInit(napi_env env, napi_callback_info info)
{
    // c/c++的类型转化为napi_value,ret=0
    napi_value ret = nullptr;
    NAPI_CALL(env, napi_create_int32(env, 0, &ret));
    if (g_initFlag) {
        HILOG_INFO(LOG_CORE, "already init");
        return ret;
    }
    // 打开串口
    const char *uartDev = UART_TTL_NAME;
    g_fd = open(uartDev, O_RDWR);
    if (g_fd == -1) {
        HILOG_DEBUG(LOG_CORE, "open file error");
        // 打开文件失败返回错误码-1
        NAPI_CALL(env, napi_create_int32(env, OPENERR, &ret)); 
        return ret;
    }
    if ((UartInit(g_fd)) == -1) {
        close(g_fd);
        g_fd = -1;
        HILOG_DEBUG(LOG_CORE, "uart init error");
        // 串口初始化失败则返回错误码-2
        NAPI_CALL(env, napi_create_int32(env, INITERR, &ret)); 
        return ret;
        return ret;
    }
    g_initFlag = 1;

    return ret;
}

/*
 *8. UartClose同步NAPI实现方法，关闭串口
 */
static napi_value UartClose(napi_env env, napi_callback_info info)
{
    napi_value ret = nullptr;
    NAPI_CALL(env, napi_create_int32(env, 0, &ret));
    if (!g_initFlag) {
        HILOG_INFO(LOG_CORE, "Not initialized yet");
        return ret;
    }
    close(g_fd);
    g_fd = -1;
    g_initFlag = 0;
    return ret;
}

/*
 *6.5 从串口读的线程
 */
static std::string uart_task(void)
{
    int i = 0;
    int ret = -1;
    int buf = 0;
    int recv[4] = {0};

    printf("Gesture Sensor Ready!\n");
    for (i = 0; i < FRAME_LEN; i++)
    {
        ret = read(g_fd, &buf, 1);
        if (ret == -1){
            printf("read err\n");
            exit(0);
        }
        recv[i] = buf;
    }
    std::string result = data_proce(recv);
    if (result == "错误") {
        printf("data process error\n");
        exit(0);
    }
    return result;
}

/*
 *6.uartdoInBackground()异步执行的具体内容
 */
// 业务逻辑处理函数，由worker线程池调度执行。
static void uartExecuteCB(napi_env env, void *data)
{
    UartData *uartData = (UartData *)data;
    // 执行复杂计算，不阻塞主线程。
    uartData->result = uart_task();
}

/*
 *7.UartCallBackComplete()异步任务完成后回到主线程中执行
 */
static void UartCallBackCompleteCB(napi_env env, napi_status status, void *data)
{
    UartData *uartData = (UartData *)data;
    napi_value callback = nullptr;
    napi_get_reference_value(env, uartData->callback, &callback);
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);
    napi_value callbackResult = nullptr;
    napi_value result = nullptr;
    const char *tmp = uartData->result.c_str();
    napi_create_string_utf8(env, tmp, NAPI_AUTO_LENGTH, &result);
    // 执行回调函数
    napi_call_function(env, undefined, callback, 1, &result, &callbackResult);
    // 删除napi_ref对象
    if (uartData->callback != nullptr) {
        napi_delete_reference(env, uartData->callback);
    }
    // 删除异步工作项
    napi_delete_async_work(env, uartData->asyncWork);
    delete uartData;
}

/*
 *5.UartCallBack具体方法实现
 解释：首先从 napi_callback_info 中读取 napi_value 类型的参数放入到 args 中，然后从 args 中读取参数并把 napi_value 类型转换成 C++ 类型后进行加操作
        env:环境
*/
static napi_value UartCallBack(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value thisArg = nullptr;
    // 调用napi_get_cb_info方法，从 info 中读取传递进来的参数放入args里
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));
    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete中传递数据
    auto uartData = new UartData{
        .asyncWork = nullptr,
    };
    // call back的回调方式，需要创建call back的引用

    NAPI_CALL(env, napi_create_reference(env, args[0], 1, &uartData->callback));
    // 创建async work，创建成功后通过最后一个参数接收async work的handle
    napi_value resourceName = nullptr;
    // 创建一个异步任务,
    // napi_create_async_work() 方法的第 3 、 4 个参数需要注意，uartdoInBackground() 方法是在异步线程中执行的，UartCallBackComplete() 方法在异步线程结束后切换到主线程中执行。
    napi_create_string_utf8(env, "UartCallBack", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, uartExecuteCB, UartCallBackCompleteCB, (void *)uartData,
                           &uartData->asyncWork);
    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, uartData->asyncWork);
    // 原生方法返回空对象
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

/*
 * 3.方法映射,北向→南向
 */
static napi_value registerUartnapi(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("uart_init_napi", UartInit),
        DECLARE_NAPI_FUNCTION("uart_close_napi", UartClose),
        DECLARE_NAPI_FUNCTION("uartcallback", UartCallBack)};
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

/*
 * 1.napi模块定义
 */
static napi_module uartnapiModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registerUartnapi,
    .nm_modname = "uartnapi", // 模块名,要与BUILD.gn里的一样
    .nm_priv = ((void *)0),
    .reserved = {0},
};

/*
 * 2.注册模块 __attribute__标记用于在加载库时自动注册
 */
extern "C" __attribute__((constructor)) void RegisterUartnapiModule(void)
{
    napi_module_register(&uartnapiModule); // 接口注册函数
}