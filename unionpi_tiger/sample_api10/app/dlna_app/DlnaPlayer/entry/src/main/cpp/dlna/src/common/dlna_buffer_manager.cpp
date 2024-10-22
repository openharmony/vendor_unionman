/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Description: dlna buffer manager
 * Author: 
 * Create: 2023-10-20
 */

#include <stdarg.h>
//#include "dlna_log.h"
#include <hilog/log.h>
#include "dlna_common.h"
#include "securec.h"
#include "dlna_buffer_manager.h"


namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

//DEFINE_DLNA_LABEL("Dlna_dmr-buffer-manager");
#define DLNA_LOG_TAG "Dlna_dmr-buffer-manager"

    void *DlnaBufferManager::AllocStrBuff(void) {
        DlnaStrBuff *pstStrBuffer = (DlnaStrBuff *)malloc(sizeof(DlnaStrBuff));
        if (pstStrBuffer == DLNA_NULL_PTR) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                         "[CastEngineDlna][%{public}s][:%{public}d]-:Memory Allocation failure \r\n", __FUNCTION__,
                         __LINE__);
            return DLNA_NULL_PTR;
        }

        pstStrBuffer->ulCurSize = DLNA_NULL;
        pstStrBuffer->ulMaxSize = DLNA_STR_BUFF_LIM;
        pstStrBuffer->pcStrBuf = (char *)malloc(DLNA_STR_BUFF_LIM * sizeof(char));
        if (pstStrBuffer->pcStrBuf == DLNA_NULL_PTR) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                         "[CastEngineDlna][%{public}s][:%{public}d]-:Memory Allocation failure \r\n", __FUNCTION__,
                         __LINE__);
            DLNA_FREE(pstStrBuffer);
            return DLNA_NULL_PTR;
        }

        if (memset_s(pstStrBuffer->pcStrBuf, (int)pstStrBuffer->ulMaxSize, '\0', (int)pstStrBuffer->ulMaxSize) != EOK) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG, "[%{public}s][%{public}d]: memeset_s failed \r\n",
                         __FUNCTION__, __LINE__);
            DLNA_FREE(pstStrBuffer->pcStrBuf);
            DLNA_FREE(pstStrBuffer);
            return DLNA_NULL_PTR;
        }

        return (void *)pstStrBuffer;
    }

    void DlnaBufferManager::StrBuffAppendStr(void *pvStrBufHdl, const char *pcStr) {
        unsigned int ulCurLen;
        unsigned int ulInputStrLen;
        DlnaStrBuff *pstStrBuf = (DlnaStrBuff *)pvStrBufHdl;

        if ((pstStrBuf == DLNA_NULL_PTR) || (pcStr == DLNA_NULL_PTR) || (pstStrBuf->pcStrBuf == DLNA_NULL_PTR)) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                         "[CastEngineDlna][%{public}s][:%{public}d]-:Invalid Parameters has been "
                         "Passed \r\n",
                         __FUNCTION__, __LINE__);
            return;
        }

        ulInputStrLen = strlen(pcStr);

        ulCurLen = pstStrBuf->ulCurSize + ulInputStrLen;
        if (ulCurLen >= pstStrBuf->ulMaxSize) {
            char *pcTempBuf = (char *)DLNA_NULL_PTR;
            pstStrBuf->ulMaxSize = DLNA_MAX((ulCurLen + 1), (2 * pstStrBuf->ulMaxSize)); // 2x value
            if (pstStrBuf->ulMaxSize > 0) {
                pcTempBuf = (char *)DlnaCommon::getInstance().Realloc((void *)pstStrBuf->pcStrBuf, pstStrBuf->ulCurSize,
                                                                      pstStrBuf->ulMaxSize);
            }
            /* When Realloc fails, the input pointer to realloc would still be valid
            so, the normal processing of cleaning this should anyways happen */
            if (pcTempBuf == DLNA_NULL_PTR) {
                OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                             "[CastEngineDlna][%{public}s][:%{public}d]-:Realloc failure "
                             " \r\n",
                             __FUNCTION__, __LINE__);
                return;
            }

            pstStrBuf->pcStrBuf = pcTempBuf;
        }

        if (strncat_s(pstStrBuf->pcStrBuf, pstStrBuf->ulMaxSize, pcStr, (strlen(pcStr) + 1)) != EOK) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                         "[CastEngineDlna][%{public}s][:%{public}d]-:strncat_s failed "
                         " \r\n",
                         __FUNCTION__, __LINE__);
            return;
        }
        pstStrBuf->ulCurSize += ulInputStrLen;

        return;
    }

    void DlnaBufferManager::StrBuffAppendFormatStr(void *pvStrBufHdl, const char *format, ...) {
        char *pcTempStr = (char *)DLNA_NULL_PTR;
        int sTotalSizeWritten;
        DlnaStrBuff *pstStrBuf = (DlnaStrBuff *)pvStrBufHdl;
        va_list va;

        if ((pstStrBuf == DLNA_NULL_PTR) || (format == DLNA_NULL_PTR)) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                         "[CastEngineDlna][%{public}s][:%{public}d]-:Invalid (null) Parameters has "
                         "been Passed \r\n",
                         __FUNCTION__, __LINE__);
            return;
        }

        va_start(va, format);

        pcTempStr = (char *)malloc(DLNA_STR_BUFF_LIM);
        if (pcTempStr == DLNA_NULL_PTR) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                         "[CastEngineDlna][%{public}s][:%{public}d]-:Memory Allocation failure \r\n", __FUNCTION__,
                         __LINE__);
            va_end(va);
            return;
        }

        sTotalSizeWritten = vsnprintf_s(pcTempStr, DLNA_STR_BUFF_LIM, DLNA_STR_BUFF_LIM - 1, format, va);
        if (sTotalSizeWritten == -1) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                         "[CastEngineDlna][%{public}s][:%{public}d]-:vasprintf returning failure\r\n", __FUNCTION__,
                         __LINE__);
            DLNA_FREE(pcTempStr);
            va_end(va);
            return;
        }

        if (sTotalSizeWritten >= DLNA_STR_BUFF_LIM) {
            char *pcTempBuf =
                (char *)DlnaCommon::getInstance().Realloc(pcTempStr, DLNA_STR_BUFF_LIM, sTotalSizeWritten + 1);
            /* When Realloc fails, the input pointer to realloc would still be valid
            so, the normal processing of cleaning this should anyways happen */
            if (pcTempBuf == DLNA_NULL_PTR) {
                OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                             "[CastEngineDlna][%{public}s][:%{public}d]-:Realloc failure "
                             " \r\n",
                             __FUNCTION__, __LINE__);
                DLNA_FREE(pcTempStr);
                va_end(va);
                return;
            }

            pcTempStr = pcTempBuf;

            if (vsnprintf_s(pcTempStr, sTotalSizeWritten + 1, sTotalSizeWritten, format, va) == -1) {
                OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                             "[CastEngineDlna][%{public}s][:%{public}d]-:vsnprintf_s failed \r\n", __FUNCTION__,
                             __LINE__);
                DLNA_FREE(pcTempStr);
                va_end(va);
                return;
            }
        }

        StrBuffAppendStr(pstStrBuf, pcTempStr);
        DLNA_FREE(pcTempStr);
        va_end(va);

        return;
    }

    const char *DlnaBufferManager::StrBuffGetBuff(const void *pvStrBufHdl) {
        if (pvStrBufHdl == DLNA_NULL_PTR) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                         "[CastEngineDlna][%{public}s][:%{public}d]-:Invalid (null) Parameters has "
                         "been Passed \r\n",
                         __FUNCTION__, __LINE__);
            return (char *)DLNA_NULL_PTR;
        }

        return ((DlnaStrBuff *)pvStrBufHdl)->pcStrBuf;
    }

    void DlnaBufferManager::FreeStrBuff(void *pvStrBufHdl) {
        DlnaStrBuff *pstStrBuf = (DlnaStrBuff *)pvStrBufHdl;

        if (pstStrBuf == DLNA_NULL_PTR) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, 0xFF00, DLNA_LOG_TAG,
                         "[CastEngineDlna][%{public}s][:%{public}d]-:Invalid (null) Parameters has "
                         "been Passed \r\n",
                         __FUNCTION__, __LINE__);
            return;
        }
        DLNA_FREE(pstStrBuf->pcStrBuf);
        DLNA_FREE(pstStrBuf);

        return;
    }

}
}
}
}