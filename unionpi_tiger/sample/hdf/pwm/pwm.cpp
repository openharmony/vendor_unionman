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
#include <cstdlib>
#include <pthread.h>
#include <csignal>
#include <semaphore.h>

#include "pwm_if.h"
#include "hdf_log.h"
#include "osal_irq.h"
#include "osal_time.h"

#ifdef HDF_LOG_ON
#define LOGD(format, ...)                                     \
    do {                                                      \
            HDF_LOGD("[%{public}s:%{public}d] " format "\n",  \
                __FUNCTION__, __LINE__,                       \
                ##__VA_ARGS__);                               \
    } while (0)

#define LOGI(format, ...)                                                             \
    do {                                                                              \
            HDF_LOGI("[%{public}s:%{public}d] " format "\n", __FUNCTION__, __LINE__,  \
                ##__VA_ARGS__);                                                       \
    } while (0)

#define LOGW(format, ...)                                                         \
    do {                                                                          \
        HDF_LOGW("[%{public}s:%{public}d] " format "\n", __FUNCTION__, __LINE__,  \
            ##__VA_ARGS__);                                                       \
    } while (0)

#define LOGE(format, ...)                                   \
    do {                                                    \
        HDF_LOGE(                                           \
            "\033[0;32;31m"                                 \
            "[%{public}s:%{public}d] " format "\033[m"      \
            "\n",                                           \
            __FUNCTION__,  __LINE__, ##__VA_ARGS__);        \
    } while (0)
#else
#define LOGD(format, ...)                                   \
    do {                                                    \
            printf("[%s:%d] " format "\n",                  \
                __FUNCTION__, __LINE__,                     \
                ##__VA_ARGS__);                             \
    } while (0)

#define LOGI(format, ...)                                       \
    do {                                                        \
        printf("[%s:%d] " format "\n", __FUNCTION__, __LINE__,  \
            ##__VA_ARGS__);                                     \
    } while (0)

#define LOGW(format, ...)                                       \
    do {                                                        \
        printf("[%s:%d] " format "\n", __FUNCTION__, __LINE__,  \
            ##__VA_ARGS__);                                     \
    } while (0)

#define LOGE(format, ...)                             \
    do {                                              \
        printf("\033[0;32;31m"                        \
            "[%s:%d] " format "\033[m"                \
            "\n",                                     \
            __FUNCTION__,  __LINE__, ##__VA_ARGS__);  \
    } while (0)
#endif

#define PWM_NUM_0 (0)
#define PWM_NUM_2 (2)

static DevHandle g_pwmHandle = nullptr;

static void SignalHandler(int32_t sig)
{
    if (sig == SIGINT) {
        if (g_pwmHandle != nullptr) {
            (void)PwmDisable(g_pwmHandle);
            PwmClose(g_pwmHandle);
        }

        LOGI("\nCatch Ctrl + C, waitting for exit.\n");
    }

    return ;
}

int32_t main(int argc, char *argv[])
{
    int32_t ret;
    uint32_t num;
    struct PwmConfig pcfg;
    struct sigaction sigAction;

    memset(&sigAction, 0x0, sizeof(struct sigaction));
    sigAction.sa_handler = SignalHandler;
    sigemptyset(&sigAction.sa_mask);
    sigAction.sa_flags = 0;
    sigaction(SIGINT, &sigAction, nullptr);
    sigAction.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sigAction, nullptr);

    if (argc > 1) {
        num = atoi(argv[1]);
        if ((num != PWM_NUM_0) && (num != PWM_NUM_2)) {
            LOGE("Only support pwm num 0 and 2!\n");
            return HDF_FAILURE;
        }
    } else {
        num = 0; /* 0 - PWM_1, 2 - PWM_2 */
    }

    LOGD("PwmOpen PWM_%d (for Unionpi Tiger)\n", (num == 0) ? 1 : 2);

    g_pwmHandle = PwmOpen(num);
    if (g_pwmHandle == NULL) {
        LOGE("PwmOpen: failed!\n");
        return HDF_FAILURE;
    }

    ret = PwmSetPeriod(g_pwmHandle, 50000000);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("PwmSetPeriod: failed, ret %d\n", ret);
        PwmClose(g_pwmHandle); 
        return HDF_FAILURE;
    }

    ret = PwmSetDuty(g_pwmHandle, 25000000);
    if (ret != HDF_SUCCESS) {
        LOGE("PwmSetDuty: failed, ret %d\n", ret);
        PwmClose(g_pwmHandle); 
        return HDF_FAILURE;
    }

    ret = PwmSetPolarity(g_pwmHandle, PWM_INVERTED_POLARITY);
    if (ret != HDF_SUCCESS) {
        LOGE("PwmSetPolarity: failed, ret %d\n", ret);
        PwmClose(g_pwmHandle); 
        return HDF_FAILURE;
    }

    ret = PwmEnable(g_pwmHandle);
    if (ret != HDF_SUCCESS) {
        LOGE("PwmEnable: failed, ret %d\n", ret);
        PwmClose(g_pwmHandle); 
        return HDF_FAILURE;
    }

    pcfg.duty = 10000000; /* 占空时间为10000000纳秒 */
    pcfg.period = 40000000; /* 周期为40000000纳秒 */
    pcfg.polarity = PWM_NORMAL_POLARITY; /* 极性为正 */
    pcfg.status = PWM_ENABLE_STATUS; /* 运行状态为启用 */
    ret = PwmSetConfig(g_pwmHandle, &pcfg);
    if (ret != HDF_SUCCESS) {
        LOGE("PwmSetConfig: failed, ret %d\n", ret);
        PwmClose(g_pwmHandle); 
        return HDF_FAILURE;
    }

    LOGD("Watting <Enter> to finish ...\n");
    getchar();

    ret = PwmDisable(g_pwmHandle);
    if (ret != HDF_SUCCESS) {
        LOGE("PwmDisable: failed, ret %d\n", ret);
        PwmClose(g_pwmHandle); 
        return HDF_FAILURE;
    }

    PwmClose(g_pwmHandle);

    return HDF_SUCCESS;
}