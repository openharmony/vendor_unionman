#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "um_adc.h"
#include "um_gpio.h"
#include "um_pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

struct PumpOnData {
    napi_async_work asyncWork = nullptr; // 异步工作项
    napi_deferred deferred = nullptr; // 用于Promise的resolve、reject处理
    napi_ref callback = nullptr;      // 回调函数
    int args[1] = {0};                // 2个输入参数
    int result = 0;                   // 业务逻辑处理结果（返回值）
};

// 水泵定时器触发的回调函数
static void _timer_handler_pump(union sigval arg) {
    UM_GPIO_SetValue(UM_GPIO_07, 1);

    usleep(30 * 60 * 1000 * 1000);

    UM_GPIO_SetValue(UM_GPIO_07, 0);
}

// 舵机定时器触发的回调函数
static void _timer_handler_steering(union sigval arg) {
    int pwmChannel = PWM1;
    int angle = 90;

    init_pmw(PWM1);
    // 设置pwm的溢出值
    set_pwm_period(pwmChannel, 2600000L);

    // 设置转动角度
    set_angle(PWM1, angle, PWM_POLARITY_NORMAL);

    usleep(1000 * 1000);

    set_angle(PWM1, 0, PWM_POLARITY_NORMAL);
}

/*
    业务逻辑处理函数
    在time秒后打开水泵
*/
static void turn_on_pump(napi_env env, void *data) {
    PumpOnData *pumpOnData = (PumpOnData *)data;

    int gpioNum = UM_GPIO_07;
    int bExport = UM_GPIO_EXPORTED;
    int direction = UM_GPIO_DIRECTION_OUT;

    struct sigevent sev1;
    struct itimerspec ts1;
    timer_t timerid1;

    UM_GPIO_Export(gpioNum, bExport);

    UM_GPIO_SetDirection(gpioNum, direction);

    // 设置定时器
    sev1.sigev_notify = SIGEV_THREAD; // 使用线程通知方式
    sev1.sigev_notify_function = _timer_handler_pump; // 设置回调函数
    sev1.sigev_notify_attributes = NULL;
    sev1.sigev_value.sival_ptr = &timerid1; // 将定时器 ID 传递给回调函数

    // 创建第一个定时器
    timer_create(CLOCK_REALTIME, &sev1, &timerid1);

    // 设置第一个定时器的触发时间为 time秒后
    ts1.it_value.tv_sec = pumpOnData->args[0]; // time 秒后触发
    ts1.it_value.tv_nsec = 0;
    ts1.it_interval.tv_sec = 0; // 不重复触发
    ts1.it_interval.tv_nsec = 0;

    // 启动第一个定时器
    printf("timer of pump start!\n");
    timer_settime(timerid1, 0, &ts1, NULL);

    pumpOnData->result = 0;
}

/*
    在time秒后打开舵机
*/
static void turn_on_steering(napi_env env, void *data) {
    PumpOnData *pumpOnData = (PumpOnData *)data;

    struct sigevent sev1;
    struct itimerspec ts1;
    timer_t timerid1;
    // 设置定时器
    sev1.sigev_notify = SIGEV_THREAD; // 使用线程通知方式
    sev1.sigev_notify_function = _timer_handler_steering; // 设置回调函数
    sev1.sigev_notify_attributes = NULL;
    sev1.sigev_value.sival_ptr = &timerid1; // 将定时器 ID 传递给回调函数

    // 创建第一个定时器
    timer_create(CLOCK_REALTIME, &sev1, &timerid1);

    // 设置第一个定时器的触发时间为 time秒后
    ts1.it_value.tv_sec = pumpOnData->args[0]; // time 秒后触发
    ts1.it_value.tv_nsec = 0;
    ts1.it_interval.tv_sec = 0; // 不重复触发
    ts1.it_interval.tv_nsec = 0;

    // 启动第一个定时器
    printf("timer of steering start!\n");
    timer_settime(timerid1, 0, &ts1, NULL);

    pumpOnData->result = 0;
}

/*
    获取水位传感器数据
*/
static void turn_on_waterlever(napi_env env, void *data) {
    PumpOnData *addOnData = (PumpOnData *)data;
    int value = 0;
    double water_level = 0;
    get_adc_data(ADC_1, &value);
    water_level = ((double)value) * 3 / 4096;
    addOnData->result = water_level;
}

/*
    业务逻辑处理完成回调函数
*/
static void completeCBForPromise(napi_env env, napi_status status, void *data) {
    PumpOnData *pumpOnData = (PumpOnData *)data;
    napi_value result = nullptr;
    napi_create_int32(env, pumpOnData->result, &result);
    napi_resolve_deferred(env, pumpOnData->deferred, result);

    // 删除napi_ref对象
    if (pumpOnData->callback != nullptr) {
        napi_delete_reference(env, pumpOnData->callback);
    }

    // 删除异步工作项
    napi_delete_async_work(env, pumpOnData->asyncWork);
    delete pumpOnData;
}

static napi_value setPumpWithPromise(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto addonData = new PumpOnData{
        .asyncWork = nullptr,
        .deferred = deferred,
    };

    // 将被收到的参数传入
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &addonData->args[0]));

    // 创建async
    // work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, __func__, NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, turn_on_pump,
                           completeCBForPromise, (void *)addonData,
                           &addonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    // 返回promise
    return promise;
}

static napi_value setSteeringWithPromise(napi_env env,
                                         napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto addonData = new PumpOnData{
        .asyncWork = nullptr,
        .deferred = deferred,
    };

    // 将被收到的参数传入
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &addonData->args[0]));

    // 创建async
    // work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, __func__, NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, turn_on_steering,
                           completeCBForPromise, (void *)addonData,
                           &addonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    // 返回promise
    return promise;
}

static napi_value getWaterLevelWithPromise(napi_env env,
                                           napi_callback_info info) {
    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto addonData = new PumpOnData{
        .asyncWork = nullptr,
        .deferred = deferred,
    };

    // 创建async
    // work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, __func__, NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, turn_on_waterlever,
                           completeCBForPromise, (void *)addonData,
                           &addonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    // 返回promise
    return promise;
}

/*
    注册接口
*/
static napi_value registerFishTank_DemoApis(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("setPumpWithPromise", setPumpWithPromise),
        DECLARE_NAPI_FUNCTION("setSteeringWithPromise", setSteeringWithPromise),
        DECLARE_NAPI_FUNCTION("getWaterLevelWithPromise",
                              getWaterLevelWithPromise),

    };
    NAPI_CALL(env, napi_define_properties(
                       env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

/*
    模块定义
*/
static napi_module fish_tank_demo_module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registerFishTank_DemoApis,
    .nm_modname = "fish_tank_demo", // 模块名
    .nm_priv = ((void *)0),
    .reserved = {0},
};

/*
    注册模块
*/
__attribute__((constructor)) void RegisterFishTankDemoModule(void) {
    napi_module_register(&fish_tank_demo_module); // 接口注册函数
}

#ifdef __cplusplus
}
#endif
