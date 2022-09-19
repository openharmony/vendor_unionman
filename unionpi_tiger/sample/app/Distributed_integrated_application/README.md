## 分布式综合应用

### 应用介绍

采用分布式数据管理和分布式任务调度的综合应用，实现两个开发板的数据库互通，并且可以让一个开发板拉起另一个开发板的应用。

**control_application**：负责拉起 APP，设置舵机的转动角度（模拟开关窗帘），读取同步数据库的温湿度值，把设置的舵机角度值写入数据库并同步。

**peripheral_application**：调用i2c温湿度传感器、pwm舵机外设，把读取到的温湿度值写入数据库并同步，以及读取同步数据库的舵机角度值，并设置舵机转动相应的角度

### 使用教程

在使用前请先阅读：

1. [添加i2c的napi到系统中](https://gitee.com/openharmony/vendor_unionman/tree/master/unionpi_tiger/sample/app/i2c_app)
2. [添加pwm的napi到系统中](https://gitee.com/openharmony/vendor_unionman/tree/master/unionpi_tiger/sample/app/pwm_app)
3. [分布式数据库的使用](https://gitee.com/openharmony/vendor_unionman/tree/master/unionpi_tiger/sample/app/distributed_database)

把 **control_application** APP 烧写进开发板A，把 **peripheral_application** APP 烧写进开发板B，并且把 i2c 和 pwm 的 napi 添加到开发板B的系统中。

将位于`unionman\unionpi_tiger\sample\napi\napisubsys`中的`@ohos.pwmtest.d.ts` 和 ` @ohos.i2c.d.ts`复制并放在`X:\Users\XXXXX\AppData\Local\OpenHarmony\Sdk\ets\3.1.6.6\api`文件夹内 

*此路径是由SDK安装目录决定，如不清楚自己路径可打开DevEco Studio—工具—SDK管理—外观和行为—OpenHarmony SDK中查看*

注意：**记得修改系统权限，才能使 APP 能够访问i2c驱动和pwm驱动**（在**看前阅读**的1、2里面有详细步骤）

使用：启动开发板A的APP之前，请先按照 **分布式数据库的使用 **那里**进行两台设备的配对**。

### 例子演示

![](../figures/Distributed_integrated_application/1.gif)