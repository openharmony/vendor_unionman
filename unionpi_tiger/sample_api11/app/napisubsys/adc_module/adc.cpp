/* Copyright 2023 Unionman Technology Co., Ltd.
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

#include <cstdio>
#include <cstring>
#include <unistd.h>

#include "iadc.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

const char* Dir = "/data/storage/el2/base/haps/entry/files";
const double TEMP_CONST = 0.042;

struct FileData {
    napi_async_work asyncWork = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callback = nullptr;
};

// 存储每一秒的adc值到文件中
static int writeTemperature_To_File()
{
    constexpr int adc_channel = 1;
    int value = 0;
    std::string file_dir(Dir);
    file_dir.append("/temp_record.txt");
    int ret = 0;
    int num = 10;
    while (num >= 0) {
        if (get_adc_data(adc_channel, &value) < 0) {
            ret = -1;
            break;
        }
        if (writeTemperaturetoFile(file_dir.c_str(), value * TEMP_CONST) < 0) {
            ret = -1;
            break;
        }
        sleep(1);
        num--;
    }
    return 0;
}

// 业务逻辑处理函数，由worker线程池调度执行。
static void FileExecuteCB(napi_env env, void* data)
{
    (void)data;
    // 执行复杂计算，不阻塞主线程。
    writeTemperature_To_File();
}

static void FileCallbackCompleteCB(napi_env env, napi_status status, void* data)
{
    FileData* fileData = (FileData*)data;
    napi_value callback = nullptr;
    napi_get_reference_value(env, fileData->callback, &callback);
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);
    napi_value callbackResult = nullptr;
    napi_value result = nullptr;
    // 执行回调函数
    napi_call_function(env, undefined, callback, 1, &result, &callbackResult);
    // 删除napi_ref对象
    if (fileData->callback != nullptr) {
        napi_delete_reference(env, fileData->callback);
    }
    // 删除异步工作项
    napi_delete_async_work(env, fileData->asyncWork);
    delete fileData;
}

static napi_value filewriteCallback(napi_env env, napi_callback_info info)
{
    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete中传递数据
    auto fileData = new FileData {
        .asyncWork = nullptr,
    };
    // 创建async work，创建成功后通过最后一个参数接收async work的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "filewriteCallback", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        env, nullptr, resourceName, FileExecuteCB, FileCallbackCompleteCB, (void*)fileData, &fileData->asyncWork);
    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, fileData->asyncWork);
    // 原生方法返回空对象
    napi_value result = 0;
    NAPI_CALL(env, napi_get_null(env, &result));
    return result;
}

// 同步方法实现获取adc值
static napi_value get_adc_value(napi_env env, napi_callback_info info)
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
/*
 * 注册接口
 */
static napi_value registerFunc(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        // 声明函数供ets调用
        DECLARE_NAPI_FUNCTION("get_adc_value", get_adc_value),
        DECLARE_NAPI_FUNCTION("filewriteCallback", filewriteCallback),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}
/*
 * 模块定义
 */
static napi_module adc_napiModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registerFunc,
    .nm_modname = "adc_napi", // 模块名
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

/*
 * 注册模块
 */
extern "C" __attribute__((constructor)) void Registeradc(void)
{
    napi_module_register(&adc_napiModule); // 接口注册函数
}