/* Copyright 2024 Unionman Technology Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <cstring>
#include <cstdio>
#include "napi/native_node_api.h"
#include "napi/native_api.h"

#include "./pwm/um_pwm.h"
#include "./uart/um_uart.h"
//用于在各个线程之间传递数据
struct AddonData {
    napi_async_work async_work = nullptr;   // 异步工作项
    napi_deferred deferred = nullptr;   // 用于Promise的resolve、reject处理
    napi_ref callback = nullptr;    // 回调函数
    int args[2] = {0};      //设置传参
    int result = 0;     //设置返回值
};

//Execute线程 业务逻辑函数
static void uart_get_execute(napi_env env, void *data)
{
    AddonData *addonData = (AddonData *)data;

    addonData->result = UmGetUart();
}
//业务逻辑处理完成回调函数
static void uart_get_complete(napi_env env, napi_status status, void *data) 
{
    //获取data数据
    AddonData *addOnData = (AddonData *) data;
    napi_value callback = nullptr;
    napi_get_reference_value(env, addOnData->callback, &callback);
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);
    //转换数据类型
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
    napi_delete_async_work(env, addOnData->async_work);
    delete addOnData;
}

//串口数据获取 - CallBack异步
static napi_value uart_get(napi_env env, napi_callback_info info)
{
    //只需要修改获取的参数值
    size_t argc = 1;
    napi_value args[1];
    napi_value thisArg = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete中传递数据
    auto addonData = new AddonData{
        .async_work = nullptr,
    };

    // 将接收到的参数传入用户自定义上下文数据
    NAPI_CALL(env, napi_create_reference(env, args[0], 1, &addonData->callback));

    // 创建async work，创建成功后通过最后一个参数接收async work的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "uart_get", NAPI_AUTO_LENGTH, &resourceName);   // ，上下文的xx对象，字符长度，异步资源标识符
    // ， ，异步资源标识符，业务逻辑函数，业务完成函数，addonData传递数据，返回当前函数调用创建的异步工作项
    napi_create_async_work(env, nullptr, resourceName, uart_get_execute, uart_get_complete, (void *)addonData,    
                            &addonData->async_work);       

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->async_work);

    // 原生方法返回空对象
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}
//串口初始化 - 同步函数
static napi_value uart_init(napi_env env, napi_callback_info info)
{
    size_t argc = 0;     //参数个数 
    napi_value thisVar = nullptr;   //JS对象的this参数 
    void* data = nullptr;    //回调数据指针 

    //根据环境变量获取参数
    napi_get_cb_info(env, info, &argc, nullptr, &thisVar, &data); 
        
    //业务代码
    int ret = UmInitUart();
    
    //将结果返回,转换回NAPI类型
    napi_value result;
    NAPI_CALL(env, napi_create_int32(env, ret, &result));
    return result; //返回JS对象 
}

//pwm控制-同步函数
static napi_value pwm_set(napi_env env, napi_callback_info info)
{
    size_t argc = 1; //参数个数 
    napi_value argv[1] = {0}; //参数定义 
    napi_value thisVar = nullptr; //JS对象的this参数 
    void* data = nullptr; //回调数据指针 

    //根据环境变量获取参数
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data); 
        
    //NAPI类型转换C/C++数据类型
    int value;
    NAPI_CALL(env, napi_get_value_int32(env, argv[0], &value));
    
    //业务代码
    int ret = 0;
    value += 50; //转换成0-100
    int finalDuty=500000L + value * 20000L;
    
    UmInitPwm(PWM1);
    UmSetPwmPeriod(PWM1, 20000000L);
    UmSetPwmDutyCycle(PWM1, finalDuty);
    UmSetPwmEnable(PWM1, PWM_IS_ENABLED);
    
    //将结果返回,转换回NAPI类型
    napi_value result;
    NAPI_CALL(env, napi_create_int32(env, ret, &result));
    return result; 
}
//传感器休眠 - 同步函数
static napi_value uart_sleep(napi_env env, napi_callback_info info)
{
    size_t argc = 1;    //参数个数 
    napi_value argv[1] = {0};     //参数定义 
    napi_value thisVar = nullptr;   //JS对象的this参数 
    void* data = nullptr;   //回调数据指针 

    //根据环境变量获取参数
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data); 
        
    //NAPI类型转换C/C++数据类型
    int value;
    NAPI_CALL(env, napi_get_value_int32(env, argv[0], &value));
    
    //业务代码
    int ret = 0;
    ret = UmSleepSet(value);
    
    //将结果返回,转换回NAPI类型
    napi_value result;
    NAPI_CALL(env, napi_create_int32(env, ret, &result));
    return result; //返回JS对象 
}
/**********以下步骤为注册操作*************/

//接口注册函数
static napi_value register_init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("uart_get", uart_get),
        DECLARE_NAPI_FUNCTION("uart_init", uart_init),
        DECLARE_NAPI_FUNCTION("uart_sleep", uart_sleep),
        DECLARE_NAPI_FUNCTION("pwm_set", pwm_set),    //TS和CPP对应函数，第一个参数就是TS里的函数名，TS定义需一致
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

// 定义napi_module，import nm_modname from '@ohos.ohos_shared_library_name'
static napi_module napitestModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = register_init,  //接口注册函数
    .nm_modname = "control_demo",  //定义模块 对应import test_demo ，和.d.ts里面的namespace一致
    .nm_priv = ((void* ) 0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void NapitestModuleRegister(void) 
{
    napi_module_register(&napitestModule);  //接口注册函数
}