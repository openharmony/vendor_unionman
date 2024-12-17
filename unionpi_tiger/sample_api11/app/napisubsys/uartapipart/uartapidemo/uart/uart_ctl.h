/*
 * Copyright (c) 2023 Unionman Technology Co., Ltd.
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

#ifndef UART_CTL_H
#define UART_CTL_H

#include<cstdint>
#include<uart_if.h>

namespace Uart {
    enum class eventCode {
        right = 0x01,
        left = 0x02,
        back = 0x03,
        forward = 0x04,
        pullUp = 0x05,
        pullDown = 0x06,
        pullMove = 0x07,
        touch1 = 0x21,
        touch2 = 0x22,
        touch3 = 0x23,
        touch4 = 0x24,
        touch5 = 0x25
    };

    class UartCtl {
    public:
        explicit UartCtl(uint32_t Channel);
        ~UartCtl();
        int32_t readTouch();
    protected:
        DevHandle handle;
    };
}

#endif