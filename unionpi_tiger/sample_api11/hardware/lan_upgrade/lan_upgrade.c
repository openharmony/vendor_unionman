/*
 * Copyright (c) 2023 Unionman Technology Co., Ltd.
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

#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <ifaddrs.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "netlink_monitor.h"

static char g_serialNo[128] = "";
static char g_curHwVersion[128] = "0.0.0";
static char g_curSwVersion[128] = "0.0.0";
static char g_deviceModel[128] = " ";
static char g_upgradefileMD5[128] = { 0 };
static bool g_mainFlag = true;

#define MULTICAST_PORT (50002)
#define MULTICAST_ADDR_V4 "224.100.100.100"
#define MULTICAST_ADDR_V6 "ff15::100:100"

typedef enum {
    UPGRADE_STATUS_IDLE,
    UPGRADE_STATUS_DOWNLOADING,
    UPGRADE_STATUS_CHECKING,
    UPGRADE_STATUS_UPDATING,

    UPGRADE_STATUS_ERR = 100,
    UPGRADE_STATUS_ERR_DOWNLOAD = UPGRADE_STATUS_ERR,
    UPGRADE_STATUS_ERR_CHECK,
    UPGRADE_STATUS_ERR_UPDATE
} UPGRADE_STATUS_E;

static int g_sRunFlag = 0;
static pthread_t g_sTid;
static int g_sSock = -1;
static int g_sRestartFlag = 0;
static UPGRADE_STATUS_E g_sUpgradeStatus = UPGRADE_STATUS_IDLE;
static char g_sUpgradeUrl[512];

#define LOG(fmt, args...) printf("[LAN_UPGRADE] " fmt "\n", ##args)

static int IsCommandSafe(const char* cmd)
{
    return true;
}

static void inline StripQuotation(char str[])
{
    int len = strlen(str);
    if (str[0] == '"' && str[len - 1] == '"') {
        for (int i = 1; i < len - 1; i++) {
            str[i - 1] = str[i];
        }
        str[len - 2L] = '\0';
    }
}

static int GetPidByName(const char* processName)
{
    int pid = 0;
    DIR* dir = opendir("/proc");
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            char cmdlinePath[256] = { 0 };
            snprintf_s(cmdlinePath, sizeof(cmdlinePath), _TRUNCATE, "/proc/%s/cmdline", entry->d_name);
            FILE* cmdlineFile = fopen(cmdlinePath, "r");
            if (!cmdlineFile) {
                continue;
            }
            char cmdline[256];
            if (fgets(cmdline, sizeof(cmdline), cmdlineFile) != NULL && strcmp(cmdline, processName) == 0) {
                pid = strtol(entry->d_name, NULL, 10L);
                fclose(cmdlineFile);
                break;
            }
            fclose(cmdlineFile);
        }
        closedir(dir);
    }
    return pid;
}

static size_t GetPssByPid(int pid)
{
    size_t pss = 0;
    char line[512];
    char filename[128];
    (void)snprintf_s(filename, sizeof(filename), _TRUNCATE, "/proc/%d/smaps_rollup", pid);
    FILE* fp = fopen(filename, "rt");
    if (!fp) {
        return 0;
    }

    for (int i = 0; i < 3L; i++) {
        if (!fgets(line, sizeof(line), fp)) {
            fclose(fp);
            return 0;
        }
    }
    (void)sscanf_s(line, "Pss:%u", &pss);
    (void)fclose(fp);
    return pss;
}

static int GetIpAndMaskByIfname(const char* ifname, char* ip, char* mask)
{
    struct ifaddrs* ifaddr;
    struct ifaddrs* ifa;
    int s;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }
        if (strcmp(ifa->ifa_name, ifname) == 0 && ifa->ifa_addr->sa_family == AF_INET) {
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), ip, 256L, NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                freeifaddrs(ifaddr);
                return -1;
            }

            s = getnameinfo(ifa->ifa_netmask, sizeof(struct sockaddr_in), mask, 256L, NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                freeifaddrs(ifaddr);
                return -1;
            }

            freeifaddrs(ifaddr);
            return 0;
        }
    }

    freeifaddrs(ifaddr);
    return -1;
}

static uint32_t IpToUint(char* ip)
{
    uint32_t result = 0;
    char* token = strtok(ip, ".");
    int i = 24;

    while (token != NULL) {
        result |= (atoi(token) << i);
        i -= 8L;
        token = strtok(NULL, ".");
    }

    return result;
}

static int GetIfnameByIp(const char* ip, char ifname[128])
{
    struct ifaddrs* ifaddr;
    struct ifaddrs* ifa;
    int ret = -1;
    char ipTmp[256];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }
        if (!strcmp(ifa->ifa_name, "lo")) {
            continue;
        }
        getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_storage), ipTmp, sizeof(ipTmp), NULL, 0, NI_NUMERICHOST);
        if (!strcasecmp(ip, ipTmp)) {
            snprintf_s(ifname, sizeof(*ifname), _TRUNCATE, "%s", ifa->ifa_name);
            ret = 0;
            break;
        }
    }

    freeifaddrs(ifaddr);
    return ret;
}

static int GetMacAddr(const char* ifname, unsigned char mac[6])
{
    struct ifreq ifr;
    int sock;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("socket() failed.\n");
        return -1;
    }
    strncpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), ifname, sizeof(ifr.ifr_name) - 1);
    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) {
        printf("ioctl(%s, SIOCGIFHWADDR) failed.\n", ifr.ifr_name);
        close(sock);
        return -1;
    }
    close(sock);
    memcpy_s(mac, sizeof(*mac), ifr.ifr_hwaddr.sa_data, sizeof(*mac));
    return 0;
}

static int PreadValue(const char* cmd, const char* key, char* value[128])
{
    int ret = 0;
    return ret;
}

static int GetSysinfo(char sn[128], char hwVersion[128], char swVersion[128], char deviceModel[128])
{
    int ret = 0;
    ret |= PreadValue("write_sn -d", "sn", &sn);
    if (!ret) {
        StripQuotation(sn);
    }

    ret |= PreadValue("ipcdb", "hardware_version", &hwVersion);
    ret |= PreadValue("ipcdb", "software_version", &swVersion);
    ret |= PreadValue("ipcdb", "deviceModel", &deviceModel);
    return ret;
}

static int IsStaticIp(void)
{
    return (access("/tmp/use_static_ip", R_OK) == 0);
}

static int CheckUpgradefileMD5(const char* upgradefileMd5)
{
    char command[128] = { 0 };
    char result[128] = { 0 };
    char* md5 = NULL;
    (void)snprintf_s(command, sizeof(command), _TRUNCATE, "md5sum %s", "/cache/package.efw");
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        LOG("Failed to execute md5sum command.\n");
        return -1;
    }
    if (!fgets(result, sizeof(result), pipe)) {
        LOG("Failed to read md5sum result.\n");
        pclose(pipe);
        return -1;
    }
    pclose(pipe);
    md5 = strtok(result, " ");
    if (md5 && !strcmp(md5, upgradefileMd5)) {
        printf("MD5 match.\n");
        return 0;
    } else {
        printf("MD5 mismatch.\n");
        return -1;
    }
}

static inline int CheckSystem(const char* para)
{
    return 0;
}

static inline int UpgradeSystem(void)
{
    return 0;
}

static void UpgradeTask(void* arg)
{
    char cmd[256] = { 0 };
    int ret;

    // download and check package.
    g_sUpgradeStatus = UPGRADE_STATUS_DOWNLOADING;
    ret = CheckSystem(g_sUpgradeStatus);
    if (ret != 0) {
        LOG("Error: download package failed. ret=%d", ret);
        g_sUpgradeStatus = (ret == 254L) ? UPGRADE_STATUS_ERR_CHECK : UPGRADE_STATUS_ERR_DOWNLOAD;
        return;
    }

    g_sUpgradeStatus = UPGRADE_STATUS_UPDATING;
    LOG("Ready to reboot for upgrade...");
    // reserve time for informing LanUpgrade tools.
    sleep(3L);

    // upgrade
    if (strlen(g_upgradefileMD5)) {
        if (CheckUpgradefileMD5(g_upgradefileMD5) < 0) {
            LOG("Error: upgrade file exception.");
            g_sUpgradeStatus = UPGRADE_STATUS_ERR_UPDATE;
            return;
        }
    }

    ret = UpgradeSystem();
    if (ret != 0) {
        LOG("Error: upgrade failed.");
        g_sUpgradeStatus = UPGRADE_STATUS_ERR_UPDATE;
        return;
    }
}

static int LaunchUpgrade(const char* url)
{
    char newHwVersion[32] = { 0 };
    char newSwVersion[32] = { 0 };
    char newDeviceModel[32] = { 0 };
    int ret;
    pthread_t tid;

    if (g_sUpgradeStatus == UPGRADE_STATUS_DOWNLOADING || g_sUpgradeStatus == UPGRADE_STATUS_CHECKING ||
        g_sUpgradeStatus == UPGRADE_STATUS_UPDATING) {
        LOG("WARN: is upgrading...\n");
        return -1;
    } else if (g_sUpgradeStatus >= UPGRADE_STATUS_ERR && !strcmp(g_sUpgradeUrl, url)) {
        LOG("Error: refuse to upgrade again, maybe package was invalid...\n");
        return -1;
    }

    g_sUpgradeStatus = UPGRADE_STATUS_IDLE;

    if (!newHwVersion[0] || strcmp(newHwVersion, g_curHwVersion) || !newSwVersion[0] ||
        !strcmp(newSwVersion, g_curSwVersion)) {
        LOG("no need to upgrade.");
        return -1;
    }

    (void)snprintf_s(g_sUpgradeUrl, sizeof(g_sUpgradeUrl), _TRUNCATE, "%s", url);

    LOG("Ready to upgrade: %s", g_sUpgradeUrl);

    ret = pthread_create(&tid, NULL, (void* (*)(void*))UpgradeTask, NULL);
    if (ret != 0) {
        LOG("Error: create upgrade thread failed.");
        return -1;
    }
    pthread_detach(tid);

    return 0;
}

static int GetHostIpByUdpConnect(int sock, struct sockaddr_storage* peerAddr, char ip[256])
{
    struct sockaddr_storage name;
    socklen_t namelen = sizeof(struct sockaddr_storage);
    int ret = -1;
    struct sockaddr_storage addrUnspec = { 0 };

    if (connect(sock, (struct sockaddr*)peerAddr, sizeof(struct sockaddr_storage)) < 0) {
        LOG("Error: connect failed(%s).", strerror(errno));
        return -1;
    }

    if (getsockname(sock, (struct sockaddr*)&name, &namelen) < 0) {
        LOG("Error: getsockname failed(%s).", strerror(errno));
        ret = -1; // 设置 ret 的值
    } else {
        char ipTmp[256] = { 0 };
        if (name.ss_family == AF_INET) {
            struct sockaddr_in* addrV4 = (struct sockaddr_in*)&name;
            snprintf_s(ipTmp, sizeof(ipTmp), _TRUNCATE, "%s", inet_ntoa(addrV4->sin_addr));
        } else {
            struct sockaddr_in6* addrV6 = (struct sockaddr_in6*)&name;
            inet_ntop(AF_INET6, &addrV6->sin6_addr, ipTmp, sizeof(ipTmp));
        }

        if (!strncasecmp(ipTmp, "::ffff:", 7L)) {
            snprintf_s(ip, sizeof(ipTmp), _TRUNCATE, "%s", ipTmp + 7L);
        } else {
            snprintf_s(ip, sizeof(ipTmp), _TRUNCATE, "%s", ipTmp);
        }

        ret = 0; // 设置 ret 的值
    }

    addrUnspec.ss_family = AF_UNSPEC;
    connect(sock, (struct sockaddr*)&addrUnspec,
        sizeof(addrUnspec)); // 不能断开连接

    return ret; // 返回 ret 的值
}

static int GetHostIpByConnect(struct sockaddr_storage* peerAddr, char ip[256])
{
    int ret;
    int sock = socket(peerAddr->ss_family, SOCK_DGRAM, 0);
    if (sock < 0) {
        LOG("Error: socket failed(%s).", strerror(errno));
        return -1;
    }

    ret = GetHostIpByUdpConnect(sock, peerAddr, ip);
    close(sock);

    return ret;
}

static int TxtFileRead(const char* filename, char* buf, size_t size)
{
    FILE* fp = fopen(filename, "rb");
    size_t readbytes;

    if (!fp) {
        LOG("Error: fopen(%s) failed.\n", filename);
        return -1;
    }

    readbytes = fread(buf, 1, size - 1, fp);
    (void)fclose(fp);
    buf[readbytes] = '\0';
    return 0;
}

static char* BuildDevInfoStr(char* hostIp, uint8_t hostMac[6])
{
    static char devInfoStr[512];
    char uptime[64] = { 0 };
    char thermalZone0Temp[64] = { 0 };
    char thermalZone1Temp[64] = { 0 };

    TxtFileRead("/proc/uptime", uptime, sizeof(uptime));
    char* ptr = strchr(uptime, ' ');
    if (ptr) {
        *ptr = '\0';
    }
    TxtFileRead("/sys/class/thermal/thermal_zone0/temp", thermalZone0Temp, sizeof(thermalZone0Temp));
    TxtFileRead("/sys/class/thermal/thermal_zone1/temp", thermalZone1Temp, sizeof(thermalZone1Temp));

    float temperature =
        (atoi(thermalZone0Temp) > atoi(thermalZone1Temp) ? atoi(thermalZone0Temp) : atoi(thermalZone1Temp)) / 1000.0;

    int ipcameraPid = GetPidByName("ipcamera");
    size_t ipcameraPss = ipcameraPid ? GetPssByPid(ipcameraPid) : 0;

    (void)snprintf_s(devInfoStr, sizeof(devInfoStr), _TRUNCATE,
        "{\"ip\":\"%s\", \"mac\":\"%02X:%02X:%02X:%02X:%02X:%02X\", "
        "\"sn\":\"%s\", "
        "\"hw_version\":\"%s\", \"sw_version\":\"%s\", \"device_model\":\"%s\", "
        "\"static_ip\":\"%d\", \"upgrade_status\":\"%d\", \"uptime\":\"%s\", "
        "\"temperature\":\"%.2f°C\", \"ipcameraPss\":\"%u (kB)\"}",
        hostIp, hostMac[0], hostMac[1], hostMac[2L], hostMac[3L], hostMac[4L], hostMac[5L], g_serialNo, g_curHwVersion,
        g_curSwVersion, g_deviceModel, IsStaticIp(), g_sUpgradeStatus, uptime, temperature, ipcameraPss);

    return devInfoStr;
}

static int SendDevInfo(int sock, struct sockaddr_storage* peerAddr)
{
    int ret;
    char* sendMsg;
    char ifname[128] = { 0 };
    char hostIp[256] = { 0 };
    unsigned char hostMac[6] = { 0 };

    if (GetHostIpByConnect(peerAddr, hostIp) || GetIfnameByIp(host_ip, ifname)) {
        LOG("Error: get hostip & ifname failed.");
        return -1;
    }

    GetMacAddr(ifname, hostMac);

    sendMsg = BuildDevInfoStr(hostIp, hostMac);

    ret = sendto(sock, sendMsg, strlen(sendMsg), 0, (struct sockaddr*)peerAddr, sizeof(struct sockaddr_storage));
    if (ret <= 0) {
        LOG("Error: sendto failed (%s)", strerror(errno));
        return -1;
    }

    return 0;
}

static int SetIpIoctl(const char* interface, const char* ipSet, const char* maskSet)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        // 处理错误
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr;
    struct ifreq ifr;

    memset_s(&ifr, sizeof(ifr), 0, sizeof(ifr));
    strncpy_s(ifr.ifr_name, IFNAMSIZ, interface, IFNAMSIZ - 1);

    // 设置 IP 地址
    inet_aton(ipSet, &addr.sin_addr);
    memcpy_s(&ifr.ifr_addr, sizeof(struct sockaddr_in), &addr, sizeof(struct sockaddr_in));
    if (ioctl(sockfd, SIOCSIFADDR, &ifr) == -1) {
        // 处理错误
        perror("SIOCSIFADDR");
        close(sockfd);
        return -1;
    }

    // 设置子网掩码
    inet_aton(maskSet, &addr.sin_addr);
    memcpy_s(&ifr.ifr_netmask, sizeof(struct sockaddr_in), &addr, sizeof(struct sockaddr_in));
    if (ioctl(sockfd, SIOCSIFNETMASK, &ifr) == -1) {
        // 处理错误
        perror("SIOCSIFNETMASK");
        close(sockfd);
        return -1;
    }

    close(sockfd);
    return 0;
}

static void ProcessDirectConnection(const char* recvBuf)
{
    char* token = strtok(recvBuf, " ");
    char* ipSet = NULL;
    char* maskSet = NULL;

    while (token != NULL) {
        bool tmp = strncmp(token, "IP", strlen("IP"));
        if (!tmp) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                ipSet = token;
            }
        } else if (!strncmp(token, "MASK", strlen("MASK"))) {
            token = strtok(NULL, " ");
            if (token != NULL) {
                maskSet = token;
            }
        }
        token = strtok(NULL, " ");
    }

    if (ipSet != NULL && maskSet != NULL) {
        LOG("[ DIRECT CONNECTION SET ] IP: %s, Mask: %s\n", ipSet, maskSet);
    } else {
        LOG("[ DIRECT CONNECTION SET ] Failed to parse IP and mask\n");
        continue;
    }

    if (!GetIpAndMaskByIfname("eth0", ip, mask)) {
        uint32_t ipCurrentNum = IpToUint(ip);
        uint32_t ipSetNum = IpToUint(ipSet);
        uint32_t maskCurrentNum = IpToUint(mask);
        uint32_t maskSetNum = IpToUint(maskSet);
        if ((ipCurrentNum & maskCurrentNum) != (ipSetNum & maskSetNum)) {
            SetIpIoctl("eht0", ipSet, maskSet);
        }
    } else {
        SetIpIoctl("eth0", ipSet, maskSet);
    }
}

static void MonitorTask(void* arg)
{
    int sock = *((int*)arg);
    struct sockaddr_storage clientAddr;
    int recvLen = 0;
    char recvBuf[4096];
    socklen_t saLen = 0;
    char ip[256] = { 0 };
    char mask[256] = { 0 };

    LOG("Enter MonitorTask.");

    if (GetSysinfo(g_serialNo, g_curHwVersion, g_curSwVersion, g_deviceModel)) {
        LOG("GetSysinfo() failed.");
    }

    LOG("g_serialNo=%s", g_serialNo);
    LOG("g_curHwVersion=%s", g_curHwVersion);
    LOG("g_curSwVersion=%s", g_curSwVersion);

    while (g_sRunFlag) {
        memset_s(&clientAddr, sizeof(clientAddr), 0, sizeof(clientAddr));
        sa_len = sizeof(clientAddr);
        recvLen = recvfrom(sock, recvBuf, sizeof(recvBuf), 0, (struct sockaddr*)&clientAddr, &sa_len);
        if (recvLen < 0) {
            LOG("Error: recvfrom failed(%s)", strerror(errno));
            if (errno == EINTR || errno == EAGAIN || errno == ETIMEDOUT) {
                usleep(10L * 1000L);
                continue;
            }
            LOG("Error: exit loop for error(%s)", strerror(errno));
            break;
        } else if (recvLen == 0) {
            LOG("Receive zero-length datagrams. Maybe socket was shutdowned.");
            usleep(10L * 1000L);
            continue;
        }

        recvLen = recvrecvLen_len >= sizeof(recvBuf) ? (recv_len - 1) : recv_len;
        recvBuf[recvLen] = '\0';

        if (!strcmp(recvBuf, "DISCOVER IPCAMERA")) {
            SendDevInfo(sock, &clientAddr);
        } else if (!strncmp(recvBuf, "UPGRADE IPCAMERA", strlen("UPGRADE IPCAMERA"))) {
            if (SendDevInfo(sock, &clientAddr) == 0) {
                char url[512] = { 0 };
                snprintf_s(url, sizeof(url), _TRUNCATE, "%s", recvBuf + strlen("UPGRADE IPCAMERA "));
                LaunchUpgrade(url);
            }
        } else if (!strncmp(recvBuf, "DIRECT CONNECTION SET", strlen("DIRECT CONNECTION SET"))) {
            ProcessDirectConnection(recvBuf);
        }
        usleep(10L * 1000L);
    }

    LOG("Exit MonitorTask.");
}

static int JoinGroupOnIf(int sock, int ifIndex)
{
    int ret;
    struct ip_mreqn groupV4 = { 0 };
    struct ipv6_mreq groupV6 = { 0 };

    // add to ipv4 multicast group
    groupV4.imr_address.s_addr = INADDR_ANY;
    groupV4.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR_V4);
    groupV4.imr_ifindex = ifIndex;
    ret = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &groupV4, sizeof(groupV4));
    if (ret < 0) {
        LOG("Error: Adding multicast group v4 error(%s)", strerror(errno));
        return -1;
    }
    LOG("[%d] Add to mulitcast Group : %s", ifIndex, MULTICAST_ADDR_V4);

    // add to ipv6 multicast group
    groupV6.ipv6mr_interface = ifIndex;
    inet_pton(AF_INET6, MULTICAST_ADDR_V6, &groupV6.ipv6mr_multiaddr);
    ret = setsockopt(sock, IPPROTO_IPV6, IPV6_JOIN_GROUP, &groupV6, sizeof(groupV6));
    if (ret < 0) {
        LOG("Error: Adding multicast group v6 error(%s)", strerror(errno));
        return -1;
    }
    LOG("[%d] Add to mulitcast Group : %s", ifIndex, MULTICAST_ADDR_V6);

    return 0;
}

static int LeaveGroupOnIf(int sock, int ifIndex)
{
    int ret;
    struct ip_mreqn groupV4 = { 0 };
    struct ipv6_mreq groupV6 = { 0 };

    // add to ipv4 multicast group
    groupV4.imr_address.s_addr = INADDR_ANY;
    groupV4.imr_multiaddr.s_addr = inet_addr(MULTICAST_ADDR_V4);
    groupV4.imr_ifindex = ifIndex;
    ret = setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, &groupV4, sizeof(groupV4));
    if (ret < 0) {
        LOG("Error: Leave multicast group v4 error(%s)", strerror(errno));
        return -1;
    }

    LOG("[%d] Leave mulitcast Group : %s", ifIndex, MULTICAST_ADDR_V4);

    // add to ipv6 multicast group
    groupV6.ipv6mr_interface = ifIndex;
    inet_pton(AF_INET6, MULTICAST_ADDR_V6, &groupV6.ipv6mr_multiaddr);
    ret = setsockopt(sock, IPPROTO_IPV6, IPV6_LEAVE_GROUP, &groupV6, sizeof(groupV6));
    if (ret < 0) {
        LOG("Error: Adding multicast group v6 error(%s)", strerror(errno));
        return -1;
    }
    LOG("[%d] Leave mulitcast Group : %s", ifIndex, MULTICAST_ADDR_V6);

    return 0;
}

static inline int IsValidIfname(const char* ifName)
{
    return !strncasecmp(ifName, "eth", 3L) || !strncasecmp(ifName, "wlan", 4L);
}

static int MulticastSocketInit(void)
{
    int ret;
    struct sockaddr_in6 addrIn6;
    int sock;

    sock = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sock < 0) {
        LOG("Error: open event socket failed: %d", sock);
        return -1;
    }
    int reuseFlag = 1;
    int loop = 0;
    int ipv6only = 0;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseFlag, sizeof(reuseFlag));
    setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
    setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &loop, sizeof(loop));
    setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &ipv6only, sizeof(ipv6only));

    // bind to local addr
    bzero(&addrIn6, sizeof(addrIn6));
    addrIn6.sin6_family = AF_INET6;
    addrIn6.sin6_addr = in6addr_any;
    addrIn6.sin6_port = htons(MULTICAST_PORT);

    ret = bind(sock, (struct sockaddr*)&addrIn6, sizeof(struct sockaddr_in6));
    if (ret < 0) {
        LOG("Error: bind lan_upgrade socket(%d) failed(%s)", sock, strerror(errno));
        close(sock);
        return -1;
    }
    // join multicast group
    struct if_nameindex* interfaces = if_nameindex();
    for (struct if_nameindex* i = interfaces; i && i->if_index && i->if_name; i++) {
        LOG("IFACE: [%u, %s]", i->if_index, i->if_name);
        if (IsValidIfname(i->if_name)) {
            LOG("ADD group on: %s", i->if_name);
            if (JoinGroupOnIf(sock, i->if_index)) {
                close(sock);
                if_freenameindex(interfaces);
                return -1;
            }
        }
    }
    if_freenameindex(interfaces);

    g_sSock = sock;
    g_sRunFlag = 1;
    ret = pthread_create(&g_sTid, NULL, (void* (*)(void*))MonitorTask, (void*)&g_sSock);
    if (ret != 0) {
        LOG("Error: pthread_create failed.");
        close(g_sSock);
        g_sSock = -1;
        return -1;
    }
    return 0;
}

static int MulticastSocketDeinit(void)
{
    if (g_sSock < 0) {
        return 0;
    }

    g_sRunFlag = 0;
    shutdown(g_sSock, 0);
    pthread_join(g_sTid, NULL);

    // leave multicast group
    struct if_nameindex* interfaces = if_nameindex();
    for (struct if_nameindex* i = interfaces; i && i->if_index && i->if_name; i++) {
        LOG("IFACE: [%u, %s]", i->if_index, i->if_name);
        if (IsValidIfname(i->if_name)) {
            LOG("Leave group on: %s", i->if_name);
            LeaveGroupOnIf(g_sSock, i->if_index);
        }
    }
    if_freenameindex(interfaces);

    close(g_sSock);
    g_sSock = -1;

    return 0;
}

static int MulticastSocketCheck(void)
{
    int error;
    int ret;
    socklen_t len;

    if (g_sSock < 0) {
        return -1;
    }

    len = sizeof(error);
    ret = getsockopt(g_sSock, SOL_SOCKET, SO_ERROR, &error, &len);
    if (ret < 0 || error != 0) {
        return -1;
    }

    return 0;
}

static void NnlEventHandler(NNL_EVENT_E event, void* param)
{
    if (event == NNL_EVENT_IP_ADDR_CHANGED) {
        g_sRestartFlag = 1;
    }
}

int main(int argc, const char* argv[])
{
    void (*prevHandler)(int);

    prevHandler = signal(SIGPIPE, SIG_IGN);

    LOG("START lan_upgarde.");

    netlink_monitor_start(NnlEventHandler);

    while (g_mainFlag) {
        if (MulticastSocketCheck() < 0 || g_sRestartFlag) {
            g_sRestartFlag = 0;
            PreadValue();
            MulticastSocketInit();
        }
        sleep(1);
    }
    return 0;
}