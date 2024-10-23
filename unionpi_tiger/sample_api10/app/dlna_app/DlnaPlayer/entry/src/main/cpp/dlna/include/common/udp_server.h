//
// Created on 2024/8/6.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef DlnaPlayer_inet_pton_H
#define DlnaPlayer_inet_pton_H

#ifdef __cplusplus
extern "C" {
#endif

#define DLNA_LOG_TAG "[OH_DLNA]"

typedef void (*DataCallback)(int msgType, const void *);

int createUdpServer(const char *serverIp, int serverPort, DataCallback cbk);

int GetIfInfo();

#ifdef __cplusplus
}
#endif

#endif //DlnaPlayer_inet_pton_H
