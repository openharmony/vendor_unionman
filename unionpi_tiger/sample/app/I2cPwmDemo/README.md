# i2cpwmdemo
## 作品简介
i2c读取温度湿度后，可控制舵机转到温度或者湿度对应的角度，也会在屏幕上显示出来温度或者湿度，以及温湿度指针
## 开发环境简介
1. 搭载OpenHarmony-3.2-Beat5版本的Unionpi Tiger开发板  
2. DevEco Studio 3.1.0.0  
3. 温湿度模块SHT3x-DIS  
4. SG90舵机
## 舵机控制部分
详见 [PWM舵机调试](../../hardware/pwm/README.md)
## 温湿度传感器采集部分
详见 [I2C读写及温湿度传感器使用](../../hardware/i2c/README.md)
## NAPI
全称 Native API，是OpenHarmony系统中的一套原生模块拓展开发框架，基于N-API开发，为开发者提供了JS与C/C++不同语言模块之间的相互访问，交互的能力。它可以用于规范化封装IO、OS底层等，并可以提供相应的JS接口供开发者调用。
### 编写子系统组件
在`vendor/unionman/unionpi_tiger/sample/napi/napisubsys`目录下新建`pwmnapipart/i2cpwmdemo`,`i2cnapipart/i2cpwmdemo`文件夹，分别对应`pwm`操作组件以及`i2c`操作组件
#### 编写GN构建脚本
`pwmnapipart/i2cpwmdemo/BUILD.gn`
```python
import("//build/ohos.gni")
import("//drivers/hdf_core/adapter/uhdf2/uhdf.gni")

ohos_shared_library("pwmnapi") {
    sources = [
        "pwm_napi.cpp",
        "pwm/pwm_ctl.cpp"
    ]
    deps = [
        "//foundation/arkui/napi:ace_napi",
        "//drivers/hdf_core/adapter/uhdf2/platform:libhdf_platform"
    ]
    include_dirs = [
        "//base/hiviewdfx/hilog/interfaces/native/innerkits/include",
    ]
    
    external_deps = [
        "hiviewdfx_hilog_native:libhilog",
    ]

    relative_install_dir = "module"
    part_name = "pwmnapi"
    subsystem_name = "napisubsys"
}
```
`i2cnapipart/i2cpwmdemo/BUILD.gn`
```python
import("//build/ohos.gni")
import("//drivers/hdf_core/adapter/uhdf2/uhdf.gni")

ohos_shared_library("i2cnapi") {
    sources = [
        "i2c_napi.cpp",
        "i2c/i2c_ctl.cpp"
    ]

    include_dirs = [
        "//drivers/hdf_core/framework/include/platform",
        "//base/hiviewdfx/hilog/interfaces/native/innerkits/include",
    ]
    
    external_deps = [
        "hiviewdfx_hilog_native:libhilog",
    ]

    deps = [
        "//foundation/arkui/napi:ace_napi",
        "//drivers/hdf_core/adapter/uhdf2/platform:libhdf_platform"
    ]

    relative_install_dir = "module"
    part_name = "i2cnapi"
    subsystem_name = "napisubsys"
}
```
#### 定义组件
打开`unionpi_tiger/sample/napi/napisubsys/ohos.build`配置文件，在`"parts":`字段中添加下列语句,用于注册组件并列出组件需要的模块列表
```JSON
"i2cnapi":{
    "variants": [
        "phone"
    ],
    "module_list": [
        "//vendor/unionman/unionpi_tiger/sample/napi/napisubsys/i2cnapipart/i2cpwmdemo:i2cnapi"
    ]
},
"pwmnapi":{
    "variants": [
        "phone"
    ],
    "module_list": [
        "//vendor/unionman/unionpi_tiger/sample/napi/napisubsys/i2cpwmdemo:pwmnapi"
    ]
}
```
#### 将组件添加进子系统列表中
打开`vendor/unionman/unionpi_tiger/config.json`文件，在`"subsystems":napisubsys`的`components`字段中添加下列语句
```JSON
{
    "component": "i2cnapi",
    "features": []
},
{
    "component": "pwmnapi",
    "features": []
}
```
### 编写TS接口定义文件
编写好NAPI代码之后，需要编写TS接口定义文件(.d.ts),这样开发OpenHarmony应用时，编译器才知道模块定义了哪些函数以及对应的返回值，参数列表等。  
`@ohos.i2cnapi.d.ts`
```TypeScript
declare namespace i2c_ctl {
  export class TemHum {
    /// @brief 摄氏度
    temperatureC: number;
    /// @brief 湿度
    humidity: number;
    /// @brief 华氏度
    temperatureF: number;
  }
  /// @brief 获取温湿度数据
  function getDataAsync(): Promise<TemHum>;
}

export default i2c_ctl;
```
`@ohos.pwmnapi.d.ts`
```TypeScript
declare namespace pwm_ctl {
/// @brief 设置舵机角度(单位为°)
  function setAngleAsync(Angle: number): Promise<void>;
}

export default pwm_ctl
```

## 构建
进入`OpenHarmony`源代码的根目录，执行编译  
>./build.sh --product-name unionpi_tiger –ccache

如果开发板已经烧录了同版本的`OpenHarmony`操作系统,可以在`out/unionpi_tiger/[子系统名]/[组件名]`文件夹中找到组件对应的动态库,然后使用hdc工具发送到开发板的`/system/lib/module`目录中即可，省去了烧录所花的时间
> hdc_std file send libxxx.z.so /system/lib/module

在发送时，可能出现以下错误
>[Fail]Error opening file: read-only file system, path:/system/lib/module/libxxx.z.so

这是因为`system`分区被开机挂载为只读分区，解决方案是重新挂载`system`分区为可读写分区
>hdc_std shell mount / -o rw,remount

然后再重新发送动态库至开发板即可
## OpenHarmony应用开发
打开`DevEco Studio`,新建一个`empty ability`项目  
![1.png](../figures/I2cPwmDemo/1.png)  
将先前编写的TS接口定义文件放入`entry/src/main`文件夹中，IDE便会自动识别此TS接口定义文件  
![2.png](../figures/I2cPwmDemo/2.png)    
### 相关代码
#### 读取温湿度数据并刷新页面
设置三个文本框，并设置相应的string变量与之绑定，在ui加载完毕事件的事件处理函数中，打开一个1s执行一次的定时器，每1s读取一次温湿度传感器的数值,然后根据选择框的选项计算对应的角度,设置舵机的角度以及调用flushDraw在屏幕上画出对应的指针，更新string变量，由于string变量设置了@State,文本框的显示也会相应更新。
```TypeScript
setInterval(async () => {
    let data = await i2c_ctl.getDataAsync();
    this.temperatureC = '摄氏度: ' + data.temperatureC.toFixed(2) + '°C';
    this.temperatureF = '华氏度:' + data.temperatureF.toFixed(2) + '℉';
    this.humidity = '湿度:' + data.humidity.toFixed(2) + '%';
    let drawData: number;
    switch (this.selectIndex) {
    case 0:
        drawData = data.temperatureC;
        break;
    case 1:
        drawData = data.humidity;
        break;
     default:
        drawData = 0;
        break;
    }
    this.flushDraw(drawData / 100 * Math.PI);
    pwm_ctl.setAngleAsync(180 - (drawData / 100 * 180));
});
```
#### Canvas画出指针
首先获取绘画区的大小，再清除绘画区，调用`translate()`将坐标系的圆点移到指针的中心位置，再调用`rotate()`旋转坐标系到对应的角度，然后调用`drawImage()`画出指针并还原之前的设置
```TypeScript
flushDraw(rad: number): void {
    let width = this.context.width;
    let height = this.context.height;

    this.context.clearRect(0, 0, width, height);
    this.context.translate(width / 2, height - this.ih * 3);
    this.context.rotate(rad);

    this.context.drawImage(this.img, -129, -this.ih / 2);

    this.context.rotate(-rad);
    this.context.translate(-width / 2, -(height - this.ih * 3));
}
```

## 演示效果
![3.jpg](../figures/I2cPwmDemo/3.jpg) 
## 参考链接
[SHT3x-DIS驱动及应用详解(附源码和手册)](https://blog.csdn.net/qq_33194301/article/details/103652311)  
[OpenHarmony 源码解析之JavaScript API框架(NAPI)](https://ost.51cto.com/posts/8390)  