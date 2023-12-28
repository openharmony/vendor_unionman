/*
 * Copyright (c) 2023 Unionman Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PRODUCTCONFIG_H
#define PRODUCTCONFIG_H


#define VERSION_STR             "1.0.2"

#define HTTP_SERVER_PORT        (52000)

#define MULTICAST_LOCAL_PORT    (50012)

#define MULTICAST_PORT          (50002)

#define MULTICAST_IP_ADDRESS_V4    "224.100.100.100"

#define MULTICAST_IP_ADDRESS_V6    "ff15::100:100"

#define OFFLINE_TIMEOUT         (3 * 1000) // 毫秒

#define TCP_SERVER_PORT         (MULTICAST_LOCAL_PORT)

#endif // PRODUCTCONFIG_H
