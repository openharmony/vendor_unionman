/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: dlna common
 * Author: 
 * Create: 2023-10-20
 */

#include <netinet/in.h>
#include <sys/ioctl.h>
//#include "dlna_log.h"
#include "inet_pton.h"
#include "upnpdebug.h"
#include "upnptools.h"
#include <hilog/log.h>
#include "upnpapi.h"
#include "dlna_common.h"
#include "dlna_dmr.h"
#include "dlna_web_server.h"
#include "securec.h"
#include "udp_server.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

constexpr  int DEVICE_NAME_LANE = 50;
uint16_t commonPort_ = DLNA_DEFAULT_PORT;

char *commonDeviceName_ = (char *)DLNA_NULL_PTR;
char commonMacAddress_[MAC_LEN] = {0};
char commonHttpIpAddr_[HTTPIPADDRLEN];
char commonURI_[URILEN];
unsigned int commonHttpPort_;
constexpr int DLNA_INVALID_UINT = 0xFFFFFFFF;
constexpr int DLNA_DMR_DEVICE_MAXAGE = 80;
constexpr int DLNA_DECIMAL_BASE = 10;
constexpr unsigned char DLNA_SIGN_MINUS = '-';
constexpr unsigned char DLNA_SIGN_PLUS = '+';
constexpr int DLNA_SINT32_POS_RANGE_CHECK = 214748364;
constexpr int DLNA_SINT32_NEG_RANGE_CHECK = -214748364;
constexpr int DLNA_UINT32_RANGE_CHECK = 429496729;
constexpr static char * DMS_DMR_WEBROOT_DIR = "/upnp";
/* Represent the ObjectID argument. */
constexpr static char *DLNA_STRING_OBJECTID = "ObjectID";
constexpr static char *DLNA_STRING_CONTAINERID = "ContainerID";
static char *dmrMimeStr_ = "http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_SM;DLNA.ORG_FLAGS=8D700000000000000000000000000000,"
    "http-get:*:audio/L16:DLNA.ORG_PN=LPCM;DLNA.ORG_FLAGS=8D700000000000000000000000000000,"
    "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_NTSC;DLNA.ORG_FLAGS=8D700000000000000000000000000000,"
    "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL;DLNA.ORG_FLAGS=8D700000000000000000000000000000,"
    "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_EU;DLNA.ORG_FLAGS=8D700000000000000000000000000000,"
    "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_EU_T;DLNA.ORG_FLAGS=8D700000000000000000000000000000,"
    "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_EU_ISO;DLNA.ORG_FLAGS=8D700000000000000000000000000000,"
    "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA;DLNA.ORG_FLAGS=8D700000000000000000000000000000,"
    "http-get:*:video/vnd.dlna.mpeg-tts:DLNA.ORG_PN=MPEG_TS_SD_NA_T;DLNA.ORG_FLAGS=8D700000000000000000000000000000,"
    "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_TS_SD_NA_ISO;DLNA.ORG_FLAGS=8D700000000000000000000000000000,"
    "http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_TN,"
    "http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_MED,"
    "http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_LRG,"
    "http-get:*:image/jpeg:DLNA.ORG_PN=JPEG_SM_ICO,"
    "http-get:*:video/mp4:DLNA.ORG_PN=AVC_MP4_BL_CIF15_AAC_520,"
    "http-get:*:audio/mpeg:DLNA.ORG_PN=MP3,"
    "http-get:*:image/jpeg:*,"
    "http-get:*:image/png:*,"
    "http-get:*:video/mkv:*,"
    "http-get:*:video/mpeg:*,"
    "http-get:*:video/vnd.dlna.mpeg-tts:*,"
    "http-get:*:audio/L16:*,"
    "http-get:*:video/avi:*,"
    "http-get:*:video/x-ms-wmv:*,"
    "http-get:*:video/mp4:*,"
    "http-get:*:video/mpeg2:*,"
    "http-get:*:video/3gpp:*,"
    "http-get:*:audio/mpeg:*,"
    "http-get:*:audio/x-ms-wma:*,"
    "http-get:*:audio/amr:*,"
    "http-get:*:video/wmv:*,"
    "http-get:*:video/x-motion-jpeg:*,"
    "http-get:*:video/x-motion-jpeg:*,"
    "http-get:*:video/mp2p:*,"
    "http-get:*:video/quicktime:*,"
    "http-get:*:video/x-dv:*,"
    "http-get:*:audio/x-aac:*,"
    "http-get:*:audio/x-ac3:*,"
    "http-get:*:audio/wav:*,"
    "http-get:*:audio/x-ms-wma:*,"
    "http-get:*:audio/x-pn-realaudio:*,"
    "http-get:*:video/x-matroska:*,"
    "http-get:*:video/flv:*,"
    "http-get:*:video/x-ms-asf:*,"
    "http-get:*:video/x-msvideo:*,"
    "http-get:*:audio/3gpp:*,"
    "http-get:*:audio/mp4:*,"
    "http-get:*:audio/x-matroska:*,"
    "http-get:*:audio/wma:*,"
    "http-get:*:audio/mpeg3:*,"
    "http-get:*:audio/x-wav:*,"
    "http-get:*:audio/x-mpeg:*,"
    "http-get:*:application/octet-stream:*,"
    "http-get:*:application/vnd.rn-realmedia:*,"
    "http-get:*:video/vnd.rn-realvideo:*,"
    "http-get:*:application/vnd.rn-realmedia-vbr:*,"
    "http-get:*:audio/x-pn-realaudio:*,"
    "http-get:*:image/GIF:*,"
    "http-get:*:image/gif:*";

//DEFINE_DLNA_LABEL("Dlna_dmr-common");
#define DLNA_LOG_TAG "Dlna_dmr-common"

DlnaCommon &DlnaCommon::getInstance(){
	static DlnaCommon instance{};
	return instance;
}

DlnaCommon::DlnaCommon(){
    dlnaDmrHdl_ = (DlnaDmrDevice *)malloc(sizeof(DlnaDmrDevice));
    memset(dlnaDmrHdl_, 0, sizeof(DlnaDmrDevice));
}

DlnaCommon::~DlnaCommon(){
}


DlnaStateType DlnaCommon::GetInitMode()
{
    return initMode_;
}

char *DlnaCommon::GetPcDeviceName()
{
    return commonDeviceName_;
}

void DlnaCommon::FreePcDeviceName()
{
    DLNA_FREE(commonDeviceName_);
}

void DlnaCommon::SetPcDeviceName(char *pcDeviceName)
{
    commonDeviceName_ = pcDeviceName;
}

DlnaCommon::DlnaMetaDataList *DlnaCommon::GetStMetaDataList()
{
    return &metaDataList_;
}

DlnaStateType DlnaCommon::GetState()
{
	OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][%{public}d] dlnaCommonState_ = %{public}d\n", __FUNCTION__, __LINE__, dlnaCommonState_);
    return dlnaCommonState_;
}

void DlnaCommon::DeInit(void)
{
    dlnaCommonState_ = DlnaStateType::DLNA_STATE_UNINTIALISED;
	OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][%{public}d] dlnaCommonState_ = %{public}d\n", __FUNCTION__, __LINE__, dlnaCommonState_);
    if (DLNA_IS_DMR_INIT(initMode_)) {
        DlnaDmr::getInstance().DeInit();
    }

    /* 3. Close UpNp Sercvies */
    (void)UpnpFinish();

    initMode_ = DlnaStateType::DLNA_STATE_UNINTIALISED;

    return;
}

int DlnaCommon::VirtualDirInit(void)
{
    if (UpnpEnableWebserver(DLNA_TRUE) != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Failed to start the webserver \n", __FUNCTION__, __LINE__);
        return DLNA_RET_COMM_INIT_FAIL;
    }

	if (UpnpVirtualDir_set_GetInfoCallback(DlnaWebServer::GetItemInfo) != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Cannot set virtual directory\n", __FUNCTION__, __LINE__);
        return DLNA_RET_COMM_INIT_FAIL;
    }

    if (UpnpVirtualDir_set_OpenCallback(DlnaWebServer::Open) != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Cannot set open callbacks\n", __FUNCTION__, __LINE__);
        return DLNA_RET_COMM_INIT_FAIL;
    }

    if (UpnpVirtualDir_set_ReadCallback(DlnaWebServer::Read) != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Cannot set read callbacks\n", __FUNCTION__, __LINE__);
        return DLNA_RET_COMM_INIT_FAIL;
    }

    if (UpnpVirtualDir_set_WriteCallback(DlnaWebServer::Write) != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Cannot set write callbacks\n", __FUNCTION__, __LINE__);
        return DLNA_RET_COMM_INIT_FAIL;
    }

    if (UpnpVirtualDir_set_SeekCallback(DlnaWebServer::Seek) != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Cannot set seek callbacks\n", __FUNCTION__, __LINE__);
        return DLNA_RET_COMM_INIT_FAIL;
    }

    if (UpnpVirtualDir_set_CloseCallback(DlnaWebServer::Close) != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Cannot set close callbacks\n", __FUNCTION__, __LINE__);
        return DLNA_RET_COMM_INIT_FAIL;
    }

    if (UpnpSetWebServerRootDir(DMS_DMR_WEBROOT_DIR) != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:upnp set web server root dir fail \r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_COMM_INIT_FAIL;
    }
	
    if (UpnpAddVirtualDir(VIRTUAL_DIR, NULL, NULL) != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Cannot add web server\n", __FUNCTION__, __LINE__);
        return DLNA_RET_COMM_INIT_FAIL;
    }

    return DLNA_RET_SUCCESS;
}

int DlnaCommon::UpnpInit(const char *pcAdapterName, DlnaStateType ulInitMode)
{
    int iRet;
    
     if (DLNA_IS_DMR_INIT(ulInitMode)) {
        commonPort_ = DLNA_DMR_PORT;
        if (strcmp(pcAdapterName, DLNA_P2P0_NAME) == 0) {
            commonPort_ = DLNA_DMR_OVER_P2P_PORT;
        }
    } else {
        commonPort_ = DLNA_DEFAULT_PORT;
    }

    UpnpSetLogFileNames("/data/storage/el2/base/haps/entry/files/upnp.log", NULL);
    UpnpSetLogLevel(UPNP_ALL);
    UpnpInitLog();

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "UpnpInit2 interface:%{public}s commonPort_=%{public}d\n", pcAdapterName, commonPort_);
    iRet = UpnpInit2(pcAdapterName, commonPort_);
//	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "regist ip %{public}s:%{public}d", UpnpGetServerIpAddress(),UpnpGetServerPort());
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "UpnpInit2 ret:%{public}d, err msg:%{public}s", iRet, strerror(errno));
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "regist ip %{public}s:%{public}d", UpnpGetServerIpAddress(),UpnpGetServerPort());
    if (iRet != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:DLNA Init Fails for DMS due to "
            "UPnP Init Failure With return code "
            "--%{public}d\n",
            __FUNCTION__, __LINE__, iRet);
//        return DLNA_RET_COMM_INIT_FAIL;
    }

    /* Set the max content length to 0 i.e. infinite */
    if (UpnpSetMaxContentLength(0) != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Could not set Max content UPnP\n", __FUNCTION__, __LINE__);
        UpnpFinish();
        return DLNA_RET_COMM_INIT_FAIL;
    }

    if (DLNA_IS_DMR_INIT(ulInitMode)) {
        iRet = VirtualDirInit();
        if (iRet != DLNA_RET_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "%{public}s L%{public}d:VirtualDirInit fail", __FUNCTION__, __LINE__);
            UpnpFinish();
        }
    }

    return DLNA_RET_SUCCESS;
}

int DlnaCommon::UpnpInitExt(const char *pcAdapterName, const char* ipAddr, DlnaStateType ulInitMode) {
    int iRet;

    if (DLNA_IS_DMR_INIT(ulInitMode)) {
        commonPort_ = DLNA_DMR_PORT;
        if (strcmp(pcAdapterName, DLNA_P2P0_NAME) == 0) {
            commonPort_ = DLNA_DMR_OVER_P2P_PORT;
        }
    } else {
        commonPort_ = DLNA_DEFAULT_PORT;
    }

    UpnpSetLogFileNames("/data/storage/el2/base/haps/entry/files/upnp.log", NULL);
    UpnpSetLogLevel(UPNP_ALL);
    UpnpInitLog();

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "UpnpInit2 interface:%{public}s commonPort_=%{public}d\n",
                 pcAdapterName, commonPort_);
    UpnpSetServerIpAddress(ipAddr);
    iRet = UpnpInit2(pcAdapterName, commonPort_);
    //	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "regist ip %{public}s:%{public}d", U
    //npGetServerIpAddress(),UpnpGetServerPort());
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "UpnpInit2 ret:%{public}d, err msg:%{public}s", iRet,
                 strerror(errno));
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "regist ip %{public}s:%{public}d", UpnpGetServerIpAddress(),
                 UpnpGetServerPort());
    if (iRet != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                     "[CastEngineDlna][%{public}s][:%{public}d]-:DLNA Init Fails for DMS due to "
                     "UPnP Init Failure With return code "
                     "--%{public}d\n",
                     __FUNCTION__, __LINE__, iRet);
        //        return DLNA_RET_COMM_INIT_FAIL;
    }

    /* Set the max content length to 0 i.e. infinite */
    if (UpnpSetMaxContentLength(0) != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                     "[CastEngineDlna][%{public}s][:%{public}d]-:Could not set Max content UPnP\n", __FUNCTION__,
                     __LINE__);
        UpnpFinish();
        return DLNA_RET_COMM_INIT_FAIL;
    }

    if (DLNA_IS_DMR_INIT(ulInitMode)) {
        iRet = VirtualDirInit();
        if (iRet != DLNA_RET_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG, "%{public}s L%{public}d:VirtualDirInit fail",
                         __FUNCTION__, __LINE__);
            UpnpFinish();
        }
    }

    return DLNA_RET_SUCCESS;
}

unsigned int DlnaCommon::ServceInit(const char *pcAdapterName, const char *pcMacAddress, const DlnaStateType ulInitMode)
{
    int iRet;
    /* Do common init for the required devices */
    iRet = UpnpInit(pcAdapterName, ulInitMode);
    if (iRet != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:UpnpInit( ) failure, "
            "return code --%{public}d\r\n", __FUNCTION__, __LINE__, iRet);
        return DLNA_RET_COMM_INIT_FAIL;
    }

    /* Do device specific init */
    /* if DMR */
    if (DLNA_IS_DMR_INIT(ulInitMode)) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][%{public}d] DLNA_IS_DMR_INIT ulInitMode=%{public}d\n", __FUNCTION__, __LINE__, ulInitMode);
        iRet = DlnaDmr::getInstance().Init(pcMacAddress);
        if (iRet != DLNA_RET_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:DMR Init failure, "
                "return code --%{public}d\r\n", __FUNCTION__, __LINE__, iRet);
            DeInit();
            return DLNA_RET_COMM_INIT_FAIL;
        }
    }

    return DLNA_RET_SUCCESS;
}

unsigned int DlnaCommon::ServceInitExt(const char *pcAdapterName, const char *ipAddr, const char *pcMacAddress,
                                    const DlnaStateType ulInitMode) {
    int iRet;
    /* Do common init for the required devices */
    iRet = UpnpInitExt(pcAdapterName, ipAddr, ulInitMode);
    if (iRet != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                     "[CastEngineDlna][%{public}s][:%{public}d]-:UpnpInit( ) failure, "
                     "return code --%{public}d\r\n",
                     __FUNCTION__, __LINE__, iRet);
        return DLNA_RET_COMM_INIT_FAIL;
    }

    /* Do device specific init */
    /* if DMR */
    if (DLNA_IS_DMR_INIT(ulInitMode)) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,
                     "[%{public}s][%{public}d] DLNA_IS_DMR_INIT ulInitMode=%{public}d\n", __FUNCTION__, __LINE__,
                     ulInitMode);
        iRet = DlnaDmr::getInstance().Init(pcMacAddress);
        if (iRet != DLNA_RET_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                         "[CastEngineDlna][%{public}s][:%{public}d]-:DMR Init failure, "
                         "return code --%{public}d\r\n",
                         __FUNCTION__, __LINE__, iRet);
            DeInit();
            return DLNA_RET_COMM_INIT_FAIL;
        }
    }

    return DLNA_RET_SUCCESS;
}

unsigned int DlnaCommon::Create(const char *pcAdapterName, const char *pcMacAddress,
    DlnaInitModeType enInitMode)
{
    DlnaStateType ulInitMode = DlnaStateType::DLNA_STATE_BUTT;
    unsigned int iRet;

    if (strncpy_s(commonMacAddress_, sizeof(commonMacAddress_), pcMacAddress, strlen(pcMacAddress)) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: strncpy_s failed\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_COMM_CREATE_FAILED;
    }
    commonMacAddress_[sizeof(commonMacAddress_) - 1] = 0;
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "Create Start...");

    /* Check the state of stack */
    if (dlnaCommonState_ == DlnaStateType::DLNA_STATE_ACTIVE) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: cannot INIT Again !!! \r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_COMM_CREATE_FAILED;
    }

    iRet = MapInitEnum(enInitMode, &ulInitMode);
    if (iRet != DLNA_RET_SUCCESS) {
        /* DeInit is Not Required in this Case */
        return iRet;
    }

    initMode_ = ulInitMode;

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][%{public}d] ulInitMode=%{public}d\n", __FUNCTION__, __LINE__, ulInitMode);
    if (ServceInit(pcAdapterName, pcMacAddress, ulInitMode) != DLNA_RET_SUCCESS) {
        return DLNA_RET_COMM_INIT_FAIL;
    }

    dlnaCommonState_ = DlnaStateType::DLNA_STATE_ACTIVE;

    /* Now Send the Advertisement for All devices */
    iRet = Run(ulInitMode);
    if (iRet != DLNA_RET_SUCCESS) {
        /* Sending of the Advertisment failed */
        DeInit();
        return DLNA_RET_COMM_INIT_FAIL;
    }
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][%{public}d] dlnaCommonState_ = %{public}d\n", __FUNCTION__, __LINE__, dlnaCommonState_);
    return DLNA_RET_SUCCESS;
}

unsigned int DlnaCommon::CreateExt(const char *pcAdapterName, const char *ipAddr, const char *pcMacAddress, DlnaInitModeType enInitMode) {
    DlnaStateType ulInitMode = DlnaStateType::DLNA_STATE_BUTT;
    unsigned int iRet;

    if (strncpy_s(commonMacAddress_, sizeof(commonMacAddress_), pcMacAddress, strlen(pcMacAddress)) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                     "[CastEngineDlna][%{public}s][:%{public}d]-: strncpy_s failed\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_COMM_CREATE_FAILED;
    }
    commonMacAddress_[sizeof(commonMacAddress_) - 1] = 0;
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "Create Start...");

    /* Check the state of stack */
    if (dlnaCommonState_ == DlnaStateType::DLNA_STATE_ACTIVE) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                     "[CastEngineDlna][%{public}s][:%{public}d]-: cannot INIT Again !!! \r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_COMM_CREATE_FAILED;
    }

    iRet = MapInitEnum(enInitMode, &ulInitMode);
    if (iRet != DLNA_RET_SUCCESS) {
        /* DeInit is Not Required in this Case */
        return iRet;
    }

    initMode_ = ulInitMode;

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "[%{public}s][%{public}d] ulInitMode=%{public}d\n",
                 __FUNCTION__, __LINE__, ulInitMode);
    if (ServceInitExt(pcAdapterName, ipAddr, pcMacAddress, ulInitMode) != DLNA_RET_SUCCESS) {
        return DLNA_RET_COMM_INIT_FAIL;
    }

    dlnaCommonState_ = DlnaStateType::DLNA_STATE_ACTIVE;

    /* Now Send the Advertisement for All devices */
    iRet = Run(ulInitMode);
    if (iRet != DLNA_RET_SUCCESS) {
        /* Sending of the Advertisment failed */
        DeInit();
        return DLNA_RET_COMM_INIT_FAIL;
    }
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG, "[%{public}s][%{public}d] dlnaCommonState_ = %{public}d\n",
                 __FUNCTION__, __LINE__, dlnaCommonState_);
    return DLNA_RET_SUCCESS;
}

unsigned int DlnaCommon::MapInitEnum(DlnaInitModeType enInitMode, DlnaStateType *pulInitMode)
{
    DlnaStateType ulInitMode = DlnaStateType::DLNA_STATE_BUTT;

    switch (enInitMode) {
        case DlnaInitModeType::DLNA_INIT_MODE_DMR:
            DLNA_SET_DMR_INIT(&ulInitMode);
            break;

        default:
            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Invalid Init Option\r\n", __FUNCTION__, __LINE__);
            return DLNA_RET_COMM_INVALID_OPTION;
    }

    *pulInitMode = ulInitMode;

    return DLNA_RET_SUCCESS;
}

unsigned int DlnaCommon::Run(DlnaStateType ulInitMode)
{
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][:%{public}d]", __FUNCTION__, __LINE__);
    int iRet;
    if (DLNA_IS_DMR_INIT(ulInitMode)) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][:%{public}d]-:Sending UPnP advertisement "
            "for DMR !!!\n",
            __FUNCTION__, __LINE__);

        iRet = UpnpSendAdvertisement(dlnaDmrHdl_->dev, DLNA_DMR_DEVICE_MAXAGE);
        if (iRet != UPNP_E_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][:%{public}d]-:Sending Advertisements For "
                "DMR failed with return code: %{public}d\n",
                __FUNCTION__, __LINE__, iRet);
            return iRet;
        }
    }

    return DLNA_RET_SUCCESS;
}

void DlnaCommon::Destroy(void)
{
    /* depend on global variable */
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Destroy is "
        "Invoked \r\n ",
        __FUNCTION__, __LINE__);

    /* Check the state of stack */
    if (dlnaCommonState_ != DlnaStateType::DLNA_STATE_ACTIVE) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Stack is not initialzed, "
            "cannot De-INIT!!! \r\n",
            __FUNCTION__, __LINE__);

        return;
    }

    DLNA_FREE(commonDeviceName_);

    DeInit();

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Destroy is Successful"
        " \r\n ",
        __FUNCTION__, __LINE__);

    return;
}

unsigned int DlnaCommon::ChangeDeviceDescription(ChangeDeviceDes changeDeviceDes)
{
    IXML_Document *ixmlDmrDes = (IXML_Document *)DLNA_NULL_PTR;
    IXML_NodeList *ixmlNodeList = (IXML_NodeList *)DLNA_NULL_PTR;
    IXML_Node *ixmlNode = (IXML_Node *)DLNA_NULL_PTR;
    char *pcDmrDescrption = (char *)DLNA_NULL_PTR;
    int iResult;

    *changeDeviceDes.ppszModDescription = (char *)DLNA_NULL_PTR;

    /*
        We are not Validating the pszDescriptionFile as the same is
        validated inside ixmlLoadDocument
    */
    iResult = ixmlParseBufferEx(changeDeviceDes.pszDescription, &ixmlDmrDes);
    if (iResult != 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Parsing of XMLDoc Failed code-:%{public}d\r\n", __FUNCTION__, __LINE__, iResult);
        return DLNA_RET_FAILURE;
    }

    ixmlNodeList = ixmlDocument_getElementsByTagName(ixmlDmrDes, "UDN");
    if (ixmlNodeList == DLNA_NULL_PTR) {
        ixmlDocument_free(ixmlDmrDes);
        return DLNA_RET_FAILURE;
    }

    ixmlNode = ixmlNodeList_item(ixmlNodeList, 0);

    ixmlNodeList_free(ixmlNodeList);

    ixmlNode = ixmlNode_getFirstChild(ixmlNode);
    *changeDeviceDes.ppszUDN = GetUDN(changeDeviceDes.pucUdnPrefix, changeDeviceDes.pcMacAddress);

    if (*changeDeviceDes.ppszUDN == DLNA_NULL_PTR) {
        ixmlDocument_free(ixmlDmrDes);
        return DLNA_RET_FAILURE;
    }

    iResult = (unsigned int)ixmlNode_setNodeValue(ixmlNode, (const char *)*changeDeviceDes.ppszUDN);
    if (iResult != DLNA_RET_SUCCESS) {
        ixmlDocument_free(ixmlDmrDes);
        return DLNA_RET_FAILURE;
    }

    if (commonDeviceName_ != NULL) {
        iResult = ChangeDescriptionFriendlyName(ixmlDmrDes, (const char *)commonDeviceName_);
        if (iResult != 0) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Parsing of XMLDoc Failed -:%{public}d\r\n", __FUNCTION__, __LINE__, iResult);
            ixmlDocument_free(ixmlDmrDes);
            return DLNA_RET_FAILURE;
        }
    }

    pcDmrDescrption = ixmlPrintDocument(ixmlDmrDes);
    if (pcDmrDescrption == DLNA_NULL_PTR) {
        ixmlDocument_free(ixmlDmrDes);
        return DLNA_RET_FAILURE;
    }

    *changeDeviceDes.ppszModDescription = (char *)pcDmrDescrption;

    ixmlDocument_free(ixmlDmrDes);
    return DLNA_RET_SUCCESS;
}

int DlnaCommon::ChangeDescriptionFriendlyName(IXML_Document *pstDescDoc, const char *pszDeviceName)
{
    IXML_NodeList *ixmlNodeList = (IXML_NodeList *)DLNA_NULL_PTR;
    IXML_Node *ixmlNode = (IXML_Node *)DLNA_NULL_PTR;
    int iResult;

    ixmlNodeList = ixmlDocument_getElementsByTagName(pstDescDoc, "friendlyName");
    if (ixmlNodeList == DLNA_NULL_PTR) {
        return DLNA_RET_FAILURE;
    }
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:friendlyName=%{public}s\r\n ",
        __FUNCTION__, __LINE__, pszDeviceName );
    ixmlNode = ixmlNodeList_item(ixmlNodeList, 0);

    ixmlNode = ixmlNode_getFirstChild(ixmlNode);
    iResult = (unsigned int)ixmlNode_setNodeValue(ixmlNode, pszDeviceName);
    ixmlNodeList_free(ixmlNodeList);
    return iResult;
}

#define UNI_LEN 15
char *DlnaCommon::GetPchUdn(const char *pucUdnPrefix, const char *pchUniString, int pchUniStringLen)
{
    char *pchUdn = NULL;
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:pchUniStringLen=%{public}d UDN_LEN=%{public}d\r\n",
        __FUNCTION__, __LINE__, pchUniStringLen, UDN_LEN );
    if (pchUniStringLen >= UDN_LEN) {
        return (char *)DLNA_NULL_PTR;
    }

    pchUdn = (char *)malloc(UDN_LEN);
    if (pchUdn == DLNA_NULL_PTR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:In GetUDN Malloc failed"
            " for Creating UDN\r\n",
            __FUNCTION__, __LINE__);
        return (char *)DLNA_NULL_PTR;
    }

    if (memset_s(pchUdn, UDN_LEN, 0, UDN_LEN) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:memset_s failed\r\n", __FUNCTION__, __LINE__);
        DLNA_FREE(pchUdn);
        return (char *)DLNA_NULL_PTR;
    }
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:pucUdnPrefix=%{public}s pchUniString=%{public}s\r\n", __FUNCTION__, __LINE__, pucUdnPrefix, pchUniString );
    if (snprintf_s(pchUdn, UDN_LEN, UDN_LEN - 1, "uuid:bb5e21ce-%s-11b2-f918-%s", pucUdnPrefix, pchUniString) < EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:snprintf_s failed\r\n", __FUNCTION__, __LINE__);
        DLNA_FREE(pchUdn);
        return (char *)DLNA_NULL_PTR;
    }
    return pchUdn;
}

bool DlnaCommon::CheckPcMacAddress(const char *pcMacAddress, int index)
{
    return (pcMacAddress[index] >= 'a' && pcMacAddress[index] <= 'z') ||
                (pcMacAddress[index] >= 'A' && pcMacAddress[index] <= 'Z') ||
                (pcMacAddress[index] >= '0' && pcMacAddress[index] <= '9');
}

char *DlnaCommon::GetUDN(char *pucUdnPrefix, const char *pcMacAddress)
{
    char pchUniString[UNI_LEN + 1];
    unsigned int newIndex = 0;
    unsigned int oldIndex = 0;

    if (memset_s(pchUniString, sizeof(pchUniString), 0, sizeof(pchUniString)) != EOK) {
        return (char *)DLNA_NULL_PTR;
    }
    if (pcMacAddress != NULL && strlen(pcMacAddress) > 0) {
        while (pcMacAddress[oldIndex] != '\0' && newIndex < UNI_LEN) {
            if (!DlnaCommon::getInstance().CheckPcMacAddress(pcMacAddress, oldIndex)) {
                oldIndex++;
                continue;
            }
            pchUniString[newIndex++] = pcMacAddress[oldIndex++];
        }
        return DlnaCommon::getInstance().GetPchUdn(pucUdnPrefix, pchUniString, UNI_LEN + 1);
    }

    if (DlnaCommon::getInstance().GetUniString(pchUniString, UNI_LEN) < 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:In GetUDN GetUniString failed"
            " for Creating UDN\r\n",
            __FUNCTION__, __LINE__);
        return (char *)DLNA_NULL_PTR;
    }
    return DlnaCommon::getInstance().GetPchUdn(pucUdnPrefix, pchUniString, UNI_LEN + 1);
}

int DlnaCommon::AddParamToActionResponse(DmrActionParam *event, const char *key, const char *value,
    int len)
{
    int iRetval;
    if (len < 0) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:add key-value to the action response len is %{public}d\r\n",
            __FUNCTION__, __LINE__, len);
    }
    IXML_Document *actionResultDoc = NULL;
    actionResultDoc = UpnpActionRequest_get_ActionResult(event->pstActionRequest);
    iRetval = UpnpAddToActionResponse(&actionResultDoc,
        UpnpString_get_String(UpnpActionRequest_get_ActionName(event->pstActionRequest)),
        event->pcServiceType, key, value);
    if (iRetval != UPNP_E_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Failed to add key-value to the "
            "action response len is %{public}d\r\n", __FUNCTION__, __LINE__, len);

        /* swith UPnP's error code */
        event->iErrorCode = DLNA_E_UPNP_603;
        event->pErrorStr = DLNA_E_UPNP_603_STRING;
        return DLNA_RET_FAILURE;
    }
    UpnpActionRequest_set_ActionResult(event->pstActionRequest, actionResultDoc);

    return DLNA_RET_SUCCESS;
}

bool DlnaCommon::GetTextNode(IXML_NodeList *pstNodeList, IXML_Node **pstTextNode)
{
    IXML_Node *pstElementNode = NULL;
    pstElementNode = ixmlNodeList_item(pstNodeList, 0);
    ixmlNodeList_free(pstNodeList);
    if (pstElementNode == DLNA_NULL_PTR) {
        return false;
    }

    (*pstTextNode) = ixmlNode_getFirstChild(pstElementNode);
    if (((*pstTextNode) == DLNA_NULL_PTR) || (ixmlNode_getNodeType(*pstTextNode)) != eTEXT_NODE) {
        return false;
    }
    return true;
}

const char *DlnaCommon::DocumentGetChildElementValue(IXML_Document *pstParent, const char *pcTagName)
{
    IXML_NodeList *pstNodeList = NULL;
    IXML_Node *pstTextNode = NULL;

    if ((pstParent == DLNA_NULL_PTR) || (pcTagName == DLNA_NULL_PTR)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Invalid Parameter has been "
            "Passed to DocumentGetChildElementValue\r\n",
            __FUNCTION__, __LINE__);
        return (char *)DLNA_NULL_PTR;
    }

    pstNodeList = ixmlDocument_getElementsByTagName(pstParent, pcTagName);
    if (pstNodeList == DLNA_NULL_PTR) {
        return (char *)DLNA_NULL_PTR;
    }

    if (!GetTextNode(pstNodeList, &pstTextNode)) {
        return (char *)DLNA_NULL_PTR;
    }

    return ixmlNode_getNodeValue(pstTextNode);
}

char *DlnaCommon::GetStr(UpnpActionRequest *request, const char *key)
{
    char *pStrVal = (char *)DLNA_NULL_PTR;

    if ((request == DLNA_NULL_PTR) || (key == DLNA_NULL_PTR)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Invalid Parameter has been "
            "Passed to GetStr\r\n",
            __FUNCTION__, __LINE__);
        return (char *)DLNA_NULL_PTR;
    }

    pStrVal = (char *)DlnaCommon::getInstance().DocumentGetChildElementValue(UpnpActionRequest_get_ActionRequest(request), key);
    if (pStrVal == DLNA_NULL_PTR) {
        return (char *)DLNA_NULL_PTR;
    } else {
        pStrVal = strdup(pStrVal);
    }

    return pStrVal;
}

int DlnaCommon::ConvAscToSIntInit(unsigned char **pucBufPtr, int *bIsNeg)
{
    /* Skip leading whitespaces */
    while (DLNA_IS_WHITESPACE(**pucBufPtr)) {
        (*pucBufPtr)++;
    }

    /* The sign if present must be the first character */
    if ((**pucBufPtr == DLNA_SIGN_MINUS) || (**pucBufPtr == DLNA_SIGN_PLUS)) {
        if (**pucBufPtr == DLNA_SIGN_MINUS) {
            *bIsNeg = DLNA_TRUE;
        }

        (*pucBufPtr)++;
    }

    /* Check if it has atleast one digit */
    if (**pucBufPtr == '\0') {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Invalid Data received\r\n", __FUNCTION__, __LINE__);
        return DLNA_NULL_INT32;
    }
    return DLNA_TRUE;
}

bool DlnaCommon::ConvValNormal(int bIsNeg, int convVal)
{
    return ((bIsNeg == DLNA_FALSE) && (convVal > DLNA_SINT32_POS_RANGE_CHECK)) ||
            ((bIsNeg == DLNA_TRUE) && (convVal < DLNA_SINT32_NEG_RANGE_CHECK));
}

bool DlnaCommon::IsInt(unsigned char ucChar)
{
    return (ucChar < '0') || (ucChar > '9');
}

int DlnaCommon::ConvAscToSInt(unsigned char *pucString)
{
    unsigned char ucChar;
    int convVal = 0;
    int iTemp;
    unsigned char *pucBufPtr = pucString;
    int bIsNeg = DLNA_FALSE;

    if (ConvAscToSIntInit(&pucBufPtr, &bIsNeg) != DLNA_TRUE) {
        return DLNA_NULL_INT32;
    }

    /* There can be leading zeros */
    while (*pucBufPtr != '\0') {
        ucChar = *pucBufPtr;

        if (IsInt(ucChar)) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Invalid DataType received in string: %{public}s\r\n", __FUNCTION__, __LINE__,
                pucString);
            return DLNA_NULL_INT32;
        }

        if (ConvValNormal(bIsNeg, convVal)) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: DataType OverFlowed "
                "while converting string: %{public}s\r\n",
                __FUNCTION__, __LINE__, pucString);
            return DLNA_NULL_INT32;
        }

        iTemp = convVal * DLNA_DECIMAL_BASE;
        if (bIsNeg != 0) {
            convVal = iTemp - (ucChar - '0');
            pucBufPtr++;
            continue;
        }
        convVal = iTemp + (ucChar - '0');
        pucBufPtr++;
    }

    return convVal;
}

unsigned int DlnaCommon::ConvAscToUInt(unsigned char *pucString)
{
    unsigned char ucChar;
    unsigned int ulConvVal = 0;
    unsigned int ulTemp;
    unsigned char *pucBufPtr = pucString;

    /* Skip leading whitespaces */
    while (DLNA_IS_WHITESPACE(*pucBufPtr)) {
        pucBufPtr++;
    }

    /* Check if it has atleast one digit */
    if (*pucBufPtr == '\0') {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Invalid Data received in string: %{public}s\r\n", __FUNCTION__, __LINE__, pucString);
        return DLNA_INVALID_UINT;
    }

    /* There can be leading zeros */
    while (*pucBufPtr != '\0') {
        ucChar = *pucBufPtr;

        if ((ucChar < '0') || (ucChar > '9')) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Invalid DataType received in string: %{public}s\r\n", __FUNCTION__, __LINE__,
                pucString);
            return DLNA_INVALID_UINT;
        }

        if (ulConvVal > DLNA_UINT32_RANGE_CHECK) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: DataType OverFlowed "
                "while converting string: %{public}s\r\n",
                __FUNCTION__, __LINE__, pucString);
            return DLNA_INVALID_UINT;
        }

        ulTemp = ulConvVal * DLNA_DECIMAL_BASE;
        ulConvVal = ulTemp + (ucChar - '0');
        if (ulConvVal < ulTemp) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: DataType OverFlowed "
                "while converting string: %{public}s\r\n",
                __FUNCTION__, __LINE__, pucString);
            return DLNA_INVALID_UINT;
        }

        pucBufPtr++;
    }

    return ulConvVal;
}

int DlnaCommon::GetInt(UpnpActionRequest *request, const char *key)
{
    char *strValue = (char *)DLNA_NULL_PTR;
    int val;

    if (request == DLNA_NULL_PTR || key == DLNA_NULL_PTR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Invalid Parameter has been "
            "Passed to GetInt\r\n",
            __FUNCTION__, __LINE__);
        return DLNA_NULL_INT32;
    }

    strValue = GetStr(request, key);
    /* If the Arrribute is Not Present in the Message , it must be Rejected */
    if (strValue == DLNA_NULL_PTR) {
        return DLNA_NULL_INT32;
    }

    val = ConvAscToSInt((unsigned char *)strValue);
    DLNA_FREE(strValue);

    return val;
}

unsigned int DlnaCommon::GetIntValue(UpnpActionRequest *request, const char *key, int *piValue)
{
    char *strValue = (char *)DLNA_NULL_PTR;
    int val;

    if (request == DLNA_NULL_PTR || key == DLNA_NULL_PTR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Invalid Parameter has been "
            "Passed to GetIntValue\r\n",
            __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    strValue = GetStr(request, key);
    /* If the Arrribute is Not Present in the Message , it must be Rejected */
    if (strValue == DLNA_NULL_PTR) {
        return DLNA_RET_FAILURE;
    }

    val = ConvAscToSInt((unsigned char *)strValue);
    DLNA_FREE(strValue);

    *piValue = val;

    return DLNA_RET_SUCCESS;
}

unsigned int DlnaCommon::GetUInt(UpnpActionRequest *request, const char *chObject)
{
    char *strValue = (char *)DLNA_NULL_PTR;
    unsigned int val;

    if (request == DLNA_NULL_PTR || chObject == DLNA_NULL_PTR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Invalid Parameter has been "
            "Passed to GetInt\r\n",
            __FUNCTION__, __LINE__);
        return DLNA_INVALID_UINT;
    }

    /* some doubt, why can't find object ID, then find container ID */
    strValue = GetStr(request, chObject);
    if ((strValue == DLNA_NULL_PTR) && (strcmp(chObject, DLNA_STRING_OBJECTID) == 0)) {
        strValue = GetStr(request, DLNA_STRING_CONTAINERID);
    }

    /* If the Arrribute is Not Present in the Message , it must be Rejected */
    if (strValue == DLNA_NULL_PTR) {
        return DLNA_INVALID_UINT;
    }

    val = ConvAscToUInt((unsigned char *)strValue);
    DLNA_FREE(strValue);

    return val;
}

const char *DlnaCommon::GetVersion()
{
    return (const char *)DLNA_STACK_VERSION;
}

unsigned int DlnaCommon::SetDeviceName(const char *pcNewName)
{
    DlnaStateType ulInitMode = DlnaStateType::DLNA_STATE_BUTT;

    if (dlnaCommonState_ != DlnaStateType::DLNA_STATE_ACTIVE) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][:%{public}d]-: Stack Is not Active.\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    if (pcNewName == DLNA_NULL_PTR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][:%{public}d]-: Invalid Parameter has been passed\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    DLNA_FREE(commonDeviceName_);
    commonDeviceName_ = strdup(pcNewName);
    if (commonDeviceName_ == DLNA_NULL_PTR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][:%{public}d]-: Memory Allocation Failed in strdup \r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    /* If DMR is Init, re-register the device and reset the DMR states */
    if (dlnaDmrHdl_->ulDeviceInit) {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][%{public}d]unRegister dmr root device", __FUNCTION__, __LINE__);
        (void)UpnpUnRegisterRootDevice(dlnaDmrHdl_->dev);
        DlnaDmr::getInstance().HandleDataFree();

        DLNA_LOCK(&(dlnaDmrHdl_->dmrAVTVariableMutex));
        dlnaDmrHdl_->ulDeviceInit = DLNA_FALSE;
        DLNA_UNLOCK(&(dlnaDmrHdl_->dmrAVTVariableMutex));

        if (DlnaDmr::getInstance().Init(commonMacAddress_) != DLNA_RET_SUCCESS) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][:%{public}d]-:DMR Init failure\r\n", __FUNCTION__, __LINE__);
            Destroy();
            return DLNA_RET_COMM_INIT_FAIL;
        }

        DLNA_SET_DMR_INIT(&ulInitMode);
    }

    (void)sleep(1);
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[%{public}s][%{public}d]rerun stack", __FUNCTION__, __LINE__);
    (void)Run(ulInitMode);

    return DLNA_RET_SUCCESS;
}


int DlnaCommon::FindServiceAndAction(UpnpActionRequest *request, DmrServiceTable *pstServiceTable,
    char **pcServiceType, DmrServiceAction **pcAction)
{
    unsigned int ulServiceIDLoop;
    unsigned int ulActionIdLoop;

    /* initial output parameter */
    *pcServiceType = (char *)DLNA_NULL_PTR;
    *pcAction = (DmrServiceAction *)DLNA_NULL_PTR;

    if (request == DLNA_NULL_PTR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Invalid Parameter has been "
            "passed\r\n",
            __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    for (ulServiceIDLoop = 0; pstServiceTable[ulServiceIDLoop].pszServiceID != DLNA_NULL_PTR; ulServiceIDLoop++) {
        DmrServiceTable *ptmpServiceTable = &pstServiceTable[ulServiceIDLoop];
        if (strcmp(ptmpServiceTable->pszServiceID, UpnpString_get_String(UpnpActionRequest_get_ServiceID(request)))) {
            continue;
        }
        *pcServiceType = ptmpServiceTable->pszServiceType;

        for (ulActionIdLoop = 0; ptmpServiceTable->pServerActions[ulActionIdLoop].pszActionName; ulActionIdLoop++) {
            DmrServiceAction *pTmpActionTable = &ptmpServiceTable->pServerActions[ulActionIdLoop];
            if (!strcmp(pTmpActionTable->pszActionName,
                UpnpString_get_String(UpnpActionRequest_get_ActionName(request)))) {
                *pcAction = pTmpActionTable;
                return DLNA_RET_SUCCESS;
            }
        }

        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: The action (%{public}s)recevied "
            "is not supported by the service with id-%{public}s"
            "\r\n",
            __FUNCTION__, __LINE__, UpnpString_get_String(UpnpActionRequest_get_ActionName(request)),
            UpnpString_get_String(UpnpActionRequest_get_ServiceID(request)));
        return DLNA_RET_FAILURE;
    }

    return DLNA_RET_FAILURE;
}

char *DlnaCommon::ConnectionManagerServiceCollectProtoInfo(DlnaLocalDeviceType enDeviceType)
{
    if (enDeviceType == DlnaLocalDeviceType::DLNA_LOCAL_DEVICE_TYPE_DMR) {
        return dmrMimeStr_;
    }
    return "";
}

int DlnaCommon::HttpReceiveMsg(int s32Socket, char *pchMsgBuf, unsigned int *u32MsgLen)
{
    fd_set tReadFd;
    struct timeval tTimeOut;
    int u32ActReadLen;
    int iRet;

    FD_ZERO(&tReadFd);
    FD_SET(s32Socket, &tReadFd);
    tTimeOut.tv_sec = 3; // the 3s
    tTimeOut.tv_usec = 0;
    iRet = select(s32Socket + 1, &tReadFd, 0, 0, &tTimeOut);
    if (iRet < 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "http Client Read Message Failed!Socket Not Ready.\n");
        return -1;
    } else if (iRet == 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "HttpReceiveMsg timeout after 5s \n");
        return -1;
    }
    u32ActReadLen = recv(s32Socket, pchMsgBuf, *u32MsgLen, MSG_NOSIGNAL);
    if (u32ActReadLen <= 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "http can't read from Server u32ActReadLen : %{public}d\n", u32ActReadLen);
        return -1;
    }

    // set terminated string
    if (u32ActReadLen >= HTTPREQUESTLEN) {
        pchMsgBuf[HTTPREQUESTLEN - 1] = '\0';
    } else {
        pchMsgBuf[u32ActReadLen - 1] = '\0';
    }

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "recv len : %{public}d\r\n", u32ActReadLen);
    *u32MsgLen = u32ActReadLen;
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "pchMsgBuf : %{public}s,len : %{public}d\r\n", pchMsgBuf, u32ActReadLen);
    return DLNA_RET_SUCCESS;
}

int DlnaCommon::HTTPSendMsgCommom(int s32Socket, char *pchMsgBuf, unsigned int u32MsgLen)
{
    fd_set tWriteFd;
    struct timeval tTimeOut;
    int u32ActSend;

    if (pchMsgBuf == 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "HTTP Client Send Message Failed!Message Buffer is Null.\n");
        return -1;
    }
    FD_ZERO(&tWriteFd);
    FD_SET(s32Socket, &tWriteFd);
    tTimeOut.tv_sec = 3; // the 3s
    tTimeOut.tv_usec = 0;
    if (select(s32Socket + 1, 0, &tWriteFd, 0, &tTimeOut) <= 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "HTTP Client Send Message Failed!Socket Not Ready.\n");
        return DLNA_RET_FAILURE;
    }
    u32ActSend = send(s32Socket, pchMsgBuf, u32MsgLen, 0);
    if (u32ActSend < 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "send error\n");
        return DLNA_RET_FAILURE;
    }
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "http Client Send Message :\n%{public}s\n,u32ActSend  = %{public}d\n", pchMsgBuf, u32ActSend);
    return DLNA_RET_SUCCESS;
}

int DlnaCommon::InitURI(char **pFlagPtr)
{
    if (memset_s(commonURI_, URILEN, 0, URILEN) == EOK && strlen(*pFlagPtr) < sizeof(commonURI_)) {
        if (strncpy_s(commonURI_, sizeof(commonURI_), *pFlagPtr, (strlen(*pFlagPtr) + 1)) != EOK) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[DLNA][%{public}s][:%{public}d]:strncpy_s failed\r\n", __FUNCTION__, __LINE__);
            return DLNA_RET_FAILURE;
        }
    } else {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "define url len < data len!!\r\n");
        return DLNA_RET_FAILURE;
    }
    return DLNA_RET_SUCCESS;
}

int DlnaCommon::GetIpaddrAndPort(const char *pURL)
{
    const char *pHttpHead = "http://";
    char *pFlagPtr = NULL;
    char *pEndPtr = NULL;
    char pHttpPort[HTTPPORTLEN];

    pFlagPtr = (char *)strstr(pURL, pHttpHead);
    if (pFlagPtr == DLNA_NULL_PTR) {
        return DLNA_RET_FAILURE;
    }

    pFlagPtr += strlen(pHttpHead);
    pEndPtr = strchr(pFlagPtr, ':');
    if (pEndPtr == NULL) {
        pEndPtr = strchr(pFlagPtr, '/');
        if (pEndPtr == NULL) {
            return DLNA_RET_FAILURE;
        }
        if (memset_s(commonHttpIpAddr_, HTTPIPADDRLEN, 0, HTTPIPADDRLEN) != EOK) {
            return DLNA_RET_FAILURE;
        }
        if (strncpy_s(commonHttpIpAddr_, HTTPIPADDRLEN, pFlagPtr, strlen(pFlagPtr)) != EOK) {
            return DLNA_RET_FAILURE;
        }
        commonHttpPort_ = 80; // the port
    } else {
        if (memset_s(commonHttpIpAddr_, HTTPIPADDRLEN, 0, HTTPIPADDRLEN) != EOK) {
            return DLNA_RET_FAILURE;
        }
        if (strncpy_s(commonHttpIpAddr_, HTTPIPADDRLEN, pFlagPtr, strlen(pFlagPtr)) != EOK) {
            return DLNA_RET_FAILURE;
        }
        pFlagPtr = pEndPtr + 1;
        pEndPtr = strchr(pFlagPtr, '/');
        if (pEndPtr == NULL) {
            return DLNA_RET_FAILURE;
        }
        if (memset_s(pHttpPort, HTTPPORTLEN, 0, HTTPPORTLEN) != EOK) {
            return DLNA_RET_FAILURE;
        }
        if (strncpy_s(pHttpPort, sizeof(pHttpPort), pFlagPtr, strlen(pFlagPtr)) != EOK) {
            return DLNA_RET_FAILURE;
        }
        if (sscanf_s(pHttpPort, "%5u", &commonHttpPort_) != 1) {
            commonHttpPort_ = 80; // the port
        }
    }

    return InitURI(&pEndPtr);
}

int DlnaCommon::SafetyInitHttpData(char *httpRequest, int httpRequestLen, char *httpMsgBuffer, int httpMsgBufferLen)
{
    if (memset_s(httpRequest, httpRequestLen, 0, httpRequestLen) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[DLNA][%{public}s][:%{public}d] : memset_s failed\r\n", __FUNCTION__, __LINE__);
        goto close_socket;
    }
    if (memset_s(httpMsgBuffer, httpMsgBufferLen, 0, httpMsgBufferLen) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[DLNA][%{public}s][:%{public}d] : memset_s failed\r\n", __FUNCTION__, __LINE__);
        goto close_socket;
    }
    if (snprintf_s(httpRequest, httpRequestLen, httpRequestLen - 1, "HEAD %{public}s HTTP/1.1\r\n", commonURI_) < EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[DLNA][%{public}s][:%{public}d] : snprintf_s failed\r\n", __FUNCTION__, __LINE__);
        goto close_socket;
    }
    if (snprintf_s(httpMsgBuffer, httpMsgBufferLen, httpMsgBufferLen - 1,
        "Host:%{public}s:%u\r\n", commonHttpIpAddr_, commonHttpPort_) < EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[DLNA][%{public}s][:%{public}d] : snprintf_s failed\r\n", __FUNCTION__, __LINE__);
        goto close_socket;
    }
    if (strncat_s(httpRequest, httpRequestLen, httpMsgBuffer, strlen(httpMsgBuffer)) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[DLNA][%{public}s][:%{public}d] : strncat_s failed\r\n", __FUNCTION__, __LINE__);
        goto close_socket;
    }
    if (snprintf_s(httpMsgBuffer, httpMsgBufferLen, httpMsgBufferLen - 1, "Connection: Keep-Alive\r\n") < EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[DLNA][%{public}s][:%{public}d] : snprintf_s failed\r\n", __FUNCTION__, __LINE__);
        goto close_socket;
    }
    if (strncat_s(httpRequest, httpRequestLen, httpMsgBuffer, strlen(httpMsgBuffer)) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[DLNA][%{public}s][:%{public}d] : strncat_s failed\r\n", __FUNCTION__, __LINE__);
        goto close_socket;
    }
    if (snprintf_s(httpMsgBuffer, httpMsgBufferLen, httpMsgBufferLen - 1, "Accept-Encoding: gzip\r\n\r\n") < EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[DLNA][%{public}s][:%{public}d] : snprintf_s failed\r\n", __FUNCTION__, __LINE__);
        goto close_socket;
    }
    if (strncat_s(httpRequest, httpRequestLen, httpMsgBuffer, strlen(httpMsgBuffer)) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[DLNA][%{public}s][:%{public}d] : strncat_s failed\r\n", __FUNCTION__, __LINE__);
        goto close_socket;
    }
    return DLNA_RET_SUCCESS;
close_socket:
    return DLNA_RET_FAILURE;
}

int DlnaCommon::AddrInit(int *s32Socket, struct sockaddr_in *addr)
{
    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[DLNA][%{public}s][:%{public}d] : http head URI : %{public}s\r\n", __FUNCTION__, __LINE__, commonURI_);
    if (memset_s(addr, sizeof(struct sockaddr_in), 0, sizeof(struct sockaddr_in)) != EOK) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[DLNA][%{public}s][:%{public}d]:memset_s failed\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
    inet_aton(commonHttpIpAddr_, (struct in_addr*)&(addr->sin_addr).s_addr);
    addr->sin_port = htons(commonHttpPort_);
    addr->sin_family = AF_INET;

    /* parse compelete */
    *s32Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (*s32Socket < 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "http socket error!!\r\n");
        return DLNA_RET_FAILURE;
    }
    return DLNA_RET_SUCCESS;
}

int DlnaCommon::CreateConnect(int *s32Socket, struct sockaddr_in *addr)
{
    unsigned long long ul = 1;
    unsigned int retryCnt = 0;
    const unsigned int tryTimes = 5;
    if (ioctl(*s32Socket, FIONBIO, &ul) == -1) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "http socket error!!\r\n");
        return DLNA_RET_FAILURE;
    }

    if (connect(*s32Socket, (struct sockaddr *)(addr), sizeof(struct sockaddr_in)) < 0) {
        while (retryCnt++ < tryTimes) {
            if (connect(*s32Socket, (struct sockaddr *)(addr), sizeof(struct sockaddr_in)) == 0) {
                break;
            }
            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "http connect error on times : %{public}d !!\r\n", retryCnt);
        }
        if (retryCnt >= tryTimes) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "http connect error!!\r\n");
            return DLNA_RET_FAILURE;
        }
    }
    return DLNA_RET_SUCCESS;
}

int DlnaCommon::GetUniString(char *uniUND, unsigned int uniUNDLen)
{
    unsigned int indexUND = 0;
    unsigned int indexIp = 0;
    char *pSrvIpAddr = NULL;
    size_t ipLen;

    pSrvIpAddr = UpnpGetServerIpAddress();
    if (pSrvIpAddr == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]: UpnpGetServerIpAddress fail, IP Address is NULL");
        return -1;
    }

    ipLen = strlen(pSrvIpAddr);

    while (indexIp < ipLen && indexUND < uniUNDLen) {
        if (pSrvIpAddr[indexIp] >= '0' && pSrvIpAddr[indexIp] <= '9') {
            uniUND[indexUND++] = pSrvIpAddr[indexIp];
        }
        indexIp++;
    }
    // fill by random data
    int fillBits = uniUNDLen - indexUND;
    if (fillBits > 0) {
        unsigned long fillData = 1;
        while (fillBits--) {
            fillData *= DECIMAL;
        }
        srand((unsigned int)time(NULL));
        fillData = fillData / DECIMAL + rand() % ((fillData - 1) - fillData / DECIMAL);
        if (snprintf_s(uniUND + indexUND, uniUNDLen - indexUND + 1, uniUNDLen - indexUND, "%lu", fillData) < EOK) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[DLNA][%{public}s][:%{public}d] : snprintf_s failed\r\n", __FUNCTION__, __LINE__);
            return DLNA_RET_FAILURE;
        }
    }

    return 0;
}

void *DlnaCommon::Realloc(void *src, unsigned int oldSize, unsigned int newSize)
{
    void *dst = NULL;

    if (newSize <= oldSize) {
        return src;
    }

    dst = malloc(newSize);
    if (dst != NULL) {
        if (memset_s(dst, newSize, 0, newSize) != EOK) {
            DLNA_FREE(dst);
            return NULL;
        }
    }

    if (src != NULL) {
        if (dst != NULL) {
            if (memcpy_s(dst, newSize, src, oldSize) != EOK) {
                DLNA_FREE(dst);
                return NULL;
            }
        }
    }

    return dst;
}

DlnaCommon::DlnaDmrDevice *DlnaCommon::GetDmrHdl()
{
    return dlnaDmrHdl_;
}

const char* DlnaCommon::GetIpBySockaddrStorage(const struct sockaddr_storage *sockAddr, char *tmpIp, int tmpIpSize)
{
    if (sockAddr == NULL) {
        return NULL;
    }

#if DLNA_ENABLE_IPV6
    const char *ret = inet_ntop(AF_INET6,
        &(((struct sockaddr_in*)(struct sockaddr*)sockAddr)->sin_addr), tmpIp, tmpIpSize);
    return ret;
#else
    const char *ret = inet_ntop(AF_INET,
        &(((struct sockaddr_in*)(struct sockaddr*)sockAddr)->sin_addr), tmpIp, tmpIpSize);
    return ret;
#endif
}

}
}
}
}