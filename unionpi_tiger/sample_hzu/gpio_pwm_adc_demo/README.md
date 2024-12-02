#### **项目结构**
\
├─app\entry\src\main
|          |
|          ├─module.json5
|          │
|          ├─ets
|          │  ├─entryability
|          │  │
|          │  └─pages
|          │          gpioled.ets   // 控制GPIO的双色LED相关接口
|          │          gpiovibration.ets // 控制GPIO的震动传感器相关接口
|          │          Index.ets   // 主页面
|          │          pwmled.ets    // 控制PWM的双色LED相关接口
|          └─resources
└─napi
    ├─gpioled_napi
    │      BUILD.gn                      // 构建脚本，配置NAPI模块的构建过程
    │      gpioled_napi.cpp               // C++实现文件，提供gpioLed的原生接口
    │      um_gpio.c                     // GPIO相关的底层操作实现
    │      um_gpio.h                     // GPIO相关的头文件，定义接口和常量
    │
    ├─gpiovibration_napi
    │      BUILD.gn                      // 构建脚本，配置NAPI模块的构建过程
    │      gpiovibration_napi.cpp         // C++实现文件，提供gpioVibration的原生接口
    │      um_gpio.c                     // GPIO相关的底层操作实现
    │      um_gpio.h                     // GPIO相关的头文件，定义接口和常量
    │
    └─pwmled_napi
            BUILD.gn                    // 构建脚本，配置NAPI模块的构建过程
            pwmled_napi.cpp              // C++实现文件，提供pwmLed的原生接口
            um_pwm.c                     // PWM相关的底层操作实现
            um_pwm.h                     // PWM相关的头文件，定义接口和常量
            
### gpioLed、pwmled 和 gpioVibration 接口说明文档

#### **作品简介**

本项目提供了三种硬件控制接口：`gpioLed`、`pwmled` 和 `gpioVibration`，适用于 OpenHarmony 系统上的硬件开发。通过这些接口，用户能够控制 LED 灯的开关状态、调节 PWM 输出的占空比以及检测振动状态。它们为各种物联网设备的开发提供了便捷的硬件控制手段，适合用于智能硬件、传感器监测等场景。

#### **开发环境简介**

- **操作系统**：OpenHarmony 4.0（及以上版本）
- **硬件平台**：支持 GPIO 和 PWM 功能的开发板（例如 UnionMan A311D）
- **开发工具**：DevEco Studio 4.0 及以上版本
- **模块依赖**：OpenHarmony NAPI（Native API）

#### **gpioLed 模块：LED 灯控制**

`gpioLed` 模块提供了控制 LED 灯的功能，开发者可以通过指定 GPIO 引脚设置 LED 灯的开关状态。该模块支持同步和异步两种调用方式，方便不同开发需求。

- **常量**：
  - `LED_ON = 1`：LED 开启
  - `LED_OFF = 0`：LED 关闭

- **函数**：
  - `setLedStatusWithCallback(pin: number, status: number, callback: (result: number) => void): void`  
    设置指定引脚的 LED 灯状态，操作完成后通过回调函数处理结果。
  
  - `setLedStatusWithPromise(pin: number, status: number): Promise<number>`  
    设置指定引脚的 LED 灯状态，返回一个 Promise，操作成功时返回 `1`，失败时返回错误代码。
  
  - `getLedStatusWithCallback(pin: number, callback: (result: number) => void): void`  
    获取指定引脚的 LED 灯当前状态，通过回调函数处理返回结果。
  
  - `getLedStatusWithPromise(pin: number): Promise<number>`  
    获取指定引脚的 LED 灯当前状态，返回一个 Promise。

#### **pwmled 模块：PWM 控制**

`pwmled` 模块用于控制设备的 PWM 引脚，开发者可以设置 PWM 的占空比（0%-100%）来调节设备的输出，例如调节 LED 的亮度或电机的速度。

- **常量**：
  - `pwmPin1 = 1`：第一个 PWM 引脚
  - `pwmPin2 = 2`：第二个 PWM 引脚

- **函数**：
  - `setPwmStatus(pwmPex: number, dutyCycle: number): Promise<number>`  
    设置指定 PWM 引脚的占空比，返回一个 Promise，操作成功时返回 `1`，失败时返回错误代码。

#### **gpioVibration 模块：振动检测**

`gpioVibration` 模块用于检测振动传感器的状态。通过读取指定 GPIO 引脚的振动状态，开发者可以获取设备是否发生振动，适用于智能家居、工业自动化等应用场景。

- **常量**：
  - `Vibration_Not_Detected = 1`：未检测到振动
  - `Vibration_Detected = 0`：检测到振动

- **函数**：
  - `getVibrationStatus(pin: number, callback: (result: number) => void): void`  
    获取指定引脚的振动状态，通过回调函数处理返回结果。

  - `getVibrationStatus(pin: number): Promise<number>`  
    获取指定引脚的振动状态，返回一个 Promise。

#### **接口说明（TypeScript 接口定义文件）**

为了方便开发者在 OpenHarmony 应用中调用这些接口，我们提供了 TypeScript 接口定义文件：

```ts
declare namespace gpioLed {
    const LED_ON: number;
    const LED_OFF: number;

    function setLedStatusWithCallback(pin: number, status: number, callback: (result: number) => void): void;
    function setLedStatusWithPromise(pin: number, status: number): Promise<number>;
    function setLedStatus(pin: number, status: number, callback: (result: number) => void): void;
    function setLedStatus(pin: number, status: number): Promise<number>;
    function getLedStatusWithCallback(pin: number, callback: (result: number) => void): void;
    function getLedStatusWithPromise(pin: number): Promise<number>;
    function getLedStatus(pin: number, callback: (result: number) => void): void;
    function getLedStatus(pin: number): Promise<number>;
}

declare namespace pwmled {
    const pwmPin1: number;
    const pwmPin2: number;
    function setPwmStatus(pwmPex: number, dutyCycle: number): Promise<number>;
}

declare namespace gpioVibration {
    const Vibration_Not_Detected: number;
    const Vibration_Detected: number;

    function getVibrationStatus(pin: number, callback: (result: number) => void): void;
    function getVibrationStatus(pin: number): Promise<number>;
}

export default gpioLed;
export default pwmled;
export default gpioVibration;
```

#### 5.修改系统配置文件

修改系统权限，使 APP 能够使用开发板 gpio，pwm，i2c，adc 的系统文件

修改`device\board\unionman\unionpi_tiger\config\init\arm\init.A311D.cfg `文件，在cmds中添加相关命令

```
"write /sys/class/gpio/export 380",
"write /sys/class/gpio/export 381",
"write /sys/class/gpio/export 382",
"write /sys/class/gpio/export 383",
"write /sys/class/gpio/export 384",
"write /sys/class/gpio/export 385",
"write /sys/class/gpio/export 386",
"write /sys/class/gpio/export 387",
"write /sys/class/gpio/export 388",
"write /sys/class/gpio/export 389",
"chmod 666 /sys/class/gpio/gpio380/direction",
"chmod 666 /sys/class/gpio/gpio381/direction",
"chmod 666 /sys/class/gpio/gpio382/direction",
"chmod 666 /sys/class/gpio/gpio383/direction",
"chmod 666 /sys/class/gpio/gpio384/direction",
"chmod 666 /sys/class/gpio/gpio385/direction",
"chmod 666 /sys/class/gpio/gpio386/direction",
"chmod 666 /sys/class/gpio/gpio387/direction",
"chmod 666 /sys/class/gpio/gpio388/direction",
"chmod 666 /sys/class/gpio/gpio389/direction",
"chmod 666 /sys/class/gpio/gpio380/value",
"chmod 666 /sys/class/gpio/gpio381/value",
"chmod 666 /sys/class/gpio/gpio382/value",
"chmod 666 /sys/class/gpio/gpio383/value",
"chmod 666 /sys/class/gpio/gpio384/value",
"chmod 666 /sys/class/gpio/gpio385/value",
"chmod 666 /sys/class/gpio/gpio386/value",
"chmod 666 /sys/class/gpio/gpio387/value",
"chmod 666 /sys/class/gpio/gpio388/value",
"chmod 666 /sys/class/gpio/gpio389/value",
"write /sys/class/pwm/pwmchip0/export 0",
"write /sys/class/pwm/pwmchip2/export 0",
"chmod 666 /sys/class/pwm/pwmchip0/pwm0/duty_cycle",
"chmod 666 /sys/class/pwm/pwmchip0/pwm0/period",
"chmod 666 /sys/class/pwm/pwmchip0/pwm0/enable",
"chmod 666 /sys/class/pwm/pwmchip0/pwm0/polarity",
"chmod 666 /sys/class/pwm/pwmchip2/pwm0/duty_cycle",
"chmod 666 /sys/class/pwm/pwmchip2/pwm0/period",
"chmod 666 /sys/class/pwm/pwmchip2/pwm0/enable",
"chmod 666 /sys/class/pwm/pwmchip2/pwm0/polarity"
```

#### **构建和部署**

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

#### **演示效果**

| 功能                         | 操作演示                          |
| ---------------------------- | --------------------------------- |
| **LED 灯开关控制**            | 控制指定引脚上的 LED 灯的开关状态   |
| **PWM 控制**                  | 调节 LED 或其他设备的亮度或速度    |
| **振动检测**                  | 检测指定引脚是否发生振动           |


通过这些接口，开发者可以方便地实现 LED 灯、PWM 控制和振动检测等硬件功能，满足各种物联网应用的需求。