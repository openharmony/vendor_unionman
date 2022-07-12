#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#include "um_spi.h"

static uint8_t bits = 8;
HARDWARE_SPI hardware_SPI;
char *default_dev = DEFAULT_DEV;

struct spi_ioc_transfer tr;

int DEV_HARDWARE_SPI_begin()
{
    int ret = SPI_SUCCESS;
    if ((hardware_SPI.fd = open(default_dev, O_RDWR)) < 0) {
        printf("Failed to open SPI device.\n");
        ret = SPI_OPEN_DEV_FAIL;
        hardware_SPI.fd = 0;
    } else {
        printf("open : %s", default_dev);
        if (ioctl(hardware_SPI.fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) {
            printf("can't set bits per word\r\n");
            return SPI_SET_MODE_FAIL;
        }
    }

    return ret;
}

int DEV_HARDWARE_SPI_end(void)
{
    int ret = SPI_SUCCESS;
    if (hardware_SPI.fd <= 0) {
        return SPI_NOT_OPENED_FAIL;
    }
    hardware_SPI.mode = 0;
    close(hardware_SPI.fd);
    return ret;
}

int DEV_HARDWARE_SPI_Mode(int mode)
{

    if (SPI_MODE_0 != mode && SPI_MODE_1 != mode && SPI_MODE_2 != mode && SPI_MODE_3 != mode) {
        return SPI_PARAM_FAIL;
    }

    if (hardware_SPI.fd <= 0) {
        return SPI_NOT_OPENED_FAIL;
    }

    hardware_SPI.mode &= 0xfC; // Clear low 2 digits
    hardware_SPI.mode |= mode; // Setting mode

    // Write device
    if (ioctl(hardware_SPI.fd, SPI_IOC_WR_MODE, &hardware_SPI.mode) == -1) {
        printf("can't set spi mode");
        return SPI_SET_MODE_FAIL;
    }
    return SPI_SUCCESS;
}

int DEV_HARDWARE_SPI_GET_Mode(int *mode)
{

    if (hardware_SPI.fd <= 0) {
        return SPI_NOT_OPENED_FAIL;
    }

    int tmp = 0;
    int ret = ioctl(hardware_SPI.fd, SPI_IOC_RD_MODE, &tmp);
    if (ret == -1)
        return SPI_GET_MODE_FAIL;

    printf("DEV_HARDWARE_SPI_GET_Mode = %x", tmp);
    *mode = tmp & 0x3;

    return SPI_SUCCESS;
}

int DEV_HARDWARE_SPI_SET_BitOrder(int order)
{

    if (SPI_BIT_ORDER_LSBFIRST != order && SPI_BIT_ORDER_MSBFIRST != order) {
        return SPI_PARAM_FAIL;
    }

    if (hardware_SPI.fd <= 0) {
        return SPI_NOT_OPENED_FAIL;
    }

    ioctl(hardware_SPI.fd, SPI_IOC_RD_MODE, &hardware_SPI.mode);

    if (order == SPI_BIT_ORDER_LSBFIRST) {
        hardware_SPI.mode |= SPI_LSB_FIRST;
        printf("SPI_LSB_FIRST");
    } else if (order == SPI_BIT_ORDER_MSBFIRST) {
        hardware_SPI.mode &= ~SPI_LSB_FIRST;
        printf("SPI_MSB_FIRST");
    }

    int fd = ioctl(hardware_SPI.fd, SPI_IOC_WR_MODE, &hardware_SPI.mode);
    printf("fd = %d\r\n", fd);
    if (fd == -1) {
        printf("can't set spi SPI_LSB_FIRST\r\n");
        return SPI_SET_MODE_FAIL;
    }

    return SPI_SUCCESS;
}

int DEV_HARDWARE_SPI_GET_BitOrder(int *order)
{

    if (hardware_SPI.fd <= 0) {
        return SPI_NOT_OPENED_FAIL;
    }

    int tmp = 0;
    int ret = ioctl(hardware_SPI.fd, SPI_IOC_RD_MODE, &tmp);
    if (ret == -1)
        return SPI_GET_MODE_FAIL;

    // printf("2 DEV_HARDWARE_SPI_GET_BitOrder = %x", tmp);
    if (((tmp & 0x08) >> 3) == 0) {
        *order = 1; // msb
    } else {
        ;
        *order = 0; // lsb
    }

    return SPI_SUCCESS;
}

int DEV_HARDWARE_SPI_SET_ChipSelect(int CS_Mode)
{

    if (hardware_SPI.fd <= 0) {
        return SPI_NOT_OPENED_FAIL;
    }

    ioctl(hardware_SPI.fd, SPI_IOC_RD_MODE, &hardware_SPI.mode);

    if (CS_Mode == SPI_CS_Mode_HIGH) {
        hardware_SPI.mode |= SPI_CS_HIGH;
        hardware_SPI.mode &= ~SPI_NO_CS;
        printf("CS HIGH ");
    } else if (CS_Mode == SPI_CS_Mode_LOW) {
        hardware_SPI.mode &= ~SPI_CS_HIGH;
        hardware_SPI.mode &= ~SPI_NO_CS;
        printf("CS LOW ");
    } else if (CS_Mode == SPI_CS_Mode_NONE) {
        hardware_SPI.mode |= SPI_NO_CS;
        printf("NO CS");
    } else {
        return SPI_PARAM_FAIL;
    }

    if (ioctl(hardware_SPI.fd, SPI_IOC_WR_MODE, &hardware_SPI.mode) == -1) {
        printf("can't set spi mode\r\n");
        return SPI_SET_MODE_FAIL;
    }
    return SPI_SUCCESS;
}

int DEV_HARDWARE_SPI_GET_ChipSelect(int *CS_Mode)
{

    if (hardware_SPI.fd <= 0) {
        return SPI_NOT_OPENED_FAIL;
    }

    int tmp = 0;
    int ret = ioctl(hardware_SPI.fd, SPI_IOC_RD_MODE, &tmp);
    if (ret == -1)
        return SPI_GET_MODE_FAIL;

    printf("DEV_HARDWARE_SPI_GET_ChipSelect = %x", tmp);
    if ((tmp >> 6) & 0x1 == 1) {
        *CS_Mode = SPI_CS_Mode_NONE;
    } else if ((tmp >> 2) & 0x1 == 1) {
        *CS_Mode = SPI_CS_Mode_HIGH;
    } else {
        *CS_Mode = SPI_CS_Mode_LOW;
    }

    return SPI_SUCCESS;
}

int DEV_HARDWARE_SPI_SET_SPEED(int speed)
{

    if (hardware_SPI.fd <= 0) {
        return SPI_NOT_OPENED_FAIL;
    }

    // Write speed
    if (ioctl(hardware_SPI.fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) {
        printf("can't set max speed hz\r\n");

        return SPI_SET_MODE_FAIL;
    }

    return SPI_SUCCESS;
}

int DEV_HARDWARE_SPI_GET_SPEED(int *speed)
{

    if (hardware_SPI.fd <= 0) {
        return SPI_NOT_OPENED_FAIL;
    }

    // Read the speed of just writing
    int tmp;
    if (ioctl(hardware_SPI.fd, SPI_IOC_RD_MAX_SPEED_HZ, &tmp) == -1) {
        printf("can't get max speed hz");
        return SPI_GET_MODE_FAIL;
    }

    *speed = tmp;

    return SPI_SUCCESS;
}

int DEV_HARDWARE_SPI_transferFullDuplex(unsigned char *txbuf, int len, unsigned char *rxbuf)
{

    if (hardware_SPI.fd <= 0) {
        return SPI_NOT_OPENED_FAIL;
    }

#if 0
    int i = 0;
    for(i = 0; i < len; i++){
        printf("transferFullDuplex %x ", txbuf[i]);
    }

#endif

    struct spi_ioc_transfer xfer;
    memset(&xfer, 0, sizeof(xfer));

    xfer.len = len;
    xfer.tx_buf = (unsigned long)txbuf;
    xfer.rx_buf = (unsigned long)rxbuf;

    // ioctl Operation, transmission of data
    if (ioctl(hardware_SPI.fd, SPI_IOC_MESSAGE(1), &xfer) < 1) {
        printf("can't send spi message");
        return SPI_TRANSFER_FAIL;
    }

    return SPI_SUCCESS;
}
