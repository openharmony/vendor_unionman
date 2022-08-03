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
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include "libsocketcan.h"
#include "can_config.h"

const char *can_states[CAN_STATE_MAX] = {"ERROR-ACTIVE", "ERROR-WARNING", "ERROR-PASSIVE",
                                         "BUS-OFF",      "STOPPED",       "SLEEPING"};

const char *config_keywords[] = {"baudrate", "bitrate",    "bittiming", "ctrlmode",  "restart",         "start",
                                 "stop",     "restart-ms", "state",     "clockfreq", "bittiming-const", "berr-counter"};

const char *bittimg_frame[] = {"tq", "prop-seg", "phase-seg1", "phase-seg2", "sjw"};

static void um_arg_next(int *argc, char ***argv)
{
    (*argv)++;
    if (--(*argc) < 0) {
        fprintf(stderr, "missing parameter for %s\n", **argv);
        exit(EXIT_FAILURE);
    }
}

static inline int str_check(const char **haystack, unsigned int stack_size, const char *needle)
{
    unsigned int i = 0;
    int found = 0;

    while (i < stack_size) {
        if (!strcmp(needle, haystack[i])) {
            found = 1;
            break;
        }
        i++;
    }
    return found;
}

static void help(void)
{
    (void)fprintf(stderr,
                  "|-------------------------------------------------------------------------------------\n"
                  "|usage:\n"
                  "|-------------------------------------------------------------------------------------\n"
                  "|    canconfig <dev> bitrate { BR } [sample-point { SP }]\n"
                  "|            BR := <bitrate in Hz>\n"
                  "|            SP := <sample-point {0...0.999}> (optional)\n"
                  "|    canconfig <dev> bittiming [ VALs ]\n"
                  "|            VALs := <tq | prop-seg | phase-seg1 | phase-seg2 | sjw>\n"
                  "|            tq <time quantum in ns>\n"
                  "|            prop-seg <no. in tq>\n"
                  "|            phase-seg1 <no. in tq>\n"
                  "|            phase-seg2 <no. in tq\n"
                  "|            sjw <no. in tq> (optional)\n"
                  "|    canconfig <dev> restart-ms { RESTART-MS }\n"
                  "|            RESTART-MS := <autorestart interval in ms>\n"
                  "|    canconfig <dev> ctrlmode { CTRLMODE }\n"
                  "|            CTRLMODE:=<[loopback | listen-only | triple-sampling | berr-reporting] [on|off]>\n"
                  "|    canconfig <dev> {ACTION}\n"
                  "|            ACTION := <[start|stop|restart]>\n"
                  "|    canconfig <dev> clockfreq\n"
                  "|    canconfig <dev> bittiming-constants\n"
                  "|    canconfig <dev> berr-counter\n"
                  "|-------------------------------------------------------------------------------------\n");
    exit(EXIT_FAILURE);
}

static void bitrate_display(const char *name)
{
    struct can_bittiming bt;

    if (can_get_bittiming(name, &bt) < 0) {
        (void)fprintf(stderr, "%s: failed to get bitrate\n", name);
        exit(EXIT_FAILURE);
    } else {
        (void)fprintf(stdout, "%s bitrate: %u, sample-point: %0.3f\n", name, bt.bitrate,
            (float)((float)bt.sample_point / 1000L));
    }
}

static void set_bitrate(int argc, char *argv[], const char *name)
{
    __u32 bitrate = 0;
    __u32 sample_point = 0;
    int err;
    int argc_temp = argc;
    char **argv_temp = argv;

    while (argc_temp > 0) {
        if (!strcmp(*argv_temp, "bitrate")) {
            um_arg_next(&argc_temp, &argv_temp);
            bitrate = (__u32)strtoul(*argv_temp, NULL, 0);
        } else if (!strcmp(*argv_temp, "sample-point")) {
            um_arg_next(&argc_temp, &argv_temp);
            sample_point = (__u32)(strtod(*argv_temp, NULL) * 1000L);
        }
        argc_temp--, argv_temp++;
    }

    if (sample_point) {
        err = can_set_bitrate_samplepoint(name, bitrate, sample_point);
    } else {
        err = can_set_bitrate(name, bitrate);
    }

    if (err < 0) {
        (void)fprintf(stderr, "failed to set bitrate of %s to %u\n", name, bitrate);
        exit(EXIT_FAILURE);
    }
}

static void cmd_bitrate(int argc, char *argv[], const char *name)
{
    int if_set = 1;

    unsigned int size = sizeof(config_keywords) / sizeof(char *);

    if (argc > 0) {
        if_set = str_check(config_keywords, size, argv[1]);
    }

    if (!if_set) {
        set_bitrate(argc, argv, name);
    }

    bitrate_display(name);
}

static void set_bittiming(int argc, char *argv[], const char *name)
{
    struct can_bittiming bt;
    int bt_par_count = 0;
    int argc_temp = argc;
    char **argv_temp = argv;

    (void)memset_s(&bt, sizeof(bt), 0, sizeof(bt));

    while (argc_temp > 0) {
        if (!strcmp(*argv_temp, bittimg_frame[0])) {
            um_arg_next(&argc_temp, &argv_temp);
            bt.tq = (__u32)strtoul(*argv_temp, NULL, 0);
            bt_par_count++;
            continue;
        }
        if (!strcmp(*argv_temp, bittimg_frame[1])) {
            um_arg_next(&argc_temp, &argv_temp);
            bt.prop_seg = (__u32)strtoul(*argv_temp, NULL, 0);
            bt_par_count++;
            continue;
        }
        if (!strcmp(*argv_temp, bittimg_frame[2L])) {
            um_arg_next(&argc_temp, &argv_temp);
            bt.phase_seg1 = (__u32)strtoul(*argv_temp, NULL, 0);
            bt_par_count++;
            continue;
        }
        if (!strcmp(*argv_temp, bittimg_frame[3L])) {
            um_arg_next(&argc_temp, &argv_temp);
            bt.phase_seg2 = (__u32)strtoul(*argv_temp, NULL, 0);
            bt_par_count++;
            continue;
        }
        if (!strcmp(*argv_temp, bittimg_frame[4L])) {
            um_arg_next(&argc_temp, &argv_temp);
            bt.sjw = (__u32)strtoul(*argv_temp, NULL, 0);
            continue;
        }
        argc_temp--, argv_temp++;
    }

    if (bt_par_count < 4L) {
        fprintf(stderr,
                "%s: missing bittiming parameters, "
                "try help to figure out the correct format\n",
                name);
        exit(1);
    }
    if (can_set_bittiming(name, &bt) < 0) {
        (void)fprintf(stderr, "%s: unable to set bittiming\n", name);
        exit(EXIT_FAILURE);
    }
}

static void bittiming_display(const char *name)
{
    struct can_bittiming bt;

    if (can_get_bittiming(name, &bt) < 0) {
        (void)fprintf(stderr, "%s: failed to get bittiming\n", name);
        exit(EXIT_FAILURE);
    } else {
        (void)fprintf(stdout,
            "%s bittiming:\n\t"
            "tq: %u, prop-seq: %u phase-seq1: %u phase-seq2: %u "
            "sjw: %u, brp: %u\n",
            name, bt.tq, bt.prop_seg, bt.phase_seg1, bt.phase_seg2, bt.sjw, bt.brp);
    }
}

static void cmd_bittiming(int argc, char *argv[], const char *name)
{
    int if_set = 1;

    unsigned int size = sizeof(config_keywords) / sizeof(char *);

    if (argc > 0) {
        if_set = str_check(config_keywords, size, argv[1]);
    }

    if (!if_set) {
        set_bittiming(argc, argv, name);
    }

    bittiming_display(name);
    bitrate_display(name);
}

static void bittiming_const_display(const char *name)
{
    struct can_bittiming_const btc;

    if (can_get_bittiming_const(name, &btc) < 0) {
        (void)fprintf(stderr, "%s: failed to get bittiming_const\n", name);
        exit(EXIT_FAILURE);
    } else {
        (void)fprintf(stdout,
            "%s bittiming-constants: name %s,\n\t"
            "tseg1-min: %u, tseg1-max: %u, "
            "tseg2-min: %u, tseg2-max: %u,\n\t"
            "sjw-max %u, brp-min: %u, brp-max: %u, brp-inc: %u,\n",
            name, btc.name, btc.tseg1_min, btc.tseg1_max, btc.tseg2_min, btc.tseg2_max, btc.sjw_max, btc.brp_min,
            btc.brp_max, btc.brp_inc);
    }
}

static void cmd_bittiming_const(int argc, char *argv[], const char *name)
{
    bittiming_const_display(name);
}

static void state_display(const char *name)
{
    int state;

    if (can_get_state(name, &state) < 0) {
        (void)fprintf(stderr, "%s: failed to get state \n", name);
        exit(EXIT_FAILURE);
    }

    if (state >= 0 && state < CAN_STATE_MAX) {
        (void)fprintf(stdout, "%s state: %s\n", name, can_states[state]);
    } else {
        (void)fprintf(stderr, "%s: unknown state\n", name);
    }
}

static void cmd_state(int argc, char *argv[], const char *name)
{
    state_display(name);
}

static void clockfreq_display(const char *name)
{
    struct can_clock clock;

    (void)memset_s(&clock, sizeof(struct can_clock), 0, sizeof(struct can_clock));
    if (can_get_clock(name, &clock) < 0) {
        (void)fprintf(stderr, "%s: failed to get clock parameters\n", name);
        exit(EXIT_FAILURE);
    }

    (void)fprintf(stdout, "%s clock freq: %u\n", name, clock.freq);
}

static void cmd_clockfreq(int argc, char *argv[], const char *name)
{
    clockfreq_display(name);
}

static void do_restart(const char *name)
{
    if (can_do_restart(name) < 0) {
        (void)fprintf(stderr, "%s: failed to restart\n", name);
        exit(EXIT_FAILURE);
    } else {
        (void)fprintf(stdout, "%s restarted\n", name);
    }
}

static void cmd_restart(int argc, char *argv[], const char *name)
{
    do_restart(name);
}

static void do_start(const char *name)
{
    if (can_do_start(name) < 0) {
        (void)fprintf(stderr, "%s: failed to start\n", name);
        exit(EXIT_FAILURE);
    } else {
        state_display(name);
    }
}

static void cmd_start(int argc, char *argv[], const char *name)
{
    do_start(name);
}

static void do_stop(const char *name)
{
    if (can_do_stop(name) < 0) {
        (void)fprintf(stderr, "%s: failed to stop\n", name);
        exit(EXIT_FAILURE);
    } else {
        state_display(name);
    }
}

static void cmd_stop(int argc, char *argv[], const char *name)
{
    do_stop(name);
}

static inline void print_ctrlmode(__u32 cm_flags)
{
    (void)fprintf(stdout,
                  "loopback[%s], listen-only[%s], tripple-sampling[%s],"
                  "one-shot[%s], berr-reporting[%s]\n",
                  (cm_flags & CAN_CTRLMODE_LOOPBACK) ? "ON" : "OFF",
                  (cm_flags & CAN_CTRLMODE_LISTENONLY) ? "ON" : "OFF",
                  (cm_flags & CAN_CTRLMODE_3_SAMPLES) ? "ON" : "OFF",
                  (cm_flags & CAN_CTRLMODE_ONE_SHOT) ? "ON" : "OFF",
                  (cm_flags & CAN_CTRLMODE_BERR_REPORTING) ? "ON" : "OFF");
}

static void ctrlmode_display(const char *name)
{
    struct can_ctrlmode cm;

    if (can_get_ctrlmode(name, &cm) < 0) {
        (void)fprintf(stderr, "%s: failed to get controlmode\n", name);
        exit(EXIT_FAILURE);
    } else {
        (void)fprintf(stdout, "%s ctrlmode: ", name);
        print_ctrlmode(cm.flags);
    }
}

/* this is shamelessly stolen from iproute and slightly modified */
static inline void set_ctrlmode(char *name, char *arg, struct can_ctrlmode *cm, __u32 flags)
{
    if (strcmp(arg, "on") == 0) {
        cm->flags |= flags;
    } else if (strcmp(arg, "off") != 0) {
        (void)fprintf(stderr, "Error: argument of \"%s\" must be \"on\" or \"off\" %s\n", name, arg);
        exit(EXIT_FAILURE);
    }
    cm->mask |= flags;
}

static void do_set_ctrlmode(int argc, char *argv[], const char *name)
{
    struct can_ctrlmode cm;
    int argc_temp = argc;
    char **argv_temp = argv;

    (void)memset_s(&cm, sizeof(cm), 0, sizeof(cm));

    while (argc_temp > 0) {
        if (!strcmp(*argv_temp, "loopback")) {
            um_arg_next(&argc_temp, &argv_temp);
            set_ctrlmode("loopback", *argv_temp, &cm, CAN_CTRLMODE_LOOPBACK);
        } else if (!strcmp(*argv_temp, "listen-only")) {
            um_arg_next(&argc_temp, &argv_temp);
            set_ctrlmode("listen-only", *argv_temp, &cm, CAN_CTRLMODE_LISTENONLY);
        } else if (!strcmp(*argv_temp, "triple-sampling")) {
            um_arg_next(&argc_temp, &argv_temp);
            set_ctrlmode("triple-sampling", *argv_temp, &cm, CAN_CTRLMODE_3_SAMPLES);
        } else if (!strcmp(*argv_temp, "one-shot")) {
            um_arg_next(&argc_temp, &argv_temp);
            set_ctrlmode("one-shot", *argv_temp, &cm, CAN_CTRLMODE_ONE_SHOT);
        } else if (!strcmp(*argv_temp, "berr-reporting")) {
            um_arg_next(&argc_temp, &argv_temp);
            set_ctrlmode("berr-reporting", *argv_temp, &cm, CAN_CTRLMODE_BERR_REPORTING);
        }

        argc_temp--, argv_temp++;
    }

    if (can_set_ctrlmode(name, &cm) < 0) {
        (void)fprintf(stderr, "%s: failed to set ctrlmode\n", name);
        exit(EXIT_FAILURE);
    }
}

static void cmd_ctrlmode(int argc, char *argv[], const char *name)
{
    int if_set = 1;

    unsigned int size = sizeof(config_keywords) / sizeof(char *);

    if (argc > 0) {
        if_set = str_check(config_keywords, size, argv[1]);
    }

    if (!if_set) {
        do_set_ctrlmode(argc, argv, name);
    }

    ctrlmode_display(name);
}

static void restart_ms_display(const char *name)
{
    __u32 restart_ms;

    if (can_get_restart_ms(name, &restart_ms) < 0) {
        (void)fprintf(stderr, "%s: failed to get restart_ms\n", name);
        exit(EXIT_FAILURE);
    } else {
        (void)fprintf(stdout, "%s restart-ms: %u\n", name, restart_ms);
    }
}

static void do_set_restart_ms(int argc, char *argv[], const char *name)
{
    int ret = (__u32)strtoul(argv[1], NULL, 10);
    int ret_fprintf = (__u32)strtoul(argv[1], NULL, 10);
    if (can_set_restart_ms(name, ret) < 0) {
        (void)fprintf(stderr, "failed to set restart_ms of %s to %d\n", name, ret_fprintf);
        exit(EXIT_FAILURE);
    }
}

static void cmd_restart_ms(int argc, char *argv[], const char *name)
{
    int if_set = 1;

    unsigned int size = sizeof(config_keywords) / sizeof(char *);

    if (argc > 0) {
        if_set = str_check(config_keywords, size, argv[1]);
    }

    if (!if_set) {
        do_set_restart_ms(argc, argv, name);
    }

    restart_ms_display(name);
}

static void berr_counter_display(const char *name)
{
    struct can_berr_counter bc;
    struct can_ctrlmode cm;

    if (can_get_ctrlmode(name, &cm) < 0) {
        (void)fprintf(stderr, "%s: failed to get controlmode\n", name);
        exit(EXIT_FAILURE);
    }

    if (cm.flags & CAN_CTRLMODE_BERR_REPORTING) {
        (void)memset_s(&bc, sizeof(struct can_berr_counter), 0, sizeof(struct can_berr_counter));

        if (can_get_berr_counter(name, &bc) < 0) {
            (void)fprintf(stderr, "%s: failed to get berr counters\n", name);
            exit(EXIT_FAILURE);
        }

        (void)fprintf(stdout, "%s txerr: %u rxerr: %u\n", name, bc.txerr, bc.rxerr);
    }
}

static void cmd_berr_counter(int argc, char *argv[], const char *name)
{
    berr_counter_display(name);
}

static void cmd_baudrate(int argc, char *argv[], const char *name)
{
    (void)fprintf(stderr, "%s: baudrate is deprecated, pleae use bitrate\n", name);

    exit(EXIT_FAILURE);
}

static void interface_display(const char *name)
{
    bitrate_display(name);
    bittiming_display(name);
    state_display(name);
    restart_ms_display(name);
    ctrlmode_display(name);
    clockfreq_display(name);
    bittiming_const_display(name);
    berr_counter_display(name);

    exit(EXIT_SUCCESS);
}

static void spec_cmd(int argc, char **argv, const char *name)
{
    if ((argc < 2L) || (!strcmp(argv[1], "--help"))) {
        help();
    }

    if (!strcmp(argv[1], "--version")) {
        (void)fprintf(stdout, "Version: %s\n", VERSION);
        exit(EXIT_SUCCESS);
    }

    if (argc < 3L) {
        interface_display(name);
    }
}

int main(int argc, char *argv[])
{
    const char *name = argv[1];
    int cmd_paramnum = argc;
    char **cmd_param = argv;

    spec_cmd(cmd_paramnum, cmd_param, name);

    while (cmd_paramnum-- > 0) {
        if (!strcmp(cmd_param[0], config_keywords[0])) {
            cmd_baudrate(cmd_paramnum, cmd_param, name);
        }
        if (!strcmp(cmd_param[0], config_keywords[1])) {
            cmd_bitrate(cmd_paramnum, cmd_param, name);
        }
        if (!strcmp(cmd_param[0], config_keywords[2L])) {
            cmd_bittiming(cmd_paramnum, cmd_param, name);
        }
        if (!strcmp(cmd_param[0], config_keywords[3L])) {
            cmd_ctrlmode(cmd_paramnum, cmd_param, name);
        }
        if (!strcmp(cmd_param[0], config_keywords[4L])) {
            cmd_restart(cmd_paramnum, cmd_param, name);
        }
        if (!strcmp(cmd_param[0], config_keywords[5L])) {
            cmd_start(cmd_paramnum, cmd_param, name);
        }
        if (!strcmp(cmd_param[0], config_keywords[6L])) {
            cmd_stop(cmd_paramnum, cmd_param, name);
        }
        if (!strcmp(cmd_param[0], config_keywords[7L])) {
            cmd_restart_ms(cmd_paramnum, cmd_param, name);
        }
        if (!strcmp(cmd_param[0], config_keywords[8L])) {
            cmd_state(cmd_paramnum, cmd_param, name);
        }
        if (!strcmp(cmd_param[0], config_keywords[9L])) {
            cmd_clockfreq(cmd_paramnum, cmd_param, name);
        }
        if (!strcmp(cmd_param[0], config_keywords[10L])) {
            cmd_bittiming_const(cmd_paramnum, cmd_param, name);
        }
        if (!strcmp(cmd_param[0], config_keywords[11L])) {
            cmd_berr_counter(cmd_paramnum, cmd_param, name);
        }
        cmd_param++;
    }

    exit(EXIT_SUCCESS);
}
