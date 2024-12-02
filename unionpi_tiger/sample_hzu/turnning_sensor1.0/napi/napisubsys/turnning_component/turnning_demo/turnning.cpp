#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "um_gpio.h"

struct LedAddOnData {//不会用
    napi_async_work asyncWork = nullptr; // 异步工作项
    napi_deferred deferred = nullptr;    // 用于Promise的resolve、reject处理
    napi_ref callback = nullptr;         // 回调函数
    int args[3] = {0};                   // 3个输出参数！！！！！！//这个不知道一个怎么设置
    int result = 0;                      // 业务逻辑处理结果（返回值）
};

// 业务逻辑处理函数，由worker线程池调度执行
static void mainExecuteCB(napi_env env, void *data)
{
    LedAddOnData *addOnData = (LedAddOnData *)data;
    int gpioNum1 = UM_GPIO_07;// 默认操作的GPIO编号为UM_GPIO_07
    int gpioNum2 = UM_GPIO_06;// 默认操作的GPIO编号为UM_GPIO_06
    int gpioNum3 = UM_GPIO_05;// 3应该连接SW即按键
    int bExport = UM_GPIO_EXPORTED; // 默认操作为导出GPIO
    int direction = UM_GPIO_DIRECTION_IN; // 默认设置GPIO方向为输入
    int getValue1 = -1;// 用于存储从GPIO读取的值
    int getValue2 = -1;
    int getValue3 = -1;// 3应该连接SW即按键
    int current1_Value = -1;//当前电平值
    int current2_Value = -1;
    int previous1_Value = -1;//上一秒电平值
    int previous2_Value = -1;
    int turn_click = 0;//转动次数

    // 判断gpio口是否已经导出，如未导出则执行对应函数
    UM_GPIO_IsExport(gpioNum1, &getValue1);// 检查GPIO是否已导出
    if (getValue1 == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpioNum1, bExport);// 如果GPIO未导出，则导出GPIO
    }
    UM_GPIO_IsExport(gpioNum2, &getValue2);
    if (getValue2 == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpioNum2, bExport);
    }
    UM_GPIO_IsExport(gpioNum3, &getValue3);
    if (getValue3 == UM_GPIO_NOT_EXPORT) {
        UM_GPIO_Export(gpioNum3, bExport);
    }

    // 设置gpio口为输入或输出模式
    UM_GPIO_SetDirection(gpioNum1, direction); // 设置GPIO方向
    UM_GPIO_SetDirection(gpioNum2, direction);
    UM_GPIO_SetDirection(gpioNum3, direction);

    // 获取对应gpio口的模式并打印
    UM_GPIO_GetDirection(gpioNum1, &getValue1);// 获取GPIO方向
    printf("gpioNum1:[%d], direction:[%d]\n", gpioNum1, getValue1);// 打印GPIO编号和方向
    UM_GPIO_GetDirection(gpioNum2, &getValue2);
    printf("gpioNum2:[%d], direction:[%d]\n", gpioNum2, getValue2);
    UM_GPIO_GetDirection(gpioNum3, &getValue3);// 3应该连接SW即按键
    printf("gpioNum3:[%d], direction:[%d]\n", gpioNum3, getValue3);



    while(1){

        // 获取对应gpio口的电平值并打印
        UM_GPIO_GetValue(gpioNum1, &getValue1); // 获取GPIO值
        current1_Value = getValue1;
        printf("gpioNum1:[%d], value1:[%d]\n", gpioNum1, getValue1);// 打印GPIO编号和值

        UM_GPIO_GetValue(gpioNum2, &getValue2); 
        current2_Value = getValue2;
        printf("gpioNum2:[%d], value2:[%d]\n", gpioNum2, getValue2);
        
        UM_GPIO_GetValue(gpioNum3, &getValue3); //3应该连接SW即按键
        printf("gpioNum3:[%d], value3:[%d]\n", gpioNum3, getValue3);

        if((previous1_Value != -1) && (previous2_Value != -1)){// 初始化上一秒的值（假设为 -1 以指示未设置）  
            if((current1_Value != previous1_Value) && (current2_Value != previous2_Value)){//当旋转一小格时增加旋转次数
                    turn_click++;
                    printf("当前转动了%d小格",turn_click);
            }
        }

        napi_value ret;
        NAPI_CALL(env, napi_create_number(env, turn_click ,&ret));//将转动格数数据更新至北向
        return ret

        if(getValue3 == 0){//退出计数器且将计数器清零
            turn_click = 0;
            printf("计数已清零，即将退出程序",turn_click);
            NAPI_CALL(env, napi_create_number(env, turn_click ,&ret));//将转动格数数据更新至北向
            return ret;
            exit(0);
        }
        //更新上一秒的值
        previous1_Value = current1_Value;
        previous2_Value = current2_Value;

        usleep(500000);//延时0.5s

    }
}
//promise的回调函数
static void completeCBForPromise(napi_env env, napi_status status, void *data)//------------------2----------------删除promise对象
{
    LedAddOnData *addOnData = (LedAddOnData *)data;
    napi_value result = nullptr;
    napi_create_int32(env, addOnData->result, &result);
    napi_resolve_deferred(env, addOnData->deferred, result);//promise独有的deferred

    // 删除napi_ref对象
    if (addOnData->callback != nullptr) {
        napi_delete_reference(env, addOnData->callback);
    }

    // 删除异步工作项
    napi_delete_async_work(env, addOnData->asyncWork);
    delete addOnData;
}

static napi_value getStatusWithPromise(napi_env env, napi_callback_info info)
{
    //获取1个参数，值的类型是js类型(napi_value)(按照我的理解应该是获取三个参数)
    // 个人认为不需要获取数据就不需要这一段
    // size_t argc = 0;
    // napi_value args[1];
    // napi_value thisArg = nullptr;
    // NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto addonData = new LedAddOnData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };

    // 将被收到的参数传入
    // 个人认为不需要获取数据就不需要这一段
    // NAPI_CALL(env, napi_get_value_int32(env, args[0], &addonData->args[0]));

    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "getLedStatusWithPromise", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName,mainExecuteCB , completeCBForPromise, (void *)addonData,
                           &addonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    // 返回promise
    return promise;
}

/*
 * 注册接口
 */
static napi_value registerGpioLed_DemoApis(napi_env env, napi_value exports)
{
    // napi_value gpioValHigh = gpioValHigh;
    // napi_value gpioValLow = nullptr;
    // napi_create_int32(env, UM_GPIO_HIGH_LEVE, &gpioValHigh);
    // napi_create_int32(env, UM_GPIO_LOW_LEVE, &gpioValLow);
    // napi_create_n(env, UM_GPIO_HIGH_LEVE, &gpioValHigh);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getStatusWithPromise", getStatusWithPromise),  
        // DECLARE_NAPI_FUNCTION("setLedStatusWithPromise", setLedStatusWithPromise),  //传输两个参数-选定引脚编号，并且设置LED状态
        // DECLARE_NAPI_FUNCTION("getLedStatusWithPromise", getLedStatusWithPromise),  //接收一个引脚编号，返回LED状态
        // DECLARE_NAPI_FUNCTION("setLedStatus", setLedStatus),   //此函数是根据参数个数选择promise或者callback
        // DECLARE_NAPI_FUNCTION("getLedStatus", getLedStatus),    //作用未知
        // DECLARE_NAPI_STATIC_PROPERTY("LED_ON", gpioValHigh),    //给北向用的常量
        // DECLARE_NAPI_STATIC_PROPERTY("LED_OFF", gpioValLow),

    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

/*
 * 模块定义
 */
static napi_module turinning_demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registerGpioLed_DemoApis,
    .nm_modname = "turunning_demo", // 模块名
    .nm_priv = ((void *)0),
    .reserved = {0},
};


/*
 * 注册模块
 */
extern "C" __attribute__((constructor)) void RegisterGpioLed_DemoModule(void)
{
    napi_module_register(&turnning_demoModule); // 接口注册函数
}

/*存在的问题
1、LedAddOnData的参数不知道应该如何使用
2、getStatusWithPromise说的获取的参数不清楚，我这里觉得不需要获取参数，所以设置了0
3、mainExecuteCB能不能在业务逻辑函数直接将想要的值传给北向
4、registerGpioLed_DemoApis中的napi_creat_int32在这里有什么作用
5、在业务逻辑函数中返回的变量在注册接口中需要定义吧？不清楚怎么定义，因此还没有写这里的接口
6、.d.ts文件以模块名命名，不知道这样对不对
*/
// ./build.sh --product-name unionpi_tiger --build-target turnning_demo --ccache

/*代码上传
在根目录的git下输入下列代码
git config --global user.email 'hzu5@unionman.com.cn'
git add turning_sensor1.0
git commit -sm "add turnning_sensor"
git push
*/