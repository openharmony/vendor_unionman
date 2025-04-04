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

#ifndef _SERIAL_UART_H_
#define _SERIAL_UART_H_

#define UART_TTL_NAME "/dev/ttyS1"
#define UART_40PIN_NAME "/dev/ttyACM0"
#define UART_485_NAME "/dev/ttyACM1"
#define UART_SUCCESS 0
#define UART_TERMINAL_ERROR (-1)
#define UART_OPEN_FAIL (-2)
#define UART_INIT_FAIL (-3)
#define UART_WRITE_FAIL (-4)
#define UART_READ_FAIL (-5)
#define UART_READ_ERROR (-6)

#define FRAME_LEN 4

#ifdef __cplusplus
extern "C" {
#endif

int uarttest(int device_num);

#ifdef __cplusplus
}
#endif

#endif
