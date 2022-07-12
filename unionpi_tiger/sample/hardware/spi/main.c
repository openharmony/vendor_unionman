/*
 * Minimal BPF JIT image disassembler
 *
 * Disassembles BPF JIT compiler emitted opcodes back to asm insn's for
 * debugging or verification purposes.
 *
 * To get the disassembly of the JIT code, do the following:
 *
 *  1) `echo 2 > /proc/sys/net/core/bpf_jit_enable`
 *  2) Load a BPF filter (e.g. `tcpdump -p -n -s 0 -i eth1 host 192.168.20.0/24`)
 *  3) Run e.g. `bpf_jit_disasm -o` to read out the last JIT code
 *
 * Copyright 2013 Daniel Borkmann <borkmann@redhat.com>
 * Licensed under the GNU General Public License, version 2.0 (GPLv2)
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

static unsigned char mode = SPI_MODE_0;
static unsigned char bits = 8;
static unsigned int speed = 1000000;

static int cs_gpio = 486;
#define NUM_MAXBUF 4
#define DIR_MAXSIZ 60
#define IN 0
#define OUT 1

#define LOW 0
#define HIGH 1

int SYSFS_GPIO_Export(int s32GpioNum, int bExport)
{
    int ret = 0;
    char buffer[256];

    if (bExport) {
        snprintf(buffer, sizeof(buffer), "echo %d > /sys/class/gpio/export", s32GpioNum);
    } else {
        snprintf(buffer, sizeof(buffer), "echo %d > /sys/class/gpio/unexport", s32GpioNum);
    }

    printf("shl %s\n", buffer);

    system(buffer);

    return ret;
}

int SYSFS_GPIO_Direction(int Pin, int Dir)
{
    const char dir_str[] = "in\0out";
    char path[DIR_MAXSIZ];
    int fd;

    snprintf(path, DIR_MAXSIZ, "/sys/class/gpio/gpio%d/direction", Pin);
    fd = open(path, O_WRONLY);
    if (fd < 0) {
        printf("Set Direction failed: Pin%d\n", Pin);
        return -1;
    }

    if (write(fd, &dir_str[Dir == IN ? 0 : 3], Dir == IN ? 2 : 3) < 0) {
        printf("failed to set direction!\r\n");
        return -1;
    }

    close(fd);
    return 0;
}

int SYSFS_GPIO_Read(int Pin)
{
    char path[DIR_MAXSIZ];
    char value_str[3];
    int fd;

    snprintf(path, DIR_MAXSIZ, "/sys/class/gpio/gpio%d/value", Pin);
    fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("Read failed Pin%d\n", Pin);
        return -1;
    }

    if (read(fd, value_str, 3) < 0) {
        printf("failed to read value!\n");
        return -1;
    }

    close(fd);
    return (atoi(value_str));
}

int SYSFS_GPIO_Write(int Pin, int value)
{
    const char s_values_str[] = "01";
    char path[DIR_MAXSIZ];
    int fd;

    snprintf(path, DIR_MAXSIZ, "/sys/class/gpio/gpio%d/value", Pin);
    fd = open(path, O_WRONLY);
    if (fd < 0) {
        printf("Write failed : Pin%d,value = %d\n", Pin, value);
        return -1;
    }

    if (write(fd, &s_values_str[value == LOW ? 0 : 1], 1) < 0) {
        printf("failed to write value!\n");
        return -1;
    }

    close(fd);
    return 0;
}

int SPI_open()
{
    int fd, ret;
    fd = open("/dev/spidev32766.0", O_RDWR);
    // printf("*************************************************fd = %d", fd);

    if (fd < 0) {
        printf("can not open SPI device\n");
    }

    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
        printf("can't set bits per word");

    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
        printf("can't get bits per word");

    // mode &= 0xfC;
    // mode |= SPI_LSB_FIRST;
    // mode &= ~SPI_CS_HIGH;
    // mode &= ~SPI_NO_CS;

    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
        printf("can't set spi mode");

    mode &= ~0x04;
    mode &= ~0x40;
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
        printf("can't set spi mode 1");

    mode &= ~SPI_LSB_FIRST;
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
        printf("can't set spi mode 2");

    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
        printf("can't get spi mode");

    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        printf("can't set max speed hz");

    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        printf("can't get max speed hz");

    // printf("spi mode: %d\n", mode);
    // printf("bits per word: %d\n", bits);
    // printf("max speed: %d Hz (%d MHz)\n", speed, speed/1000000);

    return fd;
}

int SPI_close(int dev_fh)
{
    return close(dev_fh);
}

int SPI_readFlashID()
{
    int ret;
    int fd;
    uint8_t readIDCmd[4];
    uint8_t dataID[4] = {0};

    fd = SPI_open();
    if (fd < 0) {
        printf("fail open device ==================fd = %d", fd);
        return -1;
    }
    struct spi_ioc_transfer xfer;
    memset(&xfer, 0, sizeof(xfer));

    readIDCmd[0] = 0x9f;
    readIDCmd[1] = 0x0;
    readIDCmd[2] = 0x0;
    readIDCmd[3] = 0x0;
    xfer.tx_buf = (unsigned long)readIDCmd;
    xfer.rx_buf = (unsigned long)dataID;
    xfer.len = 4;

    SYSFS_GPIO_Write(cs_gpio, 0);

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);
    if (ret < 0) {
        printf("********************Error: SPI_IOC_MESSAGE\n");
        return -1;
    }

    SYSFS_GPIO_Write(cs_gpio, 1);

    printf("dataID[0]************************************************* = %x\n", dataID[0]);
    printf("dataID[1]************************************************* = %x\n", dataID[1]);
    printf("dataID[2]************************************************* = %x\n", dataID[2]);
    printf("dataID[3]************************************************* = %x\n", dataID[3]);

    SPI_close(fd);

    return ret;
}

int main(int argc, char **argv)
{
    SYSFS_GPIO_Export(cs_gpio, 1);
    SYSFS_GPIO_Direction(cs_gpio, OUT);
    SPI_readFlashID();
    SYSFS_GPIO_Export(cs_gpio, 0);

    return 0;
}
