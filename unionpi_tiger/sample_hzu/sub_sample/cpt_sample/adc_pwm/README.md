## 基于adc和pwm实现的高温报警器
具体说明：实现读取adc温度传感器数据并用来控制pwm蜂鸣器达到一定高温时报警的功能，当温度超过预设阈值时，蜂鸣器会播放一段旋律，直到温度降低到阈值以下。

## 实现步骤
### 一、添加子系统
1.在源码根目录下创建文件夹“sub_sample”
2.在/build/subsystem_config.json文件中添加子系统描述如下语句
```json
  "sub_sample": {
      "path": "sub_sample",
      "name": "sub_sample"
    }
```

### 二、添加部件
1.在sub_sample文件夹中新建cpt_sample文件夹
2.在cpt_sample文件夹中新建BUILD.gn、bundle.json文件
3.部件配置：编写bundle.json文件:先复制源码根目录下vendor/unionman/unionpi_tiger/bundle.json，然后进行修改，修改语句如下:
- 部件名字：cpt_sample
- 所属子系统：sub_sample
```json
  "component": {
    "name": "cpt_sample",
    "subsystem": "sub_sample",
    ...
  }
```
- 添加部件编译目标：
```json
  "build": {
        "sub_component": [
          "//sub_sample/cpt_sample:cpt_sample"
        ],
        ...
  }
```
4.编写部件的BUILD.gn，添加下列语句（注意要先写后面的业务代码再回来补充deps）
```gn
  import("//build/ohos.gni")
  group("cpt_sample"){
    deps = [

    ]
  }
```
5.编写业务代码：在cpt_sample文件夹中新建adc_pwm文件夹，在adc_pwm_test文件夹内新建src文件夹（存放.c源文件）、include文件夹（存放.h头文件）、BUILD.gn文件
6.分别编写.h及.c文件
7.对模块的BUILD.gn文件进行编辑————打开adc_pwm_test文件夹内的BUILD.gn进行编写，可执行模块名为adc_pwm_test

### 三、纳入编译
1.加入部件编译群组 （/sub_sample/cpt_sample/BUILD.gn）
```gn
  import("//build/ohos.gni")
  group("cpt_sample"){
    deps = [
      "adc_pwm:adc_pwm_test",
    ]
  }
```
2.加入产品配置
打开`vendor/unionman/unionpi_tiger/config.json`文件，在`"subsystems":`中添加下列语句
```json
    {
      "subsystem": "sub_sample",
      "components": [
        {
          "component": "cpt_sample",
          "features": []
        }
      ]
    },
```
## 目录

```
sub_sample/cpt_sample/       # 产品模块详细配置目录
├── bundle.json
├── BUILD.gn
└── adc_pwm                  # adc_pwm配置目录
    ├── include
        ├── um_pwm.h 
        └── um_adc.h            
    ├── src      
        ├── main.c
        ├── um_pwm.c 
        └── um_adc.c
    ├── README.md            
    └── BUILD.gn
```

### 四、模块编译
1.编译模块指令，在终端输入下列语句
./build.sh --product-name unionpi_tiger --build-target adc_pwm_test --ccache
生成的可执行文件处于out文件夹中，产品/子系统/部件/路径下：out/unionpi_tiger/sub_sample/cpt_sample/adc_pwm_test
2.将可执行文件adc_pwm_test下载到Windows本地，如E盘根目录

## 五、硬件连接
**准备**: 
  Unionpi-Tiger开发板
	杜邦线
	DS18B20温度传感器
	有源蜂鸣器

***接线:***Unionpi-Tiger上的ADC与DS18B20温度传感器连接
	***3.3V——VCC***
	***ADC_1——SDA***
	***GND——GND***

***接线:***Unionpi-Tiger上的PWM与蜂鸣器连接
	***3.3V——VCC***
	***PWM_1——SDA***
	***GND——GND***

### 六、使用hdc工具推送可执行文件
- 在Windows中下载hdc工具，将hdc工具路径配置到系统环境变量
- 开发板连接USB_OTG口，并接通电源
- 在Windows环境中打开命令行工具（如cmd），使用hdc工具
- 挂载（Windows环境下）
```
hdc shell mount -o remount,rw /
```
- 推送可执行文件
```
hdc file send E:\adc_pwm_test /bin/
```
- 与开发板交互
```
hdc shell
```
- 修改权限
```
chmod 777 /bin/adc_pwm_test
```
- 运行程序(这里选择pwm_1,所以第一个参数是1，然后是1500000Hz)
```
./bin/adc_pwm_test 1 1500000
```



