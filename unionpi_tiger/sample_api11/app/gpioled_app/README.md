### 一、**样例介绍**

> 基于OpenHarmony NAPI框架实现北向应用端控制南向设备端扩展板指定GPIO口对应LED灯的亮灭

### 二、**开发环境**

1. 搭载OpenHarmony-3.1-release版本的Unionpi Tiger开发板 
2. DevEco Studio 3.0.0.991
3. 40PIN测试小板
4. Ubuntu20.04虚拟机
5. USB_Burning_Tool烧录工具

**环境搭建就不详细介绍了，社区也可以搜得到，这里给出笔者参考的几篇资料**

**\[1]**[OpenHarmony在Amlogic A311D芯片平台的快速开发上手指南-开源基础软件社区-51CTO.COM](https://ost.51cto.com/posts/11117)

**\[2]**[docs/UnionpiTiger_helloworld · OpenHarmony-SIG/knowledge_demo_temp - 码云 - 开源中国 (gitee.com)](https://gitee.com/openharmony-sig/knowledge_demo_temp/tree/master/docs/UnionpiTiger_helloworld)

**[3]**[unionpi_tiger/README_zh.md · OpenHarmony/device_board_unionman - Gitee.com](https://gitee.com/openharmony/device_board_unionman/blob/master/unionpi_tiger/README_zh.md#搭建开发环境)

### 三、**OpenHarmony系统技术架构**

OpenHarmony整体遵从分层设计，从下向上依次为：内核层、系统服务层、框架层和应用层。系统功能按照“**系统 > 子系统 > 组件**”逐级展开，在多设备部署场景下，支持根据实际需求裁剪某些非必要的子系统或组件。子系统是一个逻辑概念，它具体由对应的组件构成。

![img](https://gitee.com/openharmony/docs/raw/master/zh-cn/figures/1.png)

### 四、**NAPI框架简介**

NAPI（Native API）是 OpenHarmony 标准系统的一种JS API实现机制，适合封装IO、CPU密集型、OS底层等能力并对外暴露JS接口，实现**JS与C/C++代码互相访问**。

![NAPI](figures/NAPI.png)

### 五、**实现步骤**

### **1、创建NAPI扩展库**

**新增子系统gpioled_subsys**

在OpenHarmony源码目录下任意位置创建一个目录`gpioled_subsys`作为子系统的目录（子系统可创建在OpenHarmony源码目录任意位置）。子系统目录下创建`ohos.build`文件，构建时会先读取此文件。把新增的子系统配置到`build/subsystem_config.json`。

```json
"gpioled_subsys": {
  "path": "vendor/unionman/a311d/sample/napi/gpioled/gpioled_subsys",
  "name": "gpioled_subsys"
},
```

**新增gpioled_part组件** 

在子系统目录下创建一个子组件目录`gpioled_part`。 

修改子系统根目录下的ohos.build文件，添加组件配置

```
{
  "subsystem": "gpioled_subsys",
  "parts": {
    "gpioled_part": {
      "variants": [
        "phone"
      ],
      "module_list": [
        "//vendor/unionman/a311d/sample/napi/gpioled/gpioled_subsys/gpioled_part/gpioled_demo:gpioled"
      ]
    }
  }
}
```

**新增扩展动态库**

在组件目录下创建一个子目录`gpioled_demo`，作为 NAPI扩展库的代码目录。 

在`gpioled_demo`目录下创建代码文件`gpioled.cpp`。 

在`gpioled_demo`目录下创建`BUILD.gn`文件，编写构建配置。

```
import("//build/ohos.gni")

ohos_shared_library("gpioled") {
  include_dirs = [ "//foundation/ace/napi/interfaces/kits" ]

  sources = [
    "gpioled.cpp",
    "um_gpio.c",
  ]

  deps = [
    "//foundation/ace/napi:ace_napi",
    "//utils/native/base:utils",
  ]

  external_deps = [ "hiviewdfx_hilog_native:libhilog" ]

  relative_install_dir = "module"
  subsystem_name = "gpioled_subsys"
  part_name = "gpioled_part"
}
```

**将组件添加到产品定义中** 

产品定义文件存放在productdefine/common/products 目录，文件名就是产品名称。 如：a311d开发板的产品定义文件为productdefine/common/products/a311d.json 

按格式：“subsystemA:partA“:{}在产品定义中添加组件。

![1](figures/1.png)

### **2、NAPI接口开发**

**模块注册**

1)添加NAPI框架头文件，引入框架提供的方法。 

```c++
#include "napi/native_api.h"
#include "napi/native_node_api.h"
```

2)定义模块。

3)注册模块，加载动态库时自动调用注册。

```c++
/*
 * 模块定义
 */
static napi_module gpioled_demoModule = {
        .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = registerGpioLed_DemoApis,
        .nm_modname = "gpioled_demo",  //模块名
        .nm_priv = ((void *) 0),
        .reserved = {0},
};

/*
 * 注册模块
 */
extern "C" __attribute__((constructor)) void RegisterGpioLed_DemoModule(void) {
    napi_module_register(&gpioled_demoModule);  //接口注册函数
}
```

使用**DECLARE_NAPI_FUNCTION**("js函数名", c++实现函数名)定义接口函数、**DECLARE_NAPI_PROPERTY**、 **DECLARE_NAPI_STATIC_PROPERTY**等定义属性，再通过**napi_define_properties**赋给exports对象，最后返回exports对象。

```c++
/*
 * 注册接口
 */
static napi_value registerGpioLed_DemoApis(napi_env env, napi_value exports) {
    napi_value gpioValHigh = gpioValHigh;
    napi_value gpioValLow = nullptr;
    napi_create_int32(env, UM_GPIO_HIGH_LEVE, &gpioValHigh);
    napi_create_int32(env, UM_GPIO_LOW_LEVE, &gpioValLow);
    napi_property_descriptor desc[] = {
            DECLARE_NAPI_FUNCTION("setLedStatusWithCallback", setLedStatusWithCallback),
            DECLARE_NAPI_FUNCTION("getLedStatusWithCallback", getLedStatusWithCallback),
            DECLARE_NAPI_FUNCTION("setLedStatusWithPromise", setLedStatusWithPromise),
            DECLARE_NAPI_FUNCTION("getLedStatusWithPromise", getLedStatusWithPromise),
            DECLARE_NAPI_FUNCTION("setLedStatus", setLedStatus),
            DECLARE_NAPI_FUNCTION("getLedStatus", getLedStatus),
            DECLARE_NAPI_STATIC_PROPERTY("LED_ON", gpioValHigh),
            DECLARE_NAPI_STATIC_PROPERTY("LED_OFF", gpioValLow),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}
```

#### **实现原理**

**NAPI支持Callback、 Promise 二种异步模型** 

> • Callback：任务结果以参数的形式提供给用户注册的回调函数；代码逻辑复杂，可读性差，回调地狱。 
>
> • Promise：ES6提供的一种异步编程解决方案，比传统的解决方案——回调函数更加优雅。可使得异步执行可以 按照同步的流表示出来，避免了层层嵌套的回调函数，代码更易读。 
>

**NAPI框架中，异步接口实现基于napi_create_async_work() 函数创建的异步工作项**

> napi_status napi_create_async_work(napi_env env, 
>
> ​																  napi_value async_resource, 
>
> ​																  napi_value async_resource_name, 
>
> ​																  napi_async_execute_callback execute, 
>
> ​																  napi_async_complete_callback complete, 
>
> ​																  void* data, napi_async_work* result);

参数说明： 

> [in] env: 传入接口调用者的环境，包含js引擎等，由框架提供。 
>
> [in] async_resource: 可选项，关联async_hooks。 
>
> [in] async_resource_name: 异步资源标识符，主要用于async_hooks API暴露断言诊断信息。 
>
> [in] execute: 异步工作项的处理函数，当工作项被调度到后在worker线程中执行，用于处理业务逻辑，并得到结果。 
>
> [in] complete: execute参数指定的函数执行完成或取消后，触发执行该函数，将结果返回给JS。此函数在EventLoop中执行。 
>
> [in] data: 异步工作项上下文数据（Addon），用于在主线程接口实现方法、execute、complete之间传递数据。 
>
> [out] result: napi_async_work*指针，返回创建的异步工作项对象。 
>
> 返回值：返回napi_ok表示转换成功，其他值失败。

**异步工作项工作时序图**

![2](figures/2.png)

**定义异步工作项上下文数据**

根据实际场景需要定义异步工作项上下文数据结构，用于在主线程方法、execute、complete之间传递数据，一般包含异步工作项对象、napi_deferred对象、回调函数、参数数组、返回结果等。

```c
struct LedAddOnData {
    napi_async_work asyncWork = nullptr; //异步工作项
    napi_deferred deferred = nullptr;    //用于Promise的resolve、reject处理
    napi_ref callback = nullptr;         //回调函数
    int args[2] = {0};                   //2个输入参数
    int result = 0;                      //业务逻辑处理结果（返回值）
};
```

#### **异步接口--Callback实现**

主线程方法接收参数，初始化异步工作项上下文数据，创建异步工作项，并将其加到调度队列。最后方法返回空值。

```c++
static napi_value setLedStatusWithCallback(napi_env env, napi_callback_info info) {
    //获取3个参数，值的类型是js类型（napi_value）
    size_t argc = 3;
    napi_value args[3];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    //异步工作项上下文用户数据，传递到异步工作项的execute、complete中传递数据
    auto addonData = new LedAddOnData{
            .asyncWork = nullptr,
    };

    //将接收到的参数传入用户自定义的上下文数据
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &addonData->args[0]));
    NAPI_CALL(env, napi_get_value_int32(env, args[1], &addonData->args[1]));
    NAPI_CALL(env, napi_create_reference(env, args[2], 1, &addonData->callback));

    //创建async work，创建成功后通过最后一个参数接受async work的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "setLedStatusWithCallback", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, setLedStatusExecuteCB, completeCBForCallback, (void *) addonData,
                           &addonData->asyncWork);

    //将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    //原生方法返回空对象
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}
```

worker线程执行业务逻辑计算，将结果存入上下文数据

```c++
//业务逻辑处理函数，由worker线程池调度执行。
static void setLedStatusExecuteCB(napi_env env, void *data) {
    LedAddOnData *addOnData = (LedAddOnData *) data;
    int gpioNum = addOnData->args[0];
    int bExport = UM_GPIO_EXPORTED;
    int direction = UM_GPIO_DIRECTION_OUT;
    int getValue = -1;

    UM_GPIO_IsExport(gpioNum, &getValue);
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpioNum, bExport);
    }

    UM_GPIO_SetDirection(gpioNum, direction);
    addOnData->result = UM_GPIO_SetValue(gpioNum, addOnData->args[1]);
}
```

执行Complete函数，从上下文数据中获取结果，将结果转换为JS类型，调用回调函数返回结果给调用方。

```c++
//业务逻辑处理完成回调函数，在业务逻辑处理函数执行完成或取消后触发，由EventLoop线程中执行。
static void completeCBForCallback(napi_env env, napi_status status, void *data) {
    LedAddOnData *addOnData = (LedAddOnData *) data;
    napi_value callback = nullptr;
    napi_get_reference_value(env, addOnData->callback, &callback);
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);
    napi_value result = nullptr;
    napi_create_int32(env, addOnData->result, &result);

    //执行回调函数
    napi_value returnVal = nullptr;
    napi_call_function(env, undefined, callback, 1, &result, &returnVal);

    //删除napi_ref对象
    if (addOnData->callback != nullptr) {
        napi_delete_reference(env, addOnData->callback);
    }

    //删除异步工作项
    napi_delete_async_work(env, addOnData->asyncWork);
    delete addOnData;
}
```

#### **异步接口-- Promise实现**

NAPI框架提供napi_create_promise()函数用于创建Promise，返回promise、deferred 二个对象， promise用于主线程方法返回， deferred对象用于resolve、reject处理。 

> napi_status napi_create_promise(napi_env env, 
>
> ​															napi_deferred* deferred, 
>
> ​															napi_value* promise);

**参数说明：** 

> [in] env: 传入接口调用者的环境，包含js引擎等，由框架提供，默认情况下直接传入即可。 
>
> [out] deferred: 返回接收刚创建的deferred对象，关联Promise对象，后面用于napi_resolve_deferred() 或 napi_reject_deferred() 更新状态，返回数据。 
>
> [out] promise: 关联上面deferred对象的JS Promise对象，用于主线程方法返回。 
>
> 返回值：返回napi_ok表示转换成功，其他值失败。

主线程方法接收参数，创建Promise，初始化异步工作项上下文数据，创建异步工作项，并将其加到调度队列。最后返回Promise对象。

```c++
static napi_value setLedStatusWithPromise(napi_env env, napi_callback_info info) {
    //获取2个参数，值的类型是js类型(napi_value)
    size_t argc = 2;
    napi_value args[2];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    //创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    //异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto addonData = new LedAddOnData{
            .asyncWork = nullptr,
            .deferred = deferred,
    };

    //将被收到的参数传入
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &addonData->args[0]));
    NAPI_CALL(env, napi_get_value_int32(env, args[1], &addonData->args[1]));

    //创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "setLedStatusWithPromise", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, setLedStatusExecuteCB, completeCBForPromise, (void *) addonData,
                           &addonData->asyncWork);

    //将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    //返回promise
    return promise;
}
```

worker线程执行业务逻辑计算，将结果存入上下文数据。

```c++
//业务逻辑处理函数，由worker线程池调度执行。
static void setLedStatusExecuteCB(napi_env env, void *data) {
    LedAddOnData *addOnData = (LedAddOnData *) data;
    int gpioNum = addOnData->args[0];
    int bExport = UM_GPIO_EXPORTED;
    int direction = UM_GPIO_DIRECTION_OUT;
    int getValue = -1;

    UM_GPIO_IsExport(gpioNum, &getValue);
    if (getValue == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpioNum, bExport);
    }

    UM_GPIO_SetDirection(gpioNum, direction);
    addOnData->result = UM_GPIO_SetValue(gpioNum, addOnData->args[1]);
}
```

执行Complete函数，从上下文数据中获取结果，将结果转换为JS类型，调用 napi_resolve_deferred、 napi_reject_deferred更新状态、返回结果给调用方。

```c++
static void completeCBForPromise(napi_env env, napi_status status, void *data) {
    LedAddOnData *addOnData = (LedAddOnData *) data;
    napi_value result = nullptr;
    napi_create_int32(env, addOnData->result, &result);
    napi_resolve_deferred(env, addOnData->deferred, result);

    //删除napi_ref对象
    if (addOnData->callback != nullptr) {
        napi_delete_reference(env, addOnData->callback);
    }

    //删除异步工作项
    napi_delete_async_work(env, addOnData->asyncWork);
    delete addOnData;
}
```

修改device/unionman/a311d/system/cfg/init.A311D.cfg 文件，在boot的cmds中添加相关命令

```cfg
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
"chmod 666 /sys/class/gpio/gpio389/value"
```

### **3.NAPI接口应用**

**编写定义.d.ts文件**

编写接口定义@ohos.**nameX**.d.ts文件，放到OpenHarmony SDK目录ets\\\${ets_version}\\api目录下。使用SDK 8则${ets_version}为3.1.6.6，SDK 7则为3.0.0.0。 

注意@ohos.**nameX**必须和NAPI模块的BUILD.gn文件中ohos_shared_library("**nameX**")指定的动态库名一致。

![3](figures/3.png)

使用DevEco Studio创建标准应用App，并引入模块：

![4](figures/4.png)

![5](figures/5.png)

```js
import gpioled from '@ohos.gpioled';
```

页面初始化，同步按钮开关状态与选择的GPIO口对应的值保持一致

```js
onInit() {
    this.syncButtonStatus()
},
```

```js
//同步按钮开关状态
syncButtonStatus() {
    gpioled.getLedStatus(this.pin).then((result) => {
        this.status = Boolean(result)
    })
}
```

使用select组件实现选择指定GPIO口的功能

```html
<select @change="changeGpio" style="font-size : 30fp; weights : 400;">
    <option value="380">
        UM_GPIO_01
    </option>
    ...
    <option value="389">
        UM_GPIO_10
    </option>
</select>
```

当下拉选择新值时，触发change事件并调用changeGpio函数

```js
changeGpio(msg) {
    this.pin = Number(msg.newValue)
    this.syncButtonStatus()
}
```

使用switch组件，当开关状态切换时触发事件调用switchChange方法

```html
<switch @change="switchChange" checked="{{status}}">
</switch>
```

```js
switchChange(e) {
    if (e.checked) {
        this.addLedEffect()
    } else {
        this.removeLedEffect()
    }
}
```

新增两个方法，封装开启、关闭指定的LED灯

```js
//开灯
addLedEffect() {
    gpioled.setLedStatus(this.pin, gpioled.LED_ON).then((result) => {
        if (result === 0) {
            prompt.showToast({
                message: "开灯"
            })
        } else {
            prompt.showToast({
                message: "开灯失败"
            })
        }
    })
},

//关灯
removeLedEffect() {
    gpioled.setLedStatus(this.pin, gpioled.LED_OFF).then((result) => {
        if (result === 0) {
            prompt.showToast({
                message: "关灯"
            })
        } else {
            prompt.showToast({
                message: "关灯失败"
            })
        }
    })
}
```

点击File->Project Structure

![6](figures/6.png)

选择自动签名

![7](figures/7.png)

使用Micro USB数据线连接PC与开发板OTG口并接通电源后点击Run即可

![8](figures/8.png)

### **六、构建与烧录**

进入源码根目录，执行如下命令进行版本编译

```
./build.sh --product-name a311d –ccache
```

编译完成后，效果如图所示：

![9](figures/9.png)

编译完成后需要，进行对镜像进行打包，然后进行烧写。
执行以下命令固件打包

```
./device/unionpi/build/packer-unionpi.sh
```

固件打包完成，生成路径为编译根目录下的out/a311d/packages/phone/images/OpenHarmony.img。

![10](figures/10.png)

打开烧录工具，使用Micro USB数据线连接PC与开发板OTG口并接通电源，导入烧录包后开始烧录即可(可关闭校验IMG)

![11](figures/11.png)

![12](figures/12.png)

![13](figures/13.png)

系统烧录后，如系统版本未变，可直接使用hdc_std工具将新构建的out/a311d/packages/phone/system/lib/module/libgpioled.z.so文件复制替换开发板系统中的/system/lib/module/libgpioled.z.so文件，提升验证效率。参考命令如下：

```
hdc_std shell mount -o remount,rw /          //重新挂载为已经挂载了的文件系统（以读写权限挂载）
hdc_std file send libgpioled.z.so /system/lib/module/
```

*使用hdc工具还有另一个好处就是调试过程中不需要将Micro USB数据线在开发板OTG口和DEBUG口来回切换，不管是烧录，串口调试还是应用安装都是连接OTG口就行*

**hdc_std工具获取方式：**

通过OpenHarmony sdk获取，hdc_std在sdk的toolchains目录下，例如笔者的hdc工具存放路径为：C:\Users\\haoyuan.chen\AppData\Local\OpenHarmony\Sdk\toolchains\3.1.6.6

![14](figures/14.png)

将其添加到环境变量，显示以下结果即可

![15](figures/15.png)

更多资料请参考：[OpenAtom OpenHarmony](https://docs.openharmony.cn/pages/v3.1/zh-cn/device-dev/subsystems/subsys-toolchain-hdc-guide.md/)

### **七、演示效果**

![演示视频](figures/演示视频.gif)

### **八、参考资料**

[标准设备应用开发—Native Api-开源基础软件社区-51CTO.COM](https://ost.51cto.com/activity/59)



