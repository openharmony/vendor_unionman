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
    int csGpio = 486;
    int i = 0;
    uint16_t len = maxlen;
    uint8_t writebuff[maxlen];
    uint8_t readbuff[maxlen];
    
    // 设置spi接口传输
    SysfsGpioExport(csGpio, 1);
    // 设置spi接口传输方向为输出
    SysfsGpioDirection(csGpio, OUT);
    
    SpiReadFlashid();
    SpiReadDeviceid();
    SpiSectorErase(FLASH_WRITE_ADDRESS);
    for (i = 0; i < maxlen; i++) {
        writebuff[i] = i;
    }
    SpiBufferWrite(writebuff, FLASH_WRITE_ADDRESS, len);
    SpiBufferRead(readbuff, FLASH_READ_ADDRESS, len);
    printf("SPI_FLASH_BufferRead\n");
    SpiReadData(0, len - 1, readbuff);
    SysfsGpioExport(csGpio, 0);
    return 0;
}