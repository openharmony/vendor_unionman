#ifndef SEND_H
#define SEND_H

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>      // 用于 usleep 函数（微秒级延时）
#include <sys/time.h>    // 用于获取时间（gettimeofday）
#include "um_pwm.h"      // 引入 PWM 库

// 默认 PWM 通道配置
extern int pwmChannel;  // 需要在一个源文件中定义该变量

// 函数声明
uint32_t getTimeInMicros();
void ledOff();
void ledOn();
uint16_t mark(uint16_t usec);
void space(uint32_t time);
void sendData(uint16_t onemark, uint32_t onespace, uint16_t zeromark, uint32_t zerospace, uint64_t data, uint16_t nbits, bool MSBfirst);

#endif  // SEND_H
