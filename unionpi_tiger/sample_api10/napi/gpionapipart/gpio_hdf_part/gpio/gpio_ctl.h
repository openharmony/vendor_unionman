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
#ifndef GPIO_CTL_H
#define GPIO_CTL_H
#include <cstdint>
#include <gpio_if.h>
namespace Gpio {
    enum class Direction {
        input,
        output,
        error
    };

    enum class Val {
        low,
        height
    };

    enum class GpioName {
        GPIO_01 = 380,
        GPIO_02,
        GPIO_03,
        GPIO_04,
        GPIO_05,
        GPIO_06,
        GPIO_07,
        GPIO_08,
        GPIO_09,
        GPIO_10,
        GPIO_11,
        GPIO_12,
        GPIO_13,
        GPIO_14,
        GPIO_15,
        GPIO_16,
    };
    class GpioCtl {
    public:
        GpioCtl(GpioName gpio, Direction direction);
        Val read();
        bool write(Val data);
    protected:
        GpioName gpio;
    };
} // namespace Gpio
#endif