##  智能家居中控

### 样例介绍

基于OpenHarmony NAPI框架实现北向应用端控制南向设备实现智能家居功能

###  开发环境

搭载OpenHarmony的Unionpi_Tiger开发板

DevEco Studio

Ubuntu20.04虚拟机

USB_Burning_Tool烧录工具

### 工程目录

```
\entry\src\main\ets
├─entryability
|   └─EntryAbility.ts
|
├─image 							  //页面截图
|   ├─alarm.png
|   ├─curtain.png
|   ├─fan.png
|   └─index.png
|   └─light.png
|
├─pages
|   ├─alarm.ets					      //闹钟控制页面
|   ├─curtain.ets                     //窗帘控制页面
|   ├─fan.ets                         //风扇控制页面
|   ├─index.ets                       //主页面
|   └─light.ets                       //灯光控制
|
├─ServiceExtAbility
    └─ServiceExtAbility.ts

```

### NAPI接口实现

NAPI接口封装在`unionman\unionpi_Tiger\sample\app\gpioled_app\README.md` 中有详细介绍及说明。

本次智能家居中控系统，LED，舵机，直流电机（与TB6612FNG电机驱动模块配合使用）外设接口采用Promise异步封装，温度传感器，光敏电阻传感器使用同步封装。

#### **1.创建子系统目录**

在OpenHarmony源码目录`unionpi_tiger/sample/napi/subsys`下创建一个目录作为子系统目录。如：`gpioled_part`，`pwmnapipart`，`adc_component`。

#### 2.在子系统目录下新增组件

在子系统目录下创建相应的子组件目录，如`gpioled_demo` ，`pwmnapidemo`。

#### 3.新增拓展动态库

将子组件目录作为 NAPI扩展库的代码目录，在子组件目录下创建文件代码。

以GPIO为例在`gpioled_demo` 目录下创建代码文件`gpioled.cpp`。

在`gpioled_demo` 目录下创建`BUILD.gn`文件，编写构建配置。

```
import("//build/ohos.gni")

ohos_shared_library("gpioled") {
  include_dirs = [ "//foundation/ace/napi/interfaces/kits" ]

  sources = [
    "gpioled.cpp",
    "um_gpio.c",
  ]

 # deps = [ "//foundation/arkui/napi:ace_napi" ]

  external_deps = [
    "c_utils:utils",
    "hilog:libhilog",
    "napi:ace_napi",
  ]


  part_name = "product_unionpi_tiger"
}
```

#### 4.修改编译相关配置信息
由于OHOS版本更新导致子系统采用新文件`bundle.json`管理，流程如下。

修改`ohos4.0release/vendor/unionman/unionpi_tiger/bundle.json`文件，在`"deps"`中添加`"napi"`
```
"deps": {
        "components": [
          "hilog",
          "c_utils",
          "hdf_core",
          "napi"
        ],
```


修改`ohos4.0release/vendor/unionman/unionpi_tiger/sample/BUILD.gn`文件，添加组件依赖。
```
import("//build/ohos.gni")

group("sample_group") {
  deps = [
     "hardware:hardware",
      # "hdf:hdf",
      "napi/napisubsys/gpioled_part/gpioled_demo:gpioled",
      "napi/napisubsys/pwmnapipart/pwmnapidemo:pwmtest",
      "napi/napisubsys/adc_component:adc_napi"
  ]
}
```

5.修改系统配置文件

修改系统权限，使 应用 能够使用开发板 gpio，pwm，adc 的系统文件

修改`device\board\unionman\unionpi_tiger\config\init\arm\init.A311D.cfg `文件，在cmds中添加相关命令

```
"write /sys/class/gpio/export 384",
"write /sys/class/gpio/export 385",
"write /sys/class/gpio/export 386",
"write /sys/class/gpio/export 387",
"write /sys/class/gpio/export 388",
"chmod 666 /sys/class/gpio/gpio384/direction",
"chmod 666 /sys/class/gpio/gpio385/direction",
"chmod 666 /sys/class/gpio/gpio386/direction",
"chmod 666 /sys/class/gpio/gpio387/direction",
"chmod 666 /sys/class/gpio/gpio388/direction",
"chmod 666 /sys/class/gpio/gpio384/value",
"chmod 666 /sys/class/gpio/gpio385/value",
"chmod 666 /sys/class/gpio/gpio386/value",
"chmod 666 /sys/class/gpio/gpio387/value",
"chmod 666 /sys/class/gpio/gpio388/value",
"chmod 666 /sys/class/pwm/pwmchip0/pwm0/duty_cycle",
"chmod 666 /sys/class/pwm/pwmchip0/pwm0/period",
"chmod 666 /sys/class/pwm/pwmchip0/pwm0/enable",
"chmod 666 /sys/class/pwm/pwmchip0/pwm0/polarity",
"chmod 666 /sys/class/pwm/pwmchip2/pwm0/duty_cycle",
"chmod 666 /sys/class/pwm/pwmchip2/pwm0/period",
"chmod 666 /sys/class/pwm/pwmchip2/pwm0/enable",
"chmod 666 /sys/class/pwm/pwmchip2/pwm0/polarity",
```

**注：不能有多余的空格或符号，否则会导致开发板无法正常开机。**

#### 6.编译和烧录
进入根目录编译

```
./build.sh --product-name unionpi_tiger –ccache
```

打包

```
./device/board/unionman/unionpi_tiger/common/tools/packer-unionpi.sh
```

然后使用烧录工具将镜像文件烧入开发板

### APP具体实现
实时控制灯光、窗帘、风扇、闹钟的开关功能的四大功能模块封装在Index.ets
**点击四个模块按钮:** 调用`pushUrl()`导航到各个场景页面。进入模块后，可以点击左上角`<`退出页面。

**灯光开关:** 使用`setLedStatus`实现开关功能，可调用`startAutoMode()`进入自动模式，当光强减弱到一定程度时即可进行开灯，调用`updateImagePath`在不同状态下显示不同的页面图片。

**窗帘开关:** 使用`setPwmStatus()`实现窗帘的拉开和闭合，并调用`updateImagePath()`在不同状态下显示不同的页面图片。

**风扇开关:** 使用`setPwmStatus()`实现风扇的开关，可调用`startAutoMode()`进入自动模式，当温度上升至一定程度时即可开启风扇，调用`updateImagePath`在不同状态下显示不同的页面图片。

**光敏电阻传感器:** 使用`getAdcValueAndLog()`来获取adc1的值。

**温度传感器:** 使用`getAdcValueAndLog()`来获取adc2的值，在应用上通过乘以温度系数转换成温度

### 使用说明

1. 在主界面，分为四大模块，点击任何一个即可进入相对应的功能页面
2. 在灯光控制界面，点击“模式选择”按钮可选择不同模式；选择自动模式时，当adc的值大于阈值时开灯，小于时则关灯；
3. 在风力控制页面，点击“模式选择”按钮可选择不同模式；选择自动模式时，当adc的值大于阈值时开风扇，小于时则关风扇；
4. 在窗帘控制页面，点击拉开和收起按钮，可对舵机进行不同的控制操作；
5. 在闹钟页面可添加一个倒计时闹钟，对该闹钟可以加上备注，闹钟开启或者取消时都有提示框提醒，退出页面正在运行的闹钟也不会消失。

### 页面截图
页面截图可见 \entry\src\main\ets\image