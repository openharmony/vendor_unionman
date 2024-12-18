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

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#ifdef __cplusplus
#include "serial_uart.h"
extern "C" {
#endif

// 同步方法
static napi_value sync_UM_uart_test(napi_env env, napi_callback_info info) {
  /* 根据环境变量获取参数 */
  size_t argc = 1;          // 参数个数
  napi_value argv[1] = {0}; // 参数定义

  /* 入参变量获取 */
  napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
  // 获取入参的类型
  napi_valuetype valueType0 = napi_undefined;
  napi_typeof(env, argv[0], &valueType0);

  if (valueType0 != napi_number) {
    // 输入参数类型错误
    napi_throw_type_error(env, NULL, "Wrong arguments. 1 number is expected.");
    return NULL;
  }
  // 入参值（数字）转换为C/C++可以操作的数据类型
  double device_num;
  napi_get_value_double(env, argv[0], &device_num);

  napi_value ret = nullptr;
  int result = uarttest(device_num);

  napi_create_int32(env, result, &ret);
  return ret;
}

// 异步方法
struct uartOnData {
  napi_async_work asyncWork = nullptr; // 异步工作项
  napi_deferred deferred = nullptr;    // 用于Promise的resolve、reject处理
  napi_ref callback = nullptr;         // 回调函数
  int args[1] = {0};
  int result = 0; // 业务逻辑处理结果（返回值）
};

// 业务逻辑处理函数，由worker线程池调度执行。
static void executeCB(napi_env env, void *data) {
  uartOnData *uartData = (uartOnData *)data;
  int device_num = uartData->args[0];
  int ret = 0;
  ret = uarttest(device_num);
  uartData->result = ret;
}

// 业务逻辑处理完成回调函数，在业务逻辑处理函数执行完成或取消后触发，由EventLoop线程中执行。
static void completeCBForPromise(napi_env env, napi_status status, void *data) {
  uartOnData *uartData = (uartOnData *)data;
  napi_value result = nullptr;
  napi_create_int32(env, uartData->result, &result);
  napi_resolve_deferred(env, uartData->deferred, result);

  // 删除napi_ref对象
  if (uartData->callback != nullptr) {
    napi_delete_reference(env, uartData->callback);
  }

  // 删除异步工作项
  napi_delete_async_work(env, uartData->asyncWork);
  delete uartData;
}

static napi_value UM_uart_test(napi_env env, napi_callback_info info) {
  // 获取1个参数，值的类型是js类型(napi_value)
  size_t argc = 1;
  napi_value args[1];
  napi_value thisArg = nullptr;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));

  // 创建promise
  napi_value promise = nullptr;
  napi_deferred deferred = nullptr;
  NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

  // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
  auto uartData = new uartOnData{
      .asyncWork = nullptr,
      .deferred = deferred,
  };

  // 将被收到的参数传入
  NAPI_CALL(env, napi_get_value_int32(env, args[0], &uartData->args[0]));

  // 创建async
  // work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
  napi_value resourceName = nullptr;
  napi_create_string_utf8(env, "UM_uart_test", NAPI_AUTO_LENGTH, &resourceName);
  napi_create_async_work(env, nullptr, resourceName, executeCB,
                         completeCBForPromise, (void *)uartData,
                         &uartData->asyncWork);

  // 将刚创建的async work加到队列，由底层去调度执行
  napi_queue_async_work(env, uartData->asyncWork);

  // 返回promise
  return promise;
}

/*
 * 注册接口
 */
static napi_value registeruartnapiApis(napi_env env, napi_value exports) {
  napi_property_descriptor desc[] = {
      DECLARE_NAPI_FUNCTION("UM_uart_test", UM_uart_test),
      DECLARE_NAPI_FUNCTION("sync_UM_uart_test", sync_UM_uart_test),
  };
  NAPI_CALL(env, napi_define_properties(env, exports,
                                        sizeof(desc) / sizeof(desc[0]), desc));
  return exports;
}

/*
 * 模块定义
 */
static napi_module uartnapiModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registeruartnapiApis,
    .nm_modname = "uarttest", // 模块名
    .nm_priv = ((void *)0),
    .reserved = {0},
};

#ifdef __cplusplus
}
#endif
/*
 * 注册模块
 */
extern "C" __attribute__((constructor)) void RegisteruartnapiModule(void) {
  napi_module_register(&uartnapiModule); // 接口注册函数
}
