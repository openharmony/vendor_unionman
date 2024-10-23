/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: dlna web server
 * Author: 
 * Create: 2023-10-20
 */

#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
//#include "dlna_log.h"
#include <hilog/log.h>
#include "upnptools.h"
#include "securec.h"
#include "dmr_device_desc.h"
#include "dlna_web_server.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

using namespace std;

//DEFINE_DLNA_LABEL("Dlna_dmr-webserver");
#define DLNA_LOG_TAG "Dlna_dmr-webserver"

const static char *SERVICE_CONTENT_TYPE = "text/xml; charset=\"utf-8\"";

ithread_mutex_t *DlnaWebServer::GetMetaDataMutex()
{
    return &metaDataMutex_;
}

bool DlnaWebServer::ReadDataCheck(const UpnpWebFileHandle pvHdl, const char *cBuf)
{
    return (pvHdl == DLNA_NULL_PTR) || (cBuf == DLNA_NULL_PTR);
}

bool DlnaWebServer::SeekSetCheck(off64_t iOffset, const DlnaWebHdl *pstWebHdl)
{
    return (iOffset < DLNA_NULL) || (iOffset > pstWebHdl->FileInfo.stFileMem.iContentLen);
}

bool DlnaWebServer::SeekCurCheck(off64_t iOffset, const DlnaWebHdl *pstWebHdl)
{
    return ((pstWebHdl->FileInfo.stFileMem.ipos + iOffset) < DLNA_NULL) ||
                ((pstWebHdl->FileInfo.stFileMem.ipos + iOffset) > pstWebHdl->FileInfo.stFileMem.iContentLen);
}


bool DlnaWebServer::GetMemItemInfoForSpecInit(DlnaEntryNode *pstEntry, struct stat *st, UpnpFileInfo *pstFileInfo)
{
    DLNA_LOCK(GetMetaDataMutex());
    if (pstEntry == DLNA_NULL_PTR || pstEntry->fullpath == DLNA_NULL_PTR) {
        DLNA_UNLOCK(GetMetaDataMutex());
        return false;
    }

    if (access(pstEntry->fullpath, R_OK) == -1) {
        if (errno == EACCES) {
            UpnpFileInfo_set_IsReadable(pstFileInfo, DLNA_FALSE);
        } else {
            DLNA_UNLOCK(GetMetaDataMutex());
            return false;
        }
    }
    DLNA_UNLOCK(GetMetaDataMutex());
    return true;
}


int DlnaWebServer::GetMemItemInfo(UpnpFileInfo *pstFileInfo, const char *contentType)
{
    UpnpFileInfo_set_ContentType(pstFileInfo, ixmlCloneDOMString(contentType));
    if (UpnpFileInfo_get_ContentType(pstFileInfo) == DLNA_NULL_PTR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: ixmlCloneDOMString failed! \r\n", __FUNCTION__, __LINE__);
        return DLNA_RET_FAILURE;
    }
    return DLNA_RET_SUCCESS;
}

int DlnaWebServer::GetItemInfo(const char *pcFileName, UpnpFileInfo *pstFileInfo,
    const void *cookie, const void **requestCookie)
{
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-\n",__FUNCTION__, __LINE__);
    int iretVal = DLNA_RET_FAILURE;
    DLNA_UNUSED_VAR(cookie);
    DLNA_UNUSED_VAR(requestCookie);
    if ((pcFileName == DLNA_NULL_PTR) || (pstFileInfo == DLNA_NULL_PTR)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Invalid Paremeters (NULL) has "
            "been passed pcFileName=%p, pstFileInfo=%p\r\n",
            __FUNCTION__, __LINE__, pcFileName, pstFileInfo);
        return iretVal;
    }

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:DlnaHttpGetItemInfo invoked with "
        "pcFileName:%{public}s pstFileInfo:%p \r\n ",
        __FUNCTION__, __LINE__, pcFileName, pstFileInfo);

    UpnpFileInfo_set_LastModified(pstFileInfo, DLNA_NULL);
    UpnpFileInfo_set_IsDirectory(pstFileInfo, DLNA_FALSE);
    UpnpFileInfo_set_IsReadable(pstFileInfo, DLNA_TRUE);

    if (strcmp(pcFileName, CONTROL_DEVICE_SERVICE_LOCATION) == DLNA_NULL) {
        UpnpFileInfo_set_FileLength(pstFileInfo, CONTROL_DEVICE_SERVICE_DESCRIPTION_LEN);
        iretVal = GetMemItemInfo(pstFileInfo, SERVICE_CONTENT_TYPE);
    } else if (strcmp(pcFileName, CONNECTION_MANAGER_SERVICE_LOCATION) == DLNA_NULL) {
        UpnpFileInfo_set_FileLength(pstFileInfo, CONNECTION_MANAGER_SERVICE_DESCRIPTION_LEN);
        iretVal = GetMemItemInfo(pstFileInfo, SERVICE_CONTENT_TYPE);
    } else if (strcmp(pcFileName, AVTRANSPORT_LOCATION) == DLNA_NULL) {
        UpnpFileInfo_set_FileLength(pstFileInfo, AVTRANSPORT_DESCRIPTION_LEN);
        iretVal = GetMemItemInfo(pstFileInfo, SERVICE_CONTENT_TYPE);
    } else if (strcmp(pcFileName, RENDERING_CONTROL_SERVICE_LOCATION) == DLNA_NULL) {
        UpnpFileInfo_set_FileLength(pstFileInfo, RENDERING_CONTROL_SERVICE_DESCRIPTION_LEN);
        iretVal = GetMemItemInfo(pstFileInfo, SERVICE_CONTENT_TYPE);
    } else {
        OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:%{public}s is not a valid service description File \r\n",
            __FUNCTION__, __LINE__, pcFileName);
        iretVal = DLNA_RET_FAILURE;
    }
    return iretVal;
}

UpnpWebFileHandle DlnaWebServer::Open(const char *pcFileName, enum UpnpOpenFileMode enMode,
	const void *cookie, const void *requestCookie)
{
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-\n",__FUNCTION__, __LINE__);
	DlnaEntryNode *pstEntry = NULL;
	DlnaWebHdl *pstWebHdl = NULL;
	unsigned int ulRetVal;
	DLNA_UNUSED_VAR(cookie);
	DLNA_UNUSED_VAR(requestCookie);
	if ((pcFileName == DLNA_NULL_PTR) || (enMode != UPNP_READ)) {
		OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Invalid Parameters has been passed pcFileName=%p, enMode=%{public}d \r\n",
			__FUNCTION__, __LINE__, pcFileName, enMode);
		return DLNA_NULL_PTR;
	}

	OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Open invoked with pcFileName:%{public}s enMode:%{public}d \r\n",
		__FUNCTION__, __LINE__, pcFileName, enMode);
	/* Allocate the required memory for handle */
	pstWebHdl = (DlnaWebHdl *)malloc(sizeof(DlnaWebHdl));
	if (pstWebHdl == DLNA_NULL_PTR) {
		OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  " [CastEngineDlna][%{public}s][:%{public}d]-: malloc Failed!!! \r\n", __FUNCTION__, __LINE__);
		return DLNA_NULL_PTR;
	}

	if (memset_s(pstWebHdl, sizeof(DlnaWebHdl), DLNA_NULL, sizeof(DlnaWebHdl)) != EOK) {
		OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  " [CastEngineDlna][%{public}s][:%{public}d]-: memset_s Failed !!! \r\n", __FUNCTION__, __LINE__);
		DLNA_FREE(pstWebHdl);
		return DLNA_NULL_PTR;
	}

	if (strcmp(pcFileName, CONTROL_DEVICE_SERVICE_LOCATION) == DLNA_NULL) {
		ulRetVal = FillFileInfo(pstWebHdl, CONTROL_DEVICE_SERVICE_LOCATION, CONTROL_DEVICE_SERVICE_DESCRIPTION, CONTROL_DEVICE_SERVICE_DESCRIPTION_LEN);
	} else if (strcmp(pcFileName, CONNECTION_MANAGER_SERVICE_LOCATION) == DLNA_NULL) {
		ulRetVal = FillFileInfo(pstWebHdl, CONNECTION_MANAGER_SERVICE_LOCATION, CONNECTION_MANAGER_SERVICE_DESCRIPTION, CONNECTION_MANAGER_SERVICE_DESCRIPTION_LEN);
	} else if (strcmp(pcFileName, AVTRANSPORT_LOCATION) == DLNA_NULL) {
		ulRetVal = FillFileInfo(pstWebHdl, AVTRANSPORT_LOCATION, AVTRANSPORT_DESCRIPTION, AVTRANSPORT_DESCRIPTION_LEN);
	} else if (strcmp(pcFileName, RENDERING_CONTROL_SERVICE_LOCATION) == DLNA_NULL) {
		ulRetVal = FillFileInfo(pstWebHdl, RENDERING_CONTROL_SERVICE_LOCATION, RENDERING_CONTROL_SERVICE_DESCRIPTION, RENDERING_CONTROL_SERVICE_DESCRIPTION_LEN);
	} else {
		ulRetVal = DLNA_RET_FAILURE;
	}

	if (ulRetVal != DLNA_RET_SUCCESS) {
		OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  " [CastEngineDlna][%{public}s][:%{public}d]-: Fail To Fill the file information! \r\n", __FUNCTION__, __LINE__);
		DLNA_FREE(pstWebHdl);
		return DLNA_NULL_PTR;
	}

	return (UpnpWebFileHandle)pstWebHdl;
}

int DlnaWebServer::Read(UpnpWebFileHandle pvHdl, char *buf, size_t bufLen, const void *cookie, const void *requestCookie)
{
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-\n",__FUNCTION__, __LINE__);
	DlnaWebHdl *pstWebHdl = NULL;
	int iLen;
	DLNA_UNUSED_VAR(cookie);
	DLNA_UNUSED_VAR(requestCookie);
	if (ReadDataCheck(pvHdl, buf)) {
		OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Invalid Parameters has"
			"been passed pvHdl=%p, buf=%p\r\n",
			__FUNCTION__, __LINE__, pvHdl, buf);
		return -1;
	}

	OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Read invoked with pvHdl:%p buf:%p bufLen:%lu \r\n",
		__FUNCTION__, __LINE__, pvHdl, buf, (unsigned long)bufLen);

	pstWebHdl = (DlnaWebHdl *)pvHdl;

	switch (pstWebHdl->iFileStorageType) {
		case static_cast<int>(DlnaFileStorageType::DLNA_FILE_FS):
			return read(pstWebHdl->FileInfo.stFileFS.iFileHdl, buf, bufLen);
		case static_cast<int>(DlnaFileStorageType::DLNA_FILE_MEMORY):
			iLen = (int)DLNA_MIN((int)bufLen,
				(pstWebHdl->FileInfo.stFileMem.iContentLen - pstWebHdl->FileInfo.stFileMem.ipos));
			if (memcpy_s(buf, bufLen, (pstWebHdl->FileInfo.stFileMem.pcContent + pstWebHdl->FileInfo.stFileMem.ipos), iLen) != EOK) {
				OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:memcpy_s failed "
					" \r\n",
					__FUNCTION__, __LINE__);
				return -1;
			}

			pstWebHdl->FileInfo.stFileMem.ipos += iLen;
			return iLen;
		default:
			OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Invalid Storage type %{public}d!!!"
				"\r\n ",
				__FUNCTION__, __LINE__, pstWebHdl->iFileStorageType);
			return -1;
	}
}

int DlnaWebServer::Write(UpnpWebFileHandle pvHdl, char *cBuf, size_t ulBufLen,
    const void *cookie, const void *requestCookie)
{
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-\n",__FUNCTION__, __LINE__);
    DLNA_UNUSED_VAR(pvHdl);
    DLNA_UNUSED_VAR(cBuf);
    DLNA_UNUSED_VAR(ulBufLen);
    DLNA_UNUSED_VAR(cookie);
    DLNA_UNUSED_VAR(requestCookie);

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Write is currently not "
        "supported!!!! \r\n",
        __FUNCTION__, __LINE__);

    return DLNA_NULL;
}

int DlnaWebServer::Seek(UpnpWebFileHandle pvHdl, off_t iOffset, int iOrigin, const void *cookie, const void *requestCookie)
{
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-\n",__FUNCTION__, __LINE__);
	DlnaWebHdl *pstWebHdl = NULL;
	DLNA_UNUSED_VAR(cookie);
	DLNA_UNUSED_VAR(requestCookie);
	if (pvHdl == DLNA_NULL_PTR) {
		OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:UpnpWeb File Handle has been passed as NULL\r\n", __FUNCTION__, __LINE__);
		return -1;
	}

	OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Seek invoked with pvHdl:%p "
		"iOffset:%lld iOrigin:%{public}d \r\n", __FUNCTION__, __LINE__, pvHdl, (long long)iOffset, iOrigin);

	pstWebHdl = (DlnaWebHdl *)pvHdl;

	switch (pstWebHdl->iFileStorageType) {
		case static_cast<int>(DlnaFileStorageType::DLNA_FILE_FS):
			return lseek64(pstWebHdl->FileInfo.stFileFS.iFileHdl, iOffset, iOrigin);

		case static_cast<int>(DlnaFileStorageType::DLNA_FILE_MEMORY):
			switch (iOrigin) {
				case SEEK_SET:
					if (SeekSetCheck(iOffset, pstWebHdl)) {
						OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Seek failure for file:%{public}s: Error:%{public}s\n",
							__FUNCTION__, __LINE__, pstWebHdl->pcFullPath, strerror(EINVAL));
						return -1;
					}

					pstWebHdl->FileInfo.stFileMem.ipos = iOffset;
					return pstWebHdl->FileInfo.stFileMem.ipos;
				case SEEK_CUR:
					if (SeekCurCheck(iOffset, pstWebHdl)) {
						OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Seek failure for file:%{public}s Error:%{public}s\n",
							__FUNCTION__, __LINE__, pstWebHdl->pcFullPath, strerror(EINVAL));
						return -1;
					}
					pstWebHdl->FileInfo.stFileMem.ipos += iOffset;
					return pstWebHdl->FileInfo.stFileMem.ipos;
				case SEEK_END:
					if (iOffset != DLNA_NULL) {
						OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Cannot seek to end of File:%{public}s Error:%{public}s\n",
							__FUNCTION__, __LINE__, pstWebHdl->pcFullPath, strerror(EINVAL));

						return -1;
					}
					pstWebHdl->FileInfo.stFileMem.ipos = pstWebHdl->FileInfo.stFileMem.iContentLen;
					return pstWebHdl->FileInfo.stFileMem.ipos;
				default:
					OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:Seek Mode %{public}d is invalid one\r\n", __FUNCTION__, __LINE__, iOrigin);
					return -1;
			}

		default:
			OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Invalid Storage type\r\n ", __FUNCTION__, __LINE__);
			return -1;
	}
}

int DlnaWebServer::Close(UpnpWebFileHandle pvHdl, const void *cookie, const void *requestCookie)
{
	OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-\n",__FUNCTION__, __LINE__);
    DlnaWebHdl *pstWebHdl = NULL;
    DLNA_UNUSED_VAR(cookie);
    DLNA_UNUSED_VAR(requestCookie);
    if (pvHdl == DLNA_NULL_PTR) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-:UpnpWeb File Handle has"
            "been passed as NULL\r\n",
            __FUNCTION__, __LINE__);
        return -1;
    }

    OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Close has invoked "
        "with pvHdl:%p \r\n",
        __FUNCTION__, __LINE__, pvHdl);

    pstWebHdl = (DlnaWebHdl *)pvHdl;

    switch (pstWebHdl->iFileStorageType) {
        case static_cast<int>(DlnaFileStorageType::DLNA_FILE_FS):
            (void)close(pstWebHdl->FileInfo.stFileFS.iFileHdl);
            break;

        case static_cast<int>(DlnaFileStorageType::DLNA_FILE_MEMORY):
            DLNA_FREE(pstWebHdl->FileInfo.stFileMem.pcContent);
            break;

        default:
            OH_LOG_Print(LOG_APP, LOG_INFO, 0xFF00, DLNA_LOG_TAG,  "[CastEngineDlna][%{public}s][:%{public}d]-: Invalid Storage type %{public}d!"
                "\r\n ",
                __FUNCTION__, __LINE__, pstWebHdl->iFileStorageType);
            break;
    }

    DLNA_FREE(pstWebHdl->pcFullPath);

    DLNA_FREE(pstWebHdl);

    return DLNA_RET_SUCCESS;
}

unsigned int DlnaWebServer::FillFileInfo(DlnaWebHdl *pstHdl, const char *pcFilePath,
    const char *pcDesc, const size_t ulLen)
{
    pstHdl->pcFullPath = strdup(pcFilePath);
    if (pstHdl->pcFullPath == DLNA_NULL_PTR) {
        return DLNA_RET_FAILURE;
    }

    pstHdl->FileInfo.stFileMem.pcContent = strdup(pcDesc);
    if (pstHdl->FileInfo.stFileMem.pcContent == DLNA_NULL_PTR) {
        DLNA_FREE(pstHdl->pcFullPath);
        return DLNA_RET_FAILURE;
    }

    pstHdl->iFileStorageType = static_cast<int>(DlnaFileStorageType::DLNA_FILE_MEMORY);
    pstHdl->FileInfo.stFileMem.ipos = DLNA_NULL;
    pstHdl->FileInfo.stFileMem.iContentLen = ulLen;

    return DLNA_RET_SUCCESS;
}

}
}
}
}