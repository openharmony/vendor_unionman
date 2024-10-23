//
// Created on 2024/8/6.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include <arpa/inet.h>
#include <bits/alltypes.h>
#include <errno.h>
#include <ifaddrs.h>
#include <locale.h>
#include <net/if.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "udp_server.h"
#include <hilog/log.h>

#define LINE_SIZE (size_t)180

/*! Static buffer to contain interface name. (extern'ed in upnp.h) */
char g_IF_NAME[LINE_SIZE] = {'\0'};

/*! Static buffer to contain interface IPv4 address. (extern'ed in upnp.h) */
char g_IF_IPV4[INET_ADDRSTRLEN] = {'\0'};

typedef struct {
    char *ip;
    int port;
    DataCallback callback;
} ThreadArgs;

pthread_t threadID;
ThreadArgs threadArgs;

void *receiveThread(void *arg) {
    pthread_detach(pthread_self());
    ThreadArgs *args = (ThreadArgs *)arg;
    char *ip = args->ip;
    int port = args->port;
    DataCallback callback = args->callback;
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    char readBuffer[1024];
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "socket create failed");
        pthread_exit(NULL);
    }
    
    memset(&serverAddr, 0, sizeof(struct sockaddr_in));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &(serverAddr.sin_addr)) <= 0) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "inet_pton failed");
        pthread_exit(NULL);
    }
    
    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in)) < 0) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "socket bind failed");
        close(sockfd);
        pthread_exit(NULL);
    }
    
    if (listen(sockfd, 5) < 0) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "socket listen failed. err msg:%{public}s", strerror(errno));
        close(sockfd);
        pthread_exit(NULL);
    }

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "UPD server listening on %{public}s:%{public}d", ip, port);
    fd_set readSet;
    struct timeval timeout;
    while(1) {
        FD_ZERO(&readSet);
        FD_SET(sockfd, &readSet);
        
        timeout.tv_sec = 0;
        timeout.tv_usec = 500 * 1000;
        
        int result = select(sockfd + 1, &readSet, NULL, NULL, &timeout);
        if (result < 0) {
            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "select error");
            break;
        } else if (result == 0) {
            continue;
        } else {
            if (FD_ISSET(sockfd, &readSet)) {
                memset(readBuffer, 0, sizeof(readBuffer));
                socklen_t clientAddrLen = sizeof(struct sockaddr_in);
                ssize_t numBytes = recvfrom(sockfd, readBuffer, sizeof(readBuffer) - 1, 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
                if (numBytes < 0) {
                    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "recvfrom failed");
                    continue;
                }
                if (numBytes == 0) {
                    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "Connection close by client");
                }
                if (numBytes > 0) {
                    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "readBuffer:%{public}s", readBuffer);
                    args->callback(0, readBuffer);
                }
            }
        }
    }
}

int createUdpServer(const char* serverIp, int serverPort, DataCallback cbk) {
    threadArgs.ip = (char *)serverIp;
    threadArgs.port = serverPort;
    threadArgs.callback = cbk;
    
    if (pthread_create(&threadID, NULL, receiveThread, (void *)&threadArgs) != 0) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "createUdpServer create thread failed");
        return -1;
    }
    
    return 0;
}

int GetIfInfo() {
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "socket fd:%{public}d", fd);
    if(fd < 0) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "socket failed!%{public}s", strerror(errno));
        return -1;
    } else {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "create socket SUCC");
    }
    return 0;
}