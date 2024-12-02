# hlt_napi_adc_pwm_gpio_3 硬件控制接口说明文档

## 项目结构

```
unionpi_tiger/
└─ sample_hzu/
   └─ hlt_hardware_napi_3/
      ├─ pwm_gpio_napi_part/
      │  ├─ pwm_gpio.cpp       // C++实现文件，设置LED灯的蓝色和红色混色
      │  ├─ gpio.cpp           // C++实现文件，控制绿灯的亮灭
      │  └─ bundle.json        // PWM和GPIO模块的配置文件
      │  └─@ohos.pwm_gpio_napi.d.ts // TypeScript接口定义文件，提供PWM和GPIO接口
```

## 作品简介

本项目提供了硬件控制接口，适用于 OpenHarmony 系统上的硬件开发。通过这些接口，用户能够控制LED灯的颜色和亮度，以及读取双轴XY摇杆的模拟输入值和Z轴按键状态。它们为各种物联网设备的开发提供了便捷的硬件控制手段，适合用于智能硬件、传感器监测等场景。

## pwm_gpio 模块：PWM 和 GPIO 控制

`pwm_gpio` 模块用于控制设备的 PWM 和 GPIO 引脚，开发者可以设置 PWM 的占空比来调节LED灯的蓝色和红色混色程度，并通过 GPIO 控制绿灯的亮灭。

- **函数**：
  - `setPwmGpioStatus(redDuty: number, blueDuty: number, greenLedOn: number): Promise<number>;`  
    设置蓝色和红色LED灯的PWM占空比，控制混色效果，以及设置绿灯的状态，1为亮，0为灭。


## 接口说明（TypeScript 接口定义文件）

为了方便开发者在 OpenHarmony 应用中调用这些接口，我们提供了 TypeScript 接口定义文件：

```typescript
declare namespace pwm_gpio_napi {
    function setPwmDutyCycle(blueDuty: number, redDuty: number): Promise<number>;
    function setGreenLedStatus(status: number): Promise<number>;
}


export default pwm_gpio_napi;
```

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
   使用 DevEco Studio 打开项目，添加自定义的 TypeScript 接口定义文件（如 `.d.ts` 文件）。在代码中调用相应的接口进行硬件控制。

## 演示效果

| 功能         | 操作演示                          |
| ------------ | --------------------------------- |
| **PWM 控制** | 调节LED灯的蓝色和红色混色效果     |
| **GPIO 控制**| 在PWM中控制绿灯的亮灭状态|
