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

#include "server_process.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "openssl/err.h"
#include "openssl/ssl.h"

#define SERVER_PEM "config/serverKey.pem"
#define SERVER_CER "config/serverCert.cer"
#define SRVER_PROFILE "config/serverInfo.json"
#define PORT_NUMBER 5022

static int s_sockfd = -1;
static SSL_CTX* s_ctx = NULL;
static SSL* ssl = NULL;


#define LOG_PRINT(fmt, ...) printf("[ServerProcess][%s:%d] " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#define DO_CHECK(cond, log, ...)                                                                                       \
    if (!(cond)) {                                                                                                     \
        LOG_PRINT(log);                                                                                                \
        __VA_ARGS__;                                                                                                   \
    }


char* respondContent = NULL;

int readServerProfileFromLocal(void)
{
    // write file
    const char* file_name = SRVER_PROFILE;
    FILE* serverProfileFile = fopen(file_name, "rb");

    if (serverProfileFile == NULL) {
        printf("serverInfo.json is not exist.");
        return -1;
    }

    (void)fseek(serverProfileFile, 0, SEEK_END);
    long fileSize = ftell(serverProfileFile);
    (void)fseek(serverProfileFile, 0, SEEK_SET);

    if (fileSize < 0) {
        return -1;
    }
    respondContent = (char*)malloc(fileSize + 1);
    size_t len = fread(respondContent, 1, fileSize, serverProfileFile);
    if (len < 0) {
        return -1;
    }
    respondContent[len] = 0;

    (void)fclose(serverProfileFile);

    return 0;
}

/* 创建socket环境 */
int Init(void)
{
    struct sockaddr_in serverAddr = { 0 };
    int sockfd;
    {
        WSADATA data;
        WSAStartup(MAKEWORD(2L, 2L), &data);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    DO_CHECK(sockfd != -1, "Failed to create socket");

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT_NUMBER); /* 端口号 */

    DO_CHECK(bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == 0, "Failed to bind socket");
    DO_CHECK(listen(sockfd, SOMAXCONN) == 0, "Failed to listen on socket");
    LOG_PRINT("Server is listening on port 5022 ");
    s_sockfd = sockfd;
    return 0;
}


/* 服务打开的接口 */
int Open(void)
{
    const SSL_METHOD* method;
    SSL_CTX* ctx;

    SSL_library_init();
    SSL_load_error_strings();
    method = TLS_server_method(); /* 使用TLS v1.2作为方法 */
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        return -1;
    }
    if (SSL_CTX_use_certificate_file(ctx, SERVER_CER, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return -1;
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, SERVER_PEM, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    s_ctx = ctx;
    return 0;
}


/* 每隔100ms调用一次 */
int MainLoop(void)
{
    /* 接受连接 */
    int accfd = accept(s_sockfd, NULL, NULL);

    DO_CHECK(readServerProfileFromLocal() == 0, "Failed to read ServerProfile");

    /* 创建 SSL 上下文 */
    SSL_CTX* ctx = SSL_CTX_new(SSLv23_server_method());

    /* 载入数字证书和私钥 */
    SSL_CTX_use_certificate_file(ctx, SERVER_CER, SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, SERVER_PEM, SSL_FILETYPE_PEM);

    /* 创建 SSL 对象 */
    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, accfd);

    /* 建立 SSL 连接 */
    if (SSL_accept(ssl) == -1) {
        perror("SSL_accept");
        close(accfd);
        return 0;
    }

    /* 发送数据 */
    SSL_write(ssl, respondContent, strlen(respondContent));

    /* 接收数据 */
    char buf[1024];
    int len = SSL_read(ssl, buf, sizeof(buf));
    buf[len] = '\0';
    printf("Received: %s\n", buf);

    /* 关闭连接套接字 */
    usleep(100000L);
    close(accfd);
    return 0;
}

/* 关闭服务并释放资源 */
int Close(void)
{
    /* 关闭 SSL 连接 */
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(s_ctx);
    WSACleanup();
    close(s_sockfd);
    s_sockfd = NULL;
    return 0;
}
