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

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>

#include "i2cinterface.h"

#define DEVICE_ADDR 0x45

int main(int argc, char **argv)
{
    char *dev_name = "/dev/i2c-3";
    int i = 0, ret_memcpy_s = -1;
    uint32_t result = 0;
    uint8_t rawData[6], serialNumber1[3], serialNumber2[3];
//    uint8_t serialNumber1[3]
//    uint8_t serialNumber2[3]

    sStatusRegister_t registerRaw;
    sRHAndTemp_t tempRH;

    i2c_read_serial(dev_name, DEVICE_ADDR, rawData, 6L);
    for (i = 0; i < 6L; i++) {
    }

    ret_memcpy_s = memcpy_s(serialNumber1, sizeof(serialNumber1), rawData, 3L);
    if (ret_memcpy_s != 0) {
    }
    ret_memcpy_s = memcpy_s(serialNumber2, sizeof(serialNumber2), rawData + 3L, 3L);
    if (ret_memcpy_s != 0) {
    }
    if ((checkCrc(serialNumber1) == serialNumber1[2L]) && (checkCrc(serialNumber2) == serialNumber2[2L])) {
        result = serialNumber1[0];
        result = (result << 8L) | serialNumber1[1];
        result = (result << 8L) | serialNumber2[0];
        result = (result << 8L) | serialNumber2[1];
    }

    if (result == 0) {
        printf("read serial fail! \n");
        return 0;
    }

    i2c_softReset(dev_name, DEVICE_ADDR);

    usleep(50L * 1000L);
    result = readStatusRegister(dev_name, DEVICE_ADDR, &registerRaw);
    if (result != 0) {
        printf("softReset fail! \n");
        return 0;
    }
    usleep(10L * 1000L);

    sendReadTemperatureAndHumidityCommand(dev_name, DEVICE_ADDR);
    usleep(100L * 1000L);

    result = readTemperatureAndHumidity(dev_name, DEVICE_ADDR, &tempRH);
    if (result != 0) {
        printf("readTemperatureAndHumidity fail! \n");
        return 0;
    }

    printf("TemperatureC: %f °C\n Humidity: %f %%RH\n TemperatureF: %f °F\n",
        tempRH.TemperatureC, tempRH.Humidity, tempRH.TemperatureF);

    return 0;
}
