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
#include <cstring>

// 接口业务实现C/C++代码
// std::string 需要引入string头文件，#include <string>
// 该napi_module对外具体的提供的API接口是 getHelloString
static napi_value getHelloString(napi_env env, napi_callback_info info) {
  napi_value result;
  std::string words = "Hello OpenHarmony NAPI";
  NAPI_CALL(env, napi_create_string_utf8(env, words.c_str(), words.length(),
                                         &result));
  return result;
}

// 注册对外接口的处理函数napi_addon_register_func
// 2.指定NAPI模块注册对外接口的处理函数，具体扩展的接口在该函数中声明
// 模块对外接口注册函数为registerFunc
static napi_value registerFunc(napi_env env, napi_value exports) {
  static napi_property_descriptor desc[] = {

      // 声明该napi_module对外具体的提供的API为getHelloString
      DECLARE_NAPI_FUNCTION("getHelloString", getHelloString),

  };
  NAPI_CALL(env, napi_define_properties(env, exports,
                                        sizeof(desc) / sizeof(desc[0]), desc));
  return exports;
}

// 注册NAPI模块
// 1.先定义NAPI模块，指定当前NAPI模块对应的模块名
// 以及模块注册对外接口的处理函数，具体扩展的接口在该函数中声明
// nm_modname: NAPI模块名称，对应eTS代码为import nm_modname from
// '@ohos.ohos_shared_library_name' 示例对应hap应用中eTS代码需要包含import
// hellonapi from '@ohos.hellonapi' 以下的出现的hellonapi都为注册的NAPI模块名
static napi_module hellonapiModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,

    // registerFunc是NAPI模块对外接口注册函数
    .nm_register_func = registerFunc,

    .nm_modname = "hellonapi",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

// 3.NAPI模块定义好后，调用NAPI提供的模块注册函数napi_module_register(napi_module*
// mod)函数注册到系统中。 register
// module，设备启动时自动调用此constructor函数，把定义的模块注册到OpenHarmony中。
// 以下出现的hellonapi都是注册的NAPI模块名
extern "C" __attribute__((constructor)) void hellonapiModuleRegister() {
  // napi_module_register是ohos的NAPI组件提供的模块注册函数
  napi_module_register(&hellonapiModule);
}