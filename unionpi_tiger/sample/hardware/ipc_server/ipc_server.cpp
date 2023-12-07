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
#include <iostream>
#include <fstream>
#include <ctime> // 包含时间和日期头文件
#include <sstream>
#include <string>
#include <filesystem>
#include <cstdio>
#include <cstdlib>
#include <securec.h>
#include "iremote_broker.h"
#include "iremote_stub.h"
#include "iremote_proxy.h"
#include "iremote_object.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include "ipc_skeleton.h"
#include "bundle_mgr_client.h"
#include "cJSON.h"
#include "hilog/log.h"

#define ZLOGE(LOG_LABEL, fmt, args...)                                                                                 \
    (void)OHOS::HiviewDFX::HiLog::Error(LOG_LABEL, "%{public}d: " fmt, __LINE__, ##args)
#define ZLOGW(LOG_LABEL, fmt, args...)                                                                                 \
    (void)OHOS::HiviewDFX::HiLog::Warn(LOG_LABEL, "%{public}d: " fmt, __LINE__, ##args)
#define ZLOGI(LOG_LABEL, fmt, args...)                                                                                 \
    (void)OHOS::HiviewDFX::HiLog::Info(LOG_LABEL, "%{public}d: " fmt, __LINE__, ##args)
#define ZLOGD(LOG_LABEL, fmt, args...)                                                                                 \
    (void)OHOS::HiviewDFX::HiLog::Debug(LOG_LABEL, "%{public}d: " fmt, __LINE__, ##args)

static bool flag = true;
using namespace OHOS;
// 定义消息码
static HiviewDFX::HiLogLabel LABEL = {LOG_APP, 0x0003, "ShellServer"};
const int ABILITY_SHELL = 4;

class IShellAbility : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"shell.Ability");
    virtual std::string executeCommand(const std::string &dummy) = 0; // 定义业务函数
};

class ShellAbilityStub : public IRemoteStub<IShellAbility> {
public:
    virtual int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                MessageOption &option) override;
};

class ShellAbility : public ShellAbilityStub {
public:
    std::string executeCommand(const std::string &dummy) override;
};

// 读取文件内容到缓冲区
static char* readFile(const char *path, size_t *fileSize)
{
    FILE *file = fopen(path, "r");
    if (!file) {
        return nullptr;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return -1;
    }
    *fileSize = ftell(file);
    if (fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return -1;
    }

    char *fileContent = (char *)malloc(*fileSize + 1);
    if (!fileContent) {
        fclose(file);
        return nullptr;
    }

    if (fread(fileContent, 1, *fileSize, file) != *fileSize) {
        fclose(file);
        free(fileContent);
        return nullptr;
    }

    fileContent[*fileSize] = '\0';
    if (fclose(file) != 0) {
        return -1;
    }
    return fileContent;
}

// 处理 JSON 数据
static int processJSONData(const char *jsonContent, const char *uName)
{
    cJSON *json = cJSON_Parse(jsonContent);
    if (!json) {
        return -1;
    }

    cJSON *privilegeBundleList = cJSON_GetObjectItem(json, "Privilege Bundlname List");
    if (privilegeBundleList && cJSON_IsArray(privilegeBundleList)) {
    for (int i = 0; i < cJSON_GetArraySize(privilegeBundleList); i++) {
        cJSON *element = cJSON_GetArrayItem(privilegeBundleList, i);
        if (element && cJSON_IsString(element) && strcmp(element->valuestring, uName) == 0) {
            cJSON_Delete(json);
            return 0;
        }
    }
}
    cJSON_Delete(json);
    return -1;
}

// 组合两个函数
static int processJSONFile(const char *path, const char *uName)
{
    size_t fileSize;
    char *fileContent = readFile(path, &fileSize);
    if (!fileContent) {
        return -1;
    }

    int result = processJSONData(fileContent, uName);

    free(fileContent);
    return result;
}

static int bmgrGetUersNameForUidAndVerify()
{
    std::vector<std::string> fileLines;
    std::string uersName = " ";
    int uid = 1;
    uid = IPCSkeleton::GetCallingUid();
    std::unique_ptr<AppExecFwk::BundleMgrClient> ShellClient;
    ShellClient = std::make_unique<AppExecFwk::BundleMgrClient>();
    ShellClient->GetBundleNameForUid(uid, uersName);
    ZLOGW(LABEL, "%{public}s: get calling BundleName:%{public}s", __func__, uersName.c_str());
    int result = processJSONFile("/etc/Applist.json", uersName.c_str());
    if (result == 0) {
        return 0;
    }
    return -1;
}

int ShellAbilityStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    switch (code) {
        case ABILITY_SHELL: {
            int ver = bmgrGetUersNameForUidAndVerify();
            if (ver == 0) {
                std::string dummy = data.ReadString();
                std::string result = executeCommand(dummy);
                reply.WriteString(result);
                return 0;
            } else {
                return -1;
            }
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

std::string ShellAbility::executeCommand(const std::string &dummy)
{
    ZLOGW(LABEL, "ipc_shell_server working");
    std::string cmd = dummy + " 2>&1";
    char buffer[128];
    std::string result = "successful:";
    FILE *pipe = popen(cmd.c_str(), "r");

    if (!pipe) {
        ZLOGW(LABEL, "%{public}s: popen error", __func__);
        std::cerr << "命令执行失败" << std::endl;
        return "erro";
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    ZLOGW(LABEL, "%{public}s: popen successful", __func__);
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
