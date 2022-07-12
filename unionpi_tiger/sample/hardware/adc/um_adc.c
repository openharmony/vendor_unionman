#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "um_adc.h"

int get_adc_data(int adc_channel, int *value)
{
    int ret = 0;

    char adc_name[128];
    memset(adc_name, 0, sizeof(adc_name));

    if (ADC_1 == adc_channel) {
        sprintf(adc_name, "%s", ADC_CHANNEL_1);
    } else if (ADC_2 == adc_channel) {
        sprintf(adc_name, "%s", ADC_CHANNEL_2);
    } else if (ADC_HW == adc_channel) {
        sprintf(adc_name, "%s", ADC_CHANNEL_HW);
    } else if (ADC_HP == adc_channel) {
        sprintf(adc_name, "%s", ADC_CHANNEL_HP);
    } else {
        return ADC_PARAM_ERR;
    }

    FILE *fp;
    char buffer[20];
    fp = fopen(adc_name, "r");
    fread(buffer, sizeof(buffer), 1, fp);
    // printf("buffer = %s \n", buffer);
    fclose(fp);

    *value = atoi(buffer);

    return ret;
}
