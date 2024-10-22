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

#ifndef DLNA_WEB_SERVER_H
#define DLNA_WEB_SERVER_H

#include <iostream>
#include <string>
#include <pthread.h>
#include "dlna_common.h"

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

class DlnaWebServer{
private:
    enum class DlnaFileStorageType {
        DLNA_FILE_FS = 0,
        DLNA_FILE_MEMORY = 1,
        DLNA_FILE_BUTT = DLNA_ENUM_END
    };

    struct DlnaFileFS{
        int iFileHdl;
        DlnaEntryNode *pstEntry;
    };

    struct DlnaFileMem{
        long long ipos;
        long long iContentLen;
        char *pcContent;
    };

    struct DlnaWebHdl{
        int iFileStorageType;
        char *pcFullPath;
        union _FileInfo {
            DlnaFileFS stFileFS;
            DlnaFileMem stFileMem;
        } FileInfo;
    };
	
	ithread_mutex_t metaDataMutex_;

public :
    ithread_mutex_t *GetMetaDataMutex();

    static bool ReadDataCheck(const UpnpWebFileHandle pvHdl, const char *cBuf);

    static bool SeekSetCheck(off64_t iOffset, const DlnaWebHdl *pstWebHdl);

    static bool SeekCurCheck(off64_t iOffset, const DlnaWebHdl *pstWebHdl);

    bool GetMemItemInfoForSpecInit(DlnaEntryNode *pstEntry, struct stat *st, UpnpFileInfo *pstFileInfo);

    static int GetMemItemInfo(UpnpFileInfo *pstFileInfo, const char *contentType);

    static int GetItemInfo(const char *pcFileName, UpnpFileInfo *pstFileInfo,
        const void *cookie, const void **requestCookie);

    static unsigned int FillFileInfo(DlnaWebHdl *pstHdl, const char *pcFilePath,
        const char *pcDesc, const size_t ulLen);

    static UpnpWebFileHandle Open(const char *pcFileName, enum UpnpOpenFileMode enMode,
        const void *cookie, const void *requestCookie);

    static int Read(UpnpWebFileHandle pvHdl, char *buf, size_t bufLen, const void *cookie, const void *requestCookie);

    static int Write(UpnpWebFileHandle pvHdl, char *cBuf, size_t ulBufLen,
        const void *cookie, const void *requestCookie);

    static int Seek(UpnpWebFileHandle pvHdl, off_t iOffset, int iOrigin,
        const void *cookie, const void *requestCookie);

    static int Close(UpnpWebFileHandle pvHdl, const void *cookie, const void *requestCookie);

};

}
}
}
}
#endif /* DLNA_WEB_SERVER_H */
