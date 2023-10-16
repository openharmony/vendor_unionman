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

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "hdf_log.h"
#include "osal_irq.h"
#include "osal_time.h"
#include "pwm_if.h"
#include "um_pwm.h"

int UM_set_pwm(int channalNum, int duty)
{
    static DevHandle g_pwmHandle = nullptr;
    int32_t ret;

    g_pwmHandle = PwmOpen(channalNum);
    if (g_pwmHandle == NULL) {
        printf("dont find the drives!");
        return 0;
    }

    ret = PwmSetPeriod(g_pwmHandle, 20000000L);
    if (ret != 0) {
        printf("PwmSetPeriod error!");
        return 0;
    }

    ret = PwmSetDuty(g_pwmHandle, duty);
    if (ret != 0) {
        printf("PwmSetDuty error!");
        return 0;
    }

    ret = PwmSetPolarity(g_pwmHandle, PWM_NORMAL_POLARITY);
    if (ret != 0) {
        printf("PwmSetPolarity error!");
        return 0;
    }

    ret = PwmEnable(g_pwmHandle);
    if (ret != 0) {
        printf("PwmEnable error!");
        return 0;
    }

    PwmClose(g_pwmHandle);

    return 1;
}
