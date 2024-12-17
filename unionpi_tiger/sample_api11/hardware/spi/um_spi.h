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

#ifndef __UM_SPI_H__
#define __UM_SPI_H__
#include <stdint.h>

#define LOW 0
#define WIP_FLAG 0x01

#define DIR_MAXSIZ 256
#define EXP_MAXSIZ 256
#define IN 0
#define OUT 1

#define UM_GPIO_SPI_CS 486 /* SS0 */
#define SPI_FLASH_PAGE_SIZE 256
#define SPI_FLASH_PER_WRITE_PAGE_SIZE 256

#define FLASH_ADDRESS_HIGHT 0xFF0000
#define FLASH_ADDRESS_MIDDLE 0xFF00
#define FLASH_ADDRESS_LOW 0xFF
// return
#define SPI_SUCCESS 0
// error return
#define SPI_OPEN_DEV_FAIL (-1)
#define SPI_NOT_OPENED_FAIL (-2)
#define SPI_SET_MODE_FAIL (-3)
#define SPI_GET_MODE_FAIL (-4)
#define SPI_PARAM_FAIL (-5)
#define SPI_TRANSFER_FAIL (-6)
#define SPI_SET_DIRECTION_FAIL (-7)
#define SPI_OPEN_DIRECTION_FAIL (-8)
#define SPI_OPEN_VALUE_FAIL (-9)
#define SPI_READ_VALUE_FAIL (-10)
#define SPI_WRITE_VALUE_FAIL (-11)
#define SET_WRITEENABLE_FAIL (-12)
#define SPI_OPEN_DEVICE_FAIL (-13)
#define READ_STATUS_REGISTER_FAIL (-14)
#define READ_FLASHID_FAIL (-15)
#define READ_DEVICEID_FAIL (-16)
#define SPI_SET_FLASH_WRITE_ENABLE_FAIL (-17)
#define SPI_FLASH_SECTOR_ERASE_FAIL (-18)
#define SPI_FLASH_PAGE_WRITE_FAIL (-19)
#define SPI_FLASH_BUFFER_READ_FAIL (-20)
#define SET_GPIO_FAIL (-21)
#define SPI_VALUE_NUMBER_WRONG (-22)
#define SPI_SET_FLASH_WRITE_ENABLE (-23)

#define FLASH_WRITE_ADDRESS 0x00000
#define FLASH_READ_ADDRESS FLASH_WRITE_ADDRESS
#define FLASH_SECTOR_TO_ERASE FLASH_WRITE_ADDRESS

#define DEFAULT_DEV "/dev/spidev0.0"

void SysfsGpioExport(int s32GpioNum, int bExport);

int SysfsGpioDirection(int pin, int dir);

int SysfsGpioWrite(int pin, int value);

int SpiOpen(void);

int SpiReadFlashid(void);

int SpiReadDeviceid(void);

int ReadStatusRegister(uint8_t *data, int len);

int SpiFlashWriteenable(void);

int SpiReadData(int first, int last, uint8_t *data);

int SpiTransferFullduplex(uint8_t *txbuf, int len, uint8_t *rxbuf);

void SpiWaitWriteend(void);

int SpiSectorErase(uint32_t sectorAddr);

int SpiPageWrite(uint8_t *buffer, uint32_t writeAddr, uint16_t numByteToWrite);

int SpiBufferRead(uint8_t *buffer, uint32_t readAddr, uint16_t numByteToRead);

int SpiBufferWrite(uint8_t *buffer, uint32_t writeAddress,
                   uint16_t numByteToWrite);

int SpiMultiPagesWrite(uint8_t *buffer, uint32_t writeAddress,
                       uint16_t numByteToWrite);

int SpiLessPageWrite(uint8_t *buffer, uint32_t writeAddress,
                     uint16_t numByteToWrite);
#endif // endif __UM_SPI_H__
