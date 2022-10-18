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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "serial_uart.h"

static int fd;

// 从串口读的线程
void *_serial_input_task(void)
{
    int i = 0;
    int ret = -1;
    int buf = 0;
    int recv[4] = {0};

    printf("Gesture Sensor Ready!\n");
    while (1) {
        for (i = 0; i < FRAME_LEN; i++) {
            ret = read(fd, &buf, 1);
            if (ret == -1) {
                printf("read err\n");
                exit(0);
            }
            recv[i] = buf;
        }
        ret = data_proce(recv);
        if (ret == -1) {
            printf("data process error\n");
            exit(0);
        }
    }
}

int main(int argc, char **argv)
{
    char *uart_dev = UART_TTL_NAME;
    int ret = -1;

    fd = open(uart_dev, O_RDWR);
    if (fd == -1) {
        printf("open file error\n");
        return ERR;
    }
    ret = uart_init(fd, 9600L);
    if (ret == -1) {
        printf("uart init error\n");
        return ERR;
    }

    // 创建线程  一直执行读操作
    pthread_t pid_t;
    pthread_create(&pid_t, NULL, (void *)_serial_input_task, 0);

    while (1) {}
    close(fd);

    return 0;
}