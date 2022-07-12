#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "um_gpio.h"

int UM_GPIO_Export(int s32GpioNum, int bExport)
{
    int ret = 0;
    char buffer[256];

    if (bExport) {
        snprintf(buffer, sizeof(buffer), "echo %d > %s", s32GpioNum, UM_GPIO_EXPORT);
    } else {
        snprintf(buffer, sizeof(buffer), "echo %d > %s", s32GpioNum, UM_GPIO_UNEXPORT);
    }

    system(buffer);

    return ret;
}

int UM_GPIO_SetDirection(int s32GpioNum, int direction)
{
    int ret = 0;

    // check gpio export or not
    char gpio_file_name[128];
    memset(gpio_file_name, 0, sizeof(gpio_file_name));
    sprintf(gpio_file_name, "%s%d/direction", UM_GPIO_PEX, s32GpioNum);

    if (access(gpio_file_name, F_OK) != 0) {
        return UM_GPIO_NOT_EXPROT_ERROR;
    }
    // set gpio direction

    FILE *fp;
    fp = fopen(gpio_file_name, "r+");

    if (direction == UM_GPIO_IN) {
        fprintf(fp, "%s", "in");

    } else if (direction == UM_GPIO_OUT) {
        fprintf(fp, "%s", "out");
    }

    fclose(fp);

    return ret;
}

int UM_GPIO_SetValue(int s32GpioNum, int s32Value)
{
    int ret = 0;

    // check gpio export or not
    char gpio_file_name[128];
    memset(gpio_file_name, 0, sizeof(gpio_file_name));
    sprintf(gpio_file_name, "%s%d/value", UM_GPIO_PEX, s32GpioNum);

    if (access(gpio_file_name, F_OK) != 0) {
        return UM_GPIO_NOT_EXPROT_ERROR;
    }
    // set gpio s32Value

    FILE *fp;
    fp = fopen(gpio_file_name, "r+");

    if (s32Value == UM_GPIO_LOW_LEVE) {
        fprintf(fp, "%s", "0");

    } else if (s32Value == UM_GPIO_HIGH_LEVE) {
        fprintf(fp, "%s", "1");
    }

    fclose(fp);

    return ret;
}

int UM_GPIO_IsExport(int s32GpioNum, int *ps32Value)
{
    int ret = 0;

    // check gpio export or not
    char gpio_file_name[128];
    memset(gpio_file_name, 0, sizeof(gpio_file_name));
    sprintf(gpio_file_name, "%s%d/value", UM_GPIO_PEX, s32GpioNum);

    if (access(gpio_file_name, F_OK) != 0) {
        *ps32Value = UM_GPIO_NOT_EXPORT;
    } else {
        *ps32Value = UM_GPIO_EXPORTED;
    }

    return ret;
}

int UM_GPIO_GetDirection(int s32GpioNum, int *ps32Value)
{
    int ret = 0;

    // check gpio export or not
    char gpio_file_name[128];
    memset(gpio_file_name, 0, sizeof(gpio_file_name));
    sprintf(gpio_file_name, "%s%d/direction", UM_GPIO_PEX, s32GpioNum);

    if (access(gpio_file_name, F_OK) != 0) {
        return UM_GPIO_NOT_EXPROT_ERROR;
    }

    FILE *fp;
    char buffer[20];
    fp = fopen(gpio_file_name, "r");
    fread(buffer, sizeof(buffer), 1, fp);
    // printf("buffer = %s \n", buffer);
    fclose(fp);
    if (strstr(buffer, "out") != NULL) {
        *ps32Value = UM_GPIO_OUT;
    } else if (strstr(buffer, "in") != NULL) {
        *ps32Value = UM_GPIO_IN;

    } else {
        ret = UM_GPIO_ERR;
    }
    return ret;
}

int UM_GPIO_GetValue(int s32GpioNum, int *ps32Value)
{
    int ret = 0;

    // check gpio export or not
    char gpio_file_name[128];
    memset(gpio_file_name, 0, sizeof(gpio_file_name));
    sprintf(gpio_file_name, "%s%d/s32Value", UM_GPIO_PEX, s32GpioNum);

    if (access(gpio_file_name, F_OK) != 0) {
        return UM_GPIO_NOT_EXPROT_ERROR;
    }

    FILE *fp;
    char buffer[20];
    fp = fopen(gpio_file_name, "r");
    fread(buffer, sizeof(buffer), 1, fp);

    fclose(fp);
    if (strstr(buffer, "0") != NULL) {
        *ps32Value = UM_GPIO_LOW_LEVE;
    } else if (strstr(buffer, "1") != NULL) {
        *ps32Value = UM_GPIO_HIGH_LEVE;

    } else {
        ret = UM_GPIO_ERR;
    }
    return ret;
}
