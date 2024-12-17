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

#define LOW 0
#define WIP_Flag 0x01

#define DIR_MAXSIZ 256
#define EXP_MAXSIZ 256
#define IN 0
#define OUT 1

#define UM_GPIO_SPI_CS 486 /* SS0 */
#define SPI_FLASH_PageSize 256
#define SPI_FLASH_PerWritePageSize 256

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

#define DEFAULT_DEV "/dev/spidev0.0"

#ifdef __cplusplus
extern "C" {
#endif

void sysfs_gpio_export(int s32GpioNum, int bExport);

int sysfs_gpio_direction(int Pin, int Dir);

int sysfs_gpio_write(int Pin, int value);

int spi_open(void);

int spi_read_flashid(uint8_t *dataID);

int spi_read_deviceid(uint8_t *dataID);

int read_status_register(uint8_t *data, int len);

int spi_flash_writeenable(void);

int spi_read_data(int first, int last, uint8_t *data);

int spi_transfer_fullduplex(uint8_t *txbuf, int len, uint8_t *rxbuf);

void spi_wait_writeend(void);

int spi_sector_erase(uint32_t SectorAddr);

int spi_page_write(uint8_t *Buffer, uint32_t WriteAddr, uint16_t NumByteToWrite);

int spi_buffer_read(uint8_t *Buffer, uint32_t ReadAddr, uint16_t NumByteToRead);

int spi_buffer_write(uint8_t *Buffer, uint32_t WriteAddr, uint16_t NumByteToWrite);

int spi_multi_pages_write(uint8_t *Buffer, uint32_t WriteAddr, uint16_t NumByteToWrite);

int spi_less_page_write(uint8_t *Buffer, uint32_t WriteAddr, uint16_t NumByteToWrite);

#ifdef __cplusplus
}
#endif
#endif // endif __UM_SPI_H__
