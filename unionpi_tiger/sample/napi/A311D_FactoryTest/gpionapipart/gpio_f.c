/*
 * Copyright 2023 Unionman Technology Co., Ltd.
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
 
#include <unistd.h>
#include "gpio_if.h"
#include "um_gpio.h"

void set_gpiodir(void)
{
    GpioSetDir(GPIO01, GPIO_DIR_OUT);
    GpioSetDir(GPIO02, GPIO_DIR_OUT);
    GpioSetDir(GPIO03, GPIO_DIR_OUT);
    GpioSetDir(GPIO04, GPIO_DIR_OUT);
    GpioSetDir(GPIO05, GPIO_DIR_OUT);
    GpioSetDir(GPIO06, GPIO_DIR_OUT);
    GpioSetDir(GPIO07, GPIO_DIR_OUT);
    GpioSetDir(GPIO08, GPIO_DIR_OUT);
    GpioSetDir(GPIO09, GPIO_DIR_OUT);
    GpioSetDir(GPIO10, GPIO_DIR_OUT);

    GpioSetDir(GPIO11, GPIO_DIR_OUT);
    GpioSetDir(GPIO12, GPIO_DIR_IN);
    GpioSetDir(GPIO13, GPIO_DIR_OUT);
    GpioSetDir(GPIO14, GPIO_DIR_IN);
    GpioSetDir(GPIO15, GPIO_DIR_OUT);
    GpioSetDir(GPIO16, GPIO_DIR_IN);
    GpioSetDir(GPIO17, GPIO_DIR_OUT);
    GpioSetDir(GPIO18, GPIO_DIR_IN);
}

int32_t gpiotest()
{
    set_gpiodir();
    GpioWrite(GPIO11, 0);
    GpioWrite(GPIO13, 0);
    GpioWrite(GPIO15, 0);
    GpioWrite(GPIO17, 0);
    uint16_t val;
    uint16_t val1;
    uint16_t val2;
    uint16_t val3;
    GpioRead(GPIO12, &val);
    GpioRead(GPIO14, &val1);
    GpioRead(GPIO16, &val2);
    GpioRead(GPIO18, &val3);
    uint16_t temp1 = val + val1 + val2 + val3;

    if (temp1 != 0) {
        return 0;
    }

    GpioWrite(GPIO01, 1);
    GpioWrite(GPIO02, 1);
    GpioWrite(GPIO03, 1);
    GpioWrite(GPIO04, 1);
    GpioWrite(GPIO05, 1);
    GpioWrite(GPIO06, 1);
    GpioWrite(GPIO07, 1);
    GpioWrite(GPIO08, 1);
    GpioWrite(GPIO09, 1);
    GpioWrite(GPIO10, 1);

    GpioWrite(GPIO11, 1);
    GpioWrite(GPIO13, 1);
    GpioWrite(GPIO15, 1);
    GpioWrite(GPIO17, 1);

    GpioRead(GPIO12, &val);
    GpioRead(GPIO14, &val1);
    GpioRead(GPIO16, &val2);
    GpioRead(GPIO18, &val3);
    uint16_t temp2 = val & val1 & val2 & val3;
    if (temp2 != 1) {
        return 0;
    } else {
        return 1;
    }
}

int32_t upadate()
{
    uint16_t gpio = 506;
    uint16_t val;

    GpioRead(gpio, &val);
    if (val == 0) {
        return 1;
    } else {
        return 0;
    }
}

int32_t power()
{
    uint16_t gpio = 475;
    uint16_t val;

    GpioRead(gpio, &val);
    if (val == 0) {
        return 1;
    } else {
        return 0;
    }
}