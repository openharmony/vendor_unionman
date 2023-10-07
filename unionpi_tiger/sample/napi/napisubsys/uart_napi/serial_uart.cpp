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
#include "serial_uart.h"

#include <termios.h>

constexpr int RECV_HEAD = 0xaa;
constexpr int RECV_END = 0x55;
constexpr int EVT_RIGHT = 0x01;
constexpr int EVT_LEFT = 0x02;
constexpr int EVT_BACK = 0x03;
constexpr int EVT_FORWARD = 0x04;
constexpr int EVT_PULLUP = 0x05;
constexpr int EVT_PULLDOWN = 0x06;
constexpr int EVT_PULLREMOVE = 0x07;
constexpr int EVT_TOUCH1 = 0x21;
constexpr int EVT_TOUCH2 = 0x22;
constexpr int EVT_TOUCH3 = 0x23;
constexpr int EVT_TOUCH4 = 0x24;
constexpr int EVT_TOUCH5 = 0x25;
constexpr int START = 0;
constexpr int DATA = 1;
constexpr int CHECK = 2;
constexpr int END = 3;

// 设置串行端口的波特率
static void set_baud(int fd, int baud)
{
    int ret = -1;
    struct termios opt; // 用于描述和控制终端设备的各种属性，如波特率、字符大小、停止位等。

    tcgetattr(fd, &opt); // tcgetattr用来获取终端参数，将从终端获得的信息fd，保存到opt结构体中(这个函数会根据文件描述符（fd）所代表的终端设备，从操作系统内核的终端驱动程序中获取相应的参数信息。)
    tcflush(fd, TCIOFLUSH); // 清空缓冲区
    cfsetispeed(&opt, baud);    // 源码应该是//third/musl/src/termios/cfsetospeed.c里那个
    cfsetospeed(&opt, baud);    // 同上

    ret = tcsetattr(fd, TCSANOW, &opt); //  将配置信息对象应用到指定的串行端口，TCSANOW表示立即生效
    if (ret == -1) {
        perror("tcsetattr fd");
        return;
    }

    tcflush(fd, TCIOFLUSH); // 清空缓冲区
}

// 根据输入的数据位数量设置相应的标志位，用于配置串行端口的数据位
static int setup_data_bits(int setup_databits, struct termios *options_databits)
{
    switch (setup_databits) {
        case 5L:
            options_databits->c_cflag |= CS5; // 按位或运算，只修改对应标志位，其他的保持不变
            break;
        case 6L:
            options_databits->c_cflag |= CS6;
            break;
        case 7L:
            options_databits->c_cflag |= CS7;
            break;
        case 8L:
            options_databits->c_cflag |= CS8;
            break;
        default:
            return -1;
    }
    return 1;
}

// 设置校验位
static int set_params_parity(int setup_parity, struct termios *options_parity)
{
    switch (setup_parity) {
        case 'n':
        case 'N':                               // 无奇偶校验位
            options_parity->c_cflag &= ~PARENB; // 清除PARENB位
            options_parity->c_iflag &= ~INPCK;  // 禁用输入奇偶校验检查
            break;

        case 'o':
        case 'O':                                         // 设置为奇校验
            options_parity->c_cflag |= (PARODD | PARENB); // 开启奇偶校验并把奇校验开启
            options_parity->c_iflag |= INPCK;             // 启用输入奇偶校验检查
            break;

        case 'e':
        case 'E':                               // 设置为偶校验
            options_parity->c_cflag |= PARENB;
            options_parity->c_cflag &= ~PARODD;
            options_parity->c_iflag |= INPCK;
            break;

        case 'M':
        case 'm': // 指示使用奇偶校验位作为标记位（奇校验）
            options_parity->c_cflag |= PARENB | CMSPAR | PARODD;
            options_parity->c_iflag |= INPCK; // enable parity checking /
            break;

        case 'S':
        case 's': // 指示使用奇偶校验位作为标记位（偶校验）
            options_parity->c_cflag |= PARENB | CMSPAR;
            options_parity->c_cflag &= ~PARODD;
            options_parity->c_iflag |= INPCK; // enable parity checking /
            break;

        default:
            return -1;
    }
    return 1;
}

// 设置串行端口的参数
static int set_params(int fd, int databits, int stopbits, int parity)
{
    struct termios options;
    int ret = 0;

    if (tcgetattr(fd, &options) != 0) {
        perror("tcgetattr fail");
        return -1;
    }
    // 禁用任何特殊的输入和输出处理
    options.c_iflag = 0;
    options.c_oflag = 0;

    // setup data bits
    options.c_cflag &= ~CSIZE;
    ret = setup_data_bits(databits, &options);
    if (ret == -1) {
        return -1;
    }

    // 配置校验选项
    ret = set_params_parity(parity, &options);
    if (ret == -1) {
        return -1;
    }

    // stop bits/
    switch (stopbits) {
        // CSTOPB对应位0表示一位，1表示两位
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;

        case 2L:
            options.c_cflag |= CSTOPB;
            break;

        default:
            return -1;
    }

    // 请求发送和清除发送
    options.c_cflag &= ~CRTSCTS; // 禁用硬件控制流（一种对接收双方之间数据传输速度匹配的控制机制）
    options.c_lflag = 0; // 禁用本地模式选项
    options.c_cc[VTIME] = 10; // 读取数据时，如果在该值/10秒内没有数据到达则返回
    options.c_cc[VMIN] = 1; // 最小读取字符，设置至少读取一个字符后才会返回

    tcflush(fd, TCIFLUSH);
    // 将新的参数应用到串口设备上
    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        return -1;
    }

    return 0;
}

// 转换波特率为对应的枚举值
static speed_t conver_baudrate(int baudrate)
{
    switch (baudrate) {
        case 9600L:
            return B9600;
        case 19200L:
            return B19200;
        case 38400L:
            return B38400;
        case 115200L:
            return B115200;
        case 1152000L:
            return B1152000;
        default:
            return B1152000;
    }
}

// uart配置
int uart_init(int fd, int uartBaud)
{
    set_baud(fd, conver_baudrate(uartBaud));
    // uart param /
    if (set_params(fd, 8L, 1, 'n')) {
        printf("Set uart parameters fail.\n");
        return -1;
    }
    return 0;
}

// 传感器数据处理
std::string data_proce(int *recv)
{
    std::string ret;
    if (recv == nullptr) {
        printf("data proce error");
    }
    
    if ((recv[START] == RECV_HEAD) && (recv[END] == RECV_END) && (recv[DATA] == (0xff - recv[CHECK]))) {
        switch (recv[DATA]) {
            case EVT_BACK:
                ret="手势识别为向后";
                break;
            case EVT_FORWARD:
                ret="手势识别为向前";
                break;
            case EVT_RIGHT:
                ret="手势识别为向右";
                break;
            case EVT_LEFT:
                ret="手势识别为向左";
                break;
            case EVT_PULLUP:
                ret="手势识别为上拉";
                break;
            case EVT_PULLDOWN:
                ret="手势识别为下压";
                break;
            case EVT_PULLREMOVE:
                ret="手势识别为上拉下压后松开";
                break;
            case EVT_TOUCH1:
                ret="触摸点1";
                break;
            case EVT_TOUCH2:
                ret="触摸点2";
                break;
            case EVT_TOUCH3:
                ret="触摸点3";
                break;
            case EVT_TOUCH4:
                ret="触摸点4";
                break;
            case EVT_TOUCH5:
                ret="触摸点5";
                break;
            default:
                ret="数据错误";
                break;
        }
        return ret;
    }
    ret="错误";
    return ret;
}