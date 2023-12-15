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
#include <cstdio>
#include <cstring>
#include <opencv2/opencv.hpp>
#include "napi/native_api.h"
#include "napi/native_node_api.h"

static std::string resizeAndOverlay(const std::string& imagePath, const std::string& name, int newWidth, int newHeight)
{
       // 读取原始图像
    cv::Mat originalImage = cv::imread(imagePath+"/"+name);
    // 检查图像是否成功加载
    if (originalImage.empty()) {
        std::cerr << "Failed to load the image: " << imagePath << std::endl;
        return (imagePath+"/"+name+"errogdrg");
    }
    // 调整图像大小
    cv::Mat resizedImage;
    cv::resize(originalImage, resizedImage, cv::Size(newWidth, newHeight));
    // 保存结果图像，使用不同的文件名
    cv::imwrite(imagePath+"/aaa.jpg", resizedImage);

    return "succ";
}


#ifdef __cplusplus
extern "C" {
#endif
struct LedAddOnData {
    napi_async_work asyncWork = nullptr; // 异步工作项
    napi_deferred deferred = nullptr;    // 用于Promise的resolve、reject处理
    napi_ref callback = nullptr;         // 回调函数
    char *args[2] = {nullptr};
    int  iargs[2] = {};
    char *arr;    // 2个输入参数
    char *result; // 业务逻辑处理结果（返回值）
};
// 业务逻辑处理函数。
static void completeCBForPromise(napi_env env, napi_status status, void *data)
{
    LedAddOnData *addOnData = (LedAddOnData *)data;
    napi_value result = nullptr;
    napi_create_string_utf8(env, addOnData->result, strlen(addOnData->result), &result);
    napi_resolve_deferred(env, addOnData->deferred, result);
    free(addOnData->args[0]);
    free(addOnData->result);
    // 删除napi_ref对象
    if (addOnData->callback != nullptr) {
        napi_delete_reference(env, addOnData->callback);
    }
    // 删除异步工作项
    napi_delete_async_work(env, addOnData->asyncWork);
    delete addOnData;
}
// 业务逻辑处理函数，由worker线程池调度执行。
static void ShellExecuteCB(napi_env env, void *data)
{
    LedAddOnData *addOnData = (LedAddOnData *)data;
    int width = addOnData->iargs[0];
    int height = addOnData->iargs[1];
    std::string result = resizeAndOverlay(addOnData->args[0], addOnData->args[1], Width, Height);
    addOnData->result = strdup(result.c_str()); // 使用 strdup 分配并复制字符串
}
static napi_value opencvWithPromise(napi_env env, napi_callback_info info)
{
    // 期望从 ArkTS 侧获取的参数的数量，这里只接受一个参数
    size_t argc = 4;
    napi_value args[4] = {nullptr};
    // 从 info 中获取从 ArkTS 侧传递过来的参数
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
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
    size_t strSize = 0;
    napi_get_value_string_utf8(env, args[0], nullptr, 0, &strSize);
    char *strValue = new char[strSize + 1];
    napi_status status1;
    size_t strSize1 = 0;
    status1 = napi_get_value_string_utf8(env, args[0], strValue, strSize + 1, &strSize1);
    addonData->args[0] = (char *)malloc(strlen(strValue) + 1);
    addonData->args[0] = strdup(strValue); // 使用 strdup 分配并复制字符串

    size_t strSize2 = 0;
    napi_get_value_string_utf8(env, args[1], nullptr, 0, &strSize2);
    char *strValue1 = new char[strSize2 + 1];
    napi_status status2;
    size_t strSize3 = 0;
    status2 = napi_get_value_string_utf8(env, args[1], strValue1, strSize + 1, &strSize3);
    addonData->args[1] = (char *)malloc(strlen(strValue1) + 1);
    addonData->args[1] = strdup(strValue1); // 使用 strdup 分配并复制字符串

    napi_get_value_int32(env, args[2L], &addonData->iargs[0]);
    napi_get_value_int32(env, args[3L], &addonData->iargs[1]);
    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "ShellExecuteCB", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName, ShellExecuteCB, completeCBForPromise, (void *)addonData,
                           &addonData->asyncWork);
    // 将刚创建的async work加到队列，由底层去调度执行
    napi_queue_async_work(env, addonData->asyncWork);
    // 返回promise
    return promise;
}
/* 以上为实现异步计算接下来10s内温度，可设置为传参类型 */

/*
 * 注册接口
 */
static napi_value registerdeviceinfoApis(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("opencvWithPromise", opencvWithPromise),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}
/*
 * 模块定义
 */
static napi_module writeShellnapiModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registerdeviceinfoApis,
    .nm_modname = "opencvnapi", // 模块名
    .nm_priv = ((void *)0),
    .reserved = {0},
};
#ifdef __cplusplus
}
#endif
/*
 * 注册模块
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&writeShellnapiModule); // 接口注册函数
}
