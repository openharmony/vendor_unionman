/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: dlna types
 * Author: 
 * Create: 2023-10-20
 */

#ifndef DLNA_TYPES_H
#define DLNA_TYPES_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <linux/errno.h>
#include <errno.h>

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

#ifdef __cplusplus
extern "C" {
#endif

#define EXPORT __attribute__((visibility("default")))

#define EOK 0
#define EINVAL 22
#define UINT_RANGE 12

#define DLNA_NULL 0
#define DLNA_TRUE 1
#define DLNA_FALSE 0
#define DLNA_NULL_PTR (void *)0

inline constexpr int DLNA_ENUM_END = 0xFFFFFFF;
inline constexpr int DLNA_NULL_INT32 = 0x7FFFFFFF;
inline constexpr int DLNA_COMM_ERR_CODE_START = 1000;
inline constexpr int DLNA_COMM_ERR_CODE_END = 1999;
inline constexpr int DLNA_DMS_ERR_CODE_START = 2000;
inline constexpr int DLNA_DMS_ERR_CODE_END = 2999;
inline constexpr int DLNA_DMP_ERR_CODE_START = 3000;
inline constexpr int DLNA_DMP_ERR_CODE_END = 3999;
inline constexpr int DLNA_DMR_ERR_CODE_START = 4000;
inline constexpr int DLNA_DMR_ERR_CODE_END = 4999;

inline constexpr int DLNA_RET_SUCCESS = 0;
inline constexpr int DLNA_RET_FAILURE = 1;
inline constexpr int DLNA_RET_COMM_INIT_FAIL = DLNA_COMM_ERR_CODE_START;
inline constexpr int DLNA_RET_COMM_INVALID_OPTION = (DLNA_COMM_ERR_CODE_START + 1);
inline constexpr int DLNA_RET_COMM_CREATE_FAILED = (DLNA_COMM_ERR_CODE_START + 2);

inline constexpr char *DLNA_STACK_VERSION = "OHOSDLNA V1.1.1.0";
inline constexpr int DLNA_BUFF_SIZE = 64;
inline constexpr int UDN_LEN = 250;
/* UPnP error code */
inline constexpr int EXPORT DLNA_E_UPNP_401 = 401;
inline constexpr int EXPORT DLNA_E_UPNP_402 = 402;
inline constexpr int EXPORT DLNA_E_UPNP_403 = 403;
inline constexpr int EXPORT DLNA_E_UPNP_500 = 500;
inline constexpr int EXPORT DLNA_E_UPNP_501 = 501;
inline constexpr int EXPORT DLNA_E_UPNP_600 = 600;
inline constexpr int EXPORT DLNA_E_UPNP_602 = 602;
inline constexpr int EXPORT DLNA_E_UPNP_601 = 601;
inline constexpr int EXPORT DLNA_E_UPNP_603 = 603;
inline constexpr int EXPORT DLNA_E_UPNP_604 = 604;
inline constexpr int EXPORT DLNA_E_UPNP_605 = 605;
inline constexpr int EXPORT DLNA_E_UPNP_606 = 606;
inline constexpr int EXPORT DLNA_E_UPNP_607 = 607;
inline constexpr int EXPORT DLNA_E_UPNP_608 = 608;
inline constexpr int EXPORT DLNA_E_UPNP_609 = 609;
inline constexpr int EXPORT DLNA_E_UPNP_610 = 610;
inline constexpr int EXPORT DLNA_E_UPNP_611 = 611;
inline constexpr int EXPORT DLNA_E_UPNP_612 = 612;
inline constexpr int EXPORT DLNA_E_UPNP_701 = 701;
inline constexpr int EXPORT DLNA_E_UPNP_710 = 710;
inline constexpr int EXPORT DLNA_E_UPNP_711 = 711;
inline constexpr int EXPORT DLNA_E_UPNP_712 = 712;
inline constexpr int EXPORT DLNA_E_UPNP_715 = 715;
inline constexpr int EXPORT DLNA_E_UPNP_718 = 718;

inline constexpr int EXPORT HTTPREQUESTLEN = (1024 * 3);
inline constexpr int EXPORT HTTPIPADDRLEN = 64;
inline constexpr int EXPORT HTTPPORTLEN = 8;
inline constexpr int EXPORT URILEN = 1024;
inline constexpr int EXPORT HTTPMSGBUFFERLEN = 256;
inline constexpr int EXPORT HTTPMSGTAILLEN = 1;
inline constexpr int EXPORT DLNA_SUBS_ID_LEN = 44;

inline constexpr char* EXPORT DLNA_E_UPNP_401_STRING = "Invalid Action";
inline constexpr char* EXPORT DLNA_E_UPNP_402_STRING = "Invalid Args";
inline constexpr char* EXPORT DLNA_E_UPNP_403_STRING = "Do Not Use";
inline constexpr char* EXPORT DLNA_E_UPNP_500_STRING = "Internal Server Error";
inline constexpr char* EXPORT DLNA_E_UPNP_501_STRING = "Action Failed";
inline constexpr char* EXPORT DLNA_E_UPNP_600_STRING = "Argument Value Invalid";
inline constexpr char* EXPORT DLNA_E_UPNP_601_STRING = "Argument Value Out of Range";
inline constexpr char* EXPORT DLNA_E_UPNP_602_STRING = "Optional Action Not Implemented";
inline constexpr char* EXPORT DLNA_E_UPNP_603_STRING = "Out of Memory";
inline constexpr char* EXPORT DLNA_E_UPNP_604_STRING = "Human Intervention Required";
inline constexpr char* EXPORT DLNA_E_UPNP_605_STRING = "String Argument Too Long";
inline constexpr char* EXPORT DLNA_E_UPNP_606_STRING = "Action not authorized";
inline constexpr char* EXPORT DLNA_E_UPNP_607_STRING = "Signature failure";
inline constexpr char* EXPORT DLNA_E_UPNP_608_STRING = "Signature missing";
inline constexpr char* EXPORT DLNA_E_UPNP_609_STRING = "Not encrypted";
inline constexpr char* EXPORT DLNA_E_UPNP_610_STRING = "Invalid sequence";
inline constexpr char* EXPORT DLNA_E_UPNP_611_STRING = "Invalid control URL";
inline constexpr char* EXPORT DLNA_E_UPNP_612_STRING = "No such session";
inline constexpr char* EXPORT DLNA_E_UPNP_701_STRING = "No such object";
inline constexpr char* EXPORT DLNA_E_UPNP_710_STRING = "Seek mode not supported";
inline constexpr char* EXPORT DLNA_E_UPNP_711_STRING = "Illegal seek target";
inline constexpr char* EXPORT DLNA_E_UPNP_712_STRING = "Bad metadata";
inline constexpr char* EXPORT DLNA_E_UPNP_715_STRING = "Content 'BUSY'";
inline constexpr char* EXPORT DLNA_E_UPNP_718_STRING = "Invalid InstanceID";
inline constexpr char* DLNA_RCSSUBVARIABLE = "LastChange";
inline constexpr char* UPNP_VIDEO = "object.item.videoItem";
inline constexpr char* UPNP_AUDIO_MUSIC = "object.item.audioItem.musicTrack";
inline constexpr char* UPNP_PHOTO = "object.item.imageItem.photo";
inline constexpr char* UPNP_IMAGE = "object.item.imageItem";

enum class DlnaStateType{
    DLNA_STATE_UNINTIALISED = 0,
    DLNA_STATE_ACTIVE,
    DLNA_STATE_BUTT = DLNA_ENUM_END
};

enum class DlnaDmrListenerType{
    DMR_LISTENER_INVALID = -1,
    DMR_LISTENER_SET_MEDIA_URI = 0,
    DMR_LISTENER_PLAY,
    DMR_LISTENER_PAUSE,
    DMR_LISTENER_STOP,
    DMR_LISTENER_SEEK,
    DMR_LISTENER_CURTIME,
    DMR_LISTENER_ALLTIME,
    DMR_LISTENER_SETVOL,
    DMR_LISTENER_GETVOL,
    DMR_LISTENER_SETMUTE,
    DMR_LISTENER_BUTT = DLNA_ENUM_END
};

enum class DlnaDmrAction{
    DLNA_DMR_ACTION_INVALID = -1,
    DLNA_DMR_ACTION_PLAY,
    DLNA_DMR_ACTION_STOP,
    DLNA_DMR_ACTION_PAUSE,
    DLNA_DMR_ACTION_FINISH,
    DLNA_DMR_ACTION_OPEN,
    DLNA_DMR_ACTION_BUTT = DLNA_ENUM_END
};

enum class AvTransportSeekModeType{
    AVTRANSPORT_SEEK_MODE_INVALID = -1,
    AVTRANSPORT_SEEK_MODE_TRACK_NR,
    AVTRANSPORT_SEEK_MODE_ABS_TIME,
    AVTRANSPORT_SEEK_MODE_REL_TIME,
    AVTRANSPORT_SEEK_MODE_ABS_COUNT,
    AVTRANSPORT_SEEK_MODE_REL_COUNT,
    AVTRANSPORT_SEEK_MODE_CHANNEL_FREQ,
    AVTRANSPORT_SEEK_MODE_TAPE_INDX,
    AVTRANSPORT_SEEK_MODE_REL_TAPE_INDX,
    AVTRANSPORT_SEEK_MODE_FRAME,
    AVTRANSPORT_SEEK_MODE_REL_FRAME,
    AVTRANSPORT_SEEK_MODE_BUTT = DLNA_ENUM_END
};

enum class DlnaLocalDeviceType{
    DLNA_LOCAL_DEVICE_TYPE_INVALID = -1,
    DLNA_LOCAL_DEVICE_TYPE_DMS,
    DLNA_LOCAL_DEVICE_TYPE_DMR,
    DLNA_LOCAL_DEVICE_TYPE_BUTT = DLNA_ENUM_END
};

 enum class DlnaInitModeType{
    DLNA_INIT_MODE_INVALID = -1,
    DLNA_INIT_MODE_DMS = 0,
    DLNA_INIT_MODE_DMP,
    DLNA_INIT_MODE_DMR,
    DLNA_INIT_MODE_BUTT = DLNA_ENUM_END
};

enum class DlnaParaType{
    DLNA_PARA_TYPE_INVALID = -1,
    DLNA_PARA_TYPE_STRING = 0,
    DLNA_PARA_TYPE_INT32,
    DLNA_PARA_TYPE_UINT32,
    DLNA_PARA_TYPE_FLOAT,
    DLNA_PARA_TYPE_BUTT = DLNA_ENUM_END
};

enum class EXPORT PlayerStates {
    PLAYER_STATE_ERROR = 0,
    PLAYER_IDLE = 1,
    PLAYER_INITIALIZED = 2,
    PLAYER_PREPARING = 3,
    PLAYER_PREPARED = 4,
    PLAYER_STARTED = 5,
    PLAYER_PAUSED = 6,
    PLAYER_STOPPED = 7,
    PLAYER_PLAYBACK_COMPLETE = 8,
    PLAYER_RELEASED = 9,
    PLAYER_BUFFERING = 10
};

struct MediaUrlData{
    unsigned int ulInstanceID;
    const unsigned char *pucUri;
    unsigned int ulUrlLen;
    unsigned int ulMediaId;
    const unsigned char *pucMetaData;
    unsigned int ulMetaDataLen;
    const void *pvAuxData;
    const void *pvContenData;
    int pvContenDataLen;
    char* clientIP;
};

struct DlnaMimeType{
    char *extension;
    char *mime_class;
    char *mime_protocol;
};

struct DlnaEntryNode {
    struct DlnaEntryNode *parent;
    struct DlnaEntryNode **childs; /* this entry's all childs link */
    int iTotalChild;           /* if it is item, should be -1 */
    uintptr_t id;                  /* every entry's id should be different */
    char *fullpath;              /* container or item's logic path */
    DlnaMimeType *mime_type;
    char *title; /* dc:Title */
    char *url;   /* if it is container, url is null,
                    if it is item, url is id.ext */
    long long size;  /* if it is container, size is 0 */
    int bUploadInProgress;
    int iUploadTmThdId; /* Upload timer Thread ID */
};

struct ChangeDeviceDes{
    char *pszDescription;
    char **ppszUDN;
    char **ppszModDescription;
    char *pucUdnPrefix;
    DlnaLocalDeviceType enDeviceType;
    const char *pcMacAddress;
};

struct Container {
    char *szContainerID;
    unsigned int ulStartId;
    struct Container *pstNextContainer;
};
struct DlnaStringS{
    unsigned char *pucBuf;
    unsigned int ulLen;
};

struct PlayData{
    char *playSpeed;
    char *clientIP;
};

struct StopData{
    uint32_t instanceID;
    char *clientIP;
};

struct PauseData{
    uint32_t instanceID;
    char *clientIP;
};

union DmrSeekTargetVal{
    DlnaStringS stStrVal;
    unsigned int ulVal;
    int iVal;
    float fVal;
};

struct SeekData{
    AvTransportSeekModeType seekMode;
    DlnaParaType dataType;
    const DmrSeekTargetVal *seekTarget;
    const char *clientIP;
};

#ifndef DLNA_CHK_RETURN_PRINT
#define DLNA_CHK_RETURN_PRINT(val, ret, ...) do {                                     \
        if ((val)) {                         \
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  __VA_ARGS__);          \
            return (ret);                    \
        }                                    \
    } while (0)
#endif

#define DLNA_LOCK(pMutex) do {                                                                       \
        int iRetLock = ithread_mutex_lock(pMutex);                          \
        if (iRetLock != 0) {                                                       \
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "call ithread_mutex_lock failure, iRetUnlock = %d\r\n", iRetLock); \
        }                                                                      \
    } while (0)

#define DLNA_UNLOCK(pMutex) do {                                                                         \
        int iRetUnlock = ithread_mutex_unlock(pMutex);                          \
        if (iRetUnlock != 0) {                                                         \
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "call ithread_mutex_unlock failure, iRetUnlock = %d\r\n", iRetUnlock); \
        }                                                                        \
    } while (0)

#define DLNA_MAX(a, b) ((a) > (b) ? (a) : (b))

#define DLNA_MIN(a, b) ((a) < (b) ? (a) : (b))

#define DLNA_UNUSED_VAR(var) ((void)(var))

#define DLNA_SET_RET_VAL(retVal, val) ((retVal) = (val))

inline bool EXPORT IS_CONNECTION_ID_VALID(int32_t id) 
{
    return ((id) >= 0);     // shouldnt not be negative
}

inline bool EXPORT IS_CONNECTION_ID_SUPPORTED(int32_t id)
{
    return ((id) == 0); // now, only 0 is supported
}

inline void DLNA_SLEEP(int ms)  {
        struct timespec _ts;
        _ts.tv_sec = (ms) >= 1000 ? (ms) / 1000 : 0;
        _ts.tv_nsec = (_ts.tv_sec > 0) ? ((ms) % 1000) * 1000000LL : (ms)*1000000LL;
        (void)nanosleep(&_ts, NULL);
}

inline void DLNA_FREE(void *ptr)  {                           
    if ((ptr) != DLNA_NULL) { 
        free((void *)(ptr)); 
        (ptr) = DLNA_NULL;  
    }                   
}

inline int EXPORT DLNA_STRCMP(const char *str1, const char *str2) {
    return strncmp(str1, str2, (strlen(str1) > strlen(str2) ? strlen(str1) : strlen(str2)));
}

/* Check if the value is a white space */
inline bool EXPORT DLNA_IS_WHITESPACE(unsigned char X) {
    return (((X) == ' ') || ((X) == '\n') || ((X) == '\r') || ((X) == '\t'));
}

inline void EXPORT DLNA_SET_DMR_INIT(DlnaStateType *ulMode) {
    *ulMode = DlnaStateType::DLNA_STATE_ACTIVE;
}
inline bool EXPORT DLNA_IS_DMR_INIT(DlnaStateType ulMode) {
    return ((ulMode) == DlnaStateType::DLNA_STATE_ACTIVE);
}

inline bool EXPORT IS_DMR_INSTANCE_ID_VALID(int id) {
    return (id >= 0);
}
inline bool EXPORT IS_DMR_INSTANCE_ID_SUPPORTED(int id) {
    return (id == 0);
}

inline void EXPORT DMR_RESET_STATEVAR_BITSET(unsigned int *NotifyBitSet) {
    *NotifyBitSet = 0;
}

typedef unsigned int (*pFnaDmrAvTransportSetMediaUri)(MediaUrlData mediaUrlData);
typedef unsigned int (*pFnDmrAvTransportPlay)(const PlayData *data);
typedef unsigned int (*pFnDmrAvTransportPause)(const PauseData *data);
typedef unsigned int (*pFnDmrAvTransportSeek)(const SeekData *data);
typedef unsigned int (*pFnDmrAvTransportStop)(const StopData *data);
typedef char *(*pFnDmrAvTransportGetCurrtime)();
typedef char *(*pFnDmrAvTransportGetAlltime)();
typedef char *(*pFnDmrAvTransportSetVol)(unsigned int ulDesiredVol, char *pcChannel);
typedef char *(*pFnDmrAvTransportGetVol)(int *ulVol, char *pcChannel);
typedef char *(*pFnDmrAvTransportSetMute)(int bDesiredMute, char *pcChannel);
#ifdef __cplusplus
}
#endif

}
}
}
}
#endif /* DLNA_TYPES_H */
