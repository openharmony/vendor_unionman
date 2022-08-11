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

#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdint.h>

#include <net/if.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <linux/can.h>
#include <linux/can/raw.h>

static int s = -1;
static int running = 1;

enum {
    VERSION_OPTION = CHAR_MAX + 1,
    FILTER_OPTION,
};

static void print_usages(char *prog)
{
    (void)fprintf(stderr,
                  "|------------------------------------------------------------------------------------\n"
                  "|Usage: %s [<interface>] [options]\n"
                  "|------------------------------------------------------------------------------------\n"
                  "|Options:\n"
                  "| --family or -f=FAMILY         protocol family(def. PF_CAN = %d)\n"
                  "| --type or -t=TYPE             socket type, (see man 2 socket) (default SOCK_RAW = %d)\n"
                  "| --protocol or -p=PROTOCOL     CAN protocol (default CAN_RAW = %d)\n"
                  "| --filter=id:mask[:id:mask]...\n"
                  "|                               apply filter\n"
                  "| --help or -h                  this help\n"
                  "| -o <filename>                 output into filename\n"
                  "| -d                            daemonize\n"
                  "| --version                     print version information and exit\n"
                  "|------------------------------------------------------------------------------------\n",
                  prog, PF_CAN, SOCK_RAW, CAN_RAW);
}

static void sigterm(int signo)
{
    running = 0;
}

static struct can_filter *filter = NULL;
static int filter_count = 0;

int add_filter(u_int32_t id, u_int32_t mask)
{
    filter = realloc(filter, sizeof(struct can_filter) * (filter_count + 1));
    if (!filter) {
        return -1;
    }

    filter[filter_count].can_id = id;
    filter[filter_count].can_mask = mask;
    filter_count++;

    printf("id: 0x%08x mask: 0x%08x\n", id, mask);
    return 0;
}

#define BUF_SIZ (255)

static struct option long_options[] = {
    {"help", no_argument, 0, 'h'},
    {"family", required_argument, 0, 'f'},
    {"protocol", required_argument, 0, 'p'},
    {"type", required_argument, 0, 't'},
    {"filter", required_argument, 0, FILTER_OPTION},
    {"version", no_argument, 0, VERSION_OPTION},
    {0, 0, 0, 0},
};

static int opt_daemon = 0;
static int family = PF_CAN, type = SOCK_RAW, proto = CAN_RAW;
static char *optout = NULL;

static void add_filters(char *ptr)
{
    uint32_t id, mask;
    char *p = ptr;

    while (1) {
        id = strtoul(p, NULL, 0);
        p = strchr(ptr, ':');
        if (!p) {
            (void)fprintf(stderr, "filter must be applied in the form id:mask[:id:mask]...\n");
            exit(1);
        }
        p++;
        mask = strtoul(p, NULL, 0);
        p = strchr(ptr, ':');
        add_filter(id, mask);
        if (!p) {
            break;
        }
        p++;
    }
}

static void parse_opt(int argc, char **argv)
{
    char *ptr;
    int opt;

    while ((opt = getopt_long(argc, argv, "hf:t:p:o:d", long_options, NULL)) != -1) {
        if (opt == 'h') {
            print_usages(basename(argv[0]));
            exit(0);
        } else if (opt == 'f') {
            family = strtoul(optarg, NULL, 0);
        } else if (opt == 'd') {
            opt_daemon++;
        } else if (opt == 'o') {
            optout = optarg;
        } else if (opt == 't') {
            type = strtoul(optarg, NULL, 0);
        } else if (opt == 'p') {
            proto = strtoul(optarg, NULL, 0);
        } else if (opt == VERSION_OPTION) {
            printf("candump %s\n", VERSION);
            exit(0);
        } else if (opt == FILTER_OPTION) {
            ptr = optarg;
            add_filters(ptr);
        } else {
            (void)fprintf(stderr, "Unknown option %d\n", opt);
        }
    }
}

static int socket_bind(char *intf, int so_family, int so_type, int so_proto)
{
    struct sockaddr_can addr;
    struct ifreq ifr;

    printf("interface : [%s], family : [%d], type : [%d], proto : [%d]\n", intf, so_family, so_type, so_proto);

    if ((s = socket(so_family, so_type, so_proto)) < 0) {
        (void)fprintf(stderr, "socket: %s\n", strerror(errno));
        return 1;
    }

    addr.can_family = so_family;

    (void)strncpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), intf, sizeof(ifr.ifr_name));

    if (ioctl(s, SIOCGIFINDEX, &ifr)) {
        (void)fprintf(stderr, "ioctl: %s\n", strerror(errno));
        return 1;
    }

    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        (void)fprintf(stderr, "bind: %s\n", strerror(errno));
        return 1;
    }

    (void)fprintf(stdout, "socket bind success!\n");
    return 0;
}

static int read_frame(char *buf)
{
    struct can_frame frame;
    int nbytes, i;
    int n = 0;

    if ((nbytes = read(s, &frame, sizeof(struct can_frame))) < 0) {
        (void)fprintf(stderr, "read: %s\n", strerror(errno));
        return 1;
    } else {
        if (frame.can_id & CAN_EFF_FLAG) {
            n = snprintf_s(buf, BUF_SIZ, "<0x%08x> ", frame.can_id & UM_MASK_OF_EFF);
        } else {
            n = snprintf_s(buf, BUF_SIZ, "<0x%03x> ", frame.can_id & UM_MASK_OF_SFF);
        }

        n += snprintf_s(buf + n, BUF_SIZ - n, "[%d] ", frame.can_dlc);
        for (i = 0; i < frame.can_dlc; i++) {
            n += snprintf_s(buf + n, BUF_SIZ - n, "%02x ", frame.data[i]);
        }
        if (frame.can_id & CAN_RTR_FLAG) {
            n += snprintf_s(buf + n, BUF_SIZ - n, "remote request");
        }
    }
    return 0;
}

static void flush2file(FILE *fp, char *buf)
{
    int err;
    FILE *f = fp;

    (void)fprintf(f, "%s\n", buf);
    do {
        err = fflush(f);
        if (err == -1 && errno == EPIPE) {
            err = -EPIPE;
            (void)fclose(f);
            f = fopen(optout, "a");
            if (!f) {
                exit(EXIT_FAILURE);
            }
        }
    } while (err == -EPIPE);
}

int main(int argc, char **argv)
{
    FILE *out = stdout;
    char *interface = "can0";
    char buf[BUF_SIZ];

    (void)signal(SIGPIPE, SIG_IGN);

    parse_opt(argc, argv);

    if (optind != argc) {
        interface = argv[optind];
    }

    if (socket_bind(interface, family, type, proto) != 0) {
        printf("socket bind failed!\n");
        return 1;
    }
    if (filter) {
        if (setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, filter, sizeof(struct can_filter) * filter_count) != 0) {
            (void)fprintf(stderr, "setsockopt: %s\n", strerror(errno));
            exit(1);
        }
    }

    if (opt_daemon) {
        daemon(opt_daemon, 0);
    } else {
        signal(SIGTERM, sigterm);
        signal(SIGHUP, sigterm);
    }

    if (optout) {
        out = fopen(optout, "a");
        if (!out) {
            (void)fprintf(stderr, "fopen: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    while (running) {
        if (read_frame(buf) != 0) {
            (void)fprintf(stderr, "read frame failed!\n");
            return 1;
        }
        flush2file(out, buf);
    }

    exit(EXIT_SUCCESS);
}
