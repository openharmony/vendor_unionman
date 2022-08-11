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
#include <limits.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
};

#define DEFAULT_CAN_ID (2)

void print_usages(char *prog)
{
    (void)fprintf(stderr,
                  "|-------------------------------------------------------------------------------\n"
                  "|Usage: %s [<interface>] [Options]\n"
                  "|-------------------------------------------------------------------------------\n"
                  "|Options:\n"
                  "| --receive or -r            work as receiver\n"
                  "| --extended or -e           send extended frames\n"
                  "| --loop=COUNT               send message COUNT times\n"
                  "| --identifier or -i=ID      CAN Identifier (default = %u)\n"
                  "| --poll or -p               use poll(2) to wait for buffer space while sending\n"
                  "| --quit or -q               quit if a wrong sequence is encountered\n"
                  "| --verbose or -v            to be verbose (twice to be even more verbose\n"
                  "| --help or -h               this help\n"
                  "| --version                  print version information and exit\n"
                  "|-------------------------------------------------------------------------------\n",
                  prog, DEFAULT_CAN_ID);
}

void sigterm(int signo)
{
    running = 0;
}

static struct option long_options[] = {
    {"extended", no_argument, 0, 'e'},
    {"help", no_argument, 0, 'h'},
    {"poll", no_argument, 0, 'p'},
    {"quit", no_argument, 0, 'q'},
    {"receive", no_argument, 0, 'r'},
    {"verbose", no_argument, 0, 'v'},
    {"version", no_argument, 0, VERSION_OPTION},
    {"identifier", required_argument, 0, 'i'},
    {"loop", required_argument, 0, 'l'},
    {0, 0, 0, 0},
};

static int extended = 0;
static int use_poll = 0;
static int verbose = 0, quit = 0;
static int loopcount = 1, infinite = 1;
static int receive = 0;
static struct can_filter filter[] = {
    {
        .can_id = DEFAULT_CAN_ID,
    },
};

static void parse_opt(int argc, char **argv)
{
    int opt;

    while ((opt = getopt_long(argc, argv, "ehpqrvi:l:", long_options, NULL)) != -1) {
        if (opt == 'h') {
            print_usages(basename(argv[0]));
            exit(EXIT_SUCCESS);
        } else if (opt == 'l') {
            if (optarg) {
                loopcount = strtoul(optarg, NULL, 0);
                infinite = 0;
            } else {
                infinite = 1;
            }
        } else if (opt == 'i') {
            filter->can_id = strtoul(optarg, NULL, 0);
        } else if (opt == 'e') {
            extended = 1;
        } else if (opt == 'q') {
            quit = 1;
        } else if (opt == 'p') {
            use_poll = 1;
        } else if (opt == 'r') {
            receive = 1;
        } else if (opt == VERSION_OPTION) {
            printf("cansequence %s\n", VERSION);
            exit(EXIT_SUCCESS);
        } else if (opt == 'v') {
            verbose++;
        } else {
            fprintf(stderr, "Unknown option %d\n", opt);
        }
    }
}

static int socket_bind(char *interface, int family, int type, int proto)
{
    struct ifreq ifr;
    struct sockaddr_can addr;

    printf("interface : [%s], family : [%d], type : [%d], proto : [%d]\n", interface, family, type, proto);

    s = socket(family, type, proto);
    if (s < 0) {
        (void)fprintf(stderr, "socket: %s\n", strerror(errno));
        return 1;
    }

    (void)strncpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), interface, sizeof(ifr.ifr_name));
    if (ioctl(s, SIOCGIFINDEX, &ifr)) {
        (void)fprintf(stderr, "ioctl: %s\n", strerror(errno));
        return 1;
    }

    if (setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0)) {
        (void)fprintf(stderr, "setsockopt: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    addr.can_family = family;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        (void)fprintf(stderr, "bind: %s\n", strerror(errno));
        return 1;
    }
    return 0;
}

static int read_frame(struct can_frame *frame)
{
    int nbytes;
    int sequence_init = 1;
    unsigned char sequence = 0;
    int seq_wrap = 0;
    int exit_value = EXIT_SUCCESS;

    while ((infinite || loopcount--) && running) {
        nbytes = read(s, frame, sizeof(struct can_frame));
        if (nbytes < 0) {
            (void)fprintf(stderr, "read: %s\n", strerror(errno));
            return 1;
        }

        if (sequence_init) {
            sequence_init = 0;
            sequence = frame->data[0];
        }

        if (verbose > 1) {
            (void)fprintf(stdout, "received frame. sequence number: %d\n", frame->data[0]);
        }

        if (frame->data[0] != sequence) {
            (void)fprintf(stdout, "received wrong sequence count. expected: %c, got: %d\n", sequence, frame->data[0]);
            if (quit) {
                exit_value = EXIT_FAILURE;
                break;
            }
            sequence = frame->data[0];
        }

        sequence++;
        if (verbose && !sequence) {
            (void)fprintf(stdout, "sequence wrap around (%d)\n", seq_wrap++);
        }
    }
    return exit_value;
}

static void write_frame(struct can_frame *frame)
{
    unsigned char sequence = 0;
    int seq_wrap = 0;
    int exit_value = EXIT_SUCCESS;

    while ((infinite || loopcount--) && running) {
        ssize_t len;

        if (verbose > 1) {
            (void)fprintf(stdout, "sending frame. sequence number: %c\n", sequence);
        }

        len = write(s, frame, sizeof(*frame));
        if (len != -1) {
            (unsigned char)frame->data[0]++;
            sequence++;
            if (verbose && !sequence) {
                (void)fprintf(stdout, "sequence wrap around (%d)\n", seq_wrap++);
            }
        } else if (errno == ENOBUFS) {
            int err;
            struct pollfd fds[] = {
                {
                    .fd = s,
                    .events = POLLOUT,
                },
            };

            if (!use_poll) {
                (void)fprintf(stderr, "write: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

            err = poll(fds, 1, 1000L);
            if (err == -1 && errno != -EINTR) {
                (void)fprintf(stderr, "poll(): %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        } else if (errno == EINTR) {
            continue;
        } else {
            (void)fprintf(stderr, "write: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char **argv)
{
    struct can_frame frame = {
        .can_dlc = 1,
    };
    char *interface = "can0";
    int family = PF_CAN, type = SOCK_RAW, proto = CAN_RAW;
    int exit_value = EXIT_SUCCESS;

    (void)signal(SIGTERM, sigterm);
    (void)signal(SIGHUP, sigterm);

    parse_opt(argc, argv);
    if (argv[optind]) {
        interface = argv[optind];
    }

    if (extended == 1) {
        filter->can_mask = CAN_EFF_MASK;
        filter->can_id &= CAN_EFF_MASK;
        filter->can_id |= CAN_EFF_FLAG;
    } else {
        filter->can_mask = CAN_SFF_MASK;
        filter->can_id &= CAN_SFF_MASK;
    }
    frame.can_id = filter->can_id;

    if (socket_bind(interface, family, type, proto) != 0) {
        (void)fprintf(stderr, "socket bind failed!\n");
        return 1;
    }
    if (receive) {
        /* enable recv. now */
        if (setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, filter, sizeof(filter))) {
            (void)fprintf(stderr, "setsockopt: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (read_frame(&frame) != 0) {
            (void)fprintf(stderr, "read frame fail!\n");
            return 1;
        }
    } else {
        write_frame(&frame);
    }

    exit(exit_value);
}
