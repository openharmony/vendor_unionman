#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "securec.h"
#include "hilog/log.h"
#include "um_pwm.h"

int um_init_pmw(int pwmChannel)
{
    char pwm_file_name[128] = {0};
    (void)memset_s(pwm_file_name, sizeof(pwm_file_name), 0, sizeof(pwm_file_name));

    if (PWM1 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/export", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/export", PWM2_PEX);
    } else {
        HILOG_ERROR(LOG_CORE, "PWM WRONOG CHANEEL\n");
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwm_file_name, F_OK) != 0) {
        HILOG_ERROR(LOG_CORE, "PWM EXPORT FILE NOT EXIST\n");
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp = NULL;
    fp = fopen(pwm_file_name, "w");
    if (!fp) {
        HILOG_ERROR(LOG_CORE, "Failed to open export file!");
        return PWM_FILE_NOT_EXIST;
    }
    (void)fprintf(fp, "%d", 0);
    (void)fclose(fp);
    fp = NULL;
    return 0;
}

int um_set_pwm_period(int pwmChannel, int period)
{
    char pwm_file_name[128] = {0};
    (void)memset_s(pwm_file_name, sizeof(pwm_file_name), 0, sizeof(pwm_file_name));

    if (PWM1 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/pwm0/period", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/pwm0/period", PWM2_PEX);
    } else {
        HILOG_ERROR(LOG_CORE, "PWM WRONOG CHANEEL\n");
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwm_file_name, F_OK) != 0) {
        HILOG_ERROR(LOG_CORE, "PWM PERIOD FILE NOT EXIST\n");
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp = NULL;
    fp = fopen(pwm_file_name, "r+");
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

int um_set_pwm_dutyCycle(int pwmChannel, int dutyCycle)
{
    char pwm_file_name[128] = {0};
    (void)memset_s(pwm_file_name, sizeof(pwm_file_name), 0, sizeof(pwm_file_name));

    if (PWM1 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/pwm0/duty_cycle", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/pwm0/duty_cycle", PWM2_PEX);
    } else {
        HILOG_ERROR(LOG_CORE, "PWM WRONOG CHANEEL\n");
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwm_file_name, F_OK) != 0) {
        HILOG_ERROR(LOG_CORE, "PWM DUTY_CYCLE FILE NOT EXIST\n");
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp = NULL;
    fp = fopen(pwm_file_name, "r+");
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

int um_set_pwm_polarity(int pwmChannel, int polarity)
{
    char pwm_file_name[128] = {0};
    (void)memset_s(pwm_file_name, sizeof(pwm_file_name), 0, sizeof(pwm_file_name));

    if (PWM1 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/pwm0/polarity", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/pwm0/polarity", PWM2_PEX);
    } else {
        HILOG_ERROR(LOG_CORE, "PWM WRONOG CHANEEL\n");
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwm_file_name, F_OK) != 0) {
        HILOG_ERROR(LOG_CORE, "PWM POKARITY FILE NOT EXIST\n");
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp = NULL;
    fp = fopen(pwm_file_name, "rw+");
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

int um_set_pwm_enable(int pwmChannel, int isEnable)
{
    char buffer[256] = {0};
    char pwm_file_name[128] = {0};
    (void)memset_s(pwm_file_name, sizeof(pwm_file_name), 0, sizeof(pwm_file_name));

    if (PWM1 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/pwm0/enable", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/pwm0/enable", PWM2_PEX);
    } else {
        HILOG_ERROR(LOG_CORE, "PWM WRONOG CHANEEL\n");
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwm_file_name, F_OK) != 0) {
        HILOG_ERROR(LOG_CORE, "PWM ENABLE FILE NOT EXIST\n");
        return PWM_FILE_NOT_EXIST;
    }

    (void)snprintf_s(buffer, sizeof(buffer), sizeof(buffer), "echo %d > %s", isEnable, pwm_file_name);
    system(buffer);

    return 0;
}

int um_get_pwm_period(int pwmChannel)
{
    int ret = 0;
    char pwm_file_name[128] = {0};
    (void)memset_s(pwm_file_name, sizeof(pwm_file_name), 0, sizeof(pwm_file_name));

    if (PWM1 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/pwm0/period", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/pwm0/period", PWM2_PEX);
    } else {
        HILOG_ERROR(LOG_CORE, "PWM WRONOG CHANEEL\n");
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwm_file_name, F_OK) != 0) {
        HILOG_ERROR(LOG_CORE, "PWM PERIOD FILE NOT EXIST\n");
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp = NULL;
    char buffer[32] = {0};
    (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
    fp = fopen(pwm_file_name, "r");
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

int um_get_pwm_dutyCycle(int pwmChannel)
{
    int ret = 0;
    char pwm_file_name[128] = {0};
    (void)memset_s(pwm_file_name, sizeof(pwm_file_name), 0, sizeof(pwm_file_name));
    if (PWM1 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/pwm0/duty_cycle", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/pwm0/duty_cycle", PWM2_PEX);
    } else {
        HILOG_ERROR(LOG_CORE, "PWM WRONOG CHANEEL\n");
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwm_file_name, F_OK) != 0) {
        HILOG_ERROR(LOG_CORE, "PWM DUTY_CYCLE FILE NOT EXIST\n");
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp = NULL;
    char buffer[32] = {0};
    (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
    fp = fopen(pwm_file_name, "r");
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

int um_get_pwm_polarity(int pwmChannel)
{
    int ret = 0;
    char pwm_file_name[128] = {0};
    (void)memset_s(pwm_file_name, sizeof(pwm_file_name), 0, sizeof(pwm_file_name));

    if (PWM1 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/pwm0/polarity", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/pwm0/polarity", PWM2_PEX);
    } else {
        HILOG_ERROR(LOG_CORE, "PWM WRONOG CHANEEL\n");
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwm_file_name, F_OK) != 0) {
        HILOG_ERROR(LOG_CORE, "PWM POLARITY FILE NOT EXIST\n");
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp = NULL;
    char buffer[32] = {0};
    (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
    fp = fopen(pwm_file_name, "r");
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

int um_is_pwm_enabled(int pwmChannel)
{
    int ret = 0;
    char pwm_file_name[128] = {0};
    (void)memset_s(pwm_file_name, sizeof(pwm_file_name), 0, sizeof(pwm_file_name));

    if (PWM1 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/pwm0/enable", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        (void)sprintf_s(pwm_file_name, sizeof(pwm_file_name), "%s/pwm0/enable", PWM2_PEX);
    } else {
        HILOG_ERROR(LOG_CORE, "PWM WRONOG CHANEEL\n");
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwm_file_name, F_OK) != 0) {
        HILOG_ERROR(LOG_CORE, "PWM ENABLE FILE NOT EXIST\n");
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp = NULL;
    char buffer[32] = {0};
    (void)memset_s(buffer, sizeof(buffer), 0, sizeof(buffer));
    fp = fopen(pwm_file_name, "r");
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