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

#undef LOG_TAG
#define LOG_TAG "adc_test"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "hilog/log.h"
#include "sample_client.h"
#include "securec.h"

// 打开对应路径的读写权限
int open_permission(const char *path) {
  if (path == NULL) {
    HILOG_ERROR(LOG_CORE, "path pointer: %{public}s\n", strerror(errno));
    return SOCKET_FAIL;
  }

  struct sockaddr_un addr;
  (void)memset_s(&addr, sizeof(struct sockaddr_un), 0,
                 sizeof(struct sockaddr_un));
  int ret = -1;
  int data_socket = 0;
  char buffer[BUFFER_SIZE] = {0};

  /* Create local socket. */
  data_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
  if (data_socket == -1) {
    HILOG_ERROR(LOG_CORE, "create socket failure: %{public}s\n",
                strerror(errno));
    return SOCKET_FAIL;
  }

  /* Connect socket to socket address */
  addr.sun_family = AF_UNIX;
  (void)sprintf_s(addr.sun_path, sizeof(addr.sun_path), "%s", SOCKET_NAME);
  HILOG_INFO(LOG_CORE, "client addr.sun_path: %{public}s\n", addr.sun_path);
  ret = connect(data_socket, (const struct sockaddr *)&addr,
                sizeof(struct sockaddr_un));
  if (ret == -1) {
    HILOG_ERROR(LOG_CORE, "connect failure: %{public}s\n", strerror(errno));
    return SOCKET_FAIL;
  }

  /* Send arguments. */
  ret = strcpy_s(buffer, sizeof(buffer), path);
  if (ret != 0) {
    HILOG_ERROR(LOG_CORE, "copy string failed\n");
    return SOCKET_FAIL;
  }

  ret = write(data_socket, buffer, strlen(buffer) + 1);
  if (ret == -1) {
    HILOG_ERROR(LOG_CORE, "write failure: %{public}s\n", strerror(errno));
    return SOCKET_FAIL;
  }

  /* Receive result. */
  ret = read(data_socket, buffer, BUFFER_SIZE);
  if (ret == -1) {
    HILOG_ERROR(LOG_CORE, "read failure: %{public}s\n", strerror(errno));
    return SOCKET_FAIL;
  }
  if (strcmp(buffer, "-1") == 0) {
    HILOG_ERROR(LOG_CORE, "chmod failure: %{public}s\n", strerror(errno));
    return SOCKET_FAIL;
  } else {
    HILOG_INFO(LOG_CORE, "chmod success: %{public}s\n", strerror(errno));
  }

  /* Close socket. */
  close(data_socket);

  return SOCKET_OK;
}