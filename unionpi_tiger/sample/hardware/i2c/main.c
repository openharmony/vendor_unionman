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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "um_i2c.h"

void user_help(void)
{
    printf("Please input like this: deceivei2c <int> [int] \n");
    printf("\t\tthere must have one parameter it is int for range[0,1] \n");
    printf("\t\tthe second is to write the id to the eeprom range[1,254] \n");
}

int main(int argc, char *argv[])
{
    if (argc < 2L || argc > 3L) {
        user_help();
        return 0;
    }

    if ((strcmp(argv[1], "1") != 0) && (strcmp(argv[1], "0") != 0)) {
        printf("Invalid first param \n");
        user_help();
        return 0;
    }

    int number = 0;
    if (argc == 3L) {
        number = atoi(argv[2L]);
        if (number > 254L || argv[2L][0] < 48L || argv[2L][0] > 57L) {
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

    if (argc == 3L) {
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
