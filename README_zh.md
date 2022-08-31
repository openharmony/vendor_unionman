# 九联科技-OpenHarmony开发板（Unionpi_Tiger）

## Unionpi_Tiger开发板介绍

九联科技Unionpi Tiger是一款应用于图像处理，音视频处理和深度学习等场景的智能硬件，其主芯片采用Amlogic A311D芯片方案。 A311D支持GPU和神经网络加速子系统，支持4K视频编解码器引擎和一流的HDR图像处理，并集成了所有标准音频/视频输入/输出接口。主系统的CPU采用大小核设计，主频高达2.2GHz，集成了四个Cortex-A73核心和两Cortex-A53核心 ，集成独立的5.0T NPU处理器。

## 约束

支持Unionpi_Tiger (Amlogic A311D)。

## 目录

```
vendor/unionman/          # vendor_unionman 仓的仓库路径
├── LICENSE
├── OAT.xml
├── README_zh.md
└── unionpi_tiger         # Unionpi Tiger开发板目录
    ├── audio             # audio配置目录
    ├── config.json       # 产品模块详细配置目录
    ├── hals              # hal层相关配置目录
    ├── hdf_config        # hdf相关hcs目录
    ├── power_config      # 电源配置目录
    ├── sample			  # 样例目录
    ├── product.gni
    └── resourceschedule
```

## 开发环境搭建

Unionpi Tiger使用参考 [使用教程](https://gitee.com/openharmony/device_board_unionman/blob/master/unionpi_tiger/README_zh.md)

## 样例导航

基础样例列表如下

|  No  | 样例名称                  | 路径                                                         |
| :--: | ------------------------- | ------------------------------------------------------------ |
|  1   | GPIO控制                  | [unionpi_tiger/sample/hardware/gpio](https://gitee.com/openharmony/vendor_unionman/tree/master/unionpi_tiger/sample/hardware/gpio) |
|  2   | PWM舵机调试               | [unionpi_tiger/sample/hardware/pwm](https://gitee.com/openharmony/vendor_unionman/tree/master/unionpi_tiger/sample/hardware/pwm) |
|  3   | ADC设备读取采样数据       | [unionpi_tiger/sample/hardware/adc](https://gitee.com/openharmony/vendor_unionman/tree/master/unionpi_tiger/sample/hardware/adc) |
|  4   | I2C读写及温湿度传感器使用 | [unionpi_tiger/sample/hardware/i2c](https://gitee.com/openharmony/vendor_unionman/tree/master/unionpi_tiger/sample/hardware/i2c) |
|  5   | SPI读写                   | [unionpi_tiger/sample/hardware/spi](https://gitee.com/openharmony/vendor_unionman/tree/master/unionpi_tiger/sample/hardware/spi) |

进阶样例列表如下

|  No  | 样例名称                         | 路径                                                         |
| :--: | :------------------------------- | ------------------------------------------------------------ |
|  1   | HAP应用——控制GPIO                | [unionpi_tiger/sample/app/gpioled_app](https://gitee.com/openharmony/vendor_unionman/tree/master/unionpi_tiger/sample/app/gpioled_app) |
|  2   | HAP应用——控制舵机                | [unionpi_tiger/sample/app/pwm_app](https://gitee.com/openharmony/vendor_unionman/tree/master/unionpi_tiger/sample/app/pwm_app) |
|  3   | HAP应用——读取ADC采样数据         | [unionpi_tiger/sample/app/adc_app](https://gitee.com/openharmony/vendor_unionman/tree/master/unionpi_tiger/sample/app/adc_app) |
|  4   | HAP应用——I2C读取温湿度传感器数据 | [unionpi_tiger/sample/app/i2c_app](https://gitee.com/openharmony/vendor_unionman/tree/master/unionpi_tiger/sample/app/i2c_app) |
|  5   | HAP应用——SPI读写                 | [unionpi_tiger/sample/app/spi_app](https://gitee.com/openharmony/vendor_unionman/tree/master/unionpi_tiger/sample/app/spi_app) |

## 参与贡献

[如何参与](https://gitee.com/openharmony/docs/blob/HEAD/zh-cn/contribute/%E5%8F%82%E4%B8%8E%E8%B4%A1%E7%8C%AE.md)

[Commit message规范](https://gitee.com/openharmony/device_qemu/wikis/Commit%20message%E8%A7%84%E8%8C%83?sort_id=4042860)

## 相关仓

- vendor_unionman

- [device_board_unionman](https://gitee.com/openharmony/device_board_unionman)

- [device_soc_amlogic](https://gitee.com/openharmony/device_soc_amlogic)
