/*
 * Copyright 2023 Unionman Technology Co., Ltd.
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

#include <errno.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>

#include "securec.h"

int ethtool_mygset(const char* devname, int* speed, int* duplex, int* autoneg, int* link)
{
    struct ifreq ifr;
    int fd = 0;
    int err = -1;

    struct ethtool_cmd ecmd;
    struct ethtool_value edata;

    if (devname == NULL) {
        return -2L;
    }
    (void)memset_s(&ifr, sizeof(ifr), 0, sizeof(ifr));
    (void)strcpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), devname);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        printf("Get control socket failed");
        return -1;
    }

    ecmd.cmd = ETHTOOL_GSET;
    ifr.ifr_data = (void*)&ecmd;
    err = ioctl(fd, SIOCETHTOOL, &ifr);
    if (err < 0) {
        printf("Get device settings failed");
        return -1;
    }

    printf("PHY xx - %d/%s ", ecmd.speed, (ecmd.duplex == DUPLEX_FULL) ? "Full" : "Half");
    printf(" Auto-negotiation: %s ", (ecmd.autoneg == AUTONEG_DISABLE) ? "off" : "on");

    if (ecmd.speed == 10L || ecmd.speed == 100L || ecmd.speed == 1000L || ecmd.speed == 2500L || ecmd.speed == 10000L) {
        *speed = ecmd.speed;
    } else {
        fprintf(stdout, "Unknonwn! (%i)\n", ecmd.speed);
    }

    if (ecmd.duplex == DUPLEX_HALF || ecmd.duplex == DUPLEX_FULL) {
        *duplex = ecmd.duplex;
    } else {
        fprintf(stdout, "Unknown! (%i)\n", ecmd.duplex);
    }
    *autoneg = ecmd.autoneg;

    edata.cmd = ETHTOOL_GLINK;
    ifr.ifr_data = (void*)&edata;
    err = ioctl(fd, SIOCETHTOOL, &ifr);
    if (errno != EOPNOTSUPP) {
        printf("Get link status failed");
    } else if (err == 0) {
        *link = edata.data ? 1 : 0;
        printf("%s\n", edata.data ? "Up" : "Down");
    }

    close(fd);
    return 0;
}

int spend1000(void)
{
    int speed;
    int duplex;
    int autoneg;
    int link;
    int ret;

    ret = ethtool_mygset("eth0", &speed, &duplex, &autoneg, &link);
    if (speed == 1000L) {
        return 1;
    } else {
        return 0;
    }
}
