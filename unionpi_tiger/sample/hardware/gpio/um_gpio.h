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
*/

#ifndef __UM_GPIO_H__
#define __UM_GPIO_H__

#define UM_GPIO_EXPORT "/sys/class/gpio/export"
#define UM_GPIO_UNEXPORT "/sys/class/gpio/unexport"
#define UM_GPIO_PEX "/sys/class/gpio/gpio"

// gpios                                 raspberry pie
#define UM_GPIO_01 378 /* GPIO.7 */
#define UM_GPIO_02 379 /* GPIO.0 */
#define UM_GPIO_03 380 /* GPIO.2 */
#define UM_GPIO_04 381 /* GPIO.3 */
#define UM_GPIO_05 382 /* GPIO.21 */
#define UM_GPIO_06 383 /* GPIO.22 */
#define UM_GPIO_07 384 /* GPIO.23 */
#define UM_GPIO_08 385 /* GPIO.24 */
#define UM_GPIO_09 386 /* GPIO.25 */
#define UM_GPIO_10 387 /* GPIO.1 */
#define UM_GPIO_11 388 /* GPIO.4 */
#define UM_GPIO_12 389 /* GPIO.5 */
#define UM_GPIO_13 390 /* GPIO.6 */
#define UM_GPIO_14 391 /* GPIO.30 */
#define UM_GPIO_15 392 /* GPIO.26 */
#define UM_GPIO_16 393 /* GPIO.27 */
#define UM_GPIO_17 409 /* GPIO.28 */
#define UM_GPIO_18 408 /* GPIO.29 */

// spi cs
#define UM_GPIO_SPI_CS 486 /* SS0 */

// direction
#define UM_GPIO_DIRECTION_IN 0
#define UM_GPIO_DIRECTION_OUT 1

// is export
#define UM_GPIO_NOT_EXPORT 0
#define UM_GPIO_EXPORTED 1

// errno
#define UM_GPIO_ERR (-1)
#define UM_GPIO_NOT_EXPROT (-2)

// value high - low level
#define UM_GPIO_LOW_LEVE 0
#define UM_GPIO_HIGH_LEVE 1

#define UM_GPIO_IN 0
#define UM_GPIO_OUT 1

#define UM_GPIO_NOT_EXPROT_ERROR (-2)

int UM_GPIO_Export(int s32GpioNum, int bExport);

int UM_GPIO_SetDirection(int s32GpioNum, int direction);

int UM_GPIO_SetValue(int s32GpioNum, int s32Value);

int UM_GPIO_IsExport(int s32GpioNum, int *ps32Value);

int UM_GPIO_GetDirection(int s32GpioNum, int *ps32Value);

int UM_GPIO_GetValue(int s32GpioNum, int *ps32Value);

#endif /* __UM_GPIO_H__ */
