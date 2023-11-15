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
#include "iremote_proxy.h"
#include "iremote_object.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "system_ability_definition.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "hilog/log.h"

#define ZLOGE(LOG_LABEL, fmt, args...)                                                                                 \
    (void)OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, "%{public}d: " fmt, __LINE__, ##args)
#define ZLOGW(LOG_LABEL, fmt, args...)                                                                                 \
    (void)OHOS::HiviewDFX::HiLog::Warn(LOG_LABEL, "%{public}d: " fmt, __LINE__, ##args)
#define ZLOGI(LOG_LABEL, fmt, args...)                                                                                 \
    (void)OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, "%{public}d: " fmt, __LINE__, ##args)
#define ZLOGD(LOG_LABEL, fmt, args...)                                                                                 \
    (void)OHOS::HiviewDFX::HiLog::Debug(LOG_LABEL, "%{public}d: " fmt, __LINE__, ##args)

using namespace OHOS;
// 定义消息码
const int ABILITY_SHELL = 4;
static HiviewDFX::HiLogLabel LABEL = {LOG_APP, 0x0002, "ShellServer"};
class IShellAbility : public IRemoteBroker {
public:
    // DECLARE_INTERFACE_DESCRIPTOR是必需的，入参需使用std::u16string；
    DECLARE_INTERFACE_DESCRIPTOR(u"shell.Ability");
    virtual std::string executeCommand(const std::string &dummy) = 0; // 定义业务函数
};

class ShellAbilityProxy : public IRemoteProxy<IShellAbility> {
public:
    explicit ShellAbilityProxy(const sptr<IRemoteObject> &impl);
    std::string executeCommand(const std::string &dummy) override;

private:
    static inline BrokerDelegator<ShellAbilityProxy> delegator_; // 方便后续使用iface_cast宏
};

ShellAbilityProxy::ShellAbilityProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IShellAbility>(impl)
{
}

std::string ShellAbilityProxy::executeCommand(const std::string &dummy)
{
    MessageOption option;
    MessageParcel dataParcel, replyParcel;
    dataParcel.WriteString(dummy);
    int error = Remote()->SendRequest(ABILITY_SHELL, dataParcel, replyParcel, option);
    std::string result = (error == ERR_NONE) ? replyParcel.ReadString() : "-1";

    return result;
}

static std::string EexecuteCommand(const std::string data)
{
    int ServerId = 1602;
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        ZLOGW(LABEL, "erro samgr");
        return "erro samgr";
    } else {
        ZLOGW(LABEL, "samgr  got");
    }
    sptr<IRemoteObject> remoteObject = samgr->GetSystemAbility(ServerId);
    if (remoteObject != nullptr) {
        ZLOGW(LABEL, "Got Shell Service object");
    } else {
        ZLOGW(LABEL, "Got Shell Service object error");
        return "Got Shell Service object error";
    }
    sptr<IShellAbility> ShellAbility = iface_cast<IShellAbility>(remoteObject); // 使用iface_cast宏转换成具体类型

    std::string result = ShellAbility->executeCommand(data.c_str());
    ZLOGW(LABEL, "client ipc shell serevr %{pubilc}s", result.c_str());
    return result;
}

#ifdef __cplusplus
extern "C" {
#endif

static napi_value EexecuteCommandnapi(napi_env env, napi_callback_info info)
{
    // 期望从 ArkTS 侧获取的参数的数量，这里只接受一个参数
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    // 从 info 中获取从 ArkTS 侧传递过来的参数
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    // 将获取的 ArkTS 参数转换为 native 信息，这里将其转换为字符串类型
    size_t strSize = 0;
    napi_status status = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strSize);
    char *strValue = new char[strSize + 1];
    napi_status status1;
    size_t strSize1 = 0;
    status1 = napi_get_value_string_utf8(env, args[0], strValue, strSize + 1, &strSize1);
    std::string path = std::string(strValue);
    std::string result = EexecuteCommand(path.c_str());
    napi_value ret;
    napi_create_string_utf8(env, result.c_str(), result.length(), &ret);
    return ret;
}
struct LedAddOnData {
    napi_async_work asyncWork = nullptr; // 异步工作项
    napi_deferred deferred = nullptr;    // 用于Promise的resolve、reject处理
    napi_ref callback = nullptr;         // 回调函数
    char *args[1] = {nullptr};
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
    std::string result = EexecuteCommand(addOnData->args[0]);
    addOnData->result = strdup(result.c_str()); // 使用 strdup 分配并复制字符串
}
static napi_value EexecuteCommandNapiWithPromise(napi_env env, napi_callback_info info)
{
    // 期望从 ArkTS 侧获取的参数的数量，这里只接受一个参数
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    // 从 info 中获取从 ArkTS 侧传递过来的参数
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    // 将获取的 ArkTS 参数转换为 native 信息，这里将其转换为字符串类型
    ZLOGW(LABEL, "EexecuteCommandNapiWithPromise got");
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
    napi_status status5 = napi_get_value_string_utf8(env, args[0], nullptr, 0, &strSize);
    char *strValue = new char[strSize + 1];
    napi_status status1;
    size_t strSize1 = 0;
    status1 = napi_get_value_string_utf8(env, args[0], strValue, strSize + 1, &strSize1);
    addonData->args[0] = (char *)malloc(strlen(strValue) + 1);
    addonData->args[0] = strdup(strValue); // 使用 strdup 分配并复制字符串
    // 创建async work，创建成功后通过最后一个参数(addonData->asyncWork)返回asyncwork的handle
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "ShellExecuteCB", NAPI_AUTO_LENGTH, &resourceName);
    ZLOGW(LABEL, "EexecuteCommandNapiWithPromise working");
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
        DECLARE_NAPI_FUNCTION("EexecuteCommandnapi", EexecuteCommandnapi),
        DECLARE_NAPI_FUNCTION("EexecuteCommandNapiWithPromise", EexecuteCommandNapiWithPromise),
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
    .nm_modname = "ipcnapi", // 模块名
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