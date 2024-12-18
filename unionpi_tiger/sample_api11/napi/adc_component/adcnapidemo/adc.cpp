/*
 * Copyright (c) 2023 Unionman Technology Co., Ltd.
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

#include <cstdio>
#include <cstring>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "sample_client.h"
#include "um_adc.h"

struct AddOnData {
  napi_async_work asyncWork = nullptr; // 异步工作项
  napi_deferred deferred = nullptr;    // 用于Promise的resolve、reject处理
  napi_ref callback = nullptr;         // 回调函数
  int args[1] = {};
  float result = 0; // 业务逻辑处理结果（返回值）
};
// 异步实现ADC读取
static void getAdcExecuteCB(napi_env env, void *data) {
  AddOnData *addOnData = (AddOnData *)data;
  int adc_channel = (addOnData->args[0]) - 1;
  int value = 5;
  int ret = 0;
  if (UM_hdc_get_value(adc_channel, &value) < 0) {
    ret = -1;
  } else {
    if (adc_channel < 2L) {
      if (value < 960L && value > 920L) {
        ret = 1;
      } else {
        ret = 0;
      }
    } else {
      if (value > 300L) {
        ret = 1;
      } else {
        ret = 0;
      }
    }
  }
  /* 业务函数 */
  addOnData->result = ret; // 将需要返回的值赋予给result
}

static void completeCBForPromise(napi_env env, napi_status status, void *data) {
  AddOnData *addOnData = (AddOnData *)data;
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

static napi_value UM_adc_test(napi_env env, napi_callback_info info) {
  // 获取2个参数，值的类型是js类型(napi_value)
  size_t argc = 1;
  napi_value args[1];
  napi_value thisArg = nullptr;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisArg, nullptr));
  // 创建promise
  napi_value promise = nullptr;
  napi_deferred deferred = nullptr;
  NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));

  // 异步工作项上下文用户数据，传递到异步工作项的execute、complete之间传递数据
  auto addonData = new AddOnData{
      .asyncWork = nullptr,
      .deferred = deferred,
  };
  // 将被收到的参数传入
  NAPI_CALL(env, napi_get_value_int32(env, args[0], &addonData->args[0]));
  // 创建async
  // work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
  napi_value resourceName = nullptr;
  napi_create_string_utf8(env, "UM_adc_test", NAPI_AUTO_LENGTH, &resourceName);
  napi_create_async_work(env, nullptr, resourceName, getAdcExecuteCB,
                         completeCBForPromise, (void *)addonData,
                         &addonData->asyncWork);

  // 将刚创建的async work加到队列，由底层去调度执行
  napi_queue_async_work(env, addonData->asyncWork);

  // 返回promise
  return promise;
}

// 同步方法实现获取adc值
static napi_value sync_get_adc_value(napi_env env, napi_callback_info info) {
  constexpr int adc_channel = 1;
  int value = 0;
  if (get_adc_data(adc_channel, &value) < 0) {
    value = -1;
  }

  napi_value ret;
  NAPI_CALL(env, napi_create_int32(env, value, &ret));
  return ret;
}
// 同步方法实现获取权限
static napi_value sync_get_permission(napi_env env, napi_callback_info info) {
  int result = 0;
  constexpr char path[128] = "/sys/bus/iio/devices/iio:device0/in_voltage2_raw";
  result = open_permission(path);

  napi_value ret;
  NAPI_CALL(env, napi_create_int32(env, result, &ret));
  return ret;
}
/*
 * 注册接口
 */
static napi_value registeradc_napiApis(napi_env env, napi_value exports) {
  napi_property_descriptor desc[] = {
      // 声明函数供ets调用
      DECLARE_NAPI_FUNCTION("sync_get_adc_value", sync_get_adc_value),
      DECLARE_NAPI_FUNCTION("sync_get_permission", sync_get_permission),
      DECLARE_NAPI_FUNCTION("UM_adc_test", UM_adc_test),
  };
  NAPI_CALL(env, napi_define_properties(env, exports,
                                        sizeof(desc) / sizeof(desc[0]), desc));
  return exports;
}
/*
 * 模块定义
 */
static napi_module adc_napiModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registeradc_napiApis,
    .nm_modname = "adc_napi", // 模块名
    .nm_priv = ((void *)0),
    .reserved = {0},
};

/*
 * 注册模块
 */
extern "C" __attribute__((constructor)) void Registeradc_napiModule(void) {
  napi_module_register(&adc_napiModule); // 接口注册函数
}