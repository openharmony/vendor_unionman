// infrared_receiver.h

#ifndef INFRARED_RECEIVER_H
#define INFRARED_RECEIVER_H

#include "um_gpio.h"

// 初始化红外接收器GPIO
void Infrared_Receive_Setup(int gpioNum);

// 读取红外接收器GPIO电平值
int Infrared_Receive_Read(int gpioNum);

#endif // INFRARED_RECEIVER_H