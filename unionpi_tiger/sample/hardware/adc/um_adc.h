#ifndef __UM_ADC_H__
#define __UM_ADC_H__

// errno
#define ADC_PARAM_ERR -1
#define ADC_ERR -2

#define ADC_CHANNEL_1 "/sys/bus/iio/devices/iio:device0/in_voltage2_input"
#define ADC_CHANNEL_2 "/sys/bus/iio/devices/iio:device0/in_voltage3_input"
#define ADC_CHANNEL_HW "/sys/bus/iio/devices/iio:device0/in_voltage1_input"
#define ADC_CHANNEL_HP "/sys/bus/iio/devices/iio:device0/in_voltage0_input"

// channel
#define ADC_1 2
#define ADC_2 3
#define ADC_HW 1
#define ADC_HP 0

int get_adc_data(int adc_channel, int *value);

#endif // endif __UM_ADC_H__
