/*
 * Copyright (c) 2022 Unionman Co., Ltd.
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
 */

#ifndef __CIMAXPLUS_USB_TIME_H
#define __CIMAXPLUS_USB_TIME_H

/******************************************************************************
 * Include
 ******************************************************************************/
#include <linux/ktime.h>

/******************************************************************************
 * Defines
 ******************************************************************************/
#define MAX_ITEMS     100000
#define MAX_LINE_SIZE    128

/******************************************************************************
 * Enums
 ******************************************************************************/
/******************************************************************************
 * Structures
 ******************************************************************************/
struct item_s {
	struct timespec stTime;
	char            pcLine[MAX_LINE_SIZE];
};

struct item_array_s {
	int  count;
	item_s stItem[MAX_ITEMS];
};

extern struct item_array_s gstArray;

/******************************************************************************
 * Functions
 ******************************************************************************/
/******************************************************************************
 * @brief
 *   Init timestamp.
 *
 * @param
 *   None
 *
 * @return
 *   None.
 ******************************************************************************/
void InitTimestamp(void);

/******************************************************************************
 * @brief
 *   Set timestamp.
 *
 * @param   pcFormat
 *   Printf-like format
 *
 * @return
 *   None.
 ******************************************************************************/
void SetTimestamp(const char *pcFormat, ...);

/******************************************************************************
 * @brief
 *   Display all timestamps.
 *
 * @param
 *   None
 *
 * @return
 *   None.
 ******************************************************************************/
void ShowTimestamp(void);

#endif
