#include <stdio.h>
#include <securec.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define ADC_CHANNEL_1 "/sys/bus/iio/devices/iio:device0/in_voltage2_raw" // ADC1的文件路径
#define ADC_CHANNEL_2 "/sys/bus/iio/devices/iio:device0/in_voltage3_raw" // ADC1的文件路径

#define TEMP_CONST 0.042
#ifdef __cplusplus
extern "C" {
#endif
    int get_adc_data(int channel, int *value)
    {
        char adc_path[128];
        (void)memset_s(adc_path, sizeof(adc_path), 0, sizeof(adc_path)); // 数组清空
        if(channel == 1)
        {
            sprintf_s(adc_path, sizeof(adc_path), "%s", ADC_CHANNEL_1);
        }else{
            sprintf_s(adc_path, sizeof(adc_path), "%s", ADC_CHANNEL_2);
        }

        FILE *fp = NULL;           // 声明一个指向文件的指针
        char buffer[sizeof(int)];  // 从文件中读取到的数据
        fp = fopen(adc_path, "r"); // 只读打开文件
        if (!fp)
        {
            perror("fopen"); // 提示报错信息
            return -1;
        }
        fread(buffer, sizeof(buffer), 1, fp); // 读取数据的个数为1
        fclose(fp);
        *value = atoi(buffer);
        return 0;
    }
#ifdef __cplusplus
    }
#endif

