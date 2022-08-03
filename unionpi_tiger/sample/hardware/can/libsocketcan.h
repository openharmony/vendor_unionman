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

#ifndef _socket_can_h
#define _socket_can_h

#include "can_netlink.h"

struct rtnl_link_stats64; /* <linux/if_link.h> */

int can_set_restart_ms(const char *name, __u32 restart_ms);

int can_set_bittiming(const char *name, struct can_bittiming *bt);

int can_set_ctrlmode(const char *name, struct can_ctrlmode *cm);

int can_set_bitrate(const char *name, __u32 bitrate);

int can_set_bitrate_samplepoint(const char *name, __u32 bitrate, __u32 sample_point);

int can_get_restart_ms(const char *name, __u32 *restart_ms);

int can_get_bittiming(const char *name, struct can_bittiming *bt);

int can_get_ctrlmode(const char *name, struct can_ctrlmode *cm);

int can_get_state(const char *name, int *state);

int can_get_clock(const char *name, struct can_clock *clock);

int can_get_bittiming_const(const char *name, struct can_bittiming_const *btc);

int can_get_berr_counter(const char *name, struct can_berr_counter *bc);

int can_get_device_stats(const char *name, struct can_device_stats *cds);

int can_get_link_stats(const char *name, struct rtnl_link_stats64 *rls);

int can_do_restart(const char *name);

int can_do_stop(const char *name);

int can_do_start(const char *name);

#endif
