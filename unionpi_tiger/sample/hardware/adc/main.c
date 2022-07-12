#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "um_adc.h"

int main(int argc, char **argv)
{
    int adc_channel = ADC_1; // ADC_2
    int value;

    get_adc_data(adc_channel, &value);

    printf("adc:[%d]\n", value);

    return 0;
}
