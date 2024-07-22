## 2024惠州学院综合实训作品

####  介绍

2024惠州学院综合实训作品
# 第十二组-基于OpenHarmony的智能家居系统的设计与实现

###  介绍

使用了[@ohos.i2cnapidemo](entry/src/main/@ohos.i2cnapidemo.d.ts)，[@ohos.gpioled](entry/src/main/@ohos.gpioled.d.ts)，[@ohos.pwmtest](entry/src/main/@ohos.pwmtest.d.ts)等接口.

###


使用说明

1.登录：在此进入控制界面；

2.主页1:控制台，在此进行设备控制,点击开始监控开始定时获取环境温湿度，点击对应的开关可以打开对应的控制界面，并且可以通过设定预警温度来监控温度，当温度超过设定值时会报警，当监控停止或者重新设定一个更高的温度阈值时，报警效果消失；

3.主页2:风扇模块

4.主页3：温度蜂鸣器模块

5.主页4：舵机模块

### 工程目录

```
entry/src/main/ets/
|---entryability
    |---EntryAbility.ts                      //控制界面
|---pages
    |---Buzzer.ets                 //蜂鸣器控制模块
    |---Humiture.ets                    //温度控制模块
    |---Index.ets                     //主界面
    |---Madaish.ets                  //舵机控制正反转模块
    |---Servo.ets                     //舵机控制模块

###    注意事项

在开发板上运行时需要给硬件对应的访问权限，具体参考如下：

```

chmod 666 sys/class/gpio/gpio389/direction
chmod 666 sys/class/gpio/gpio389/value
chmod 666 sys/class/gpio/gpio388/direction
chmod 666 sys/class/gpio/gpio388/value
chmod 666 sys/class/gpio/gpio387/direction
chmod 666 sys/class/gpio/gpio387/value
chmod 666 sys/class/gpio/gpio386/direction
chmod 666 sys/class/gpio/gpio386/value
chmod 666 sys/class/gpio/gpio385/direction
chmod 666 sys/class/gpio/gpio385/value
chmod 666 sys/class/gpio/gpio384/direction
chmod 666 sys/class/gpio/gpio384/value
chmod 666 sys/class/gpio/gpio383/direction
chmod 666 sys/class/gpio/gpio383/value
chmod 666 sys/class/gpio/gpio382/direction
chmod 666 sys/class/gpio/gpio382/value
chmod 666 sys/class/gpio/gpio381/direction
chmod 666 sys/class/gpio/gpio381/value
chmod 666 sys/class/gpio/gpio380/direction
chmod 666 sys/class/gpio/gpio380/value

chmod 777 /sys/class/pwm/pwmchip0/pwm0/duty_cycle
chmod 777 /sys/class/pwm/pwmchip0/pwm0/period
chmod 777 /sys/class/pwm/pwmchip0/pwm0/enable
chmod 777 /sys/class/pwm/pwmchip0/pwm0/polarity
chmod 777 /sys/class/pwm/pwmchip2/pwm0/duty_cycle
chmod 777 /sys/class/pwm/pwmchip2/pwm0/period
chmod 777 /sys/class/pwm/pwmchip2/pwm0/enable
chmod 777 /sys/class/pwm/pwmchip2/pwm0/polarity

```

####  参与贡献

1. Fork 本仓库
2. 提交代码
3. 新建 Pull Request