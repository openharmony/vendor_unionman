/*
 * Copyright (c) 2022 Unionman Technology Co., Ltd.
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

#ifndef _SAMPLE_CLIENT_H
#define _SAMPLE_CLIENT_H

#define SOCKET_NAME "/data/storage/el2/base/haps/entry/files/myscoket.socket"
#define BUFFER_SIZE 128
#define SOCKET_OK 0
#define SOCKET_FAIL (-1)

#ifdef __cplusplus
extern "C" {
#endif

int open_permission(const char *path);

#ifdef __cplusplus
}
#endif

#endif // endif _SAMPLE_CLIENT_H