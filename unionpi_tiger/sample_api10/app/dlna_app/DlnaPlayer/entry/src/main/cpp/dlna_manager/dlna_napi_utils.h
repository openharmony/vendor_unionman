//
// Created on 2024/8/9.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef DlnaPlayer_dlna_napi_utils_H
#define DlnaPlayer_dlna_napi_utils_H

#define DLNA_NAPI_GET_JS_OBJ_WITH_ZERO_ARGS(env, info, status, thisVar)                                              \
    do {                                                                                                               \
        void *data;                                                                                                    \
        status = napi_get_cb_info(env, info, nullptr, nullptr, &(thisVar), &data);                                     \
    } while (0)

#endif //DlnaPlayer_dlna_napi_utils_H
