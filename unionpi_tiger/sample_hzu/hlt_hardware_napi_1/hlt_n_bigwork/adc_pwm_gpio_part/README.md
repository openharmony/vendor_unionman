# hlt_napi_adc_pwm_gpio 硬件控制接口说明文档

## 项目结构

```
unionpi_tiger/
└─ sample_hzu/
   └─ hlt_hardware_napi/
      └─ hlt_n_bigwork/
         ├─ main.c            // 主程序文件，包含硬件控制的主要逻辑
         ├─ adc.h             // ADC模块头文件，定义了ADC相关的函数
         ├─ pwm.h             // PWM模块头文件，定义了PWM相关的函数
         ├─ gpio.h            // GPIO模块头文件，定义了GPIO相关的函数
         └─ utils.h           // 工具函数头文件，提供通用的辅助功能
```

## 作品简介

本项目提供了一个硬件控制的示例，适用于 OpenHarmony 系统上的开发板。通过该项目，用户可以学习如何使用 C 语言控制硬件设备，包括读取模拟输入、控制LED灯的亮度和颜色等功能。此作品就是使用双轴XY摇杆控制三色灯颜色的变化，其中X和Y轴可以控制红色灯和蓝色灯的混色变化，Z轴按键可以控制绿色灯的变化。

## 功能说明

### 主程序功能

`main.c` 文件是项目的核心，负责初始化硬件设备并调用各模块的功能。主要功能包括：

- 初始化硬件设备
- 读取摇杆的模拟输入值
- 控制LED灯的红蓝混色变化以及绿色的亮灭
- 监控按键状态
### 三色LED灯控制

- **函数**：`setRGB(int red, int blue, int green)`
  - **功能**：设置三色LED灯的颜色。通过PWM信号调节红色和蓝色LED的亮度，并通过GPIO控制绿色LED的开关。
  - **参数**：
    - `red`：红色LED的PWM占空比。
    - `blue`：蓝色LED的PWM占空比。
    - `green`：绿色LED的GPIO状态，1为亮，0为灭。

### 摇杆输入读取

- **函数**：`get_joystick_values(int *x, int *y, int *z)`
  - **功能**：读取双轴XY摇杆的模拟输入值和Z轴按键状态。
  - **参数**：
    - `x`：指向存储X轴模拟值的指针。
    - `y`：指向存储Y轴模拟值的指针。
    - `z`：指向存储Z轴按键状态的指针，0表示按下，1表示松开。

## 逻辑流程

1. **初始化**：
   - 初始化GPIO和PWM模块。
   - 设置PWM周期和极性，并使能PWM。

2. **主循环**：
   - 不断读取摇杆的X、Y、Z轴值。
   - 根据Z轴按键状态控制绿色LED：
     - 如果按下（z == 0），点亮绿色LED，关闭红色和蓝色LED。
     - 如果未按下（z == 1），根据X、Y轴值调整红色和蓝色LED的亮度，实现混色效果。

3. **清理资源**：
   - 停止时关闭所有LED。
   - 禁用PWM并释放GPIO资源。


### ADC 模块

`adc.h` 文件定义了与模拟数字转换（ADC）相关的函数，用于读取摇杆的X、Y轴值。

- **函数**：
  - `int readJoystickX();`  
    读取摇杆的X轴模拟值，返回一个整数表示当前的输入值。
  - `int readJoystickY();`  
    读取摇杆的Y轴模拟值，返回一个整数表示当前的输入值。

### PWM 模块

`pwm.h` 文件定义了脉宽调制（PWM）相关的函数，用于控制LED灯的亮度和颜色。

- **函数**：
  - `void setLedColor(int red, int blue);`  
    设置LED灯的红色和蓝色亮度，通过调整PWM占空比实现。
  - `void setLedBrightness(int brightness);`  
    设置LED灯的整体亮度。

### GPIO 模块

`gpio.h` 文件定义了通用输入输出（GPIO）相关的函数，用于控制LED灯的开关和读取按键状态。

- **函数**：
  - `void turnOnGreenLed();`  
    打开绿灯。
  - `void turnOffGreenLed();`  
    关闭绿灯。
  - `int getButtonStatus();`  
    获取按键的当前状态，返回0或1。

### 工具函数

`utils.h` 文件提供了一些通用的工具函数，帮助简化主程序的实现。

- **函数**：
  - `void delay(int milliseconds);`  
    实现简单的延时功能。

## 构建和部署

1. **编译**  
   在 OpenHarmony 源代码根目录下执行以下命令进行编译：

   ```bash
   ./build.sh --product-name unionpi_tiger --ccache
   ```

2. **部署**  
   如果开发板已经烧录了相应的 OpenHarmony 操作系统，可以通过以下命令将编译后的动态库发送到开发板：

   ```bash
   hdc_std file send libyour_module.z.so /system/lib/module
   ```

3. **开发者调试**  
   使用 DevEco Studio 打开项目，进行代码调试和硬件控制。

## 演示效果

| 功能         | 操作演示                          |
| ------------ | --------------------------------- |
| **PWM 控制** | 调节LED灯的蓝色和红色混色效果     |
| **GPIO 控制**| 控制绿灯的亮灭状态，获取按键的状态|
| **ADC 读取** | 读取双轴XY摇杆的X、Y轴模拟值     |