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
#include <securec.h>
#include "iremote_proxy.h"
#include "iremote_object.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "ipc_skeleton.h"
#include "system_ability_definition.h"
#include "token_setproc.h"
#include "nativetoken_kit.h"
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
static HiviewDFX::HiLogLabel LABEL = {LOG_APP, 0x0002, "ShellServer"};
// 定义消息码
static bool flag = true;
const int ABILITY_SHELL = 4;

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

static void AddPermission()
{
    if (flag) {
        uint64_t tokenId;
        NativeTokenInfoParams infoInstance = {
            .dcapsNum = 0,
            .permsNum = 0,
            .aclsNum = 0,
            .dcaps = NULL,
            .perms = NULL,
            .acls = NULL,
            .processName = "com.example.shelltest",
            .aplStr = "normal",
        };
        tokenId = GetAccessTokenId(&infoInstance);
        SetSelfTokenID(tokenId);
        flag = false;
    }
}

int main()
{
    int ServerId = 1602;
    AddPermission();
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        ZLOGW(LABEL, "erro samgr");
    } else {
        ZLOGW(LABEL, "samgr  got");
    }
    sptr<IRemoteObject> remoteObject = samgr->GetSystemAbility(ServerId);
    if (remoteObject != nullptr) {
        ZLOGW(LABEL, "Got Shell Service object");
    } else {
        ZLOGW(LABEL, "Got Shell Service object error");
    }
    sptr<IShellAbility> ShellAbility = iface_cast<IShellAbility>(remoteObject); // 使用iface_cast宏转换成具体类型
    std::string result = ShellAbility->executeCommand("mkdir /data/CommandShell");
    ZLOGW(LABEL, "client ipc shell serevr %{public}s", result.c_str());
    printf("result:%s \n", result.c_str());
    IPCSkeleton::JoinWorkThread();
    return 0;
}