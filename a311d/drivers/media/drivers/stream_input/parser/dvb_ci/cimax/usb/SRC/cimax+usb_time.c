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

/*#define TIMESTAMP*/

/******************************************************************************
 * Include
 ******************************************************************************/
#include <linux/delay.h>

#include "cimax+usb_time.h"

/******************************************************************************
 * Structures
 ******************************************************************************/
#define err(format, arg...)\
	pr_err("cimax+usb_time: %s> ERROR " format "\n" , \
		__func__, ## arg)
#define info(format, arg...) pr_info("time> " format "\n", ## arg)
/******************************************************************************
 * Globals
 ******************************************************************************/
#ifdef TIMESTAMP
struct item_array_s gstArray;
#endif
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
void InitTimestamp(void)
{
#ifdef TIMESTAMP
	gstArray.count = 0;
#endif
	return;
}

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
void SetTimestamp(const char *pcFormat, ...)
{
#ifdef TIMESTAMP
	va_list stArgs;

	if (gstArray.count >= MAX_ITEMS) {
		if (gstArray.count++ == MAX_ITEMS)
			ShowTimestamp();
		return;
	}
	ktime_get_ts(&gstArray.stItem[gstArray.count].stTime);
	va_start(stArgs, pcFormat);
	vsprintf(gstArray.stItem[gstArray.count++].pcLine, pcFormat, stArgs);
	va_end(stArgs);
#endif
	return;
}

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
void ShowTimestamp(void)
{
#ifdef TIMESTAMP
	int i;

	if (gstArray.count == 0) {
		err("No timestamps available");
		return;
	}

	info("===============================================================");
	info("                         TIMESTAMPS");
	info("===============================================================");

	for (i = 0; i < gstArray.count; i++) {
		info("[%04d] [%03d.%09d] %s",
				i,
				gstArray.stItem[i].stTime.tv_sec,
				gstArray.stItem[i].stTime.tv_nsec,
				gstArray.stItem[i].pcLine);
		if ((i % 100) == 0)
			msleep(20);
	}
	info("===============================================================");
	gstArray.count = 0;

#endif
	return;
}
