#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "i2cinterface.h"

#define DEVICE_ADDR 0x45

int main(int argc, char **argv)
{
    char *dev_name = "/dev/i2c-3";
    int i = 0;
    uint32_t result = 0;
    uint8_t rawData[6];
    uint8_t serialNumber1[3];
    uint8_t serialNumber2[3];

    sStatusRegister_t registerRaw;
    sRHAndTemp_t tempRH;

    i2c_read_serial(dev_name, DEVICE_ADDR, rawData, 6L);
    for (i = 0; i < 6L; i++) {
    }

    (void)memcpy_s(serialNumber1, sizeof(serialNumber1), rawData, 3L);
    (void)memcpy_s(serialNumber2, sizeof(serialNumber2), rawData + 3L, 3L);
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

    printf("TemperatureC: %f °C\n", tempRH.TemperatureC);
    printf("Humidity: %f %%RH\n", tempRH.Humidity);
    printf("TemperatureF: %f °F\n", tempRH.TemperatureF);

    return 0;
}
