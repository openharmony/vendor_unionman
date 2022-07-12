
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "um_i2c.h"

void user_help()
{
    printf("Please input like this: deceivei2c <int> [int] \n");
    printf("\t\tthere must have one parameter it is int for range[0,1] \n");
    printf("\t\tthe second is to write the id to the eeprom range[1,254] \n");
}

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3) {
        user_help();
        return 0;
    }

    if ((strcmp(argv[1], "1") != 0) && (strcmp(argv[1], "0") != 0)) {
        printf("Invalid first param \n");
        user_help();
        return 0;
    }

    int number = 0;
    if (argc == 3) {
        number = atoi(argv[2]);
        if (number > 254 || argv[2][0] < 48 || argv[2][0] > 57) {
            printf("Invalid second param \n");
            user_help();
            return 0;
        }
    }

    int ret = 0;
    int i2cChannel = I2C_DEV_3;

    unsigned char tem[2] = {0x01};
    if (strcmp(argv[1], "1") == 0) {
        ret = i2c_write(i2cChannel, 0x53, 0xf0, tem, 1);
        if (ret != 0) {
            printf("write flag error \n");
            return 0;
        } else {
            printf("write flag success \n");
        }
    } else if (strcmp(argv[1], "0") == 0) {
        tem[0] = 0xff;
        ret = i2c_write(i2cChannel, 0x53, 0xf0, tem, 1);
        if (ret != 0) {
            printf("remove flag error \n");
            return 0;
        } else {
            printf("remove flag success \n");
        }
    }

    if (argc == 3) {
        tem[0] = number;
        ret = i2c_write(i2cChannel, 0x53, 0xf1, tem, 1);
        if (ret != 0) {
            printf("write backversion error \n");
            return 0;
        } else {
            printf("write backversion success \n");
        }
    }

    return 0;
}
