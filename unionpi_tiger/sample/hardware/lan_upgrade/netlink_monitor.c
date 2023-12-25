/*
 * Copyright (c) Huawei Device Co., Ltd. 2021-2023  All right reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include "netlink_monitor.h"

#define LOG(fmt, args...)       printf("[LAN_UPGRADE] "fmt"\n", ##args)

#undef NLMSG_OK
#define NLMSG_OK(nlh, len) (((len) >= (int32_t)(sizeof(struct nlmsghdr))) && (((nlh)->nlmsg_len) >= \
    sizeof(struct nlmsghdr)) && ((int32_t)((nlh)->nlmsg_len) <= (len)))

#define DEFAULT_NETLINK_RECVBUF (4 * 1024)

static PFUN_NNL_EVENT_HANDLER g_eventHandler = NULL;

static int32_t CreateNetlinkSocket(void)
{
    int32_t sockFd;
    struct sockaddr_nl nladdr;
    int32_t sz = DEFAULT_NETLINK_RECVBUF;

    sockFd = socket(PF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, NETLINK_ROUTE);
    if (sockFd < 0) {
        LOG("open netlink socket failed");
        return -1;
    }
    if (setsockopt(sockFd, SOL_SOCKET, SO_RCVBUFFORCE, &sz, sizeof(sz)) < 0 &&
        setsockopt(sockFd, SOL_SOCKET, SO_RCVBUF, &sz, sizeof(sz)) < 0) {
        LOG("set uevent socket SO_RCVBUF option failed");
        close(sockFd);
        return -1;
    }
    (void)memset_s(&nladdr, sizeof(nladdr), 0, sizeof(nladdr));
    nladdr.nl_family = AF_NETLINK;
    // Kernel will assign a unique nl_pid if set to zero.
    nladdr.nl_pid = 0;
    nladdr.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;
    if (bind(sockFd, (struct sockaddr *)&nladdr, sizeof(nladdr)) < 0) {
        LOG("bind netlink socket failed");
        close(sockFd);
        return -1;
    }
    return sockFd;
}

static void ParseRtAttr(struct rtattr **tb, int max, struct rtattr *attr, int len)
{
    struct rtattr *attr1 = attr;
    for (; RTA_OK(attr1, len); attr1 = RTA_NEXT(attr1, len)) {
        if (attr1->rta_type <= max) {
            tb[attr1->rta_type] = attr1;
        }
    }
}

static int32_t GetAddrTypeByIfname(const char *ifname, CONNECTION_ADDR_TYPE_E *type)
{
    if (ifname == NULL || type == NULL) {
        LOG("parameters are NULL!");
        return -1;
    }

    if (!strncasecmp(ifname, "eth", 3L)) {
        *type = CONNECTION_ADDR_ETH;
    } else if (!strncasecmp(ifname, "wlan", 4L)) {
        *type = CONNECTION_ADDR_WLAN;
    } else {
        *type = CONNECTION_ADDR_BUTT;
    }
    
    return 0;
}

static void ProcessAddrEvent(struct nlmsghdr *nlh)
{
    struct ifaddrmsg *ifa = (struct ifaddrmsg *)NLMSG_DATA(nlh);
    char name[IFNAMSIZ];
    CONNECTION_ADDR_TYPE_E type = CONNECTION_ADDR_BUTT;

    if (if_indextoname(ifa->ifa_index, name) == 0) {
        LOG("invalid iface index");
        return;
    }
    if (GetAddrTypeByIfname(name, &type) != 0) {
        LOG("ProcessAddrEvent GetAddrTypeByIfname error");
        return;
    }
    if (type == CONNECTION_ADDR_ETH || type == CONNECTION_ADDR_WLAN) {
        LOG("network addr changed, type:%d", type);
        if (g_eventHandler) {
            g_eventHandler(NNL_EVENT_IP_ADDR_CHANGED, NULL);
        }
    }
}

static void ProcessLinkEvent(struct nlmsghdr *nlh)
{
    int len;
    struct rtattr *tb[IFLA_MAX + 1] = {NULL};
    struct ifinfomsg *ifinfo = NLMSG_DATA(nlh);
    CONNECTION_ADDR_TYPE_E type = CONNECTION_ADDR_BUTT;

    len = (int32_t)nlh->nlmsg_len - NLMSG_SPACE(sizeof(*ifinfo));
    ParseRtAttr(tb, IFLA_MAX, IFLA_RTA(ifinfo), len);

    if (tb[IFLA_IFNAME] == NULL) {
        LOG("netlink msg is invalid");
        return;
    }
    if (GetAddrTypeByIfname(RTA_DATA(tb[IFLA_IFNAME]), &type) != 0) {
        LOG("ProcessAddrEvent GetAddrTypeByIfname error");
        return;
    }
    if (type == CONNECTION_ADDR_ETH || type == CONNECTION_ADDR_WLAN) {
        LOG("link status changed, type:%d", type);
        if (g_eventHandler) {
            g_eventHandler(NNL_EVENT_IP_ADDR_CHANGED, NULL);
        }
    }
}

static void *NetlinkMonitorThread(void *para)
{
    int32_t sockFd;
    int32_t len;
    uint8_t buffer[DEFAULT_NETLINK_RECVBUF];
    struct nlmsghdr *nlh = NULL;

    (void)para;
    LOG("netlink monitor thread start");
    sockFd = CreateNetlinkSocket();
    if (sockFd < 0) {
        LOG("create netlink socket failed");
        return NULL;
    }
    while (1) {
        len = recv(sockFd, buffer, DEFAULT_NETLINK_RECVBUF, 0);
        if (len < 0 && errno == EINTR) {
            continue;
        }
        if (len < 0) {
            LOG("recv netlink socket error");
            break;
        }
        if (len < (int32_t)sizeof(struct nlmsghdr)) {
            LOG("recv buffer not enough");
            continue;
        }
        nlh = (struct nlmsghdr *)buffer;
        while (NLMSG_OK(nlh, len) && nlh->nlmsg_type != NLMSG_DONE) {
            switch (nlh->nlmsg_type) {
                case RTM_NEWADDR:
                case RTM_DELADDR:
                    ProcessAddrEvent(nlh);
                    break;
                case RTM_NEWLINK:
                case RTM_DELLINK:
                    ProcessLinkEvent(nlh);
                    break;
                default:
                    break;
            }
            nlh = NLMSG_NEXT(nlh, len);
        }
    }
    close(sockFd);
    LOG("netlink monitor thread exit");
    return NULL;
}

int32_t NetlinkMonitorStart(PFUN_NNL_EVENT_HANDLER handler)
{
    pthread_t tid;

    if (handler == NULL) {
        LOG("netlink event handler is null");
        return -1;
    }
    if (pthread_create(&tid, NULL, NetlinkMonitorThread, NULL) != 0) {
        LOG("create ip change monitor thread failed");
        return -1;
    }
    g_eventHandler = handler;
    return 0;
}

