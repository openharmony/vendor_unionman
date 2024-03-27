|
/*
 * Copyright (c) 2022 Unionman Technology Co., Ltd.
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
 */#include <cstdio>
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


std::string data_proce(int *recv)
{
    std::string ret;
    if (recv == NULL) {
        printf("data proce error");
    }
    
    if ((recv[0] == RECV_HEAD) && (recv[3L] == RECV_END) && (recv[1] == (0xff - recv[2L]))) {
        switch (recv[1]) {
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
        ret="错误";
        return ret;
    }
    ret="错误";
    return ret;
}

int uart_init(int fd)
{
    struct termios options;     // 创建串口参数结构体
    tcgetattr(fd, &options);    // 获取当前串口参数

    cfsetispeed(&options, B9600);   // 设置输入波特率为9600bps
    cfsetospeed(&options, B9600);   // 设置输出波特率为9600bps
    options.c_cflag |= (CLOCAL | CREAD);    // 本地连接，接收使能
    options.c_cflag &= ~PARENB;     // 禁用奇偶校验
    options.c_iflag &= ~INPCK;
    options.c_cflag &= ~CSTOPB;     // 设置为1位停止位
    options.c_cflag &= ~CSIZE;      // 清除数据位设置
    options.c_cflag |= CS8;         // 设置为8位数据位
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);     // 设置为非规范模式，禁止回显和信号处理
    options.c_oflag &= ~OPOST;      // 输出模式设置为原始数据输出

    if(tcsetattr(fd, TCSANOW, &options) != 0) {     // 设置串口参数
        perror("tcsetattr"); 
        return 1; 
    }
    return 0;
}
