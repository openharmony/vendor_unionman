// build in 2019/11/15

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
    rawValue = (rawValue << 8) | rawTemperature[1];
    return 175.0f * (float)rawValue / 65535.0f - 45.0f;
}

float convertHumidity(uint8_t rawHumidity[])
{
    uint16_t rawValue;
    rawValue = rawHumidity[0];
    rawValue = (rawValue << 8) | rawHumidity[1];
    return 100.0f * (float)rawValue / 65535.0f;
}

uint8_t checkCrc(uint8_t data[])
{
    uint8_t bit;
    uint8_t crc = 0xFF;

    for (uint8_t dataCounter = 0; dataCounter < 2; dataCounter++) {
        crc ^= (data[dataCounter]);
        for (bit = 8; bit > 0; --bit) {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}

/*

void DFRobot_SHT3x::writeCommand(uint16_t cmd,size_t size)
{
  uint8_t _pBuf[2];
  _pBuf[0] = cmd >> 8;
  _pBuf[1] = cmd & 0xFF;
  delay(1);
  write(_pBuf,2);
}

*/

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
        return -2;
    }

    i2c_data.nmsgs = 2;
    i2c_data.msgs = (struct i2c_msg *)malloc(i2c_data.nmsgs * sizeof(struct i2c_msg));
    if (i2c_data.msgs == NULL) {
        printf("malloc error \n");
        close(fd);
        return -1;
    }

    ioctl(fd, I2C_TIMEOUT, 1);
    ioctl(fd, I2C_RETRIES, 2);

    _pBuf[0] = SHT3X_CMD_READ_SERIAL_NUMBER >> 8;
    _pBuf[1] = SHT3X_CMD_READ_SERIAL_NUMBER & 0xFF;
    // usleep(1000);

    // write reg
    i2c_data.msgs[0].len = 2;
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
        printf("read data %x  error\r\n", device_addr);
        close(fd);
        free(i2c_data.msgs);
        return -3;
    }
    free(i2c_data.msgs);
    close(fd);

#if 0
    int i;
    printf("i2c__read 0x%02x:",buftmp[0]);
    for (i = 0; i < ByteNo; i++)
    {
    printf(" 0x%02x",buff[i]);
    }
    printf("\n");
#endif

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
        return -2;
    }

    i2c_data.nmsgs = 1;
    i2c_data.msgs = (struct i2c_msg *)malloc(i2c_data.nmsgs * sizeof(struct i2c_msg));
    if (i2c_data.msgs == NULL) {
        printf("malloc error \n");
        close(fd);
        return -1;
    }

    ioctl(fd, I2C_TIMEOUT, 1);
    ioctl(fd, I2C_RETRIES, 2);

    _pBuf[0] = SHT3X_CMD_SOFT_RESET >> 8;
    _pBuf[1] = SHT3X_CMD_SOFT_RESET & 0xFF;
    // usleep(1000);

    // write reg
    i2c_data.msgs[0].len = 2;
    i2c_data.msgs[0].addr = device_addr;
    i2c_data.msgs[0].flags = 0; // 0: write 1:read
    i2c_data.msgs[0].buf = _pBuf;

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_data);
    if (ret < 0) {
        printf("read data %x  error\r\n", device_addr);
        close(fd);
        free(i2c_data.msgs);
        return -3;
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

        i2c_data.nmsgs = 2;
        i2c_data.msgs = (struct i2c_msg *)malloc(i2c_data.nmsgs * sizeof(struct i2c_msg));
        if (i2c_data.msgs == NULL) {
            printf("malloc error \n");
            close(fd);
            return -2;
        }

        ioctl(fd, I2C_TIMEOUT, 1);
        ioctl(fd, I2C_RETRIES, 2);

        _pBuf[0] = SHT3X_CMD_READ_STATUS_REG >> 8;
        _pBuf[1] = SHT3X_CMD_READ_STATUS_REG & 0xFF;
        // usleep(1000);

        // write reg
        i2c_data.msgs[0].len = 2;
        i2c_data.msgs[0].addr = device_addr;
        i2c_data.msgs[0].flags = 0; // 0: write 1:read
        i2c_data.msgs[0].buf = _pBuf;
        // read data
        i2c_data.msgs[1].len = 3;
        i2c_data.msgs[1].addr = device_addr;
        i2c_data.msgs[1].flags = 1; // 0: write 1:read
        i2c_data.msgs[1].buf = register1;

        ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_data);
        if (ret < 0) {
            printf("read data %x  error\r\n", device_addr);
            close(fd);
            free(i2c_data.msgs);
            return -3;
        }
        free(i2c_data.msgs);
        close(fd);

        if (checkCrc(register1) == register1[2]) {
            break;
        }

        usleep(1000);
    }
    data = (register1[0] << 8) | register1[1];
    memcpy(registerRaw, &data, 2);
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
        return -2;
    }

    i2c_data.nmsgs = 1;
    i2c_data.msgs = (struct i2c_msg *)malloc(i2c_data.nmsgs * sizeof(struct i2c_msg));
    if (i2c_data.msgs == NULL) {
        printf("malloc error \n");
        close(fd);
        return -1;
    }

    ioctl(fd, I2C_TIMEOUT, 1);
    ioctl(fd, I2C_RETRIES, 2);

    _pBuf[0] = SHT3X_CMD_GETDATA_H_CLOCKENBLED >> 8;
    _pBuf[1] = SHT3X_CMD_GETDATA_H_CLOCKENBLED & 0xFF;
    // usleep(1000);

    // write reg
    i2c_data.msgs[0].len = 2;
    i2c_data.msgs[0].addr = device_addr;
    i2c_data.msgs[0].flags = 0; // 0: write 1:read
    i2c_data.msgs[0].buf = _pBuf;

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_data);
    if (ret < 0) {
        printf("sendReadTemperatureAndHumidityCommand %x  error\r\n", device_addr);
        close(fd);
        free(i2c_data.msgs);
        return -3;
    }
    free(i2c_data.msgs);
    close(fd);

    return 0;
}

int readTemperatureAndHumidity(char *dev_name, unsigned char device_addr, sRHAndTemp_t *tempRH)
{

    int fd, ret;
    int i;
    struct i2c_rdwr_ioctl_data i2c_data;
    uint32_t result = 0;

    uint8_t _pBuf[2];

    uint8_t rawData[6];
    uint8_t rawTemperature[3];
    uint8_t rawHumidity[3];
    tempRH->ERR = 0;

    fd = open(dev_name, O_RDWR);
    if (fd < 0) {
        printf("can not have dev %s\n", dev_name);
        return -2;
    }

    i2c_data.nmsgs = 1;
    i2c_data.msgs = (struct i2c_msg *)malloc(i2c_data.nmsgs * sizeof(struct i2c_msg));
    if (i2c_data.msgs == NULL) {
        printf("malloc error \n");
        close(fd);
        return -1;
    }

    ioctl(fd, I2C_TIMEOUT, 2);
    ioctl(fd, I2C_RETRIES, 2);

    // read data
    i2c_data.msgs[0].len = 6;
    i2c_data.msgs[0].addr = device_addr;
    i2c_data.msgs[0].flags = 1; // 0: write 1:read
    i2c_data.msgs[0].buf = rawData;

    ret = ioctl(fd, I2C_RDWR, (unsigned long)&i2c_data);
    if (ret < 0) {
        printf("readTemperatureAndHumidity read data 0x%x  error\r\n", device_addr);
        close(fd);
        free(i2c_data.msgs);
        return -3;
    }
    free(i2c_data.msgs);
    close(fd);

    memcpy(rawTemperature, rawData, 3);
    memcpy(rawHumidity, rawData + 3, 3);
    if ((checkCrc(rawTemperature) != rawTemperature[2]) || (checkCrc(rawHumidity) != rawHumidity[2])) {
        tempRH->ERR = -1;
        printf("read data checkCrc fail\r\n");
        return -1;
    }

    for (i = 0; i < 6; i++) {
        // printf("rawData = %x \n", rawData[i]);
    }

    tempRH->TemperatureC = convertTemperature(rawTemperature);
    tempRH->Humidity = convertHumidity(rawHumidity);
    tempRH->TemperatureF = 1.8f * tempRH->TemperatureC + 32;

    return 0;
}
