# hlt_napi_adc_pwm_gpio_2 硬件控制接口说明文档

## 项目结构

```
unionpi_tiger/
└─ sample_hzu/
   └─ hlt_hardware_napi_2/
      ├─ adc_gpio_napi_part/
      │  ├─ adc_gpio.cpp       // C++实现文件，获取双轴XY摇杆的X、Y值
      │  ├─ gpio.cpp           // C++实现文件，获取Z轴按键状态
      │  └─ bundle.json        // ADC和GPIO模块的配置文件
      │  └─ @ohos.adc_gpio_napi.d.ts // TypeScript接口定义文件，提供ADC和GPIO接口
```

## 作品简介

本项目提供了硬件控制接口，适用于 OpenHarmony 系统上的硬件开发。通过这些接口，用户能够控制LED灯的颜色和亮度，以及读取双轴XY摇杆的模拟输入值和Z轴按键状态。它们为各种物联网设备的开发提供了便捷的硬件控制手段，适合用于智能硬件、传感器监测等场景。

## adc_gpio 模块：模拟输入读取和 GPIO 控制

`adc_gpio` 模块用于读取双轴XY摇杆的模拟输入值，并通过 GPIO 获取Z轴按键的状态。

- **函数**：
  - `sync_get_joystick_values(): {x: number;y: number;z: number;};`  
    获取双轴XY摇杆的X轴和Y轴模拟值，以及获取Z轴按键的状态，返回0或1。
    `sync_get_permission(): number;`
    同步检查是否具有访问 ADC 设备的权限。返回 0 表示权限通过，非 0 值表示失败。

## 接口说明（TypeScript 接口定义文件）

为了方便开发者在 OpenHarmony 应用中调用这些接口，我们提供了 TypeScript 接口定义文件：

```typescript
declare namespace adc_gpio_napi {
    function getJoystickXValue(): number;
    function getJoystickYValue(): number;
    function getZButtonStatus(): number;
}

export default adc_gpio_napi;
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
| **GPIO 控制**| 在ADC中获取Z轴按键的状态|
| **ADC 读取** | 读取双轴XY摇杆的X、Y轴模拟值|
