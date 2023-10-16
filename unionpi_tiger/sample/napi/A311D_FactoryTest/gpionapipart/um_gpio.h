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

#ifndef __UM_GPIO_H__
#define __UM_GPIO_H__

#define GPIO01 380
#define GPIO02 381
#define GPIO03 382
#define GPIO04 383
#define GPIO05 384
#define GPIO06 385 /* gpio_01, gpio Num : cat /sys/kernel/debug/gpio */
#define GPIO07 386
#define GPIO08 387
#define GPIO09 388
#define GPIO10 389
#define GPIO11 390
#define GPIO12 391
#define GPIO13 392
#define GPIO14 393
#define GPIO15 394
#define GPIO16 395
#define GPIO17 410
#define GPIO18 411

#ifdef __cplusplus
extern "C" {
#endif


int gpiotest(void);
int32_t upadate();
int32_t power();
#ifdef __cplusplus
}
#endif

#endif