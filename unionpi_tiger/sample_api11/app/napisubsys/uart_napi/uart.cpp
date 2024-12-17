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
static int epfd = -1;
static int init_flag = 0;
constexpr int OPENERR = -1;
constexpr int INITERR = -2;
constexpr int FRAME_LEN = 4;
const char UART_TTL_NAME[] = "/dev/ttyS1";

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

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    epfd = epoll_create(1);
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

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
    close(epfd);
    epfd = -1;
    close(fd);
    fd = -1;
    init_flag = 0;
    return ret;
}

static std::string dataProcecc(int data)
{
    std::string sensor_respond;
        switch (data) {
            case 0x01:
                sensor_respond="手势识别为向右";
                break;
            case 0x02:
                sensor_respond="手势识别为向左";
                break;
            case 0x03:
                sensor_respond="手势识别为向后";
                break;
            case 0x04:
                sensor_respond="手势识别为向前";
                break;
            case 0x05:
                sensor_respond="手势识别为上拉";
                break;
            case 0x06:
                sensor_respond="手势识别为下压";
                break;
            case 0x07:
                sensor_respond="手势识别为上拉下压后松开";
                break;
            case 0x21:
                sensor_respond="触摸点1";
                break;
            case 0x22:
                sensor_respond="触摸点2";
                break;
            case 0x23:
                sensor_respond="触摸点3";
                break;
            case 0x24:
                sensor_respond="触摸点4";
                break;
            case 0x25:
                sensor_respond="触摸点5";
                break;
            default:
                sensor_respond="数据错误";
                break;
        }
        return sensor_respond;
}

static bool dataRec(int *data)
{
    for (int i = 0; i < FRAME_LEN; i++) {
        if ((read(fd, (data + i), 1)) == -1) {
            HILOG_DEBUG(LOG_CORE, "Read err, read() failed");
            return false;
        }
    }
    return true;
}


static napi_value uart_read_napi(napi_env env, napi_callback_info info)
{
    napi_value respond_napi = nullptr;
    std::string sensor_respond;
    if (!init_flag) {
        HILOG_DEBUG(LOG_CORE, "Read error, uart not initialized");
        sensor_respond="notinit";
        NAPI_CALL(env, napi_create_string_utf8(env, sensor_respond.c_str(), sensor_respond.length(),
                                               &respond_napi));
        return respond_napi;
    }
    struct epoll_event event_list[1];
    int nfds = 0; // number of file descriptors
    int recv[4] = {0};
    sensor_respond="nodata";
    NAPI_CALL(env, napi_create_string_utf8(env, sensor_respond.c_str(), sensor_respond.length(), &respond_napi));
    if ((nfds = epoll_wait(epfd, event_list, FRAME_LEN, 0))) {
        for (int i = 0; i < nfds; i++) {
            bzero(recv, sizeof(recv));
            if (!dataRec(recv)) {
                return respond_napi;
            }
        }
        sensor_respond = dataProcecc(recv[1]);
        NAPI_CALL(env, napi_create_string_utf8(env, sensor_respond.c_str(), sensor_respond.length(), &respond_napi));
        return respond_napi;
    }
    return respond_napi; // 没拿到数据返回"nodata"
}

static napi_value registerUartnapi(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("uart_init_napi", uart_init_napi),
        DECLARE_NAPI_FUNCTION("uart_close_napi", uart_close_napi),
        DECLARE_NAPI_FUNCTION("uart_read_napi", uart_read_napi)
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