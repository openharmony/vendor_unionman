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

#include <stdio.h>
#include "um_spi.h"
int main(int argc, char **argv)
{
    const int maxlen = 400;
    int cs_gpio = 486;
    int i = 0;
    uint16_t len = maxlen;
    uint8_t writebuff[maxlen];
    uint8_t readbuff[maxlen];
    
    // 设置spi接口传输
    sysfs_gpio_export(cs_gpio, 1);
    // 设置spi接口传输方向为输出
    sysfs_gpio_direction(cs_gpio, OUT);
    
    spi_read_flashid();
    spi_read_deviceid();
    spi_sector_erase(FLASH_WriteAddress);
    for (i = 0; i < maxlen; i++) {
        writebuff[i] = i;
    }
    spi_buffer_write(writebuff, FLASH_WriteAddress, len);
    spi_buffer_read(readbuff, FLASH_ReadAddress, len);
    printf("SPI_FLASH_BufferRead\n");
    spi_read_data(0, len - 1, readbuff);
    sysfs_gpio_export(cs_gpio, 0);
    return 0;
}