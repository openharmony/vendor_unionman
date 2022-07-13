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

void print_usage(char *prg)
{
    (void)fprintf(stderr,
                  "Usage: %s <can-interface> [<can-interface-out>] [Options]\n"
                  "\n"
                  "Send all messages received on <can-interface> to <can-interface-out>\n"
                  "If <can-interface-out> is omitted, then <can_interface> is used for sending\n"
                  "\n"
                  "Options:\n"
                  " -f, --family=FAMILY   Protocol family (default PF_CAN = %d)\n"
                  " -t, --type=TYPE       Socket type, see man 2 socket (default SOCK_RAW = %d)\n"
                  " -p, --protocol=PROTO  CAN protocol (default CAN_RAW = %d)\n"
                  " -v, --verbose         be verbose\n"
                  " -h, --help            this help\n"
                  "     --version         print version information and exit\n",
                  prg, PF_CAN, SOCK_RAW, CAN_RAW);
}

void sigterm(int signo)
{
    running = 0;
}

int main(int argc, char **argv)
{
    struct can_frame frame;
    struct ifreq ifr[2];
    struct sockaddr_can addr[2];
    char *intf_name[2];
    int family = PF_CAN, type = SOCK_RAW, proto = CAN_RAW;
    int nbytes, i, out;
    int opt;
    int s[2];
    int verbose = 0;

    (void)signal(SIGTERM, sigterm);
    (void)signal(SIGHUP, sigterm);
    (void)signal(SIGINT, sigterm);

    struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"family", required_argument, 0, 'f'},
        {"protocol", required_argument, 0, 'p'},
        {"type", required_argument, 0, 't'},
        {"version", no_argument, 0, VERSION_OPTION},
        {"verbose", no_argument, 0, 'v'},
        {0, 0, 0, 0},
    };

    while ((opt = getopt_long(argc, argv, "hf:t:p:v", long_options, NULL)) != -1) {
        switch (opt) {
            case 'f':
                family = atoi(optarg);
                break;

            case 't':
                type = atoi(optarg);
                break;

            case 'p':
                proto = atoi(optarg);
                break;

            case 'v':
                verbose = 1;
                break;

            case 'h':
                print_usage(basename(argv[0]));
                exit(0);

            case VERSION_OPTION:
                printf("canecho %s\n", VERSION);
                exit(0);

            default:
                fprintf(stderr, "Unknown option %d\n", opt);
                break;
        }
    }

    if (optind == argc) {
        print_usage(basename(argv[0]));
        exit(0);
    }

    intf_name[0] = argv[optind++];
    if (optind == argc) {
        intf_name[1] = intf_name[0];
    } else {
        intf_name[1] = argv[optind];
    }

    printf("interface-in = %s, interface-out = %s, family = %d, type = %d, proto = %d\n", intf_name[0], intf_name[1],
           family, type, proto);

    if (intf_name[0] == intf_name[1]) {
        out = 0;
    } else {
        out = 1;
    }

    for (i = 0; i <= out; i++) {
        if ((s[i] = socket(family, type, proto)) < 0) {
            perror("socket");
            return 1;
        }

        addr[i].can_family = family;
        (void)strcpy_s(ifr[i].ifr_name, sizeof(ifr[i].ifr_name), intf_name[i]);
        ioctl(s[i], SIOCGIFINDEX, &ifr[i]);
        addr[i].can_ifindex = ifr[i].ifr_ifindex;

        if (bind(s[i], (struct sockaddr *)&addr[i], sizeof(addr)) < 0) {
            perror("bind");
            return 1;
        }
    }

    while (running) {
        if ((nbytes = read(s[0], &frame, sizeof(frame))) < 0) {
            perror("read");
            return 1;
        }
        if (verbose) {
            printf("%04x: ", frame.can_id);
            if (frame.can_id & CAN_RTR_FLAG) {
                printf("remote request");
            } else {
                printf("[%d]", frame.can_dlc);
                for (i = 0; i < frame.can_dlc; i++) {
                    printf(" %02x", frame.data[i]);
                }
            }
            printf("\n");
        }
        frame.can_id++;
        write(s[out], &frame, sizeof(frame));
    }

    return 0;
}
