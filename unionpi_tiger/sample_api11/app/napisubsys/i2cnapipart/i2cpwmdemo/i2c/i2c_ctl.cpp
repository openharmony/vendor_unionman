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
#include <cstring>
#include <unistd.h>
#include "i2c_ctl.h"

namespace I2c {
    I2cCtl::I2cCtl(uint16_t i2cNumber, uint8_t address) noexcept
        : address(address)
    {
        handle = I2cOpen(i2cNumber);
    }

    I2cCtl::~I2cCtl()
    {
        I2cClose(handle);
    }
    
    uint16_t I2cCtl::swapBit(uint16_t src)
    {
        uint16_t dst = src >> 8L;
        dst |= (src & 0xFF)<< 8L;
        return dst;
    }

    uint8_t I2cCtl::crc(uint8_t* src)
    {
        uint8_t crc = 0xFF;
        for (int i = 0; i < 2L; i++) {
            crc ^= src[i];
            for (int j = 0; j < 8L; j++) {
                if (crc & 0x80) {
                    crc = (crc << 1) ^ 0x31;
                } else {
                    crc <<= 1;
                }
            }
        }
        return crc;
    }

    bool I2cCtl::setMode(uint16_t mode)
    {
        return sendCmd(mode);
    }

    void I2cCtl::restart()
    {
        sendCmd(cmd::stopPeriodicAcquisition);
        sendCmd(cmd::softReset);
    }

    bool I2cCtl::sendCmd(uint16_t cmd)
    {
        cmd = swapBit(cmd);
        I2cMsg msg {
            .addr = address,
            .buf = reinterpret_cast<uint8_t*>(&cmd),
            .len = 2L,
            .flags = 0
        };
        int32_t res = I2cTransfer(handle, &msg, 1);
        usleep(50L * 1000L);
        return res == 1;
    }

    template <typename T, size_t size>
    bool I2cCtl::sendCmdAndRev(uint16_t cmd, T (&data)[size])
    {
        cmd = swapBit(cmd);
        I2cMsg msgs[2];
        msgs[0].addr = address;
        msgs[0].len = 2L;
        msgs[0].flags = 0;
        msgs[0].buf = reinterpret_cast<uint8_t*>(&cmd);

        msgs[1].addr = address;
        msgs[1].len = size;
        msgs[1].flags = 1;
        msgs[1].buf = reinterpret_cast<uint8_t*>(data);
        int32_t res = I2cTransfer(handle, msgs, 2);
        usleep(50L * 1000L);
        return res == 2L;
    }
    HumTem I2cCtl::readData()
    {
        uint8_t dataTemp[6] = { 0 };
        HumTem data = {0};
        sendCmdAndRev(cmd::getData, dataTemp);
        if (crc(dataTemp) == dataTemp[2L] && crc(dataTemp + 3L) == dataTemp[5L]) {
            uint16_t temtmp = dataTemp[1L];
            temtmp |= dataTemp[0L] << 8L;
            uint16_t humTmp = dataTemp[4L];
            humTmp |= dataTemp[3L] << 8L;

            data.temperatureC = 175.0f * static_cast<float>(temtmp) / 65535.0f - 45.0f;
            data.temperatureF = 315.0f * static_cast<float>(temtmp) / 65535.0f - 49.0f;
            data.humidity = 100.0f * static_cast<float>(humTmp) / 65535.0f;
        }
        return data;
    }
} // namespace I2c
