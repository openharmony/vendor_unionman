/*
 * Copyright 2023 Unionman Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <unistd.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include "i2c_if.h" // HDF i2c 接口
#include "napi/native_api.h"
#include "napi/native_node_api.h"

#ifdef __cplusplus

extern "C" {
#endif

// 重新定义结构体方便使用
typedef struct {
    struct I2cMsg* i2cMsg;
    uint8_t msgLen; // i2cMsg的长度
} I2cMessage;
// 定义命令发送函数
int32_t SendCMD(DevHandle handle, uint16_t command, char addr)
{
    int32_t ret;
    I2cMessage i2cMessage;
    i2cMessage.msgLen = 1;
    i2cMessage.i2cMsg = new I2cMsg[1]; // 申请内存
    uint8_t cmdBuf[2L] = { 0 };

    cmdBuf[0] = command >> 8L; // 将命令拆分成高低位分别保存
    cmdBuf[1] = command & 0xFF;

    i2cMessage.i2cMsg[0].len = 2L;
    i2cMessage.i2cMsg[0].addr = 0x50;
    i2cMessage.i2cMsg[0].flags = 0;
    i2cMessage.i2cMsg[0].buf = cmdBuf;
    ret = I2cTransfer(handle, i2cMessage.i2cMsg, i2cMessage.msgLen);
    if (ret < 0) {
        delete i2cMessage.i2cMsg;
        return -1;
    }

    delete i2cMessage.i2cMsg; // 释放内存
    usleep(50L * 1000L); // 等待发送完成
    return 1;
}

uint8_t ReceiveCMD(DevHandle handle, char addr, uint8_t regData)
{
    I2cMessage i2cMessage;
    i2cMessage.msgLen = 2L;
    i2cMessage.i2cMsg = new I2cMsg[2];
    uint8_t resbuf = 0x01;

    i2cMessage.i2cMsg[0].len = 1L;
    i2cMessage.i2cMsg[0].addr = addr;
    i2cMessage.i2cMsg[0].flags = 0;
    i2cMessage.i2cMsg[0].buf = &regData;

    i2cMessage.i2cMsg[1].len = 1L;
    i2cMessage.i2cMsg[1].addr = addr;
    i2cMessage.i2cMsg[1].flags = 1;
    i2cMessage.i2cMsg[1].buf = &resbuf;
    I2cTransfer(handle, i2cMessage.i2cMsg, i2cMessage.msgLen);
    delete i2cMessage.i2cMsg;
    return resbuf;
}

struct AddOnData {
    napi_async_work asyncWork = nullptr; // 异步工作项
    napi_deferred deferred = nullptr; // 用于Promise的resolve、reject处理
    napi_ref callback = nullptr; // 回调函数
    int args[1] = {};
    float result = 0; // 业务逻辑处理结果（返回值）
};
// 异步实现ADC读取
static void i2c40PinExecuteCB(napi_env env, void* data)
{
    AddOnData* addOnData = (AddOnData*)data;
    /* 业务函数 */
    DevHandle i2cHandle;
    int32_t ret;
    uint8_t resbuf = 0x01;
    uint8_t resbuf1 = 0x01;
    i2cHandle = I2cOpen(5L);
    if (i2cHandle == NULL) {
        I2cClose(i2cHandle);
    }
    ret = SendCMD(i2cHandle, 0x0111, 0x50); // 关闭reset命令
    resbuf = ReceiveCMD(i2cHandle, 0x50, 0x01);
    if (resbuf == 0x11) {
        ret = 1;
    } else {
        ret = 0;
    }
    ret = SendCMD(i2cHandle, 0x0112, 0x51);
    resbuf1 = ReceiveCMD(i2cHandle, 0x51, 0x01);
    if (resbuf1 == 0x12) {
        ret = 1;
    } else {
        ret = 0;
    }
    addOnData->result = ret; // 将需要返回的值赋予给result
}

static void i2cNineAxleExecuteCB(napi_env env, void* data)
{
    AddOnData* addOnData = (AddOnData*)data;
    /* 业务函数 */
    DevHandle i2cHandle;
    int32_t ret;
    uint8_t resbuf = 0x01;
    i2cHandle = I2cOpen(8L);
    (i2cHandle == NULL)
    {
        I2cClose(i2cHandle);
    }
    resbuf = ReceiveCMD(i2cHandle, 0x69, 0x06);
    if (resbuf == 0x41) {
        ret = 1;
    } else {
        ret = 0;
    }
    addOnData->result = ret; // 将需要返回的值赋予给result
}

static void i2cRtcExecuteCB(napi_env env, void* data)
{
    AddOnData* addOnData = (AddOnData*)data;
    /* 业务函数 */
    DevHandle i2cHandle;
    int32_t ret;
    uint8_t resbuf = 0x01;
    i2cHandle = I2cOpen(4L);
    if (i2cHandle == NULL) {
        I2cClose(i2cHandle);
    }
    ret = SendCMD(i2cHandle, 0x0825, 0x51); // 关闭reset命令
    resbuf = ReceiveCMD(i2cHandle, 0x51, 0x08);
    if (resbuf == 0X23) {
        ret = 1;
    } else {
        ret = 0;
    }
    addOnData->result = ret; // 将需要返回的值赋予给result
}


static void completeCBForPromise(napi_env env, napi_status status, void* data)
{
    AddOnData* addOnData = (AddOnData*)data;
    napi_value result = nullptr;
    napi_create_int32(env, addOnData->result, &result);
    napi_resolve_deferred(env, addOnData->deferred, result);
    /* 以上这段如果在结构体中命名不同则需要更改 */
    // 删除napi_ref对象
    if (addOnData->callback != nullptr) {
        napi_delete_reference(env, addOnData->callback);
    }
    // 删除异步工作项
    napi_delete_async_work(env, addOnData->asyncWork);
    delete addOnData;
}

static napi_value UM_i2c_40Pin_test(napi_env env, napi_callback_info info)
{
    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto addonData = new AddOnData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };
    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "UM_i2c_40Pin", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        env, nullptr, resourceName, i2c40PinExecuteCB, completeCBForPromise, (void*)addonData, &addonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    // 返回promise
    return promise;
}

static napi_value UM_i2c_nine_axles_test(napi_env env, napi_callback_info info)
{
    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto addonData = new AddOnData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };
    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "UM_i2c_nine_axles", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, i2cNineAxleExecuteCB, completeCBForPromise, (void*)addonData,
        &addonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    // 返回promise
    return promise;
}

static napi_value UM_i2c_rtc_test(napi_env env, napi_callback_info info)
{
    // 创建promise
    napi_value promise = nullptr;
    napi_deferred deferred = nullptr;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

    // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
    auto addonData = new AddOnData {
        .asyncWork = nullptr,
        .deferred = deferred,
    };
    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "UM_i2c_rtc_test", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        env, nullptr, resourceName, i2cRtcExecuteCB, completeCBForPromise, (void*)addonData, &addonData->asyncWork);

    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);

    // 返回promise
    return promise;
}

// 注册接口
static napi_value registeri2cNapiDemoApis(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("UM_i2c_40Pin_test", UM_i2c_40Pin_test),
        DECLARE_NAPI_FUNCTION("UM_i2c_nine_axles_test", UM_i2c_nine_axles_test),
        DECLARE_NAPI_FUNCTION("UM_i2c_rtc_test", UM_i2c_rtc_test),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}
// 模块定义
static napi_module i2cnapidemoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registeri2cNapiDemoApis,
    .nm_modname = "i2cnapi",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};
#ifdef __cplusplus
}
#endif
// 注册模块
extern "C" __attribute__((constructor)) void Registeri2cNapidemoModule(void)
{
    napi_module_register(&i2cnapidemoModule); // 接口注册函数
}
