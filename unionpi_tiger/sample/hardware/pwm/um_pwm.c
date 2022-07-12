#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "um_pwm.h"

int set_pwm_enable(int pwmChannel, int isEnable)
{
    int ret = 0;

    char buffer[256];
    // pwmchannel
    char pwm_file_name[128];

    memset(pwm_file_name, 0, sizeof(pwm_file_name));
    if (PWM1 == pwmChannel) {
        sprintf(pwm_file_name, "%s/enabled", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        sprintf(pwm_file_name, "%s/enabled", PWM2_PEX);
    } else {
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwm_file_name, F_OK) != 0) {
        return PWM_FILE_NOT_EXIST;
    }

    snprintf(buffer, sizeof(buffer), "echo %d > %s", isEnable, pwm_file_name);

    system(buffer);

    return ret;
}

int set_pwm_period(int pwmChannel, long period)
{
    int ret = 0;

    // pwmchannel
    char pwm_file_name[128];
    memset(pwm_file_name, 0, sizeof(pwm_file_name));
    if (PWM1 == pwmChannel) {
        sprintf(pwm_file_name, "%s/period", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        sprintf(pwm_file_name, "%s/period", PWM2_PEX);
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
    fclose(fp);
    return ret;
}

int get_pwm_dutyCycle(int pwmChannel, long *value)
{
    int ret = 0;

    char pwm_file_name[128];
    memset(pwm_file_name, 0, sizeof(pwm_file_name));
    if (PWM1 == pwmChannel) {
        sprintf(pwm_file_name, "%s/duty_cycle", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        sprintf(pwm_file_name, "%s/duty_cycle", PWM2_PEX);
    } else {
        return PWM_WRONOG_CHANNEL;
    }

    // printf("get_pwm_dutyCycle pwmchannel = %s", pwm_file_name);
    if (access(pwm_file_name, F_OK) != 0) {
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp;
    char buffer[32];
    memset(buffer, 0, sizeof(buffer));
    fp = fopen(pwm_file_name, "r");
    fread(buffer, sizeof(buffer), 1, fp);

    fclose(fp);
    printf("get_pwm_dutyCycle = %s\n", buffer);
    *value = atol(buffer);

    return ret;
}

int get_pwm_period(int pwmChannel, long *value)
{
    int ret = 0;

    char pwm_file_name[128];
    memset(pwm_file_name, 0, sizeof(pwm_file_name));
    if (PWM1 == pwmChannel) {
        sprintf(pwm_file_name, "%s/period", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        sprintf(pwm_file_name, "%s/period", PWM2_PEX);
    } else {
        return PWM_WRONOG_CHANNEL;
    }

    // printf("get_pwm_period pwmchannel = %s", pwm_file_name);
    if (access(pwm_file_name, F_OK) != 0) {
        return PWM_FILE_NOT_EXIST;
    }

    FILE *fp;
    char buffer[32];
    memset(buffer, 0, sizeof(buffer));
    fp = fopen(pwm_file_name, "r");
    fread(buffer, sizeof(buffer), 1, fp);

    fclose(fp);
    *value = atol(buffer);

    return ret;
}

int set_pwm_dutyCycle(int pwmChannel, long dutyCycle)
{
    int ret = 0;

    // pwmchannel
    char pwm_file_name[128];
    memset(pwm_file_name, 0, sizeof(pwm_file_name));
    if (PWM1 == pwmChannel) {
        sprintf(pwm_file_name, "%s/duty_cycle", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        sprintf(pwm_file_name, "%s/duty_cycle", PWM2_PEX);
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
    fclose(fp);
    return ret;
}

int is_pwm_enabled(int pwmChannel, int *value)
{
    int ret = 0;

    // check pwm enabled or not
    char pwm_file_name[128];
    memset(pwm_file_name, 0, sizeof(pwm_file_name));
    if (PWM1 == pwmChannel) {
        sprintf(pwm_file_name, "%s/enabled", PWM1_PEX);
        printf("pwmchannel = %s", pwm_file_name);
    } else if (PWM2 == pwmChannel) {
        sprintf(pwm_file_name, "%s/enabled", PWM2_PEX);
        printf("pwmchannel = %s\n", pwm_file_name);
    } else {
        return PWM_WRONOG_CHANNEL;
    }

    if (access(pwm_file_name, F_OK) != 0) {
        return PWM_FILE_NOT_EXIST;

    } else {
        //*value = PWM_IS_ENABLED;
        FILE *fp;
        char buffer[32];
        memset(buffer, 0, sizeof(buffer));
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
    memset(pwm_file_name, 0, sizeof(pwm_file_name));
    if (PWM1 == pwmChannel) {
        sprintf(pwm_file_name, "%s/polarity", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        sprintf(pwm_file_name, "%s/polarity", PWM2_PEX);
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

    fclose(fp);
    return ret;
}
int get_pwm_polarity(int pwmChannel, int *value)
{
    int ret = 0;

    // pwmchannel
    char pwm_file_name[128];
    memset(pwm_file_name, 0, sizeof(pwm_file_name));
    if (PWM1 == pwmChannel) {
        sprintf(pwm_file_name, "%s/polarity", PWM1_PEX);
    } else if (PWM2 == pwmChannel) {
        sprintf(pwm_file_name, "%s/polarity", PWM2_PEX);
    } else {
        return PWM_WRONOG_CHANNEL;
    }
    if (access(pwm_file_name, F_OK) != 0) {
        return PWM_FILE_NOT_EXIST;
    }
    FILE *fp;
    char buffer[32];
    memset(buffer, 0, sizeof(buffer));
    fp = fopen(pwm_file_name, "r");
    fread(buffer, sizeof(buffer), 1, fp);
    fclose(fp);

    if (strstr(buffer, "normal") != NULL) {
        *value = PWM_POLARITY_NORMAL;
    } else if (strstr(buffer, "inversed") != NULL) {
        *value = PWM_POLARITY_INVERSED;

    } else {
        ret = PWM_ERR;
    }

    return ret;
}
