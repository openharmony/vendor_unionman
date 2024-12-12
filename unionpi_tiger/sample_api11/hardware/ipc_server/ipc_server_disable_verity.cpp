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
#include <cstdlib>
#include <ctime> // 包含时间和日期头文件
#include <filesystem>
#include <fstream>
#include <iostream>
#include <securec.h>
#include <sstream>
#include <string>

#include "bundle_mgr_client.h"
#include "cJSON.h"
#include "hilog/log.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iremote_broker.h"
#include "iremote_object.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"
#include "iservice_registry.h"
#include "nativetoken_kit.h"
#include "system_ability_definition.h"
#include "token_setproc.h"

#define ZLOGE(LOG_LABEL, fmt, args...) \
    (void)OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, "%{public}d: " fmt, __LINE__, ##args)
#define ZLOGW(LOG_LABEL, fmt, args...) \
    (void)OHOS::HiviewDFX::HiLog::Warn(LOG_LABEL, "%{public}d: " fmt, __LINE__, ##args)
#define ZLOGI(LOG_LABEL, fmt, args...) \
    (void)OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, "%{public}d: " fmt, __LINE__, ##args)
#define ZLOGD(LOG_LABEL, fmt, args...) \
    (void)OHOS::HiviewDFX::HiLog::Debug(LOG_LABEL, "%{public}d: " fmt, __LINE__, ##args)

static bool flag = true;
using namespace OHOS;
static HiviewDFX::HiLogLabel LABEL = { LOG_APP, 0x0003, "ShellServer" };
// 定义消息码
const int ABILITY_SHELL = 4;
const int ABILITY_YLOLO = 5;
const int ABILITY_LENET = 6;

class IShellAbility : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"shell.Ability");
    virtual std::string executeCommand(const std::string& dummy) = 0; // 定义业务函数
    virtual std::string yolo5s(const std::string& dummy) = 0;
    virtual std::string lenet(const std::string& dummy) = 0;
};

class ShellAbilityStub : public IRemoteStub<IShellAbility> {
public:
    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;
};

class ShellAbility : public ShellAbilityStub {
public:
    std::string executeCommand(const std::string& dummy) override;
    std::string yolo5s(const std::string& dummy) override;
    std::string lenet(const std::string& dummy) override;
};

int ShellAbilityStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    switch (code) {
        case ABILITY_SHELL: {
            std::string dummy = data.ReadString();
            std::string result = executeCommand(dummy);
            reply.WriteString(result);
            return 0;
        }
        case ABILITY_YLOLO: {
            std::string dummy = data.ReadString();
            std::string result = yolo5s(dummy);
            reply.WriteString(result);
            return 0;
        }
        case ABILITY_LENET: {
            std::string dummy = data.ReadString();
            std::string result = lenet(dummy);
            reply.WriteString(result);
            return 0;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

std::string ShellAbility::executeCommand(const std::string& dummy)
{
    std::string cmd = dummy + " 2>&1";
    char buffer[128];
    std::string result = "successful:";
    FILE* pipe = popen(cmd.c_str(), "r");

    if (!pipe) {
        ZLOGW(LABEL, "%{public}s: popen error", __func__);
        std::cerr << "命令执行失败" << std::endl;
        return "erro";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    return result;
}

std::string ShellAbility::yolo5s(const std::string& dummy)
{
    std::string cmd = "ld-linux-aarch64.so.1 /bin/sdk19_64 /etc/yolov5s.nb " + dummy + " 2>&1";
    char buffer[128];
    std::string result = "successful:";
    FILE* pipe = popen(cmd.c_str(), "r");

    if (!pipe) {
        ZLOGW(LABEL, "%{public}s: popen error", __func__);
        std::cerr << "命令执行失败" << std::endl;
        return "erro";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    return result;
}

std::string ShellAbility::lenet(const std::string& dummy)
{
    std::string cmd = "ld-linux-aarch64.so.1 /bin/lenet /etc/lenet.nb " + dummy + " 2>&1";
    char buffer[128];
    std::string result = "successful:";
    FILE* pipe = popen(cmd.c_str(), "r");

    if (!pipe) {
        ZLOGW(LABEL, "%{public}s: popen error", __func__);
        std::cerr << "命令执行失败" << std::endl;
        return "erro";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
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
            .processName = "com.ipc.Shell",
            .aplStr = "normal",
        };
        tokenId = GetAccessTokenId(&infoInstance);
        SetSelfTokenID(tokenId);
        flag = false;
    }
}

// 注册到本设备内
int main()
{
    AddPermission();
    ZLOGW(LABEL, "ipc_shell_server init");
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    sptr<IRemoteObject> newInstance = new ShellAbility();
    int result = samgr->AddSystemAbility(1602, newInstance);
    ZLOGW(LABEL, "Add SystemAbility ipc_shell_server result:%{public}d", result);
    IPCSkeleton::JoinWorkThread();
    return 0;
}
