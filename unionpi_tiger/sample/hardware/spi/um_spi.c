/*
 * Copyright (c) 2022 Unionman Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#undef LOG_TAG
#define LOG_TAG "spi_test"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <securec.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "hilog/log.h"
#include "um_spi.h"

static unsigned char g_mode = SPI_MODE_0;
static unsigned char g_bits = 8;
static unsigned int g_speed = 1000000;

void sysfs_gpio_export(int s32GpioNum, int bExport)
{
    char buffer[EXP_MAXSIZ];
    if (bExport) {
        (void)snprintf_s(buffer, sizeof(buffer), sizeof(buffer), "echo %d > /sys/class/gpio/export", s32GpioNum);
    } else {
        (void)snprintf_s(buffer, sizeof(buffer), sizeof(buffer), "echo %d > /sys/class/gpio/unexport", s32GpioNum);
    }
    printf("shl %s\n", buffer);
    system(buffer);
}

int sysfs_gpio_direction(int Pin, int Dir)
{
    const char dir_str[] = "in\0out";
    char path[DIR_MAXSIZ];
    int fd = 0;
    // 初始化设置方向的文件名
    (void)snprintf_s(path, sizeof(path), DIR_MAXSIZ, "/sys/class/gpio/gpio%d/direction", Pin);
    // 以只写的方式打开path文件，得到返回值fd
    // 当文件打开成功时，返回一个非负整数：
    // 句柄，我们后面对于文件的读写，关闭等都通过句柄来操作；
    // 句柄可以理解为指向这些文件的指针;
    // 当文件打开失败时，返回-1
    fd = open(path, O_WRONLY);
    if (fd < 0) {
        HILOG_ERROR(LOG_CORE, "Set Direction failed: Pin%d\n", Pin);
        return SPI_OPEN_DIRECTION_FAIL;
    }
    // 成功打开后当Dir=IN即0时，写in到path文件中
    // 当Dir=OUT即1时，写out到path文件中
    // 写失败则报错
    if (write(fd, &dir_str[Dir == IN ? 0 : 3L], Dir == IN ? 2L : 3L) < 0) {
        HILOG_ERROR(LOG_CORE, "failed to set direction!\r\n");
        return SPI_SET_DIRECTION_FAIL;
    }
    // 关闭文件
    close(fd);
    return SPI_SUCCESS;
}

int sysfs_gpio_write(int Pin, int value)
{
    const char s_values_str[] = "01";
    char path[DIR_MAXSIZ];
    int fd = 0;

    (void)snprintf_s(path, sizeof(path), DIR_MAXSIZ, "/sys/class/gpio/gpio%d/value", Pin);
    fd = open(path, O_WRONLY);
    if (fd < 0) {
        HILOG_ERROR(LOG_CORE, "Write failed : Pin%{public}d,value = %{public}d\n", Pin, value);
        return SPI_OPEN_VALUE_FAIL;
    }

    if (write(fd, &s_values_str[value == LOW ? 0 : 1], 1) < 0) {
        HILOG_ERROR(LOG_CORE, "failed to write value!\n");
        return SPI_WRITE_VALUE_FAIL;
    }

    close(fd);
    return SPI_SUCCESS;
}

int spi_open(void)
{
    int fd = 0, ret = 0;
    // 以读写的方式打开
    fd = open(DEFAULT_DEV, O_RDWR);
    if (fd < 0) {
        HILOG_ERROR(LOG_CORE, "can not open SPI device\n");
        return SPI_NOT_OPENED_FAIL;
    }
    // 设置spi接口传输字的比特数为8位
    // SPI_IOC_WR_BITS_PER_WORD或SPI_IOC_RD_BITS_PER_WORD
    // 传递一个指向字节的指针，该字节将返回(RD)或分配(WR)每个SPI传输字的比特数。
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &g_bits);
    if (ret == -1) {
        HILOG_ERROR(LOG_CORE, "can't set bits per word\n");
        return SPI_SET_MODE_FAIL;
    }
    // 得到spi接口传输字的比特数
    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &g_bits);
    if (ret == -1) {
        HILOG_ERROR(LOG_CORE, "can't get bits per word\n");
        return SPI_GET_MODE_FAIL;
    }
    // SPI_MODE_0模式即（0|0）
    // 检测是否四种模式都可以设置以及得到模式值
    ret = ioctl(fd, SPI_IOC_WR_MODE, &g_mode);
    if (ret == -1) {
        HILOG_ERROR(LOG_CORE, "can't set spi mode\n");
        return SPI_SET_MODE_FAIL;
    }
    g_mode &= ~SPI_CS_HIGH;
    g_mode &= ~SPI_NO_CS;
    ret = ioctl(fd, SPI_IOC_WR_MODE, &g_mode);
    if (ret == -1) {
        HILOG_ERROR(LOG_CORE, "can't set spi mode 1\n");
        return SPI_SET_MODE_FAIL;
    }
    g_mode &= ~SPI_LSB_FIRST;
    ret = ioctl(fd, SPI_IOC_WR_MODE, &g_mode);
    if (ret == -1) {
        HILOG_ERROR(LOG_CORE, "can't set spi mode 2\n");
        return SPI_SET_MODE_FAIL;
    }
    ret = ioctl(fd, SPI_IOC_RD_MODE, &g_mode);
    if (ret == -1) {
        HILOG_ERROR(LOG_CORE, "can't get spi mode\n");
        return SPI_GET_MODE_FAIL;
    }
    // 设置传输速率
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &g_speed);
    if (ret == -1) {
        HILOG_ERROR(LOG_CORE, "can't set max speed hz\n");
        return SPI_SET_MODE_FAIL;
    }
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &g_speed);
    if (ret == -1) {
        HILOG_ERROR(LOG_CORE, "can't get max speed hz\n");
        return SPI_GET_MODE_FAIL;
    }
    // 返回/dev/spidev0.0文件的话柄
    return fd;
}

int spi_read_flashid(void)
{
    int ret;
    int len = 4L;
    uint8_t readIDCmd[4];
    uint8_t dataID[4] = {0};

    // 初始化发送的指令：查看flash的id
    readIDCmd[0] = 0x9f;
    readIDCmd[1] = 0x0;
    readIDCmd[2L] = 0x0;
    readIDCmd[3L] = 0x0;

    ret = spi_transfer_fullduplex(readIDCmd, len, dataID);
    if (ret < 0) {
        HILOG_ERROR(LOG_CORE, "can't read FlashID\n");
        return READ_FLASHID_FAIL;
    }
    printf("SPI_readFlashID\n");
    spi_read_data(0, 3L, dataID);

    return ret;
}

int spi_read_deviceid(void)
{
    int ret;
    int len = 6L;
    uint8_t readIDCmd[4];
    uint8_t dataID[6L] = {0};

    // 初始化发送的指令：查看flash的id
    readIDCmd[0] = 0x90;
    readIDCmd[1] = 0x0;
    readIDCmd[2L] = 0x0;
    readIDCmd[3L] = 0x0;

    ret = spi_transfer_fullduplex(readIDCmd, len, dataID);
    if (ret < 0) {
        HILOG_ERROR(LOG_CORE, "can't read DeviceID\n");
        return READ_DEVICEID_FAIL;
    }
    printf("spi_readDeviceID\n");
    spi_read_data(4L, len - 1, dataID);

    return ret;
}

int read_status_register(uint8_t *data, int len)
{
    uint8_t *pData = data;
    uint8_t readIDCmd[1];
    const int minlen = 2;
    int ret;
    // len=1时在传命令，len=2时，才在传状态寄存器中的值
    if (data == NULL) {
        HILOG_ERROR(LOG_CORE, "Data pointer is null\n");
        return SPI_PARAM_FAIL;
    }
    if (len < minlen) {
        HILOG_ERROR(LOG_CORE, "Parameter len error\n");
        return SPI_PARAM_FAIL;
    }
    // 初始化发送的指令
    readIDCmd[0] = 0x05;

    ret = spi_transfer_fullduplex(readIDCmd, len, pData);
    if (ret < 0) {
        HILOG_ERROR(LOG_CORE, "can't read Status Register\n");
        return READ_STATUS_REGISTER_FAIL;
    }
    return ret;
}

int spi_flash_writeenable(void)
{
    int ret = 0;
    int len = 1;
    uint8_t readIDCmd[1];
    uint8_t data[1] = {0};
    uint8_t reData[3L] = {0};
    readIDCmd[0] = 0x06;

    ret = spi_transfer_fullduplex(readIDCmd, len, data);
    if (ret < 0) {
        HILOG_ERROR(LOG_CORE, "can't set write enable\n");
        return SPI_SET_FLASH_WRITE_ENABLE;
    }
    read_status_register(reData, 3L);
    if ((reData[2L] & 0x02) == 0x02) {
        printf("set spi flash write enable success\n");
    } else {
        HILOG_ERROR(LOG_CORE, "can't set spi flash write enable\n");
        return SET_WRITEENABLE_FAIL;
    }

    return ret;
}

int spi_read_data(int first, int last, uint8_t *data)
{
    int i = 0;
    if (data == NULL) {
        HILOG_ERROR(LOG_CORE, "data pointer is null\n");
        return SPI_PARAM_FAIL;
    }
    if (first > last) {
        HILOG_ERROR(LOG_CORE, "********************Error:overflow\n");
        return SPI_PARAM_FAIL;
    }
    for (i = first; i <= last; i++) {
        printf("data[%d]************************************************* = %x\n", i, data[i]);
    }
    return SPI_SUCCESS;
}

int spi_transfer_fullduplex(uint8_t *txbuf, int len, uint8_t *rxbuf)
{
    int ret = 0;
    int fd = 0;
    uint8_t *prxbuf = rxbuf;
    // xfer用于发送指令和接受数据
    struct spi_ioc_transfer xfer;
    // 将xfer结构体中的内容全初始化为0
    (void)memset_s(&xfer, sizeof(xfer), 0, sizeof(xfer));
    // 发送数据为txbuf
    xfer.tx_buf = (unsigned long)txbuf;
    // 接受数据为rxbuf
    xfer.rx_buf = (unsigned long)prxbuf;
    // 传输的长度为len
    xfer.len = len;
    if (txbuf == NULL) {
        HILOG_ERROR(LOG_CORE, "txbuf pointer is null\n");
        return SPI_PARAM_FAIL;
    }
    if (rxbuf == NULL) {
        HILOG_ERROR(LOG_CORE, "rxbuf pointer is null\n");
        return SPI_PARAM_FAIL;
    }
    // 打开spi接口同时初始化
    fd = spi_open();
    if (fd < 0) {
        HILOG_ERROR(LOG_CORE, "fail open device ==================fd = %{public}d\n", fd);
        return SPI_OPEN_DEVICE_FAIL;
    }
    // 输出片选信号 低电平有效
    // SPI从设备是否被选中的，只有片选信号为预先规定的使能信号时（高电位或低电位），对此 SPI 从设备的操作才有效。
    sysfs_gpio_write(UM_GPIO_SPI_CS, 0);
    // SPI_IOC_MESSAGE(1)的1表示spi_ioc_transfer的数量
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);
    if (ret < 0) {
        HILOG_ERROR(LOG_CORE, "********************Error: SPI_IOC_MESSAGE\n");
        close(fd);
        return SPI_TRANSFER_FAIL;
    }
    // 片选信号变为无效高电平
    sysfs_gpio_write(UM_GPIO_SPI_CS, 1);

    close(fd);

    return ret;
}

void spi_wait_writeend(void)
{
    // 读取状态寄存器，当最后一位s0为正在擦除/写入（忙）位
    // 当编程、擦除或写入状态/安全寄存器指令完成时，
    // 忙位将被清除为0状态，表示设备已准备好接受进一步指令。
    uint8_t redata[3L] = {0};
    int len = 3L;
    // 当忙位为1时，继续读取状态寄存器等待，知道忙位为0，结束等待
    do {
        read_status_register(redata, len);
    } while ((redata[2L] & 0x01) == 0x01);
}

int spi_sector_erase(uint32_t SectorAddr)
{
    // 初始化擦除指令和地址
    int ret;
    int len = 4L;
    uint8_t readIDCmd[4];
    uint8_t data[10] = {0};

    readIDCmd[0] = 0x20;
    readIDCmd[1] = ((SectorAddr & FLASH_ADDRESS_HIGHT) >> 16L);
    readIDCmd[2L] = ((SectorAddr & FLASH_ADDRESS_MIDDLE) >> 8L);
    readIDCmd[3L] = SectorAddr & FLASH_ADDRESS_LOW;
    // 开写使能
    spi_flash_writeenable();
    // 等待写入空闲
    spi_wait_writeend();
    ret = spi_transfer_fullduplex(readIDCmd, len, data);
    if (ret < 0) {
        HILOG_ERROR(LOG_CORE, "Erase failed\n");
        return SPI_FLASH_SECTOR_ERASE_FAIL;
    }
    spi_wait_writeend();
    return SPI_SUCCESS;
}

int spi_page_write(uint8_t *Buffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    int ret = 0;
    uint16_t numByteToWrite = NumByteToWrite;
    int len = 4L + numByteToWrite;
    uint8_t readIDCmd[260];
    uint8_t data[256] = {0};
    uint8_t *pBuffer = Buffer;
    readIDCmd[0] = 0x02;
    readIDCmd[1] = ((WriteAddr & FLASH_ADDRESS_HIGHT) >> 16L);
    readIDCmd[2L] = ((WriteAddr & FLASH_ADDRESS_MIDDLE) >> 8L);
    readIDCmd[3L] = WriteAddr & FLASH_ADDRESS_LOW;
    int i = 4L;

    if (pBuffer == NULL) {
        HILOG_ERROR(LOG_CORE, "pBuffer pointer is null\n");
        return SPI_PARAM_FAIL;
    }
    // 写使能
    spi_flash_writeenable();
    // 等待写入空闲
    spi_wait_writeend();
    // 错误处理
    if (numByteToWrite > SPI_FLASH_PerWritePageSize) {
        numByteToWrite = SPI_FLASH_PerWritePageSize;
        HILOG_ERROR(LOG_CORE, "spi_page_write too large!\n");
    }
    while (numByteToWrite--) {
        readIDCmd[i] = *pBuffer;
        i++;
        pBuffer++;
    }
    ret = spi_transfer_fullduplex(readIDCmd, len, data);
    if (ret < 0) {
        HILOG_ERROR(LOG_CORE, "write failed\n");
        return SPI_FLASH_PAGE_WRITE_FAIL;
    }
    spi_wait_writeend();
    return SPI_SUCCESS;
}

int spi_buffer_read(uint8_t *Buffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    int ret;
    int i = 4L;
    uint16_t numByteToRead = NumByteToRead;
    int len = 4L + numByteToRead;
    uint8_t readIDCmd[4];
    uint8_t *data;
    uint8_t *pBuffer = Buffer;
    // 声明一个大小为len的数组
    data = (uint8_t *)malloc(len * sizeof(uint8_t));
    readIDCmd[0] = 0x03;
    readIDCmd[1] = ((ReadAddr & FLASH_ADDRESS_HIGHT) >> 16L);
    readIDCmd[2L] = ((ReadAddr & FLASH_ADDRESS_MIDDLE) >> 8L);
    readIDCmd[3L] = ReadAddr & FLASH_ADDRESS_LOW;

    if (pBuffer == NULL) {
        HILOG_ERROR(LOG_CORE, "txbuf pointer is null\n");
        return SPI_PARAM_FAIL;
    }
    ret = spi_transfer_fullduplex(readIDCmd, len, data);
    if (ret < 0) {
        HILOG_ERROR(LOG_CORE, "read buffer failed\n");
        return SPI_FLASH_BUFFER_READ_FAIL;
    }
    while (numByteToRead--) {
        *pBuffer = data[i];
        i++;
        pBuffer++;
    }
    return SPI_SUCCESS;
}

int spi_buffer_write(uint8_t *Buffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    int ret = 0;
    uint8_t *pBuffer = Buffer;
    uint32_t writeAddr = WriteAddr;
    uint16_t numByteToWrite = NumByteToWrite;
    uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
    // 取模，检查有几页
    Addr = WriteAddr % SPI_FLASH_PageSize;
    // 差count 个数值，刚好对齐到页地址
    count = SPI_FLASH_PageSize - Addr;
    // 整数页
    NumOfPage = numByteToWrite / SPI_FLASH_PageSize;
    // 不满一页的字节数
    NumOfSingle = numByteToWrite % SPI_FLASH_PageSize;
    if (pBuffer == NULL) {
        HILOG_ERROR(LOG_CORE, "txbuf pointer is null\n");
        return SPI_PARAM_FAIL;
    }
    // Addr=0 刚好按页对齐
    if (Addr == 0) {
        // numByteToWrite < SPI_FLASH_PageSize
        if (NumOfPage == 0) {
            ret = spi_page_write(pBuffer, writeAddr, numByteToWrite);
            if (ret < 0) {
                return SPI_FLASH_PAGE_WRITE_FAIL;
            }
            return SPI_SUCCESS;
        }
        // 一页写步下 numByteToWrite > SPI_FLASH_PageSize
        while (NumOfPage--) {
            ret = spi_page_write(pBuffer, writeAddr, SPI_FLASH_PageSize);
            if (ret < 0) {
                return SPI_FLASH_PAGE_WRITE_FAIL;
            }
            writeAddr += SPI_FLASH_PageSize;
            pBuffer += SPI_FLASH_PageSize;
        }
        // 剩余不满的写完
        ret = spi_page_write(pBuffer, writeAddr, NumOfSingle);
        if (ret < 0) {
            return SPI_FLASH_PAGE_WRITE_FAIL;
        }
        return SPI_SUCCESS;
    }
    // Addr!=0 地址不对齐
    /* numByteToWrite < SPI_FLASH_PageSize */
    if (NumOfPage == 0) {
        ret = spi_less_page_write(pBuffer, writeAddr, numByteToWrite);
        if (ret < 0) {
            return SPI_FLASH_PAGE_WRITE_FAIL;
        }
        return SPI_SUCCESS;
    }
    // Addr!=0 地址不对齐
    /* numByteToWrite > SPI_FLASH_PageSize */
    ret = spi_multi_pages_write(pBuffer, writeAddr, numByteToWrite);
    if (ret < 0) {
        return SPI_FLASH_PAGE_WRITE_FAIL;
    }
    return SPI_SUCCESS;
}

int spi_multi_pages_write(uint8_t *Buffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    int ret = 0;
    uint8_t *pBuffer = Buffer;
    uint32_t writeAddr = WriteAddr;
    uint16_t numByteToWrite = NumByteToWrite;
    uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
    // 取模，检查有几页
    Addr = WriteAddr % SPI_FLASH_PageSize;
    // 差count 个数值，刚好对齐到页地址
    count = SPI_FLASH_PageSize - Addr;

    // 地址不对齐多出的count分开处理，重新计算页数
    numByteToWrite -= count;
    NumOfPage = numByteToWrite / SPI_FLASH_PageSize;
    NumOfSingle = numByteToWrite % SPI_FLASH_PageSize;
    // 先写完count个数据
    ret = spi_page_write(pBuffer, writeAddr, count);
    if (ret < 0) {
        return SPI_FLASH_PAGE_WRITE_FAIL;
    }
    writeAddr += count;
    pBuffer += count;
    // 写整页
    while (NumOfPage--) {
        ret = spi_page_write(pBuffer, writeAddr, SPI_FLASH_PageSize);
        if (ret < 0) {
            return SPI_FLASH_PAGE_WRITE_FAIL;
        }
        writeAddr += SPI_FLASH_PageSize;
        pBuffer += SPI_FLASH_PageSize;
    }
    // 多余不满一页的情况
    if (NumOfSingle != 0) {
        ret = spi_page_write(pBuffer, writeAddr, NumOfSingle);
        if (ret < 0) {
            return SPI_FLASH_PAGE_WRITE_FAIL;
        }
    }
    return SPI_SUCCESS;
}

int spi_less_page_write(uint8_t *Buffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    int ret = 0;
    uint8_t *pBuffer = Buffer;
    uint32_t writeAddr = WriteAddr;
    uint16_t numByteToWrite = NumByteToWrite;
    uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
    // 取模，检查有几页
    Addr = WriteAddr % SPI_FLASH_PageSize;
    // 差count 个数值，刚好对齐到页地址
    count = SPI_FLASH_PageSize - Addr;
    // 整数页
    NumOfPage = numByteToWrite / SPI_FLASH_PageSize;
    // 不满一页的字节数
    NumOfSingle = numByteToWrite % SPI_FLASH_PageSize;
    // 当前剩余的count比NumOfSingle 小，一页写不完
    if (NumOfSingle > count) {
        temp = NumOfSingle - count;
        // 先写完当页
        ret = spi_page_write(pBuffer, writeAddr, count);
        if (ret < 0) {
            return SPI_FLASH_PAGE_WRITE_FAIL;
        }
        writeAddr += count;
        pBuffer += count;
        // 写剩余的
        ret = spi_page_write(pBuffer, writeAddr, temp);
        if (ret < 0) {
            return SPI_FLASH_PAGE_WRITE_FAIL;
        }
    } else {
        // 当前的剩余count 能写完numByteToWrite
        ret = spi_page_write(pBuffer, writeAddr, numByteToWrite);
        if (ret < 0) {
            return SPI_FLASH_PAGE_WRITE_FAIL;
        }
    }
    return SPI_SUCCESS;
}
