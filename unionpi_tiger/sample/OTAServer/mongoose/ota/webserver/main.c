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


#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "mongoose.h"
#include "server_process.h"

#define API "/api"
#define UPLOAD_API "/api/upload"
#define TRANSFER_API "/api/transfer"
#define UPDATE_TRANS "/api/updater"
#define UPDATE_TRANS1 "/api/updater/"
#define GET_OTA_INFO "/api/getOtaInfo"
#define OTA_FEEDBACK "/api/otaFeedback"
#define GET_OTA_INFO_LAST "/api/getOtaInfoLast"
#define GET_OTA_INFO_LIST "/api/getOtaInfoList"
#define HAP_TRANS "/api/appdown"

#define STORAGE "../storage/"
#define UPDATE_FILE "../storage/updater.zip"
#define HAP_FILE "../storage/OTA.hap"
#define OTA_INFO_PATH "../otaInfo/"
#define JSON_PATH "./config/serverInfo.json"

#define UM_HTTP_SUCCESS "success"
#define UM_HTTP_ERROR "error"

#define VERSION_NAME "versionName"
#define VERSION_CODE "versionCode"
#define PACKGE_SIZE "size"
#define PACKGE_URL "url"
#define PACKGE_ID "packgeId"

#define LAST_NEXT "_last.txt"
#define LIST_NEXT "_list.txt"

#define UPGRDE_SUCCESS "101"
#define UPGRDE_FAIL "202"
#define UPGRDE_REPEAT "303"

#define OP_SUCCESS (0)
#define OP_FAILURE (-1)

#define RESPONSE_DATA_LEN (5)
#define NAME_LEN (100)
#define INFO_DATA_LINE (1024)

#define CGI_JSON_COPY_LEN ((1024 * 4) + 255)
#define CYG_HTTPD_MAXOUTBUFFER (1024 * 10)

#define PATH_BUFFER_LEN (1024)
#define DATAS_LINE_LEN (1024 * 100)
#define FEEDBACK_CODE_LEN (10)

#define HTTP_STATUS_OK (200)

// 这是一个线程函数，用于执行线程的任务
void* thread_function(void* arg)
{
    SSL_library_init();
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    if (Init() != 0) {
        printf("Failed to init socket!\n");
        return -1;
    }

    /* 打开服务 */
    if (Open() != 0) {
        printf("Failed to open server!\n");
        return -1;
    }

    /* 进入主循环 */
    while (1) {
        MainLoop();
        EVP_cleanup();
        ERR_free_strings();
    }
    /* 关闭服务并释放资源 */
    Close();
    return 0;
}

char* getLocalIPAddress(void)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2L, 2L), &wsaData) != 0) {
        fprintf(stderr, "Failed to initialize Winsock.\n");
        return NULL;
    }

    char hostName[256];
    if (gethostname(hostName, sizeof(hostName)) != 0) {
        fprintf(stderr, "Failed to get host name.\n");
        WSACleanup();
        return NULL;
    }

    struct hostent* hostInfo = gethostbyname(hostName);
    if (hostInfo == NULL) {
        fprintf(stderr, "Failed to get host info.\n");
        WSACleanup();
        return NULL;
    }

    struct in_addr** addrList = (struct in_addr**)hostInfo->h_addr_list;
    if (addrList[0] != NULL) {
        char* ipAddress = strdup(inet_ntoa(*addrList[0]));
        WSACleanup();
        return ipAddress;
    } else {
        fprintf(stderr, "No local IP address found.\n");
        WSACleanup();
        return NULL;
    }
}

cJSON* readJsonFromFile(const char* filename)
{
    // 打开JSON文件以读取内容
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening JSON file for reading.\n");
        return 1;
    }

    // 获取文件大小
    (void)fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    (void)fseek(file, 0, SEEK_SET);

    if (file_size < 0) {
        return 1;
    }

    // 读取文件内容
    char* json_string = (char*)malloc(file_size + 1);
    if (json_string == NULL) {
        fprintf(stderr, "Memory allocation error.\n");
        (void)fclose(file);
        return 1;
    }

    (void)fread(json_string, 1, file_size, file);
    json_string[file_size] = '\0';

    // 解析JSON字符串
    cJSON* json = cJSON_Parse(json_string);
    if (json == NULL) {
        fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
        free(json_string);
        (void)fclose(file);
        return 1;
    }
    (void)fclose(file);
    free(json_string);
    return json;
}

int setInJSONFile(const char* filename, const char* key, const char* newValue, const char* FKey)
{
    FILE* file;
    cJSON* json = readJsonFromFile(filename);

    // 获取checkResults数组
    cJSON* checkResults = cJSON_GetObjectItemCaseSensitive(json, FKey);
    if (cJSON_IsArray(checkResults)) {
        // 获取第一个对象
        cJSON* firstObject = cJSON_GetArrayItem(checkResults, 0);
        if (firstObject != NULL) {
            // 设置新的值
            cJSON* value = cJSON_GetObjectItemCaseSensitive(firstObject, key);
            if (cJSON_IsString(value)) {
                cJSON_DeleteItemFromObject(firstObject, key); // 删除旧值
                cJSON_AddStringToObject(firstObject, key, newValue); // 添加新值
            } else {
                cJSON_DeleteItemFromObject(firstObject, key); // 删除旧值
                cJSON_AddNumberToObject(firstObject, key, atoi(newValue)); // 添加新值
                fprintf(stderr, "value is not a string.\n");
            }
        } else {
            fprintf(stderr, "No objects in checkResults array.\n");
        }
    } else {
        fprintf(stderr, "checkResults is not an array.\n");
    }
    // 重新打开JSON文件以写入新内容
    file = fopen(filename, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening JSON file for writing.\n");
        cJSON_Delete(json);
        return 1;
    }

    // 将修改后的JSON写回文件
    char* updated_json_string = cJSON_Print(json);
    if (updated_json_string != NULL) {
        fputs(updated_json_string, file);
        free(updated_json_string);
    } else {
        fprintf(stderr, "Error serializing updated JSON.\n");
    }

    // 释放资源
    cJSON_Delete(json);
    (void)fclose(file);

    return 0;
}

/// @brief 生成ota信息文件名：拼接versionName 和 versionCode
/// @param versionName 版本名
/// @param versionCode 版本号
/// @param verifyInfo 版本名
/// @param size 版本号
/// @param packageType 版本名
/// @param descriptPackageId 版本号
/// @param descriptionType 版本名
/// @param content 版本号
static void generateOtaInfoJson(char* versionName, char* verifyInfo, char* size, char* content, char* AppOrSystem)
{
    printf("AppOrSystem: %s\n", AppOrSystem);
    if (strstr(AppOrSystem, "app") != NULL) {
        printf("app loader.\n");
        char* localIP = getLocalIPAddress();
        if (localIP != NULL) {
            printf("Local IP Address: %s\n", localIP);

            // 创建URL字符串
            char url[100]; // 适当的大小
            (void)sprintf_s(url, sizeof(url), "http://%s:8002/api/appdown", localIP);
            setInJSONFile(JSON_PATH, "content", url, "descriptInfo");
            printf("URL: %s\n", url);
            free(localIP);
        } else {
            fprintf(stderr, "Failed to retrieve local IP address.\n");
        }
        setInJSONFile(JSON_PATH, "versionName", versionName, "checkResults");
        setInJSONFile(JSON_PATH, "verifyInfo", verifyInfo, "checkResults");
        setInJSONFile(JSON_PATH, "size", size, "checkResults");
    } else if (strstr(AppOrSystem, "system") != NULL) {
        printf("system loader.\n");
        char* localIP = getLocalIPAddress();
        if (localIP != NULL) {
            printf("Local IP Address: %s\n", localIP);

            // 创建URL字符串
            char url[100]; // 适当的大小
            (void)sprintf_s(url, sizeof(url), "http://%s:8002/api/updater", localIP);
            setInJSONFile(JSON_PATH, "url", url, "checkResults");
            printf("URL: %s\n", url);
            free(localIP);
        } else {
            fprintf(stderr, "Failed to retrieve local IP address.\n");
        }
        setInJSONFile(JSON_PATH, "versionName", versionName, "checkResults");
        setInJSONFile(JSON_PATH, "verifyInfo", verifyInfo, "checkResults");
        setInJSONFile(JSON_PATH, "size", size, "checkResults");
        setInJSONFile(JSON_PATH, "content", content, "descriptInfo");
    }
    printf("no loader.\n");
}

/// @brief 接收从web传过来的文件和ota升级包信息
/// @param c
/// @param hm
static void getUploadFileAndInfo(struct mg_connection* c, struct mg_http_message* hm)
{
    struct mg_http_part part;
    size_t ofs = 0;
    // 1. 先完成文件接收与存储
    ofs = mg_http_next_multipart(hm->body, ofs, &part);
    // 5个信息字段，存储对应字段的值
    char versionName[INFO_DATA_LINE] = "";
    char verifyInfo[INFO_DATA_LINE] = "";
    char content[INFO_DATA_LINE] = "";
    char size[INFO_DATA_LINE] = "";
    char AppOrSystem[INFO_DATA_LINE] = "";
    // 文件存储
    char path[PATH_BUFFER_LEN] = STORAGE;
    strncpy_s(path + strlen(path), sizeof(path) - strlen(path), part.filename.ptr, part.filename.len);
    printf("path:%s\n", path);
    FILE* fp = fopen(path, "wb+");
    if (fp == NULL) {
        return;
    }
    (void)fwrite(part.body.ptr, part.body.len, 1, fp);
    (void)fclose(fp);
    // 2. 记录信息
    // 接下来继续读取剩下的字段
    ofs = mg_http_next_multipart(hm->body, ofs, &part);
    // versionName
    strncpy_s(versionName, sizeof(versionName), part.body.ptr, part.body.len);
    // otadescirpeId
    ofs = mg_http_next_multipart(hm->body, ofs, &part);
    strncpy_s(verifyInfo, sizeof(verifyInfo), part.body.ptr, part.body.len);
    ofs = mg_http_next_multipart(hm->body, ofs, &part);
    // size
    strncpy_s(size, sizeof(size), part.body.ptr, part.body.len);
    // content
    ofs = mg_http_next_multipart(hm->body, ofs, &part);
    strncpy_s(content, sizeof(content), part.body.ptr, part.body.len);
    // AppOrSystem
    ofs = mg_http_next_multipart(hm->body, ofs, &part);
    strncpy_s(AppOrSystem, sizeof(AppOrSystem), part.body.ptr, part.body.len);

    generateOtaInfoJson(versionName, verifyInfo, size, content, AppOrSystem);
    mg_http_reply(c, HTTP_STATUS_OK, "", "upload file ok and the information of ota-package is ok!");
}
/// @brief 发送文件
/// @param c
/// @param hm
static void transferFile(struct mg_connection* c, struct mg_http_message* hm)
{
    char path[PATH_BUFFER_LEN] = STORAGE;
    char fileName[PATH_BUFFER_LEN] = "";
    strncpy_s(fileName, sizeof(fileName), hm->query.ptr, hm->query.len); /* 获取文件名参数 */
    printf("fileName:%s\n", fileName);
    strcat_s(path, sizeof(path), fileName);
    printf("path: %s\n", path);

    char tFileName[PATH_BUFFER_LEN] = "fileName:";
    strcat_s(tFileName, sizeof(tFileName), fileName);
    strcat_s(tFileName, sizeof(tFileName), "\r\n"); /* header中字段值末尾需要以\r\n结尾 */

    printf("\ntFileName:%s\n", tFileName);

    // 传输文件的名
    struct mg_http_serve_opts opts = {
        .extra_headers = tFileName,
    };
    mg_http_serve_file(c, hm, path, &opts); /* 传输文件  */
}
/// @brief transfer special ota updater.zip to ssl-server
/// @param c
/// @param hm
static void transferUpdater(struct mg_connection* c, struct mg_http_message* hm)
{
    char path[PATH_BUFFER_LEN] = UPDATE_FILE;
    printf("path: %s\n", path);

    struct mg_http_serve_opts opts = {};
    mg_http_serve_file(c, hm, path, &opts); /* support a static file  */
}
static void transferApp(struct mg_connection* c, struct mg_http_message* hm)
{
    char path[PATH_BUFFER_LEN] = HAP_FILE;
    printf("path: %s\n", path);
    struct mg_http_serve_opts opts = {};
    mg_http_serve_file(c, hm, path, &opts); /* support a static file  */
}
/// @brief 接收来自客户端app的反馈信息，根据规定的反馈码来判断升级结果，并在服务端输出升级结果信息
/// @param c
/// @param hm
static void otaFeedback(struct mg_connection* c, struct mg_http_message* hm)
{
    printf("ota feedback function:\n");
    char feedbackCode[FEEDBACK_CODE_LEN] = "";
    strncpy_s(feedbackCode, sizeof(feedbackCode), hm->query.ptr, hm->query.len); /* 获取参数--结果状态码 */

    // 比较判断，进行选择分支判断
    if (!strcmp(feedbackCode, UPGRDE_SUCCESS)) {
        printf("upgrade success!\n");
    } else if (!strcmp(feedbackCode, UPGRDE_FAIL)) {
        printf("upgrade fail!\n");
    } else if (!strcmp(feedbackCode, UPGRDE_REPEAT)) {
        printf("upgrade repeat!\n");
    }
    mg_http_reply(c, HTTP_STATUS_OK, "", "");
}

/// @brief Parse uri, choose call function by parse result
/// @param c
/// @param hm
/// @param ev
/// @return
static int webserverParseCgi(struct mg_connection* c, struct mg_http_message* hm)
{
    int res = 0;
    printf("It 's Parse and Cgi!\n");

    if (mg_http_match_uri(hm, UPLOAD_API)) {
        printf("match upload file\n");
        getUploadFileAndInfo(c, hm);
    } else if (mg_http_match_uri(hm, TRANSFER_API)) {
        printf("match transfer file\n");
        transferFile(c, hm);
    } else if ((mg_http_match_uri(hm, UPDATE_TRANS) || mg_http_match_uri(hm, UPDATE_TRANS1))) {
        printf("match get updater.zip!\n");
        transferUpdater(c, hm);
    } else if (mg_http_match_uri(hm, HAP_TRANS)) {
        printf("match get hap\n");
        transferApp(c, hm);
    } else if (mg_http_match_uri(hm, OTA_FEEDBACK)) {
        printf("match ota feedback!\n");
        otaFeedback(c, hm);
    } else {
        printf("not match all!\n");
        struct mg_http_serve_opts opts = {
            .root_dir = "../webRoot",
        }; // Serve files
        mg_http_serve_dir(c, hm, &opts); // From root_dir
    }

    return res;
}

/// @brief 事件处理程序函数，由用户定义，该函数是定义了连接的行为。
/// @param c 接收事件的连接
/// @param ev 事件编号
/// @param ev_data
/// @param fn_data
static void fn(struct mg_connection* c, int ev, void* ev_data, void* fn_data)
{
    if (ev == MG_EV_ACCEPT && fn_data != NULL) {
        struct mg_tls_opts opts = {
            .cert = "../ssl/server.crt",
            .certkey = "../ssl/server.key",
            .ca = "../ssl/ca.pem",
        };
        mg_tls_init(c, &opts);
    } else if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message* hm = (struct mg_http_message*)ev_data;
        printf("It's http or https request!\n");
        webserverParseCgi(c, hm);
    }

    (void)fn_data;
}

int main(int argc, char** argv)
{
    pthread_t thread_id; // 线程标识符
    int thread_arg = 2; // 传递给线程函数的参数
    // 创建一个新线程，传递参数并指定线程函数
    int result = pthread_create(&thread_id, NULL, thread_function, &thread_arg);
    struct mg_mgr mgr;
    mg_mgr_init(&mgr); // Init manager
    mg_log_set(MG_LL_DEBUG); // set debug level
    mg_http_listen(&mgr, "http://0.0.0.0:8002", fn, NULL); // Setup listener
    mg_http_listen(&mgr, "https://0.0.0.0:8005", fn, (void*)1); /* Set a https listener */
    for (;;)
        mg_mgr_poll(&mgr, 100L); // Infinite event loop
    mg_mgr_free(&mgr);

    return 0;
}
