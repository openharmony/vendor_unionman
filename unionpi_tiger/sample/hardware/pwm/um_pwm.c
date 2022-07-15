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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "um_pwm.h"

int set_pwm_enable(int pwmChannel, int isEnable)
{
    int ret = 0;
    int ret_snprintf_s = -1;

    char buffer[256];
    // pwmchannel
    char pwm_file_name[128];
    int ret_sprintf_s = -1;

    (void)memset_s(pwm_file_name, sizeof(pwm_file_name), 0, sizeof(pwm_file_name));
    if (PWM1 == pwmChannel) {
        ret_sprintf_s = sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/enabled", PWM1_PEX);
        if (ret_sprintf_s != 0) {
        }
    } else if (PWM2 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/enabled", PWM2_PEX);
    } else {
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwm_file_name, F_OK) != 0) {
        return PWM_FILE_NOT_EXIST;
    }

    ret_snprintf_s = snprintf_s(buffer, sizeof(buffer), sizeof(buffer), "echo %d > %s", isEnable, pwm_file_name);
    if (ret_snprintf_s != 0) {
    }

    system(buffer);

    return ret;
}

int set_pwm_period(int pwmChannel, long period)
{
    int ret = 0;

    // pwmchannel
    char pwm_file_name[128];
    (void)memset_s(pwm_file_name, sizeof(pwm_file_name), 0, sizeof(pwm_file_name));
    if (PWM1 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/period", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/period", PWM2_PEX);
    } else {
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwm_file_name, F_OK) != 0) {
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp;
    fp = fopen(pwm_file_name, "r+");
    if (period) {
        fprintf(fp, "%ld", period);
    }
    (void)fclose(fp);
    return ret;
}

int get_pwm_dutyCycle(int pwmChannel, long *value)
{
    int ret = 0;

    char pwm_file_name[128];
    (void)memset_s(pwm_file_name, sizeof(pwm_file_name), 0, sizeof(pwm_file_name));
    if (PWM1 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/duty_cycle", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/duty_cycle", PWM2_PEX);
    } else {
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwm_file_name, F_OK) != 0) {
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp;
    char buffer[32];
    (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
    fp = fopen(pwm_file_name, "r");
    (void)fread(buffer, sizeof(buffer), 1, fp);

    (void)fclose(fp);
    printf("get_pwm_dutyCycle = %s\n", buffer);
    *value = atol(buffer);

    return ret;
}

int get_pwm_period(int pwmChannel, long *value)
{
    int ret = 0;

    char pwm_file_name[128];
    (void)memset_s(pwm_file_name, sizeof(pwm_file_name), 0, sizeof(pwm_file_name));
    if (PWM1 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/period", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/period", PWM2_PEX);
    } else {
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwm_file_name, F_OK) != 0) {
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp;
    char buffer[32];
    (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
    fp = fopen(pwm_file_name, "r");
    (void)fread(buffer, sizeof(buffer), 1, fp);

    (void)fclose(fp);
    *value = atol(buffer);

    return ret;
}

int set_pwm_dutyCycle(int pwmChannel, long dutyCycle)
{
    int ret = 0;

    // pwmchannel
    char pwm_file_name[128];
    (void)memset_s(pwm_file_name, sizeof(pwm_file_name), 0, sizeof(pwm_file_name));
    if (PWM1 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/duty_cycle", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/duty_cycle", PWM2_PEX);
    } else {
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwm_file_name, F_OK) != 0) {
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp;
    fp = fopen(pwm_file_name, "r+");
    if (dutyCycle) {
        fprintf(fp, "%ld", dutyCycle);
    }
    (void)fclose(fp);
    return ret;
}

int is_pwm_enabled(int pwmChannel, int *value)
{
    int ret = 0;

    // check pwm enabled or not
    char pwm_file_name[128];
    (void)memset_s(pwm_file_name, sizeof(pwm_file_name), 0, sizeof(pwm_file_name));
    if (PWM1 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/enabled", PWM1_PEX);
        printf("pwmchannel = %s", pwm_file_name);
    } else if (PWM2 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/enabled", PWM2_PEX);
        printf("pwmchannel = %s\n", pwm_file_name);
    } else {
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwm_file_name, F_OK) != 0) {
        return PWM_FILE_NOT_EXIST;
    } else {
        FILE *fp;
        char buffer[32];
        (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
        fp = fopen(pwm_file_name, "r");
        fread(buffer, sizeof(buffer), 1, fp);

        fclose(fp);
        *value = atoi(buffer);
    }

    return ret;
}

int set_pwm_polarity(int pwmChannel, int polarity)
{
    int ret = 0;

    // pwmchannel
    char pwm_file_name[128];
    (void)memset_s(pwm_file_name, sizeof(pwm_file_name), 0, sizeof(pwm_file_name));
    if (PWM1 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/polarity", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/polarity", PWM2_PEX);
    } else {
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwm_file_name, F_OK) != 0) {
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp;
    fp = fopen(pwm_file_name, "rw+");
    if (polarity == PWM_POLARITY_NORMAL) {
        fprintf(fp, "%s", "normal");
    } else if (polarity == PWM_POLARITY_INVERSED) {
        fprintf(fp, "%s", "inversed");
    }

    (void)fclose(fp);
    return ret;
}
int get_pwm_polarity(int pwmChannel, int *value)
{
    int ret = 0;

    // pwmchannel
    char pwm_file_name[128];
    (void)memset_s(pwm_file_name, sizeof(pwm_file_name), 0, sizeof(pwm_file_name));
    if (PWM1 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/polarity", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/polarity", PWM2_PEX);
    } else {
        return PWM_WRONOG_CHANNEL;
    }
    if (access(pwm_file_name, F_OK) != 0) {
        return PWM_FILE_NOT_EXIST;
    }
    FILE *fp;
    char buffer[32];
    (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
    fp = fopen(pwm_file_name, "r");
    (void)fread(buffer, sizeof(buffer), 1, fp);
    (void)fclose(fp);

    if (strstr(buffer, "normal") != NULL) {
        *value = PWM_POLARITY_NORMAL;
    } else if (strstr(buffer, "inversed") != NULL) {
        *value = PWM_POLARITY_INVERSED;
    } else {
        ret = PWM_ERR;
    }

    return ret;
}
