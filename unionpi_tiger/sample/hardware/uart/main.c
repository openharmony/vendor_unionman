/*
 * Minimal BPF JIT image disassembler
 *
 * Disassembles BPF JIT compiler emitted opcodes back to asm insn's for
 * debugging or verification purposes.
 *
 * To get the disassembly of the JIT code, do the following:
 *
 *  1) `echo 2 > /proc/sys/net/core/bpf_jit_enable`
 *  2) Load a BPF filter (e.g. `tcpdump -p -n -s 0 -i eth1 host 192.168.20.0/24`)
 *  3) Run e.g. `bpf_jit_disasm -o` to read out the last JIT code
 *
 * Copyright 2013 Daniel Borkmann <borkmann@redhat.com>
 * Licensed under the GNU General Public License, version 2.0 (GPLv2)
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "serial_uart.h"

#define TB_KEY0 "tb_uart_check"

int uartDevInit(int fd, int uartBaud);

static int fd;

#define UART_485_NAME "/dev/ttyACM1"
#define UART_TTL_NAME "/dev/ttyS4"
#define UART_EXT_TTL_NAME "/dev/ttyACM0"

static int *__input_event_check_task(void)
{
    int ret;
    char buf[20] = {0};
    char *uart_dev = "/dev/ttyS4";

    char buf_total[128] = {0};
    int recive_len = 0;

    while (1) {

        //一次读取串口会出现读不满的情况 这里并没有做处理  fixme
        ret = read(fd, buf, 10);

        printf("read buf = %s ret = %d\n", buf, ret);

        if (strlen(buf) > 4) {
        }
        memset(buf, 0, sizeof(buf));
    }

    return 0;
}

int main(int argc, char **argv)
{
    int ret = -1;
    char buf[20] = {0};

    //三种串口接线方式对应三钟设备
    char *uart_dev = UART_TTL_NAME; // UART_485_NAME UART_EXT_TTL_NAME

    char buf_total[128] = {0};
    int recive_len = 0;

    fd = open(uart_dev, O_RDWR);
    if (fd < 0) {
        printf("open file error");
        close(fd);
        return 0;
    }

    ret = uartDevInit(fd, 115200);
    if (0 != ret) {

        printf("uartDevInit error");
        close(fd);
        return -1;
    }

    char *send_data = "tb_uart_check";
    int send_len = 0;
    char endChar[1];
    endChar[0] = 10;
    printf("sizeof(send_data) = %d sizeof(TB_KEY0) = %d\n", sizeof(send_data), sizeof(send_data));
    int need_send_len = strlen(send_data);

    pthread_t pid_t;
    pthread_create(&pid_t, NULL, (void *)__input_event_check_task, 0);

    while (1) {
        ret = write(fd, send_data + send_len, need_send_len);
        send_len += ret;
        // printf("11 ret = %d  send_len = %d\n", ret, send_len);
        if (send_len >= need_send_len) {
            write(fd, endChar, 1); //输入换行符
            send_len = 0;
            sleep(1);
            continue;
        }

        continue;
    }

    // printf("22 ret = %d  send_len = %d", ret, send_len);

    close(fd);
    // system("echo tb_uart_check > /dev/ttyAMA1");

    return 0;
}
