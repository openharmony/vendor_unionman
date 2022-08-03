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
#include <errno.h>
#include <net/if.h>
#include <netlink/netlink.h>
#include <netlink/socket.h>
#include <netlink/msg.h>
#include <netlink-private/types.h>

#include <linux/if_link.h>
#include <linux/rtnetlink.h>
#include <linux/netlink.h>

#include "libsocketcan.h"

enum {
    IF_UP = 1,
    IF_DOWN
};

enum {
    GET_STATE = 1,
    GET_RESTART_MS,
    GET_BITTIMING,
    GET_CTRLMODE,
    GET_CLOCK,
    GET_BITTIMING_CONST,
    GET_BERR_COUNTER,
    GET_XSTATS,
    GET_LINK_STATS
};

struct get_req {
    struct nlmsghdr nlh;
    struct ifinfomsg ifi;
};

struct set_req {
    struct nlmsghdr nlh;
    struct ifinfomsg ifi;
    char buf[1024];
};

struct req_info {
    __u8 restart;
    __u8 disable_autorestart;
    __u32 restart_ms;
    struct can_ctrlmode *ctrlmode;
    struct can_bittiming *bittiming;
};

static void parse_rtattr(struct rtattr **tb, int max, struct rtattr *rta, int len)
{
    struct rtattr *attr = rta;
    (void)memset_s(tb, sizeof(struct rtattr) * (max + 1), 0, sizeof(struct rtattr) * (max + 1));

    for (; RTA_OK(attr, len); attr = RTA_NEXT(attr, len)) {
        if (attr->rta_type <= max) {
            tb[attr->rta_type] = attr;
        }
    }
}

static void parse_rtattr_nested(struct rtattr **tb, int max, struct rtattr *rta)
{
    void *attr = RTA_DATA(rta);
    int len = RTA_PAYLOAD(rta);
    parse_rtattr(tb, max, attr, len);
}

static int addattr32(struct nlmsghdr *n, size_t maxlen, int type, __u32 data)
{
    int lenth = RTA_LENGTH(4);
    struct rtattr *attr;

    if (NLMSG_ALIGN(n->nlmsg_len) + lenth > maxlen) {
        (void)fprintf(stderr, "addattr32: Error! max allowed bound %zu exceeded\n", maxlen);
        return -1;
    }

    attr = nlmsg_tail(n);
    attr->rta_type = type;
    attr->rta_len = lenth;
    (void)memcpy_s(RTA_DATA(attr), maxlen, &data, 4L);
    n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + lenth;

    return 0;
}

static int addattr_l(struct nlmsghdr *n, size_t maxlen, int type, const void *data, int alen)
{
    int lenth = RTA_LENGTH(alen);
    struct rtattr *attr;
    int ret;

    if (NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(lenth) > maxlen) {
        (void)fprintf(stderr, "addattr_l ERROR: message exceeded bound of %zu\n", maxlen);
        return -1;
    }

    attr = nlmsg_tail(n);
    attr->rta_type = type;
    attr->rta_len = lenth;
    ret = memcpy_s(RTA_DATA(attr), alen, data, alen);
    if (ret != 0) {
    }
    n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(lenth);

    return 0;
}

static struct nl_sock *OpenNetlinkSocket(void)
{
    struct nl_sock *sock = NULL;

    sock = nl_socket_alloc();
    if (sock == NULL) {
        fprintf(stderr, "%s : fail to alloc socket", __FUNCTION__);
        return NULL;
    }

    if (nl_connect(sock, NETLINK_ROUTE) != 0) {
        fprintf(stderr, "%s : fail to connect socket", __FUNCTION__);
        nl_socket_free(sock);
        return NULL;
    }
    return sock;
}

static void CloseNetlinkSocket(struct nl_sock *sock)
{
    if (sock != NULL) {
        nl_socket_free(sock);
    }
}

static int send_mod_request(struct nl_sock *sk, struct nlmsghdr *n)
{
    struct sockaddr_nl nladdr;
    struct nl_msg *msg = NULL;
    msg = nlmsg_alloc();

    n->nlmsg_seq = 0;
    n->nlmsg_flags |= NLM_F_ACK;

    msg = nlmsg_convert(n);

    (void)memset_s(&nladdr, sizeof(nladdr), 0, sizeof(nladdr));

    nladdr.nl_family = AF_NETLINK;
    nladdr.nl_pid = 0;
    nladdr.nl_groups = 0;

    nl_socket_set_peer_port(sk, 0);
    nl_socket_set_peer_groups(sk, 0);

    nlmsg_set_dst(msg, &nladdr);

    int ret = nl_send_auto(sk, msg);
    if (ret < 0) {
        perror("nl_send_auto");
    }

    ret = nl_recvmsgs_default(sk);

    return ret;
}

static int send_dump_request(struct nl_sock *sk, const char *name, int family, int type)
{
    struct get_req um_get;
    int ret;

    int fd = nl_socket_get_fd(sk);

    ret = memset_s(&um_get, sizeof(um_get), sizeof(um_get), 0, sizeof(um_get));
    if (ret != 0) {
    }

    um_get.nlh.nlmsg_type = type;
    um_get.nlh.nlmsg_len = sizeof(um_get);
    um_get.nlh.nlmsg_pid = 0;
    um_get.nlh.nlmsg_seq = 0;
    um_get.nlh.nlmsg_flags = NLM_F_REQUEST;

    um_get.ifi.ifi_family = family;

    if (name == NULL) {
        um_get.nlh.nlmsg_flags |= NLM_F_DUMP;
    } else {
        um_get.ifi.ifi_index = if_nametoindex(name);
        if (um_get.ifi.ifi_index == 0) {
            (void)fprintf(stderr, "Cannot find device \"%s\"\n", name);
            return -1;
        }
    }

    return write(fd, (void *)&um_get, sizeof(um_get));
}

static int acquire_link_stats(void *res, struct rtattr **tb)
{
    int ret = -1;

    if (!tb[IFLA_STATS64]) {
        fprintf(stderr, "no link statistics (64-bit) found\n");
    } else {
        (void)memcpy_s(res, sizeof(struct rtnl_link_stats64),
            RTA_DATA(tb[IFLA_STATS64]), sizeof(struct rtnl_link_stats64));
        ret = 0;
    }

    return ret;
}

static int acquire_xstats(void *res, struct rtattr **linkinfo)
{
    int ret = -1;

    if (!linkinfo[IFLA_INFO_XSTATS]) {
        fprintf(stderr, "no can statistics found\n");
    } else {
        (void)memcpy_s(res, sizeof(struct can_device_stats),
            RTA_DATA(linkinfo[IFLA_INFO_XSTATS]), sizeof(struct can_device_stats));
    ret = 0;
    }

    return ret;
}

static int acquire_restart_ms(void *res, struct rtattr **can_attr)
{
    int ret = -1;

    if (can_attr[IFLA_CAN_RESTART_MS]) {
        *((__u32 *)res) = *((__u32 *)RTA_DATA(can_attr[IFLA_CAN_RESTART_MS]));
        ret = 0;
    } else {
        fprintf(stderr, "no restart_ms data found\n");
    }

    return ret;
}

static int acquire_state(void *res, struct rtattr **can_attr)
{
    int ret = -1;

    if (can_attr[IFLA_CAN_STATE]) {
        *((int *)res) = *((__u32 *)RTA_DATA(can_attr[IFLA_CAN_STATE]));
        ret = 0;
    } else {
        fprintf(stderr, "no state data found\n");
    }

    return ret;
}

static int acquire_ctrlmode(void *res, struct rtattr **can_attr)
{
    int ret = -1;

    if (can_attr[IFLA_CAN_CTRLMODE]) {
        (void)memcpy_s(res, sizeof(struct can_ctrlmode),
            RTA_DATA(can_attr[IFLA_CAN_CTRLMODE]), sizeof(struct can_ctrlmode));
        ret = 0;
    } else {
        fprintf(stderr, "no ctrlmode data found\n");
    }

    return ret;
}

static int acquire_bittiming(void *res, struct rtattr **can_attr)
{
    int ret = -1;

    if (can_attr[IFLA_CAN_BITTIMING]) {
        (void)memcpy_s(res, sizeof(struct can_bittiming),
            RTA_DATA(can_attr[IFLA_CAN_BITTIMING]), sizeof(struct can_bittiming));
        ret = 0;
    } else {
        fprintf(stderr, "no bittiming data found\n");
    }

    return ret;
}

static int acquire_clock(void *res, struct rtattr **can_attr)
{
    int ret = -1;

    if (can_attr[IFLA_CAN_CLOCK]) {
        (void)memcpy_s(res, sizeof(struct can_clock),
            RTA_DATA(can_attr[IFLA_CAN_CLOCK]), sizeof(struct can_clock));
        ret = 0;
    } else {
        fprintf(stderr, "no clock parameter data found\n");
    }

    return ret;
}

static int acquire_berr_counter(void *res, struct rtattr **can_attr)
{
    int ret = -1;

    if (can_attr[IFLA_CAN_BERR_COUNTER]) {
        (void)memcpy_s(res, sizeof(struct can_berr_counter),
            RTA_DATA(can_attr[IFLA_CAN_BERR_COUNTER]), sizeof(struct can_berr_counter));
        ret = 0;
    } else {
        fprintf(stderr, "no berr_counter data found\n");
    }

    return ret;
}

static int acquire_bittiming_const(void *res, struct rtattr **can_attr)
{
    int ret = -1;

    if (can_attr[IFLA_CAN_BITTIMING_CONST]) {
        (void)memcpy_s(res, sizeof(struct can_bittiming_const),
            RTA_DATA(can_attr[IFLA_CAN_BITTIMING_CONST]), sizeof(struct can_bittiming_const));
        ret = 0;
    } else {
        fprintf(stderr, "no bittiming_const data found\n");
    }

    return ret;
}

static void check_acquire(__u8 acquire, int *ret, struct rtattr **can_attr, void *res)
{
    if (acquire == GET_RESTART_MS) {
        *ret = acquire_restart_ms(res, can_attr);
    } else if (acquire == GET_STATE) {
        *ret = acquire_state(res, can_attr);
    } else if (acquire == GET_CTRLMODE) {
        *ret = acquire_ctrlmode(res, can_attr);
    } else if (acquire == GET_BITTIMING) {
        *ret = acquire_bittiming(res, can_attr);
    } else if (acquire == GET_CLOCK) {
        *ret = acquire_clock(res, can_attr);
    } else if (acquire == GET_BERR_COUNTER) {
        *ret = acquire_berr_counter(res, can_attr);
    } else if (acquire == GET_BITTIMING_CONST) {
        *ret = acquire_bittiming_const(res, can_attr);
    } else {
        fprintf(stderr, "unknown acquire mode\n");
    }
}

static int check_type(int type, int *done)
{
    if (type != RTM_NEWLINK) {
        return-1;
    }
    if (type == NLMSG_DONE) {
        (*done)++;
        return -1;
    }
    return 0;
}

static int if_rep_matched(int *done, struct rtattr **tb, const char *name)
{
    if (strcmp((char *)RTA_DATA(tb[IFLA_IFNAME]), name) == 0) {
        (*done)++;
    } else {
        return -1;
    }
    return 0;
}

static int if_acquire_link_state(__u8 acquire, int *ret, void *res, struct rtattr **tb)
{
    if (acquire == GET_LINK_STATS) {
        *ret = acquire_link_stats(res, tb);
        return 1;
    }
    return 0;
}

static int if_acquire_xstate(__u8 acquire, int *ret, void *res, struct rtattr **linkinfo)
{
    if (acquire == GET_XSTATS) {
        *ret = acquire_xstats(res, linkinfo);
        return 1;
    }
    return 0;
}

static int do_get_nl_link(struct nl_sock *sk, __u8 acquire, const char *name, void *res)
{
    struct sockaddr_nl peer;
    struct ucred *creds = NULL;
    unsigned char *buf = NULL;

    int done = 0;
    int ret = -1;

    struct nlmsghdr *hdr;
    struct rtattr *can_attr[IFLA_CAN_MAX + 1];
    ssize_t msglen;
    struct rtattr *linkinfo[IFLA_INFO_MAX + 1];

    if (send_dump_request(sk, name, AF_PACKET, RTM_GETLINK) < 0) {
        perror("Cannot send dump request");
        return ret;
    }

    while (!done && (msglen = nl_recv(sk, &peer, &buf, &creds)) > 0) {
        int u_msglen = (int)msglen;

        hdr = (struct nlmsghdr *) buf;
        while (nlmsg_ok(hdr, u_msglen)) {
            int len;
            int type = hdr->nlmsg_type;

            if (check_type(type, &done) < 0) {
                continue;
            }
            struct rtattr *tb[IFLA_MAX + 1];
            struct ifinfomsg *ifi = NLMSG_DATA(hdr);

            len = hdr->nlmsg_len - NLMSG_LENGTH(sizeof(struct ifinfomsg));
            parse_rtattr(tb, IFLA_MAX, IFLA_RTA(ifi), len);

            /* Finish process if the reply message is matched */
            if (if_rep_matched(&done, tb, name) < 0) {
                continue;
            }

            if (if_acquire_link_state(acquire, &ret, res, tb) == 1) {
                continue;
            }

            if (tb[IFLA_LINKINFO]) {
                parse_rtattr_nested(linkinfo, IFLA_INFO_MAX, tb[IFLA_LINKINFO]);
            } else {
                continue;
            }

            if (if_acquire_xstate(acquire, &ret, res, linkinfo) == 1) {
                continue;
            }

            if (!linkinfo[IFLA_INFO_DATA]) {
                fprintf(stderr, "no link data found\n");
                return ret;
            }

            parse_rtattr_nested(can_attr, IFLA_CAN_MAX, linkinfo[IFLA_INFO_DATA]);

            check_acquire(acquire, &ret, can_attr, res);

            hdr = nlmsg_next(hdr, &u_msglen);
        }
    }

    return ret;
}

static int get_link(const char *name, __u8 acquire, void *res)
{
    int err;

    struct nl_sock *sk;

    sk = OpenNetlinkSocket();
    if (sk == NULL) {
        fprintf(stderr, "%s: fail to open cmd socket", __FUNCTION__);
        return -1;
    }

    err = do_get_nl_link(sk, acquire, name, res);
    CloseNetlinkSocket(sk);

    return err;
}

static int do_set_nl_link(struct nl_sock *sk, __u8 if_state, const char *name, struct req_info *msg_req)
{
    struct set_req um_set;

    const char *type = "can";

    (void)memset_s(&um_set, sizeof(um_set), 0, sizeof(um_set));

    um_set.nlh.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
    um_set.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
    um_set.nlh.nlmsg_type = RTM_NEWLINK;
    um_set.ifi.ifi_family = 0;

    um_set.ifi.ifi_index = if_nametoindex(name);
    if (um_set.ifi.ifi_index == 0) {
        (void)fprintf(stderr, "Cannot find device \"%s\"\n", name);
        return -1;
    }

    if (if_state) {
        if (if_state == IF_UP) {
            um_set.ifi.ifi_flags |= IFF_UP;
            um_set.ifi.ifi_change |= IFF_UP;
        } else if (if_state == IF_DOWN) {
            um_set.ifi.ifi_change |= IFF_UP;
            um_set.ifi.ifi_flags &= ~IFF_UP;
        } else {
            (void)fprintf(stderr, "unknown state\n");
            return -1;
        }
    }

    if (msg_req != NULL) {
        struct rtattr *linkinfo = nlmsg_tail(&um_set.nlh);
        (void)addattr_l(&um_set.nlh, sizeof(um_set), IFLA_LINKINFO, NULL, 0);
        (void)addattr_l(&um_set.nlh, sizeof(um_set), IFLA_INFO_KIND, type, strlen(type));
        struct rtattr *data = nlmsg_tail(&um_set.nlh);
        (void)addattr_l(&um_set.nlh, sizeof(um_set), IFLA_INFO_DATA, NULL, 0);

        if (msg_req->restart_ms > 0 || msg_req->disable_autorestart) {
            (void)addattr32(&um_set.nlh, 1024L, IFLA_CAN_RESTART_MS, msg_req->restart_ms);
        }

        if (msg_req->restart) {
            (void)addattr32(&um_set.nlh, 1024L, IFLA_CAN_RESTART, 1);
        }

        if (msg_req->bittiming != NULL) {
            (void)addattr_l(&um_set.nlh, 1024L, IFLA_CAN_BITTIMING, msg_req->bittiming, sizeof(struct can_bittiming));
        }

        if (msg_req->ctrlmode != NULL) {
            (void)addattr_l(&um_set.nlh, 1024L, IFLA_CAN_CTRLMODE, msg_req->ctrlmode, sizeof(struct can_ctrlmode));
        }

        data->rta_len = (void *)nlmsg_tail(&um_set.nlh) - (void *)data;

        linkinfo->rta_len = (void *)nlmsg_tail(&um_set.nlh) - (void *)linkinfo;
    }

    return send_mod_request(sk, &um_set.nlh);
}

static int set_link(const char *name, __u8 if_state, struct req_info *req_info)
{
    int err;

    struct nl_sock *sk;

    sk = OpenNetlinkSocket();
    if (sk == NULL) {
        fprintf(stderr, "%s: fail to open cmd socket", __FUNCTION__);
        return -1;
    }

    err = do_set_nl_link(sk, if_state, name, req_info);
    CloseNetlinkSocket(sk);

    return err;
}

int can_do_start(const char *name)
{
    return set_link(name, IF_UP, NULL);
}

int can_do_stop(const char *name)
{
    return set_link(name, IF_DOWN, NULL);
}

int can_do_restart(const char *name)
{
    int state;
    __u32 restart_ms;

    if ((can_get_state(name, &state)) < 0) {
        (void)fprintf(stderr, "cannot get bustate, "
                        "something is seriously wrong\n");
        return -1;
    } else if (state != CAN_STATE_BUS_OFF) {
        (void)fprintf(stderr, "Device is not in BUS_OFF,"
                        " no use to restart\n");
        return -1;
    }

    if ((can_get_restart_ms(name, &restart_ms)) < 0) {
        (void)fprintf(stderr, "cannot get restart_ms, "
                        "something is seriously wrong\n");
        return -1;
    } else if (restart_ms > 0) {
        (void)fprintf(stderr,
            "auto restart with %ums interval is turned on,"
            " no use to restart\n",
            restart_ms);
        return -1;
    }

    struct req_info req_info = {
        .restart = 1,
    };

    return set_link(name, 0, &req_info);
}

int can_set_restart_ms(const char *name, __u32 restart_ms)
{
    struct req_info req_info = {
        .restart_ms = restart_ms,
    };

    if (restart_ms == 0) {
        req_info.disable_autorestart = 1;
    }

    return set_link(name, 0, &req_info);
}

int can_set_ctrlmode(const char *name, struct can_ctrlmode *cm)
{
    struct req_info req_info = {
        .ctrlmode = cm,
    };

    return set_link(name, 0, &req_info);
}

int can_set_bittiming(const char *name, struct can_bittiming *bt)
{
    struct req_info req_info = {
        .bittiming = bt,
    };

    return set_link(name, 0, &req_info);
}

int can_set_bitrate(const char *name, __u32 bitrate)
{
    struct can_bittiming bt;

    (void)memset_s(&bt, sizeof(bt), 0, sizeof(bt));
    bt.bitrate = bitrate;

    return can_set_bittiming(name, &bt);
}

int can_set_bitrate_samplepoint(const char *name, __u32 bitrate, __u32 sample_point)
{
    struct can_bittiming bt;

    (void)memset_s(&bt, sizeof(bt), 0, sizeof(bt));
    bt.bitrate = bitrate;
    bt.sample_point = sample_point;

    return can_set_bittiming(name, &bt);
}

int can_get_state(const char *name, int *state)
{
    return get_link(name, GET_STATE, state);
}

int can_get_restart_ms(const char *name, __u32 *restart_ms)
{
    return get_link(name, GET_RESTART_MS, restart_ms);
}

int can_get_bittiming(const char *name, struct can_bittiming *bt)
{
    return get_link(name, GET_BITTIMING, bt);
}

int can_get_ctrlmode(const char *name, struct can_ctrlmode *cm)
{
    return get_link(name, GET_CTRLMODE, cm);
}

int can_get_clock(const char *name, struct can_clock *clock)
{
    return get_link(name, GET_CLOCK, clock);
}

int can_get_bittiming_const(const char *name, struct can_bittiming_const *btc)
{
    return get_link(name, GET_BITTIMING_CONST, btc);
}

int can_get_berr_counter(const char *name, struct can_berr_counter *bc)
{
    return get_link(name, GET_BERR_COUNTER, bc);
}
