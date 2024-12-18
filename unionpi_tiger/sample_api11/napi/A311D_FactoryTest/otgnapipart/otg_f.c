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
#include <string.h>
#include <unistd.h>

int FileExist(char *filename) {
  if (access(filename, F_OK) != -1) {
    return 0;
  } else {
    return -1;
  }
}

int WriteFile(char *filename, char *data) {
  FILE *fp;
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

int SwitchUsbToOTG(void) {
  int ret;
  ret = WriteFile("/sys/devices/platform/leds/leds/usb_switch/brightness", "0");
  if (ret == 0) {
    printf("write 1 to brightness success\n");
  } else {
    printf("write 1 to brightness fail\n");
  }

  ret = WriteFile("/sys/devices/platform/soc/ffe09000.usb/usb_mode", "1");
  if (ret == 0) {
    printf("write 0 to usb_mode success\n");
  } else {
    printf("write 0 to usb_mode fail\n");
  }

  ret =
      WriteFile("/sys/class/usb_role/ffe09000.usb-role-switch/role", "device");
  if (ret == 0) {
    printf("write host to role success\n");
  } else {
    printf("write host to role fail\n");
  }

  return ret;
}

int otg_test(void) {
  SwitchUsbToOTG();
  FILE *fp;
  fp = fopen("sys/devices/virtual/android_usb/android0/state", "r");
  if (fp != NULL) {
    char buffer[1024]; // 用于存储读取的内容
    // 逐行读取文件内容
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
      printf("%s", buffer);
    }
    if (strstr(buffer, "CONFIGURED") != NULL) {
      return 1;
    } else {
      return 0;
    }
  }
  return 0;
}
