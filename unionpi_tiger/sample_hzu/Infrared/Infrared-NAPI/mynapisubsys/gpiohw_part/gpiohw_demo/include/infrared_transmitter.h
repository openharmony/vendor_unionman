// infrared_transmitter.h

#ifndef INFRARED_TRANSMITTER_H
#define INFRARED_TRANSMITTER_H

#include "um_gpio.h"

// 初始化红外发射器GPIO
void Infrared_Transmit_Setup(int gpioNum);

// 控制红外发射器GPIO电平
void Infrared_Transmit_Control(int gpioNum, int value);

#endif // INFRARED_TRANSMITTER_H