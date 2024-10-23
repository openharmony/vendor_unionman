/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: dmr connection manager service
 * Author: 
 * Create: 2023-10-20
 */

//#include "dlna_log.h"
#include <hilog/log.h>
#include "dlna_types.h"
#include "dmr_connection_manager.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

//DEFINE_DLNA_LABEL("Cast-Dmr-Connection-Manager");
#define DLNA_LOG_TAG "Dlna_dmr-Connection-Manager"

DmrConnectionManager &DmrConnectionManager::getInstance(){
    static DmrConnectionManager manager{};
    return manager;
}

DlnaMimeType* DmrConnectionManager::DlnaGetDmrMimeTypeList()
{
    DlnaMimeType dmrMimeTypeList[] = {
		/* Video files */
		{ "avi",   UPNP_VIDEO, "http-get:*:video/avi:"},
		{ "wmv", UPNP_VIDEO, "http-get:*:video/x-ms-wmv:"},
		{ "mp4",  UPNP_VIDEO, "http-get:*:video/mp4:"},
		{ "ts",     UPNP_VIDEO, "http-get:*:video/mpeg2:"},
		{ "mkv",  UPNP_VIDEO, "http-get:*:video/mpeg:"},
		{ "rmvb", UPNP_VIDEO, "http-get:*:video/mpeg:"},
		{ "3gp",  UPNP_VIDEO,  "http-get:*:video/3gpp:"},

		/* Audio files */
		{ "mp3",  UPNP_AUDIO_MUSIC, "http-get:*:audio/mpeg:"},
		{ "wma", UPNP_AUDIO_MUSIC, "http-get:*:audio/x-ms-wma:"},
		{ "amr",  UPNP_AUDIO_MUSIC, "http-get:*:audio/amr:"},

		/* Images files */
		{ "bmp",  UPNP_PHOTO, "http-get:*:image/bmp:"},
		{ "gif",  UPNP_PHOTO, "http-get:*:image/gif:"},
		{ "jpeg", UPNP_PHOTO, "http-get:*:image/jpeg:"},
		{ "jpg",  UPNP_PHOTO, "http-get:*:image/jpeg:"},
		{ "jpe",  UPNP_PHOTO, "http-get:*:image/jpeg:"},
		{ "pcd",  UPNP_PHOTO, "http-get:*:image/x-ms-bmp:"},
		{ "qti",  UPNP_PHOTO, "http-get:*:image/x-quicktime:"},
		{ "qtf",  UPNP_PHOTO, "http-get:*:image/x-quicktime:"},
		{ "qtif", UPNP_PHOTO, "http-get:*:image/x-quicktime:"},
		{ "tif",  UPNP_PHOTO, "http-get:*:image/tiff:"},
		{ "tiff", UPNP_PHOTO, "http-get:*:image/tiff:"},
	};
    return (DlnaMimeType *)dmrMimeTypeList;
}

int DmrConnectionManager::GetProtocolInfo(DmrActionParam *pstActionEvent)
{
    char *pcRespPtr = (char *)DLNA_NULL_PTR;
    int iRetVal;
    IXML_Node *resultNode = (IXML_Node *)DLNA_NULL_PTR;

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]: Peer has invoked the GetProtocolInfo action\r\n");

    pcRespPtr = DlnaCommon::getInstance().ConnectionManagerServiceCollectProtoInfo(DlnaLocalDeviceType::DLNA_LOCAL_DEVICE_TYPE_DMR);

    IXML_Document *actionRequestDoc = UpnpActionRequest_get_ActionRequest(pstActionEvent->pstActionRequest);
    resultNode = ixmlNode_getFirstChild(&(actionRequestDoc->n));
    if (resultNode != DLNA_NULL_PTR) {
        if (ixmlNode_getFirstChild(resultNode) != DLNA_NULL_PTR) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]:getprotocoinfo query is with Argument\r\n");

            pstActionEvent->iErrorCode = DLNA_E_UPNP_402;
            pstActionEvent->pErrorStr = DLNA_E_UPNP_402_STRING;

            return DLNA_RET_FAILURE;
        }
    }

    iRetVal = DlnaCommon::getInstance().AddParamToActionResponse(pstActionEvent, (char *)CONNECTION_MANAGER_SERVICE_ARG_SOURCE, "", strlen(""));
    if (iRetVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Failed to add the CONNECTION_MANAGER_SERVICE_ARG_SOURCE param in response result"
            " for action request.\r\n",
            __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    iRetVal = DlnaCommon::getInstance().AddParamToActionResponse(pstActionEvent, CONNECTION_MANAGER_SERVICE_ARG_SINK, (const char *)pcRespPtr,
        strlen((const char *)pcRespPtr));
    if (iRetVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Failed to add the CONNECTION_MANAGER_SERVICE_ARG_SINK param in response result"
            " for action request.\r\n",
            __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: GetProtocolInfo action is successful\r\n", __FUNCTION__, __LINE__);

    return DLNA_RET_SUCCESS;
}


int DmrConnectionManager::GetCurrentConnectionIDs(DmrActionParam *pstActionEvent)
{
    int iRetVal;
    IXML_Node *resultNode = (IXML_Node *)DLNA_NULL_PTR;

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Peer has invoked the "
        "GetCurrentConnectionIDs action\r\n",
        __FUNCTION__, __LINE__);

    IXML_Document *actionRequestDoc = UpnpActionRequest_get_ActionRequest(pstActionEvent->pstActionRequest);
    resultNode = ixmlNode_getFirstChild(&(actionRequestDoc->n));
    if (resultNode != DLNA_NULL_PTR) {
        if (ixmlNode_getFirstChild(resultNode) != DLNA_NULL_PTR) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:GetCurrentConnectionIDs query is with Argument\r\n", __FUNCTION__, __LINE__);
            pstActionEvent->iErrorCode = DLNA_E_UPNP_402;
            pstActionEvent->pErrorStr = DLNA_E_UPNP_402_STRING;
            return DLNA_RET_FAILURE;
        }
    }

    iRetVal = DlnaCommon::getInstance().AddParamToActionResponse(pstActionEvent, CONNECTION_MANAGER_SERVICE_ARG_CONNECTION_IDS, CONNECTION_MANAGER_SERVICE_DEFAULT_CON_ID,
        strlen(CONNECTION_MANAGER_SERVICE_DEFAULT_CON_ID));
    if (iRetVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Failed to add the CONNECTION_MANAGER_SERVICE_ARG_CONNECTION_IDS param in response result"
            " for action request.\r\n",
            __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: GetCurrentConnectionIDs action is "
        "successful\r\n",
        __FUNCTION__, __LINE__);

    return DLNA_RET_SUCCESS;
}

int DmrConnectionManager::GetCurrentConnectionInfoInit(DmrActionParam *pstActionEvent)
{
    int iInstVal;
    int iRetVal;
    UpnpActionRequest *pstActionRequest = (UpnpActionRequest *)DLNA_NULL_PTR;
    pstActionRequest = pstActionEvent->pstActionRequest;

    iInstVal = DlnaCommon::getInstance().GetInt(pstActionRequest, CONNECTION_MANAGER_SERVICE_ARG_CONNECTION_ID);
    if (iInstVal == DLNA_NULL_INT32) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna]:Received Invalid Connection ID with incorrect datatype in the "
            "GetCurrentConnectionInfo action request. \r\n");

        pstActionEvent->iErrorCode = DLNA_E_UPNP_402;
        pstActionEvent->pErrorStr = DLNA_E_UPNP_402_STRING;

        return DLNA_RET_FAILURE;
    }

    if (!(IS_CONNECTION_ID_VALID(iInstVal)) || !(IS_CONNECTION_ID_SUPPORTED(iInstVal))) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Received Connection ID = %{public}d."
            " in %{public}s action request \r\n",
            __FUNCTION__, __LINE__, iInstVal,
            UpnpString_get_String(UpnpActionRequest_get_ActionName(pstActionRequest)));

        pstActionEvent->iErrorCode = DMR_CONNECTION_MANAGER_ERROR_706;
        pstActionEvent->pErrorStr = DMR_CONNECTION_MANAGER_ERROR_706_STRING;

        return DLNA_RET_FAILURE;
    }

    iRetVal = DlnaCommon::getInstance().AddParamToActionResponse(pstActionEvent, CONNECTION_MANAGER_SERVICE_ARG_RCS_ID, SERVICE_DEFAULT_ID,
        strlen(SERVICE_DEFAULT_ID));
    if (iRetVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Failed to add the CONNECTION_MANAGER_SERVICE_ARG_RCS_ID param in response result"
            " for %{public}s action request.\r\n",
            __FUNCTION__, __LINE__,
            UpnpString_get_String(UpnpActionRequest_get_ActionName(pstActionRequest)));
        return DLNA_RET_FAILURE;
    }

    iRetVal = DlnaCommon::getInstance().AddParamToActionResponse(pstActionEvent, CONNECTION_MANAGER_SERVICE_ARG_TRANSPORT_ID, SERVICE_DEFAULT_ID,
        strlen(SERVICE_DEFAULT_ID));
    if (iRetVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Failed to add the CONNECTION_MANAGER_SERVICE_ARG_TRANSPORT_ID param in response result"
            " for %{public}s action request.\r\n",
            __FUNCTION__, __LINE__,
            UpnpString_get_String(UpnpActionRequest_get_ActionName(pstActionRequest)));
        return DLNA_RET_FAILURE;
    }
    return DLNA_RET_SUCCESS;
}

int DmrConnectionManager::AddAllParamToActionResponse(DmrActionParam *pstActionEvent)
{
    int iRetVal;
    iRetVal = DlnaCommon::getInstance().AddParamToActionResponse(pstActionEvent, CONNECTION_MANAGER_SERVICE_ARG_PEER_CON_MANAGER, "", strlen(""));
    if (iRetVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Failed to add the CONNECTION_MANAGER_SERVICE_ARG_PEER_CON_MANAGER param in response result"
            " \r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    iRetVal = DlnaCommon::getInstance().AddParamToActionResponse(pstActionEvent, CONNECTION_MANAGER_SERVICE_ARG_PEER_CON_ID, CONNECTION_MANAGER_SERVICE_UNKNOW_ID,
        strlen(CONNECTION_MANAGER_SERVICE_UNKNOW_ID));
    if (iRetVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Failed to add the CONNECTION_MANAGER_SERVICE_ARG_PEER_CON_ID param in response result"
            ".\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    iRetVal = DlnaCommon::getInstance().AddParamToActionResponse(pstActionEvent, CONNECTION_MANAGER_SERVICE_ARG_DIRECTION, CONNECTION_MANAGER_SERVICE_INPUT,
        strlen(CONNECTION_MANAGER_SERVICE_INPUT));
    if (iRetVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Failed to add the CONNECTION_MANAGER_SERVICE_ARG_DIRECTION param in response result"
            ".\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    iRetVal = DlnaCommon::getInstance().AddParamToActionResponse(pstActionEvent, CONNECTION_MANAGER_SERVICE_ARG_STATUS, CONNECTION_MANAGER_SERVICE_STATUS_OK,
        strlen(CONNECTION_MANAGER_SERVICE_STATUS_OK));
    if (iRetVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Failed to add the CONNECTION_MANAGER_SERVICE_ARG_STATUS param in response result"
            " .\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
    return DLNA_RET_SUCCESS;
}

int DmrConnectionManager::GetCurrentConnectionInfo(DmrActionParam *pstActionEvent)
{
    int iRetVal;
    char *pcRespPtr = (char *)DLNA_NULL_PTR;
    IXML_Document *pstDoc = (IXML_Document *)DLNA_NULL_PTR;

    if (DmrConnectionManager::getInstance().GetCurrentConnectionInfoInit(pstActionEvent) != DLNA_RET_SUCCESS) {
        return DLNA_RET_FAILURE;
    }

    if (DlnaCommon::getInstance().GetDmrHdl()->pszTrackMetaData && DlnaCommon::getInstance().GetDmrHdl()->pszTrackMetaData[0] != '\0') {
        IXML_NodeList *pstEleLst = NULL;
        IXML_Node *pstNode = NULL;

        pstDoc = ixmlParseBuffer(DlnaCommon::getInstance().GetDmrHdl()->pszTrackMetaData);
        if (pstDoc != DLNA_NULL_PTR) {
            pstEleLst = ixmlDocument_getElementsByTagName(pstDoc, "res");
            if (pstEleLst != DLNA_NULL_PTR) {
                pstNode = ixmlNodeList_item(pstEleLst, 0L);
                pcRespPtr =
                    (char *)ixmlElement_getAttribute((IXML_Element *)pstNode, (const DOMString) "protocolInfo");
                ixmlNodeList_free(pstEleLst);
            }
        }
    }

    if (pcRespPtr != DLNA_NULL_PTR) {
        iRetVal = DlnaCommon::getInstance().AddParamToActionResponse(pstActionEvent, CONNECTION_MANAGER_SERVICE_ARG_PROT_INFO,
            (const char *)pcRespPtr, strlen((const char *)pcRespPtr));
    } else {
        iRetVal = DlnaCommon::getInstance().AddParamToActionResponse(pstActionEvent, CONNECTION_MANAGER_SERVICE_ARG_PROT_INFO, "", strlen(""));
    }

    if (pstDoc != DLNA_NULL_PTR) {
        ixmlDocument_free(pstDoc);
    }

    if (iRetVal != DLNA_RET_SUCCESS) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Failed to add the CONNECTION_MANAGER_SERVICE_ARG_PROT_INFO param in response result.\r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }

    iRetVal = DmrConnectionManager::getInstance().AddAllParamToActionResponse(pstActionEvent);

    return iRetVal;
}

DmrServiceAction *DmrConnectionManager::getActionFuncTable(){
    static DmrServiceAction dmrCmsActionFuncTable[] = {
        { CONNECTION_MANAGER_SERVICE_ACTION_PROT_INFO, DmrConnectionManager::GetProtocolInfo},
	    { CONNECTION_MANAGER_SERVICE_ACTION_CON_ID, DmrConnectionManager::GetCurrentConnectionIDs },
	    { CONNECTION_MANAGER_SERVICE_ACTION_CON_INFO, DmrConnectionManager::GetCurrentConnectionInfo },
	    { NULL, NULL }
    };

	return (DmrServiceAction *)dmrCmsActionFuncTable;
}

}
}
}
}