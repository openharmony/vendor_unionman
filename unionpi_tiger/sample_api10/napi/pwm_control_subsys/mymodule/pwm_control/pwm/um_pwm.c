/* Copyright 2024 Unionman Technology Co., Ltd.
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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "securec.h"
#include "hilog/log.h"
#include "um_pwm.h"

int UmInitPwm(int pwmChannel)
{
    char pwmFileName[128] = {0};
    (void)memset_s(pwmFileName, sizeof(pwmFileName), 0, sizeof(pwmFileName));

    if (pwmChannel == PWM1) {
        (void)sprintf_s(pwmFileName, sizeof(pwmFileName), "%s/export", PWM1_PEX);
    } else if (pwmChannel == PWM2) {
        (void)sprintf_s(pwmFileName, sizeof(pwmFileName), "%s/export", PWM2_PEX);
    } else {
        HILOG_ERROR(LOG_CORE, "PWM WRONOG CHANEEL\n");
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwmFileName, F_OK) != 0) {
        HILOG_ERROR(LOG_CORE, "PWM EXPORT FILE NOT EXIST\n");
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp = NULL;
    fp = fopen(pwmFileName, "w");
    if (!fp) {
        HILOG_ERROR(LOG_CORE, "Failed to open export file!");
        return PWM_FILE_NOT_EXIST;
    }
    (void)fprintf(fp, "%d", 0);
    (void)fclose(fp);
    fp = NULL;
    return 0;
}

int UmSetPwmPeriod(int pwmChannel, int period)
{
    char pwmFileName[128] = {0};
    (void)memset_s(pwmFileName, sizeof(pwmFileName), 0, sizeof(pwmFileName));

    if (pwmChannel == PWM1) {
        (void)sprintf_s(pwmFileName, sizeof(pwmFileName), "%s/pwm0/period", PWM1_PEX);
    } else if (pwmChannel == PWM2) {
        (void)sprintf_s(pwmFileName, sizeof(pwmFileName), "%s/pwm0/period", PWM2_PEX);
    } else {
        HILOG_ERROR(LOG_CORE, "PWM WRONOG CHANEEL\n");
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwmFileName, F_OK) != 0) {
        HILOG_ERROR(LOG_CORE, "PWM PERIOD FILE NOT EXIST\n");
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp = NULL;
    fp = fopen(pwmFileName, "r+");
    if (!fp) {
        HILOG_ERROR(LOG_CORE, "Failed to open period file!");
        return PWM_FILE_NOT_EXIST;
    }
    if (period) {
        fprintf(fp, "%d", period);
    }
    (void)fclose(fp);
    fp = NULL;

    return 0;
}

int UmSetPwmDutyCycle(int pwmChannel, int dutyCycle)
{
    char pwmFileName[128] = {0};
    (void)memset_s(pwmFileName, sizeof(pwmFileName), 0, sizeof(pwmFileName));

    if (pwmChannel == PWM1) {
        (void)sprintf_s(pwmFileName, sizeof(pwmFileName), "%s/pwm0/duty_cycle", PWM1_PEX);
    } else if (pwmChannel == PWM2) {
        (void)sprintf_s(pwmFileName, sizeof(pwmFileName), "%s/pwm0/duty_cycle", PWM2_PEX);
    } else {
        HILOG_ERROR(LOG_CORE, "PWM WRONOG CHANEEL\n");
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwmFileName, F_OK) != 0) {
        HILOG_ERROR(LOG_CORE, "PWM DUTY_CYCLE FILE NOT EXIST\n");
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp = NULL;
    fp = fopen(pwmFileName, "r+");
    if (!fp) {
        HILOG_ERROR(LOG_CORE, "Failed to open duty_cycle file!");
        return PWM_FILE_NOT_EXIST;
    }
    if (dutyCycle) {
        fprintf(fp, "%d", dutyCycle);
    }
    (void)fclose(fp);
    fp = NULL;

    return 0;
}

int UmSetPwmPolarity(int pwmChannel, int polarity)
{
    char pwmFileName[128] = {0};
    (void)memset_s(pwmFileName, sizeof(pwmFileName), 0, sizeof(pwmFileName));

    if (pwmChannel == PWM1) {
        (void)sprintf_s(pwmFileName, sizeof(pwmFileName), "%s/pwm0/polarity", PWM1_PEX);
    } else if (pwmChannel == PWM2) {
        (void)sprintf_s(pwmFileName, sizeof(pwmFileName), "%s/pwm0/polarity", PWM2_PEX);
    } else {
        HILOG_ERROR(LOG_CORE, "PWM WRONOG CHANEEL\n");
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwmFileName, F_OK) != 0) {
        HILOG_ERROR(LOG_CORE, "PWM POKARITY FILE NOT EXIST\n");
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp = NULL;
    fp = fopen(pwmFileName, "rw+");
    if (!fp) {
        HILOG_ERROR(LOG_CORE, "Failed to open polarity file!");
        return PWM_FILE_NOT_EXIST;
    }
    if (polarity == PWM_POLARITY_NORMAL) {
        fprintf(fp, "%s", "normal");
    } else if (polarity == PWM_POLARITY_INVERSED) {
        fprintf(fp, "%s", "inversed");
    }
    (void)fclose(fp);
    fp = NULL;

    return 0;
}

int UmSetPwmEnable(int pwmChannel, int isEnable)
{
    char buffer[256] = {0};
    char pwmFileName[128] = {0};
    (void)memset_s(pwmFileName, sizeof(pwmFileName), 0, sizeof(pwmFileName));

    if (pwmChannel == PWM1) {
        (void)sprintf_s(pwmFileName, sizeof(pwmFileName), "%s/pwm0/enable", PWM1_PEX);
    } else if (pwmChannel == PWM2) {
        (void)sprintf_s(pwmFileName, sizeof(pwmFileName), "%s/pwm0/enable", PWM2_PEX);
    } else {
        HILOG_ERROR(LOG_CORE, "PWM WRONOG CHANEEL\n");
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwmFileName, F_OK) != 0) {
        HILOG_ERROR(LOG_CORE, "PWM ENABLE FILE NOT EXIST\n");
        return PWM_FILE_NOT_EXIST;
    }

    (void)snprintf_s(buffer, sizeof(buffer), sizeof(buffer), "echo %d > %s", isEnable, pwmFileName);
    system(buffer);

    return 0;
}

int UmGetPwmPeriod(int pwmChannel)
{
    int ret = 0;
    char pwmFileName[128] = {0};
    (void)memset_s(pwmFileName, sizeof(pwmFileName), 0, sizeof(pwmFileName));

    if (pwmChannel == PWM1) {
        (void)sprintf_s(pwmFileName, sizeof(pwmFileName), "%s/pwm0/period", PWM1_PEX);
    } else if (pwmChannel == PWM2) {
        (void)sprintf_s(pwmFileName, sizeof(pwmFileName), "%s/pwm0/period", PWM2_PEX);
    } else {
        HILOG_ERROR(LOG_CORE, "PWM WRONOG CHANEEL\n");
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwmFileName, F_OK) != 0) {
        HILOG_ERROR(LOG_CORE, "PWM PERIOD FILE NOT EXIST\n");
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp = NULL;
    char buffer[32] = {0};
    (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
    fp = fopen(pwmFileName, "r");
    if (!fp) {
        HILOG_ERROR(LOG_CORE, "Failed to open period file!");
        return PWM_FILE_NOT_EXIST;
    }
    (void)fread(buffer, sizeof(buffer), 1, fp);
    (void)fclose(fp);
    fp = NULL;

    ret = atoi(buffer);
    return ret;
}

int UmGetPwmDutyCycle(int pwmChannel)
{
    int ret = 0;
    char pwmFileName[128] = {0};
    (void)memset_s(pwmFileName, sizeof(pwmFileName), 0, sizeof(pwmFileName));
    if (pwmChannel == PWM1) {
        (void)sprintf_s(pwmFileName, sizeof(pwmFileName), "%s/pwm0/duty_cycle", PWM1_PEX);
    } else if (pwmChannel == PWM2) {
        (void)sprintf_s(pwmFileName, sizeof(pwmFileName), "%s/pwm0/duty_cycle", PWM2_PEX);
    } else {
        HILOG_ERROR(LOG_CORE, "PWM WRONOG CHANEEL\n");
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwmFileName, F_OK) != 0) {
        HILOG_ERROR(LOG_CORE, "PWM DUTY_CYCLE FILE NOT EXIST\n");
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp = NULL;
    char buffer[32] = {0};
    (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
    fp = fopen(pwmFileName, "r");
    if (!fp) {
        HILOG_ERROR(LOG_CORE, "Failed to open duty_cycle file!");
        return PWM_FILE_NOT_EXIST;
    }
    (void)fread(buffer, sizeof(buffer), 1, fp);
    (void)fclose(fp);
    fp = NULL;

    ret = atoi(buffer);
    return ret;
}

int UmGetPwmPolarity(int pwmChannel)
{
    int ret = 0;
    char pwmFileName[128] = {0};
    (void)memset_s(pwmFileName, sizeof(pwmFileName), 0, sizeof(pwmFileName));

    if (pwmChannel == PWM1) {
        (void)sprintf_s(pwmFileName, sizeof(pwmFileName), "%s/pwm0/polarity", PWM1_PEX);
    } else if (pwmChannel == PWM2) {
        (void)sprintf_s(pwmFileName, sizeof(pwmFileName), "%s/pwm0/polarity", PWM2_PEX);
    } else {
        HILOG_ERROR(LOG_CORE, "PWM WRONOG CHANEEL\n");
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwmFileName, F_OK) != 0) {
        HILOG_ERROR(LOG_CORE, "PWM POLARITY FILE NOT EXIST\n");
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp = NULL;
    char buffer[32] = {0};
    (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
    fp = fopen(pwmFileName, "r");
    if (!fp) {
        HILOG_ERROR(LOG_CORE, "Failed to open polarity file!");
        return PWM_FILE_NOT_EXIST;
    }
    (void)fread(buffer, sizeof(buffer), 1, fp);
    (void)fclose(fp);
    fp = NULL;

    if (strstr(buffer, "normal") != NULL) {
        ret = PWM_POLARITY_NORMAL;
    } else if (strstr(buffer, "inversed") != NULL) {
        ret = PWM_POLARITY_INVERSED;
    } else {
        HILOG_ERROR(LOG_CORE, "PWM ERROR \n");
        ret = PWM_ERR;
    }

    return ret;
}

int UmIsPwmEnabled(int pwmChannel)
{
    int ret = 0;
    char pwmFileName[128] = {0};
    (void)memset_s(pwmFileName, sizeof(pwmFileName), 0, sizeof(pwmFileName));

    if (pwmChannel == PWM1) {
        (void)sprintf_s(pwmFileName, sizeof(pwmFileName), "%s/pwm0/enable", PWM1_PEX);
    } else if (pwmChannel == PWM2) {
        (void)sprintf_s(pwmFileName, sizeof(pwmFileName), "%s/pwm0/enable", PWM2_PEX);
    } else {
        HILOG_ERROR(LOG_CORE, "PWM WRONOG CHANEEL\n");
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwmFileName, F_OK) != 0) {
        HILOG_ERROR(LOG_CORE, "PWM ENABLE FILE NOT EXIST\n");
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp = NULL;
    char buffer[32] = {0};
    (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
    fp = fopen(pwmFileName, "r");
    if (!fp) {
        HILOG_ERROR(LOG_CORE, "Failed to open enable file!");
        return PWM_FILE_NOT_EXIST;
    }
    (void)fread(buffer, sizeof(buffer), 1, fp);
    (void)fclose(fp);
    fp = NULL;

    ret = atoi(buffer);
    return ret;
}