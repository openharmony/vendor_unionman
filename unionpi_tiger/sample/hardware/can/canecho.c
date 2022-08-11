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

#include "can_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <libgen.h>
#include <getopt.h>
#include <limits.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

static int running = 1;

enum {
    VERSION_OPTION = CHAR_MAX + 1,
};

void usages(char *prog)
{
    (void)fprintf(stderr,
                  "|-----------------------------------------------------------------------------------------\n"
                  "|Usage: %s <interface> [<interface-out>] [Options]\n"
                  "|-----------------------------------------------------------------------------------------\n"
                  "|Send all messages received on <interface> to <interface-out>\n"
                  "|If <interface-out> is omitted, then <interface> is used for sending\n"
                  "|-----------------------------------------------------------------------------------------\n"
                  "|Options:\n"
                  "| --verbose or -v                   be verbose\n"
                  "| --version                         print version information and exit\n"
                  "| --protocol=PROTO or -p            CAN protocol (default CAN_RAW = %d)\n"
                  "| --family or -f =FAMILY            Protocol family (default PF_CAN = %d)\n"
                  "| --help or -h                      this help\n"
                  "| --type or -t=TYPE                 Socket type, see man 2 socket (default SOCK_RAW = %d)\n"
                  "|-----------------------------------------------------------------------------------------\n",
                  prog, CAN_RAW, PF_CAN, SOCK_RAW);
}

void sigterm(int signo)
{
    running = 0;
}

static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"family", required_argument, 0, 'f'},
    {"protocol", required_argument, 0, 'p'},
    {"type", required_argument, 0, 't'},
    {"version", no_argument, 0, VERSION_OPTION},
    {"verbose", no_argument, 0, 'v'},
    {0, 0, 0, 0},
};
static int um_family = PF_CAN, um_type = SOCK_RAW, um_proto = CAN_RAW;
static int um_verbose = 0;
static int um_out;
static int um_s[2];

static void parse_opt(int argc, char **argv)
{
    int opt;

    while ((opt = getopt_long(argc, argv, "hf:t:p:v", long_options, NULL)) != -1) {
        if (opt == 'f') {
            um_family = atoi(optarg);
        } else if (opt == 'h') {
            usages(basename(argv[0]));
            exit(0);
        } else if (opt == 't') {
            um_type = atoi(optarg);
        } else if (opt == 'p') {
            um_proto = atoi(optarg);
        } else if (opt == VERSION_OPTION) {
            printf("canecho %s\n", VERSION);
            exit(0);
        } else if (opt == 'v') {
            um_verbose = 1;
        } else {
            fprintf(stderr, "Unknown option %d\n", opt);
        }
    }
}

static int socket_bind(char *intf_name[2], int so_family, int so_type, int so_proto)
{
    int i;
    struct ifreq um_ifr[2];
    struct sockaddr_can um_addr[2];

    (void)fprintf(stdout, "in-interface : [%s], family : [%d], out-interface : [%s], type : [%d], proto : [%d]\n",
        intf_name[0], so_family, intf_name[1], so_type, so_proto);

    if (intf_name[0] == intf_name[1]) {
        um_out = 0;
    } else {
        um_out = 1;
    }

    for (i = 0; i <= um_out; i++) {
        if ((um_s[i] = socket(so_family, so_type, so_proto)) < 0) {
            (void)fprintf(stderr, "socket: %s\n", strerror(errno));
            return 1;
        }

        (void)strcpy_s(um_ifr[i].ifr_name, sizeof(um_ifr[i].ifr_name), intf_name[i]);
        (void)ioctl(um_s[i], SIOCGIFINDEX, &um_ifr[i]);

        um_addr[i].can_family = so_family;
        um_addr[i].can_ifindex = um_ifr[i].ifr_ifindex;
        if (bind(um_s[i], (struct sockaddr *)&um_addr[i], sizeof(um_addr)) < 0) {
            (void)fprintf(stderr, "bind: %s\n", strerror(errno));
            return 1;
        }
    }
    return 0;
}

static void print_frame(struct can_frame *frame)
{
    int i;

    printf("%04x: ", frame->can_id);
    if (frame->can_id & CAN_RTR_FLAG) {
        printf("remote request");
    } else {
        printf("[%d]", frame->can_dlc);
        for (i = 0; i < frame->can_dlc; i++) {
            printf(" %02x", frame->data[i]);
        }
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    struct can_frame frame;
    char *intf_name[2];
    int nbytes;

    (void)signal(SIGTERM, sigterm);
    (void)signal(SIGHUP, sigterm);
    (void)signal(SIGINT, sigterm);

    parse_opt(argc, argv);
    if (optind == argc) {
        usages(basename(argv[0]));
        exit(0);
    }

    intf_name[0] = argv[optind++];
    if (optind == argc) {
        intf_name[1] = intf_name[0];
    } else {
        intf_name[1] = argv[optind];
    }

    if (socket_bind(intf_name, um_family, um_type, um_proto) != 0) {
        printf("socket bind failed!\n");
        return 1;
    }
    while (running) {
        if ((nbytes = read(um_s[0], &frame, sizeof(frame))) < 0) {
            (void)fprintf(stderr, "read: %s\n", strerror(errno));
            return 1;
        }
        if (um_verbose) {
            print_frame(&frame);
        }
        frame.can_id++;
        write(um_s[um_out], &frame, sizeof(frame));
    }

    return 0;
}
