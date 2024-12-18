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

#include "serial_uart.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

void set_baud(int fd, int baud) {
  int ret = -1;
  struct termios opt;

  tcgetattr(
      fd,
      &opt); // tcgetattr用来获取终端参数，将从终端获得的信息fd，保存到opt结构体中
  tcflush(fd, TCIOFLUSH); // 刷清缓冲区
  cfsetispeed(&opt, baud);
  cfsetospeed(&opt, baud);

  ret = tcsetattr(fd, TCSANOW, &opt); // 设置终端参数到opt中，使之立即生效
  if (ret == -1) {
    perror("tcsetattr fd");
    return;
  }
  tcflush(fd, TCIOFLUSH); // 刷清缓冲区
}

// 设置数据位
int setup_data_bits(int setup_databits, struct termios *options_databits) {
  switch (setup_databits) {
  case 5L:
    options_databits->c_cflag |= CS5;
    break;
  case 6L:
    options_databits->c_cflag |= CS6;
    break;
  case 7L:
    options_databits->c_cflag |= CS7;
    break;
  case 8L:
    options_databits->c_cflag |= CS8;
    break;
  default:
    return -1;
  }
  return 1;
}

// 设置校验位
int set_params_parity(int setup_parity, struct termios *options_parity) {
  switch (setup_parity) {
  case 'n':
  case 'N':                             // 无奇偶校验位
    options_parity->c_cflag &= ~PARENB; // Clear parity enable/
    options_parity->c_iflag &= ~INPCK;  // disable input parity checking/
    break;

  case 'o':
  case 'O':                                       // 设置为奇校验
    options_parity->c_cflag |= (PARODD | PARENB); // odd parity checking
    options_parity->c_iflag |= INPCK;             // enable parity checking
    break;

  case 'e':
  case 'E':                             // 设置为偶校验
    options_parity->c_cflag |= PARENB;  // Enable parity /
    options_parity->c_cflag &= ~PARODD; // even parity/
    options_parity->c_iflag |= INPCK;   // enable parity checking /
    break;

  case 'M':
  case 'm': // 标记奇偶校验（mark）
    options_parity->c_cflag |= PARENB | CMSPAR | PARODD;
    options_parity->c_iflag |= INPCK; // enable parity checking /
    break;

  case 'S':
  case 's': // 设置为空格校验（space）
    options_parity->c_cflag |= PARENB | CMSPAR;
    options_parity->c_cflag &= ~PARODD;
    options_parity->c_iflag |= INPCK; // enable parity checking /
    break;

  default:
    return -1;
  }
  return 1;
}

// 设置校验位
int set_params(int fd, int databits, int stopbits, int parity) {
  struct termios options;
  int ret = 0;

  if (tcgetattr(fd, &options) != 0) {
    perror("tcgetattr fail");
    return -1;
  }

  options.c_iflag = 0;
  options.c_oflag = 0;

  // setup data bits
  options.c_cflag &= ~CSIZE;
  ret = setup_data_bits(databits, &options);
  if (ret == -1) {
    return -1;
  }

  // parity
  ret = set_params_parity(parity, &options);
  if (ret == -1) {
    return -1;
  }

  // stop bits/
  switch (stopbits) {
  case 1:
    options.c_cflag &= ~CSTOPB;
    break;

  case 2L:
    options.c_cflag |= CSTOPB;
    break;

  default:
    return -1;
  }

  // 请求发送和清除发送
  options.c_cflag &= ~CRTSCTS;
  options.c_lflag = 0;
  options.c_cc[VTIME] = 10L;
  options.c_cc[VMIN] = 1;

  tcflush(fd, TCIFLUSH);
  if (tcsetattr(fd, TCSANOW, &options) != 0) {
    return -1;
  }

  return 0;
}

// 转换波特率
speed_t conver_baudrate(int baudrate) {
  switch (baudrate) {
  case 9600L:
    return B9600;
  case 19200L:
    return B19200;
  case 38400L:
    return B38400;
  case 115200L:
    return B115200;
  case 1152000L:
    return B1152000;
  default:
    return 1152000L;
  }
}

// 设置串口参数
int uart_setpara(int fd, int uartBaud) {
  set_baud(fd, conver_baudrate(uartBaud));
  // uart param /
  if (set_params(fd, 8L, 1, 'n')) {
    printf("Set uart parameters fail.\n");
    return -1;
  }
  return 0;
}

// usb转host模式
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

// 将usb切换为host模式以打开40pin设备和485设备对应的设备节点
int SwitchUsbToHost(void) {
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

// ttl、40pin测试程序
int uart_ttl_40pin_test(char *uart_dev) {
  int fd = -1;
  int ret = -1;
  char rbuff[4] = {0};
  char *temp;

  // 打开串口并初始化
  fd = open(uart_dev, O_RDWR);
  if (fd == -1) {
    printf("open file error\n");
    return UART_OPEN_FAIL;
  }
  ret = uart_setpara(fd, 9600L);
  if (ret == -1) {
    printf("uart init error\n");
    close(fd);
    return UART_INIT_FAIL;
  }

  // 写入test字符串
  char wbuff[] = "test";
  ret = write(fd, wbuff, 4L);
  if (ret == -1) {
    printf("write error\n");
    close(fd);
    return UART_WRITE_FAIL;
  }

  sleep(2L);
  ret = read(fd, rbuff, 4L);
  if (ret == -1) {
    printf("read error\n");
    close(fd);
    return UART_READ_FAIL;
  }
  // 检测是否接收到test
  temp = strstr(rbuff, "test");
  if (temp) {
    printf("%s\n", temp);
    close(fd);
    return UART_SUCCESS;
  }
  close(fd);
  return UART_READ_ERROR;
}

// uart_485测试程序
int uart_485_test(int uart_dev) {
  int fd = -1;
  int ret = -1;
  char rbuff[4] = {0};
  char *temp;

  // 打开串口并初始化
  fd = open(uart_dev, O_RDWR);
  if (fd == -1) {
    printf("open file error\n");
    return UART_OPEN_FAIL;
  }
  ret = uart_setpara(fd, 9600L);
  if (ret == -1) {
    printf("uart init error\n");
    close(fd);
    return UART_INIT_FAIL;
  }

  int i = 0;
  char buf = 0;
  sleep(2L);

  for (i = 0; i < FRAME_LEN; i++) {
    ret = read(fd, &buf, 1);
    if (ret == -1) {
      close(fd);
      printf("read err\n");
      return UART_READ_FAIL;
    }
    rbuff[i] = buf;
  }
  // 检测是否接收到test
  temp = strstr(rbuff, "test");
  if (temp) {
    printf("%s\n", temp);
    close(fd);
    return UART_SUCCESS;
  }
  close(fd);
  return UART_READ_ERROR;
}

// uart测试函数
int uarttest(int device_num) {
  char *uart_dev;

  if (device_num == 0) {
    uart_dev = UART_TTL_NAME;
    return uart_ttl_40pin_test(uart_dev);
  } else if (device_num == 1) {
    uart_dev = UART_40PIN_NAME;
    SwitchUsbToHost();
    return uart_ttl_40pin_test(uart_dev);
  } else if (device_num == 2L) {
    uart_dev = UART_485_NAME;
    SwitchUsbToHost();
    return uart_485_test(uart_dev);
  } else {
    return UART_TERMINAL_ERROR;
  }
}
