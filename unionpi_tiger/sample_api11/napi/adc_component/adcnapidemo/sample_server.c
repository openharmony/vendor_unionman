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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>

#include "sample_server.h"
#include "securec.h"

int main(int argc, char *argv[]) {
  struct sockaddr_un name;
  (void)memset_s(&name, sizeof(struct sockaddr_un), 0,
                 sizeof(struct sockaddr_un));
  int ret = 0;
  int connection_socket = 0;
  int data_socket = 0;

  /*
   * In case the program exited inadvertently on the last run,
   * remove the socket.
   */
  unlink(SOCKET_NAME);

  /* Create local socket. */
  connection_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
  if (connection_socket == -1) {
    perror("socket");
    return SOCKET_FAIL;
  }

  /* Bind socket to socket name. */
  name.sun_family = AF_UNIX;
  (void)strncpy_s(name.sun_path, sizeof(name.sun_path), SOCKET_NAME,
                  sizeof(name.sun_path) - 1);
  printf("server name.sun_path = %s\n", name.sun_path);

  ret = bind(connection_socket, (const struct sockaddr *)&name,
             sizeof(struct sockaddr_un));
  if (ret == -1) {
    perror("bind");
    return SOCKET_FAIL;
  }
  ret = listen(connection_socket, 20L);
  if (ret == -1) {
    perror("listen");
    return SOCKET_FAIL;
  }

  /* Wait for incoming connection. */
  for (;;) {
    data_socket = accept(connection_socket, NULL, NULL);
    if (data_socket == -1) {
      perror("accept");
      return SOCKET_FAIL;
    }

    ret = communication(data_socket);
    if (ret == -1) {
      perror("communication");
      return SOCKET_FAIL;
    }

    /* Close socket. */
    close(data_socket);
  }
  close(connection_socket);
  unlink(SOCKET_NAME);
  return SOCKET_OK;
}

// deal with client socket
int communication(int data_socket) {
  int ret = 0;
  char buffer[BUFFER_SIZE] = {0};

  /* Wait for next data packet. */
  ret = read(data_socket, buffer, BUFFER_SIZE);
  if (ret == -1) {
    perror("read");
    return SOCKET_FAIL;
  }
  printf("read path = %s\n", buffer);

  /* open buffer path file read and write permission. */
  ret = chmod(buffer, READ_AND_WRITE);
  if (ret == -1) {
    perror("chmod");
    return SOCKET_FAIL;
  }

  /* Send chmod result. */
  (void)sprintf_s(buffer, sizeof(buffer), "%d", ret);
  ret = write(data_socket, buffer, BUFFER_SIZE);
  if (ret == -1) {
    perror("write");
    return SOCKET_FAIL;
  }

  return SOCKET_OK;
}
