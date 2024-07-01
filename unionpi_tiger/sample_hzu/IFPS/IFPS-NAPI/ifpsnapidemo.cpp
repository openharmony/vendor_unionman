#include <cstdio>
#include <cstring>
#include <unistd.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "um_adc.h"
#include "um_pwm.h"
#include "i2cinterface.h"
#include "sample_client.h"

static const int SERVO_INIT_DUTY = 500000;
static const int SERVO_MAX_DUTY = 2500000;

#ifdef __cplusplus
extern "C" {
#endif

// 温湿度传感器全局变量声明
static sRHAndTemp_t tempRH;
static float TemperatureC;
static float Humidity;
static float TemperatureF;

// 温湿度传感器 IIC 设备地址
static const uint16_t device_addr = 0x44;

// 温湿度传感器 IIC 设备号
char dev_name[] = "/dev/i2c-5";

// 同步方法实现温湿度传感器软重启
static napi_value Sht3xSoftReset(napi_env env, napi_callback_info info)
{
    int value;
    napi_value ret = nullptr;
    value = SoftReset(dev_name, device_addr);
    usleep(50L * 1000L);
    NAPI_CALL(env, napi_create_int32(env, value, &ret));
    return ret;
}

// 同步方法实现温湿度传感器模式选择
static napi_value Sht3xModeSet(napi_env env, napi_callback_info info)
{
    // 获取JS传入的参数
    size_t argc = 2L;
    napi_value args[2L] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));

    // 验证参数数据类型
    napi_valuetype valuetype0;
    NAPI_CALL(env, napi_typeof(env, args[0], &valuetype0));
    napi_valuetype valuetype1;
    NAPI_CALL(env, napi_typeof(env, args[1], &valuetype1));
    NAPI_ASSERT(env, valuetype0 == napi_number && valuetype1 == napi_number, "Wrong argument type. Number expected.");

    // NAPI类型数据转为C/C++类型
    int mps;
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &mps));
    int repeatability;
    NAPI_CALL(env, napi_get_value_int32(env, args[1], &repeatability));

    // 业务执行
    int value;
    napi_value ret = nullptr;
    value = ModeSet(dev_name, device_addr, mps, repeatability);
    usleep(20L * 1000L);
    NAPI_CALL(env, napi_create_int32(env, value, &ret));
    return ret;
}

// 同步方法实现读取温湿度传感器数据
static napi_value Sht3xReadData(napi_env env, napi_callback_info info)
{
    int value;
    napi_value ret = nullptr;
    value = ReadTempAndHum(dev_name, device_addr, &tempRH);
    usleep(20L * 1000L);
    NAPI_CALL(env, napi_create_double(env, value, &ret));
    TemperatureC = tempRH.TemperatureC;
    Humidity = tempRH.Humidity;
    TemperatureF = tempRH.TemperatureF;
    return ret;
}

// 将摄氏温度传到APP(运行此函数前需要运行Sht3xReadData函数)
static napi_value Sht3xReadTemperatureC(napi_env env, napi_callback_info info)
{
    napi_value tempC = nullptr;
    NAPI_CALL(env, napi_create_double(env, TemperatureC, &tempC));
    return tempC;
}
// 将湿度传到APP(运行此函数前需要运行Sht3xReadData函数)
static napi_value Sht3xReadHumidity(napi_env env, napi_callback_info info)
{
    napi_value hum = nullptr;
    NAPI_CALL(env, napi_create_double(env, Humidity, &hum));
    return hum;
}
// 将华氏温度传到APP(运行此函数前需要运行Sht3xReadData函数)
static napi_value Sht3xReadTemperatureF(napi_env env, napi_callback_info info)
{
    napi_value tempF = nullptr;
    NAPI_CALL(env, napi_create_double(env, TemperatureF, &tempF));
    return tempF;
}




// 同步方法实现获取烟雾传感器adc值
static napi_value Mq2GetAdcValue(napi_env env, napi_callback_info info)
{
    constexpr int adc_channel = 1;
    int value = 0;
    if (get_adc_data(adc_channel, &value) < 0) {
        value = -1;
    }

    napi_value ret;
    NAPI_CALL(env, napi_create_int32(env, value, &ret));
    return ret;
}

// 同步方法实现获取温湿度传感器（ADC通道）权限
static napi_value Mq2GetPermission(napi_env env, napi_callback_info info)
{
    int result = 0;
    constexpr char path[128] = "/sys/bus/iio/devices/iio:device0/in_voltage2_raw";
    result = open_permission(path);

    napi_value ret;
    NAPI_CALL(env, napi_create_int32(env, result, &ret));
    return ret;
}


// 同步方法实现获取火焰传感器adc值
static napi_value FlameSensorGetAdcValue(napi_env env, napi_callback_info info)
{
    constexpr int adc_channel = 2;
    int value = 0;
    if (get_adc_data(adc_channel, &value) < 0) {
        value = -1;
    }

    napi_value ret;
    NAPI_CALL(env, napi_create_int32(env, value, &ret));
    return ret;
}

// 同步方法实现获取火焰传感器（ADC通道）权限
static napi_value FlameSensorGetPermission(napi_env env, napi_callback_info info)
{
    int result = 0;
    constexpr char path[128] = "/sys/bus/iio/devices/iio:device0/in_voltage3_raw";
    result = open_permission(path);

    napi_value ret;
    NAPI_CALL(env, napi_create_int32(env, result, &ret));
    return ret;
}


struct PwmOnData {
    napi_async_work asyncWork = nullptr; // 异步工作项
    napi_deferred deferred = nullptr;    // 用于Promise的resolve、reject处理
    napi_ref callback = nullptr;         // 回调函数
    int args[2] = {0};                   // 2个输入参数
    int result = 0;                      // 业务逻辑处理结果（返回值）
};

// 业务逻辑处理函数，由worker线程池调度执行。
static void executeCB(napi_env env, void *data)
{
    PwmOnData *pwmData = (PwmOnData *)data;

    int pwmPex = pwmData->args[0];
    int pwmAngle = pwmData->args[1];

    set_pwm_period(pwmPex, SERVO_MAX_DUTY);
    double pwmDuty = 1.0 * SERVO_INIT_DUTY * pwmAngle / 45L + SERVO_INIT_DUTY;
    set_pwm_dutyCycle(pwmPex, static_cast<int>(pwmDuty));
    set_pwm_enable(pwmPex, 1);

    pwmData->result = is_pwm_enabled(pwmPex);
}

// 业务逻辑处理完成回调函数，在业务逻辑处理函数执行完成或取消后触发，由EventLoop线程中执行。
static void completeCBForPromise(napi_env env, napi_status status, void *data)
{
    PwmOnData *pwmData = (PwmOnData *)data;
    napi_value result = nullptr;
    napi_create_int32(env, pwmData->result, &result);
    napi_resolve_deferred(env, pwmData->deferred, result);

    // 删除napi_ref对象
    if (pwmData->callback != nullptr) {
        napi_delete_reference(env, pwmData->callback);
    }

    // 删除异步工作项
    napi_delete_async_work(env, pwmData->asyncWork);
    delete pwmData;
}

// pwm设置，用于舵机（PWM1） 蜂鸣器（PWM2）
static napi_value SetPwmStatus(napi_env env, napi_callback_info info)
{
    // 获取2个参数，值的类型是js类型(napi_value)
    size_t argc = 2L;
    napi_value args[2];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto pwmData = new PwmOnData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };

    // 将被收到的参数传入
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &pwmData->args[0]));
    NAPI_CALL(env, napi_get_value_int32(env, args[1], &pwmData->args[1]));

    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "setPwmStatus", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, executeCB, completeCBForPromise, (void *)pwmData,
                           &pwmData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, pwmData->asyncWork);

    // 返回promise
    return promise;
}




// 注册接口
static napi_value registerifpsNapiDemoApis(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("Sht3xSoftReset", Sht3xSoftReset),
        DECLARE_NAPI_FUNCTION("Sht3xModeSet", Sht3xModeSet),
        DECLARE_NAPI_FUNCTION("Sht3xReadData", Sht3xReadData),
        DECLARE_NAPI_FUNCTION("Sht3xReadTemperatureC", Sht3xReadTemperatureC),
        DECLARE_NAPI_FUNCTION("Sht3xReadHumidity", Sht3xReadHumidity),
        DECLARE_NAPI_FUNCTION("Sht3xReadTemperatureF", Sht3xReadTemperatureF),
        DECLARE_NAPI_FUNCTION("Mq2GetAdcValue", Mq2GetAdcValue),
        DECLARE_NAPI_FUNCTION("Mq2GetPermission", Mq2GetPermission),
        DECLARE_NAPI_FUNCTION("FlameSensorGetAdcValue", FlameSensorGetAdcValue),
        DECLARE_NAPI_FUNCTION("FlameSensorGetPermission", FlameSensorGetPermission),
        DECLARE_NAPI_FUNCTION("SetPwmStatus", SetPwmStatus),

    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}



// 模块定义
static napi_module ifpsnapidemoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registerifpsNapiDemoApis,
    .nm_modname = "ifpsnapidemo",
    .nm_priv = ((void *)0),
    .reserved = {0},
};
#ifdef __cplusplus
}
#endif
// 注册模块
extern "C" __attribute__((constructor)) void RegisterifpsNapidemoModule(void)
{
    napi_module_register(&ifpsnapidemoModule); // 接口注册函数
}
