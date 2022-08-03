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

#include <getopt.h>
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

#include <net/if.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <linux/can.h>
#include <linux/can/raw.h>

static void usages(char *prog)
{
    (void)fprintf(stderr,
                  "|---------------------------------------------------------------------------------\n"
                  "|Usage: %s [<interface>] [Options] <can-msg>\n"
                  "|---------------------------------------------------------------------------------\n"
                  "|Options:\n"
                  "| --identifier or -i=ID      CAN Identifier (default = 1)\n"
                  "| --rtr or -r                send remote request\n"
                  "| --help or -h               this help\n"
                  "| --version                  print version information and exit\n"
                  "| --extended or -e           send extended frame\n"
                  "| --type or -t =TYPE         Socket type, see man 2 socket (default SOCK_RAW = %d)\n"
                  "| --family or -f = FAMILY    Protocol family (default PF_CAN = %d)\n"
                  "| --protocol or -p = PROTO   CAN protocol (default CAN_RAW = %d)\n"
                  "|  -l                        send message infinite times\n"
                  "| --loop=COUNT               send message COUNT times\n"
                  "| --verbose or -v            be verbose\n"
                  "|---------------------------------------------------------------------------------\n",
                  prog, SOCK_RAW, PF_CAN, CAN_RAW);
}

enum {
    VERSION_OPTION = CHAR_MAX + 1,
};

static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"identifier", required_argument, 0, 'i'},
    {"rtr", no_argument, 0, 'r'},
    {"extended", no_argument, 0, 'e'},
    {"family", required_argument, 0, 'f'},
    {"protocol", required_argument, 0, 'p'},
    {"type", required_argument, 0, 't'},
    {"version", no_argument, 0, VERSION_OPTION},
    {"verbose", no_argument, 0, 'v'},
    {"loop", required_argument, 0, 'l'},
    {0, 0, 0, 0},
};
static int um_family = PF_CAN, um_type = SOCK_RAW, um_proto = CAN_RAW;
static int um_loopcount = 1, um_infinite = 0;
static int um_verbose = 0;
static char *um_interface;
static int um_rtr = 0, um_extended = 0;
static int fd_socket;
static struct can_frame um_frame = {
    .can_id = 1,
};

static void parse_opt(int argc, char **argv)
{
    int opt;

    while ((opt = getopt_long(argc, argv, "hf:t:p:vi:lre", long_options, NULL)) != -1) {
        if (opt == 'f') {
            um_family = strtoul(optarg, NULL, 0);
        } else if (opt == 'h') {
            usages(basename(argv[0]));
            exit(0);
        } else if (opt == 'p') {
            um_proto = strtoul(optarg, NULL, 0);
        } else if (opt == 'l') {
            if (optarg) {
                um_loopcount = strtoul(optarg, NULL, 0);
            } else {
                um_infinite = 1;
            }
        } else if (opt == 'v') {
            um_verbose = 1;
        } else if (opt == 't') {
            um_type = strtoul(optarg, NULL, 0);
        } else if (opt == 'i') {
            um_frame.can_id = strtoul(optarg, NULL, 0);
        } else if (opt == 'r') {
            um_rtr = 1;
        } else if (opt == VERSION_OPTION) {
            printf("cansend %s\n", VERSION);
            exit(0);
        } else if (opt == 'e') {
            um_extended = 1;
        } else {
            fprintf(stderr, "Unknown option %d\n", opt);
        }
    }

    if (optind == argc) {
        usages(basename(argv[0]));
        exit(0);
    }

    if (argv[optind] == NULL) {
        fprintf(stderr, "No Interface supplied\n");
        exit(-1);
    }
}

static int socket_bind(char *intf, int so_family, int so_type, int so_proto)
{
    struct ifreq ifr;
    struct sockaddr_can addr;

    (void)fprintf(stdout, "interface : [%s], type : [%d], family : [%d], proto : [%d]\n",
        intf, so_type, so_family, so_proto);
    fd_socket = socket(so_family, so_type, so_proto);
    if (fd_socket < 0) {
        (void)fprintf(stderr, "socket: %s\n", strerror(errno));
        return 1;
    }

    (void)strcpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), intf);
    if (ioctl(fd_socket, SIOCGIFINDEX, &ifr)) {
        (void)fprintf(stderr, "ioctl: %s\n", strerror(errno));
        return 1;
    }

    addr.can_ifindex = ifr.ifr_ifindex;
    addr.can_family = so_family;
    if (bind(fd_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        (void)fprintf(stderr, "bind: %s\n", strerror(errno));
        return 1;
    }
    return 0;
}

int main(int argc, char **argv)
{
    int ret, i, um_lenth = 0;

    parse_opt(argc, argv);
    um_interface = argv[optind];
    if (socket_bind(um_interface, um_family, um_type, um_proto) != 0) {
        (void)fprintf(stderr, "socket bind failed!\n");
        return 1;
    }
    for (i = optind + 1; i < argc; i++) {
        um_frame.data[um_lenth] = strtoul(argv[i], NULL, 0);
        um_lenth++;
        if (um_lenth == 8L) {
            break;
        }
    }
    um_frame.can_dlc = um_lenth;

    if (um_verbose) {
        (void)fprintf(stdout, "id: %d ", um_frame.can_id);
        (void)fprintf(stdout, "dlc: %d\n", um_frame.can_dlc);
        for (i = 0; i < um_frame.can_dlc; i++) {
            (void)fprintf(stdout, "0x%02x ", um_frame.data[i]);
        }
        (void)fprintf(stdout, "\n");
    }

    if (um_extended) {
        um_frame.can_id &= UM_MASK_OF_EFF;
        um_frame.can_id |= UM_FLAG_OF_EFF;
    } else {
        um_frame.can_id &= UM_MASK_OF_SFF;
    }

    if (um_rtr) {
        um_frame.can_id |= UM_FLAG_OF_RTR;
    }

    while (um_loopcount-- || um_infinite) {
        ret = write(fd_socket, &um_frame, sizeof(um_frame));
        if (ret == -1) {
            (void)fprintf(stderr, "write: %s\n", strerror(errno));
            break;
        }
    }

    close(fd_socket);
    fd_socket = -1L;
    return 0;
}
