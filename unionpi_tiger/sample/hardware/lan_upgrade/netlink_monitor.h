/*
 * Copyright (c) Huawei Device Co., Ltd. 2021-2023  All right reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef NETLINK_MOTOR_H
#define NETLINK_MOTOR_H
    
#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif

typedef enum {
    CONNECTION_ADDR_ETH,
    CONNECTION_ADDR_WLAN,
    CONNECTION_ADDR_BUTT
} CONNECTION_ADDR_TYPE_E;

typedef enum {
    NNL_EVENT_IP_ADDR_CHANGED,
} NNL_EVENT_E;

typedef void(*PFUN_NNL_EVENT_HANDLER)(NNL_EVENT_E event, void *param);

int32_t NetlinkMonitorStart(PFUN_NNL_EVENT_HANDLER handler);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

