/* Copyright 2022 Unionman Technology Co., Ltd.
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

#include <cstdio>
#include "gpio_if.h"
#include "hdf_log.h"
#include "osal_irq.h"
#include "osal_time.h"

#ifdef HDF_LOG_ON
#define LOGD(format, ...)                                             \
    do {                                                              \
            HDF_LOGD("[%{public}s:%{public}d] " format "\n",          \
                __FUNCTION__, __LINE__,                               \
                ##__VA_ARGS__);                                       \
    } while (0)

#define LOGI(format, ...)                                                            \
    do {                                                                             \
            HDF_LOGI("[%{public}s:%{public}d] " format "\n", __FUNCTION__, __LINE__, \
                ##__VA_ARGS__);                                                      \
    } while (0)

#define LOGW(format, ...)                                                        \
    do {                                                                         \
        HDF_LOGW("[%{public}s:%{public}d] " format "\n", __FUNCTION__, __LINE__, \
            ##__VA_ARGS__);                                                      \
    } while (0)

#define LOGE(format, ...)                                \
    do {                                                 \
        HDF_LOGE(                                        \
            "\033[0;32;31m"                              \
            "[%{public}s:%{public}d] " format "\033[m"   \
            "\n",                                        \
            __FUNCTION__,  __LINE__, ##__VA_ARGS__);     \
    } while (0)
#else
#define LOGD(format, ...)                                   \
    do {                                                    \
            printf("[%s:%d] " format "\n",                  \
                __FUNCTION__, __LINE__,                     \
                ##__VA_ARGS__);                             \
    } while (0)

#define LOGI(format, ...)                                      \
    do {                                                       \
        printf("[%s:%d] " format "\n", __FUNCTION__, __LINE__, \
            ##__VA_ARGS__);                                    \
    } while (0)

#define LOGW(format, ...)                                      \
    do {                                                       \
        printf("[%s:%d] " format "\n", __FUNCTION__, __LINE__, \
            ##__VA_ARGS__);                                    \
    } while (0)

#define LOGE(format, ...)                                   \
    do {                                                    \
        printf("\033[0;32;31m"                              \
            "[%s:%d] " format "\033[m"                      \
            "\n",                                           \
            __FUNCTION__,  __LINE__, ##__VA_ARGS__);        \
    } while (0)
#endif

#define SLEEP_TIME_MS (1000)
#define TIMEOUT_COUNT (5)

int32_t main(int argc, char *argv[])
{
    uint16_t gpio = 380; /* GPIO_01, GPIO Num : cat /sys/kernel/debug/gpio */
    uint32_t count = 0;
    uint16_t valRead;
    int32_t ret;

    LOGD("GpioSetDir : Set GPIO_01 (for Unionpi Tiger) DIR_OUT\n");

    ret = GpioSetDir(gpio, GPIO_DIR_OUT);
    if (ret != HDF_SUCCESS) {
        LOGE("%s: set dir fail! ret:%d\n", __func__, ret);
        return ret;
    }

    for (;;) {
        (void)GpioRead(gpio, &valRead);
        LOGD("GpioRead valRead : %d\n", valRead);

        (void)GpioWrite(gpio, (valRead == GPIO_VAL_LOW) ? GPIO_VAL_HIGH : GPIO_VAL_LOW);

        OsalMSleep(SLEEP_TIME_MS);

        count += 1;
        if (count >= TIMEOUT_COUNT) {
            LOGE("%s: Wait timeout break!\n", __func__);
            break;
        }
    }

    return HDF_SUCCESS;
}
