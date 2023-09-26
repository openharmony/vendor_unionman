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

#include "uart_ctl.h"

namespace Uart {
    constexpr uint32_t baud = 9600;

    namespace recvCode {
        constexpr uint8_t head = 0xaa;
        constexpr uint8_t end = 0x55;
    }

    UartCtl::UartCtl(uint32_t Channel)
    {
        handle = UartOpen(Channel);
        UartSetBaud(handle, baud);
        UartAttribute attr {
            .dataBits = UART_ATTR_DATABIT_8,
            .parity = UART_ATTR_PARITY_NONE,
            .stopBits = UART_ATTR_STOPBIT_1,
            .rts = UART_ATTR_RTS_DIS,
            .cts = UART_ATTR_CTS_DIS,
            .fifoRxEn = UART_ATTR_RX_FIFO_DIS,
            .fifoTxEn = UART_ATTR_TX_FIFO_DIS
        };
        UartSetAttribute(handle, &attr);
        UartSetTransMode(handle, UART_MODE_RD_BLOCK);
    }

    UartCtl::~UartCtl()
    {
        UartClose(handle);
    }

    int32_t UartCtl::readTouch()
    {
        uint8_t buffer[4L];
        UartRead(handle, buffer, 4L);
        if (buffer[0L] == recvCode::head &&
        buffer[3L] == recvCode::end &&
        (buffer[1L] == static_cast<uint8_t>(~buffer[2L]))) {
            return buffer[1L];
        } else {
            return -1;
        }
    }
}
