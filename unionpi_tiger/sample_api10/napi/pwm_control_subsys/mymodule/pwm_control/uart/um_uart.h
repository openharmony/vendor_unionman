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
#ifndef __UM_UART_H__
#define __UM_UART_H__

#ifdef __cplusplus
extern "C"
{
#endif 

#define RECV_HEAD 0xaa
#define RECV_END 0x55
#define EVT_RIGHT 0x01
#define EVT_LEFT 0x02
#define EVT_BACK 0x03
#define EVT_FORWARD 0x04
#define EVT_PULLUP 0x05
#define EVT_PULLDOWN 0x06
#define EVT_PULLREMOVE 0x07
#define EVT_TOUCH1 0x21
#define EVT_TOUCH2 0x22
#define EVT_TOUCH3 0x23
#define EVT_TOUCH4 0x24
#define EVT_TOUCH5 0x25
#define OK 0
#define ERR (-1)

/*
 * 作用：初始化串口
 * 参数：无
 */
int UmInitUart(void);
/*
 * 作用：获取串口数据
 * 参数：无
 * 返回值：串口数据
 */
int UmGetUart(void);
/*
 * 作用：设置传感器休眠
 * 参数：1 -> 进入休眠 ， 0 -> 退出休眠
 * 返回值：0 -> 成功， -1 -> 失败
 */
int UmSleepSet(int value);

#ifdef __cplusplus
}
#endif

#endif // endif __UM_PWM_H__