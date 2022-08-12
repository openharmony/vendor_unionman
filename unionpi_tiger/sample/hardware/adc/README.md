# 读取ADC并转化为温度demo

## ADC参数

详见`um_adc.h`：

```c
#define ADC_1	2
#define ADC_2	3
#define ADC_CHANNEL_1	"/sys/bus/iio/devices/iio:device0/in_voltage2_raw"
#define ADC_CHANNEL_2	"/sys/bus/iio/devices/iio:device0/in_voltage3_raw"
```

## 获取ADC接口：

详见`um_adc.c：

```c
int get_adc_data(int adc_channel, int * value);
```

## 外设

[LM35线性模拟温度传感器](https://wiki.dfrobot.com.cn/_SKU_DFR0023_LM35%E7%BA%BF%E6%80%A7%E6%A8%A1%E6%8B%9F%E6%B8%A9%E5%BA%A6%E4%BC%A0%E6%84%9F%E5%99%A8)

![](../figures/adc_DFRobot_LM35.jpg)

## 连线

- 红线 -- 3.3V
- 黑线 -- GND
- 蓝线 -- ADC_1

连线实物图

![](../figures/adc_demo.png)
