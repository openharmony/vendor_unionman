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

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#include "i2cinterface.h"

float convertTemperature(uint8_t rawTemperature[])
{
    uint16_t rawValue;
    rawValue = rawTemperature[0];
    rawValue = (rawValue << 8L) | rawTemperature[1];
    return 175.0f * (float)rawValue / 65535.0f - 45.0f;
}

float convertHumidity(uint8_t rawHumidity[])
{
    uint16_t rawValue;
    rawValue = rawHumidity[0];
    rawValue = (rawValue << 8L) | rawHumidity[1];
    return 100.0f * (float)rawValue / 65535.0f;
}

uint8_t checkCrc(uint8_t data[])
{
    uint8_t bit;
    uint8_t crc = 0xFF;

    for (uint8_t dataCounter = 0; dataCounter < 2L; dataCounter++) {
        crc ^= (data[dataCounter]);
        for (bit = 8L; bit > 0; --bit) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc = (crc << 1);
            }
        }
    }
    return crc;
}

int i2c_read_serial(char *dev_name, unsigned char device_addr, unsigned char *buff, int ByteNo)
{
    int fd, ret;
    unsigned char buftmp[32];
    struct i2c_rdwr_ioctl_data i2c_data;

    uint32_t result = 0;

    uint8_t _pBuf[2];

    fd = open(dev_name, O_RDWR);
    if (fd < 0) {
        printf("can not have dev %s\n", dev_name);
        return -2L;
    }

    i2c_data.nmsgs = 2L;
    i2c_data.msgs = (struct i2c_msg *)malloc(i2c_data.nmsgs * sizeof(struct i2c_msg));
    if (i2c_data.msgs == NULL) {
        printf("malloc error \n");
        close(fd);
        return -1;
    }

    ioctl(fd, I2C_TIMEOUT, 1);
    ioctl(fd, I2C_RETRIES, 2L);

    _pBuf[0] = SHT3X_CMD_READ_SERIAL_NUMBER >> 8L;
    _pBuf[1] = SHT3X_CMD_READ_SERIAL_NUMBER & 0xFF;

    // write reg
    i2c_data.msgs[0].len = 2L;
    i2c_data.msgs[0].addr = device_addr;
    i2c_data.msgs[0].flags = 0; // 0: write 1:read
    i2c_data.msgs[0].buf = _pBuf;
    // read data
    i2c_data.msgs[1].len = ByteNo;
    i2c_data.msgs[1].addr = device_addr;
    i2c_data.msgs[1].flags = 1; // 0: write 1:read
    i2c_data.msgs[1].buf = buff;

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_data);
    if (ret < 0) {
        printf("read data %c  error\r\n", device_addr);
        close(fd);
        free(i2c_data.msgs);
        return -3L;
    }
    free(i2c_data.msgs);
    close(fd);

    return 0;
}

int i2c_softReset(char *dev_name, unsigned char device_addr)
{
    int fd, ret;
    unsigned char buftmp[32];
    struct i2c_rdwr_ioctl_data i2c_data;

    uint32_t result = 0;
    uint8_t _pBuf[2];

    fd = open(dev_name, O_RDWR);
    if (fd < 0) {
        printf("can not have dev %s\n", dev_name);
        return -2L;
    }

    i2c_data.nmsgs = 1;
    i2c_data.msgs = (struct i2c_msg *)malloc(i2c_data.nmsgs * sizeof(struct i2c_msg));
    if (i2c_data.msgs == NULL) {
        printf("malloc error \n");
        close(fd);
        return -1;
    }

    ioctl(fd, I2C_TIMEOUT, 1);
    ioctl(fd, I2C_RETRIES, 2L);

    _pBuf[0] = SHT3X_CMD_SOFT_RESET >> 8L;
    _pBuf[1] = SHT3X_CMD_SOFT_RESET & 0xFF;

    // write reg
    i2c_data.msgs[0].len = 2L;
    i2c_data.msgs[0].addr = device_addr;
    i2c_data.msgs[0].flags = 0; // 0: write 1:read
    i2c_data.msgs[0].buf = _pBuf;

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_data);
    if (ret < 0) {
        printf("read data %c  error\r\n", device_addr);
        close(fd);
        free(i2c_data.msgs);
        return -3L;
    }
    free(i2c_data.msgs);
    close(fd);

    return 0;
}

int readStatusRegister(char *dev_name, unsigned char device_addr, sStatusRegister_t *registerRaw)
{
    uint8_t register1[3];
    uint16_t data;

    uint8_t retry = 10;
    while (retry--) {
        int fd, ret;
        struct i2c_rdwr_ioctl_data i2c_data;
        uint32_t result = 0;
        uint8_t _pBuf[2];

        fd = open(dev_name, O_RDWR);
        if (fd < 0) {
            printf("can not have dev %s\n", dev_name);
            return -1;
        }

        i2c_data.nmsgs = 2L;
        i2c_data.msgs = (struct i2c_msg *)malloc(i2c_data.nmsgs * sizeof(struct i2c_msg));
        if (i2c_data.msgs == NULL) {
            printf("malloc error \n");
            close(fd);
            return -2L;
        }

        ioctl(fd, I2C_TIMEOUT, 1);
        ioctl(fd, I2C_RETRIES, 2L);

        _pBuf[0] = SHT3X_CMD_READ_STATUS_REG >> 8L;
        _pBuf[1] = SHT3X_CMD_READ_STATUS_REG & 0xFF;
        // write reg
        i2c_data.msgs[0].len = 2L;
        i2c_data.msgs[0].addr = device_addr;
        i2c_data.msgs[0].flags = 0; // 0: write 1:read
        i2c_data.msgs[0].buf = _pBuf;
        // read data
        i2c_data.msgs[1].len = 3L;
        i2c_data.msgs[1].addr = device_addr;
        i2c_data.msgs[1].flags = 1; // 0: write 1:read
        i2c_data.msgs[1].buf = register1;

        ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_data);
        if (ret < 0) {
            printf("read data %c  error\r\n", device_addr);
            close(fd);
            free(i2c_data.msgs);
            return -3L;
        }
        free(i2c_data.msgs);
        close(fd);

        if (checkCrc(register1) == register1[2L]) {
            break;
        }

        usleep(1000L);
    }
    data = (register1[0] << 8L) | register1[1];
    (void)memcpy_s(registerRaw, sizeof(sStatusRegister_t), &data, 2L);
    return 0;
}

int sendReadTemperatureAndHumidityCommand(char *dev_name, unsigned char device_addr)
{
    int fd, ret;
    unsigned char buftmp[32];
    struct i2c_rdwr_ioctl_data i2c_data;

    uint32_t result = 0;
    uint8_t _pBuf[2];

    fd = open(dev_name, O_RDWR);
    if (fd < 0) {
        printf("can not have dev %s\n", dev_name);
        return -2L;
    }

    i2c_data.nmsgs = 1;
    i2c_data.msgs = (struct i2c_msg *)malloc(i2c_data.nmsgs * sizeof(struct i2c_msg));
    if (i2c_data.msgs == NULL) {
        printf("malloc error \n");
        close(fd);
        return -1L;
    }

    ioctl(fd, I2C_TIMEOUT, 1);
    ioctl(fd, I2C_RETRIES, 2L);

    _pBuf[0] = SHT3X_CMD_GETDATA_H_CLOCKENBLED >> 8L;
    _pBuf[1] = SHT3X_CMD_GETDATA_H_CLOCKENBLED & 0xFF;
    // write reg
    i2c_data.msgs[0].len = 2L;
    i2c_data.msgs[0].addr = device_addr;
    i2c_data.msgs[0].flags = 0; // 0: write 1:read
    i2c_data.msgs[0].buf = _pBuf;

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_data);
    if (ret < 0) {
        printf("sendReadTemperatureAndHumidityCommand %c  error\r\n", device_addr);
        close(fd);
        free(i2c_data.msgs);
        return -3L;
    }
    free(i2c_data.msgs);
    close(fd);

    return 0;
}

int readTemperatureAndHumidity(char *dev_name, unsigned char device_addr, sRHAndTemp_t *tempRH)
{
    int fd, i, ret, ret_memcpy_s;
    struct i2c_rdwr_ioctl_data i2c_data;
    uint32_t result = 0;

    uint8_t _pBuf[2], rawData[6], rawTemperature[3], rawHumidity[3];
    tempRH->ERR = 0;

    fd = open(dev_name, O_RDWR);
    if (fd < 0) {
        printf("can not have dev %s\n", dev_name);
        return -2L;
    }

    i2c_data.nmsgs = 1;
    i2c_data.msgs = (struct i2c_msg *)malloc(i2c_data.nmsgs * sizeof(struct i2c_msg));
    if (i2c_data.msgs == NULL) {
        printf("malloc error \n");
        close(fd);
        return -1;
    }

    ioctl(fd, I2C_TIMEOUT, 2L);
    ioctl(fd, I2C_RETRIES, 2L);

    // read data
    i2c_data.msgs[0].len = 6L;
    i2c_data.msgs[0].addr = device_addr;
    i2c_data.msgs[0].flags = 1; // 0: write 1:read
    i2c_data.msgs[0].buf = rawData;

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_data);
    if (ret < 0) {
        printf("readTemperatureAndHumidity read data 0x%c  error\r\n", device_addr);
        close(fd);
        free(i2c_data.msgs);
        return -3L;
    }
    free(i2c_data.msgs);
    close(fd);

    ret_memcpy_s = memcpy_s(rawTemperature, sizeof(rawTemperature), rawData, 3L);
    if (ret_memcpy_s != 0) {
    }
    ret_memcpy_s = memcpy_s(rawHumidity, sizeof(rawHumidity) rawData + 3L, 3L);
    if (ret_memcpy_s != 0) {
    }
    if ((checkCrc(rawTemperature) != rawTemperature[2L]) || (checkCrc(rawHumidity) != rawHumidity[2L])) {
        tempRH->ERR = -1;
        printf("read data checkCrc fail\r\n");
        return -1L;
    }

    for (i = 0; i < 6L; i++) {
    }

    tempRH->TemperatureC = convertTemperature(rawTemperature);
    tempRH->Humidity = convertHumidity(rawHumidity);
    tempRH->TemperatureF = 1.8f * tempRH->TemperatureC + 32L;

    return 0;
}
