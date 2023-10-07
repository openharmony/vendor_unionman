/* Copyright 2023 Unionman Technology Co., Ltd.
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

constexpr int baud_rate = 9600;
static int fd = -1;
static int init_flag = 0;
constexpr int OPENERR = -1;
constexpr int INITERR = -2;
constexpr int FRAME_LEN = 4;
const char UART_TTL_NAME[] = "/dev/ttyS1";

struct UartData {
    napi_async_work asyncWork = nullptr;
    napi_ref callback = nullptr;
    std::string result = "";
};

static napi_value uart_init_napi(napi_env env, napi_callback_info info)
{
    napi_value ret = nullptr;
    NAPI_CALL(env, napi_create_int32(env, 0, &ret));
    if (init_flag) {
        HILOG_INFO(LOG_CORE, "already init");
        return ret;
    }

    const char *uart_dev = UART_TTL_NAME;
    fd = open(uart_dev, O_RDWR);
    if (fd == -1) {
        HILOG_DEBUG(LOG_CORE, "open file error");
        NAPI_CALL(env, napi_create_int32(env, OPENERR, &ret));
        return ret;
    }
    if ((uart_init(fd, baud_rate)) == -1) {
        close(fd);
        fd = -1;
        HILOG_DEBUG(LOG_CORE, "uart init error");
        NAPI_CALL(env, napi_create_int32(env, INITERR, &ret));
        return ret;
    }
    init_flag = 1;

    return ret;
}

static napi_value uart_close_napi(napi_env env, napi_callback_info info)
{
    napi_value ret = nullptr;
    NAPI_CALL(env, napi_create_int32(env, 0, &ret));
    if (!init_flag) {
        HILOG_INFO(LOG_CORE, "Not initialized yet");
        return ret;
    }
    close(fd);
    fd = -1;
    init_flag = 0;
    return ret;
}

// 从串口读的线程
static std::string uart_task(void)
{
    int i = 0;
    int ret = -1;
    int buf = 0;
    int recv[4] = {0};

    printf("Gesture Sensor Ready!\n");
    for (i = 0; i < FRAME_LEN; i++) {
        ret = read(fd, &buf, 1);
        if (ret == -1) {
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

// 业务逻辑处理函数，由worker线程池调度执行。
static void uartExecuteCB(napi_env env, void *data)
{
    UartData *uartData = (UartData *)data;
    // 执行复杂计算，不阻塞主线程。
    uartData->result=uart_task();
}

static void uartCallbackCompleteCB(napi_env env, napi_status status, void *data)
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

static napi_value uartreadCallback(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));
    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete中传递数据
    auto uartData = new UartData {
        .asyncWork = nullptr,
    };
    NAPI_CALL(env, napi_create_reference(env, args[0], 1, &uartData->callback));
    // 创建async work，创建成功后通过最后一个参数接收async work的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "uartreadCallback", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, uartExecuteCB, uartCallbackCompleteCB, (void *)uartData,
                           &uartData->asyncWork);
    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, uartData->asyncWork);
    // 原生方法返回空对象
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

static napi_value registerUartnapi(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("uart_init_napi", uart_init_napi),
        DECLARE_NAPI_FUNCTION("uart_close_napi", uart_close_napi),
        DECLARE_NAPI_FUNCTION("uartreadCallback", uartreadCallback)
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

/*
 * 模块定义
 */
static napi_module uartnapiModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registerUartnapi,
    .nm_modname = "uart_napi",  // 模块名
    .nm_priv = ((void *) 0),
    .reserved = {0},
};

/*
 * 注册模块 __attribute__标记用于在加载库时自动注册
 */
extern "C" __attribute__((constructor)) void RegisterUartnapiModule(void)
{
    napi_module_register(&uartnapiModule);  // 接口注册函数
}