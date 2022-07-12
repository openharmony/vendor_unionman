#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
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

    i2c_read_serial(dev_name, DEVICE_ADDR, rawData, 6);
    for (i = 0; i < 6; i++) {
        // printf("i2c_read_serial = %x \n", rawData[i]);
    }

    memcpy(serialNumber1, rawData, 3);
    memcpy(serialNumber2, rawData + 3, 3);
    if ((checkCrc(serialNumber1) == serialNumber1[2]) && (checkCrc(serialNumber2) == serialNumber2[2])) {
        result = serialNumber1[0];
        result = (result << 8) | serialNumber1[1];
        result = (result << 8) | serialNumber2[0];
        result = (result << 8) | serialNumber2[1];
    }
    // printf("i2c_read_serial result = %x \n", result);

    if (result == 0) {
        printf("read serial fail! \n");
        return 0;
    }

    i2c_softReset(dev_name, DEVICE_ADDR);

    usleep(50 * 1000);
    result = readStatusRegister(dev_name, DEVICE_ADDR, &registerRaw);
    if (result != 0) {
        printf("softReset fail! \n");
        return 0;
    }
    // printf("registerRaw = %x \n", registerRaw);
    usleep(10 * 1000);

    sendReadTemperatureAndHumidityCommand(dev_name, DEVICE_ADDR);
    usleep(100 * 1000);

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
