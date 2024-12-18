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

#include "hdf_log.h" // 标准日志打印头文件
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "osal_time.h" // 标准延迟&睡眠接口头文件
#include "spi_if.h"

#ifdef __cplusplus
extern "C" {
#endif

struct spiOnData {
  napi_async_work asyncWork = nullptr; // 异步工作项
  napi_deferred deferred = nullptr;    // 用于Promise的resolve、reject处理
  napi_ref callback = nullptr;         // 回调函数
  int result = 0; // 业务逻辑处理结果（返回值）
};

static int32_t spi_test() {
  struct SpiDevInfo spiDevinfo;     /* SPI设备描述符 */
  DevHandle spiHandle = NULL;       /* SPI设备句柄 */
  spiDevinfo.busNum = 0;            /* SPI设备总线号 */
  spiDevinfo.csNum = 0;             /* SPI设备片选号 */
  spiHandle = SpiOpen(&spiDevinfo); /* 根据spiDevinfo获取SPI设备句柄 */
  int32_t ret;
  struct SpiMsg msg; // 自定义传输的消息
  uint8_t rbuff[5] = {0};
  uint8_t wbuff[5] = {0x9f, 0x0, 0x0, 0x0, 0x0};

  msg.wbuf = wbuff; // 写入的数据
  msg.rbuf = rbuff; // 读取的数据
  msg.len = 5L;     // 读取写入数据的长度为4
  msg.keepCs = 0; // 当前传输完成后是否保持CS活动，1表述保持，0表示关闭CS
  msg.delayUs = 1;     // 进行下一次传输前不进行延时
  msg.speed = 115200L; // 本次传输的速度
  // 进行一次自定义传输，传输的msg个数为1
  ret = SpiTransfer(spiHandle, &msg, 1);
  if (ret != HDF_SUCCESS) {
    HDF_LOGE("SpiTestSample: spi transfer fail, ret:%d!\n", ret);
    // 销毁SPI设备句柄
    SpiClose(spiHandle);
    return 0;
  }
  HDF_LOGD("SpiTestSample: spi tests end!");
  if (msg.rbuf[0] == 0x0 && msg.rbuf[1] == 0xef && msg.rbuf[2L] == 0x40 &&
      msg.rbuf[3L] == 0x16) {
    HDF_LOGD("SpiTestSample: spi tests success!");
    return 1;
  }
}

// 业务逻辑处理函数，由worker线程池调度执行。
static void executeCB(napi_env env, void *data) {
  spiOnData *spiData = (spiOnData *)data;
  int ret = 0;
  ret = spi_test();
  spiData->result = ret;
}

// 业务逻辑处理完成回调函数，在业务逻辑处理函数执行完成或取消后触发，由EventLoop线程中执行。
static void completeCBForPromise(napi_env env, napi_status status, void *data) {
  spiOnData *spiData = (spiOnData *)data;
  napi_value result = nullptr;
  napi_create_int32(env, spiData->result, &result);
  napi_resolve_deferred(env, spiData->deferred, result);

  // 删除napi_ref对象
  if (spiData->callback != nullptr) {
    napi_delete_reference(env, spiData->callback);
  }

  // 删除异步工作项
  napi_delete_async_work(env, spiData->asyncWork);
  delete spiData;
}

static napi_value UM_spi_test(napi_env env, napi_callback_info info) {
  // 创建promise
  napi_value promise = nullptr;
  napi_deferred deferred = nullptr;
  NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

  // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
  auto spiData = new spiOnData{
      .asyncWork = nullptr,
      .deferred = deferred,
  };

  // 创建async
  // work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
  napi_value resourceName = nullptr;
  napi_create_string_utf8(env, "UM_spi_test", NAPI_AUTO_LENGTH, &resourceName);
  napi_create_async_work(env, nullptr, resourceName, executeCB,
                         completeCBForPromise, (void *)spiData,
                         &spiData->asyncWork);

  // 将刚创建的async work加到队列，由底层去调度执行
  napi_queue_async_work(env, spiData->asyncWork);

  // 返回promise
  return promise;
}

/*
 * 注册接口
 */
static napi_value registerspinapiApis(napi_env env, napi_value exports) {
  napi_property_descriptor desc[] = {
      DECLARE_NAPI_FUNCTION("UM_spi_test", UM_spi_test),
  };
  NAPI_CALL(env, napi_define_properties(env, exports,
                                        sizeof(desc) / sizeof(desc[0]), desc));
  return exports;
}

/*
 * 模块定义
 */
static napi_module spinapiModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registerspinapiApis,
    .nm_modname = "spitest", // 模块名
    .nm_priv = ((void *)0),
    .reserved = {0},
};

#ifdef __cplusplus
}
#endif
/*
 * 注册模块
 */
extern "C" __attribute__((constructor)) void RegisterspinapiModule(void) {
  napi_module_register(&spinapiModule); // 接口注册函数
}
