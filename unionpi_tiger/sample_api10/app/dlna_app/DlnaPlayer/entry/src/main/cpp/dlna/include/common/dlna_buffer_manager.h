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

#ifndef DLNA_BUFFER_MANAGER_H
#define DLNA_BUFFER_MANAGER_H

namespace OHOS {
namespace CastEngine {
namespace CastEngineService {
namespace CastSessionDlna {

class DlnaBufferManager{
private:
	#define DLNA_STR_BUFF_LIM 32768

	struct DlnaStrBuff{
	    char *pcStrBuf;
	    unsigned int ulCurSize;
	    unsigned int ulMaxSize;
	};

public:
	static void *AllocStrBuff(void);

	static void FreeStrBuff(void *pvStrBufHdl);

	static void StrBuffAppendStr(void *pvStrBufHdl, const char *pcStr);

	static const char *StrBuffGetBuff(const void *pvStrBufHdl);

	static void StrBuffAppendFormatStr(void *pvStrBufHdl, const char *format, ...);
};

}
}
}
}
#endif /* DLNA_BUFFER_MANAGER_H */
