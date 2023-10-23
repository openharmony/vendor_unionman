/*
 * Copyright 2023 Unionman Technology Co., Ltd.
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
#include <unistd.h>
int FileExist(char* filename)
{
    if (access(filename, F_OK) != -1) {
        return 0;
    } else {
        return -1;
    }
}

int WriteFile(char* filename, char* data)
{
    FILE* fp;
    fp = fopen(filename, "w");
    if (fp != NULL) {
        fprintf(fp, "%s", data);
        fflush(fp);
        printf("write success\n");
        return 0;
    } else {
        printf("open fail \n");
        return -1;
    }
}

int SwitchUsbToHost(void)
{
    int ret;
    ret = WriteFile("/sys/devices/platform/leds/leds/usb_switch/brightness", "1");
    if (ret == 0) {
        printf("write 1 to brightness success\n");
    } else {
        printf("write 1 to brightness fail\n");
    }

    ret = WriteFile("/sys/devices/platform/soc/ffe09000.usb/usb_mode", "0");
    if (ret == 0) {
        printf("write 0 to usb_mode success\n");
    } else {
        printf("write 0 to usb_mode fail\n");
    }

    ret = WriteFile("/sys/class/usb_role/ffe09000.usb-role-switch/role", "host");
    if (ret == 0) {
        printf("write host to role success\n");
    } else {
        printf("write host to role fail\n");
    }

    return ret;
}

int allValuesAreOne(int arr[], int size)
{
    for (int i = 0; i < size; i++) {
        if (arr[i] != 1) {
            return -1; // 如果有任何一个元素不是1，则返回0
        }
    }
    return 0; // 所有元素都是1
}

int find_all_usb(int UsbNum)
{
    int ret;
    ret = SwitchUsbToHost();
    if (ret == 0) {
        printf("switch USB mode to Host success\n");
    } else {
        printf("switch USB mode to Host fail\n");
    }

    if (UsbNum == 1) {
        ret = FileExist("/sys/bus/usb/drivers/usb/1-2.1");
        if (ret == 0) {
            return 1;
            printf("1-2.1 exist\n");
        } else {
            return 0;
            printf("miss 1-2.1\n");
        }
    }

    if (UsbNum == 2L) {
        ret = FileExist("/sys/bus/usb/drivers/usb/1-2.2");
        if (ret == 0) {
            return 1;
            printf("1-2.2 exist\n");
        } else {
            return 0;
            printf("miss 1-2.2\n");
        }
    }

    if (UsbNum == 3L) {
        ret = FileExist("/sys/bus/usb/drivers/usb/1-2.3");
        if (ret == 0) {
            return 1;
            printf("1-2.3 exist\n");
        } else {
            return 0;
            printf("miss 1-2.3\n");
        }
    }
    if (UsbNum == 4L) {
        ret = FileExist("/sys/bus/usb/drivers/usb/1-1");
        if (ret == 0) {
            return 1;
            printf("1-1 exist\n");
        } else {
            return 0;
            printf("miss 1-1\n");
        }
    }
    return 0;
}
