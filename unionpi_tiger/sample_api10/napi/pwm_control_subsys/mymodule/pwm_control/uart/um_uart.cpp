#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include "um_uart.h"

static int fd;
int um_init_uart(void)
{
    char uart_dev[12] = "/dev/ttyS1";
    fd = open(uart_dev,O_RDWR);
    if(fd == ERR)
    {
        return ERR;
    }

    struct termios options;
    tcgetattr(fd,&options);
    
    tcflush(fd,TCIOFLUSH);  // 刷清缓冲区
    options.c_iflag = 0;
    options.c_oflag = 0;
    // options.c_cflag | = ( CLOCAL | CREAD );//(本地连接（不改变端口所有者)|接收使能)
    options.c_cflag &= ~CSIZE; // setup data bits
    options.c_cflag |= CS8;
    options.c_cflag &= ~CSTOPB; // stop bits/
    options.c_cflag &= ~PARENB; // Clear parity enable/
    options.c_iflag &= ~INPCK;  // disable input parity che
    // options.c_cflag |= IGNPAR;//忽略奇偶校验错误
    // options.c_cflag &= ~(ICRNL | IXON);//~(将CR映射到NL|启动出口硬件流控)
    options.c_cflag &= ~CRTSCTS;// 请求发送和清除发送
    options.c_lflag = 0;
    options.c_cc[VTIME] = 1L;   //超时时间1*0.1s
    options.c_cc[VMIN] = 0;

    cfsetispeed(&options,B9600);
    cfsetospeed(&options,B9600);
    // 设置终端参数到opt中，使之立即生效
    if(tcsetattr(fd,TCSANOW,&options) !=0)
    {
        return ERR;
    }
    tcflush(fd, TCIOFLUSH); // 刷清缓冲区

    return OK;
}

int um_get_uart(void)
{
    int buf = 0;
    int i = 0;
    int ret = ERR;
    int recv[4];
    for (i = 0; i < 4; i++) {
        ret = read(fd, &buf, 1);    //0.1s超时退出
        if (ret == ERR) {
            break;
        }
        if(buf == RECV_HEAD)    //若buf为起始位，设置i为0
        {
            i = 0;
        }
        recv[i] = buf;
        if ((recv[0] == RECV_HEAD) && (recv[3L] == RECV_END) && (recv[1] == (0xff - recv[2L]))) //校验
        {
            return recv[1];
        }
    }
    
    
    return ERR;
}

int um_sleep_set(int value)
{
    int send_data[5]={0xAA,0x52,0x00,0x00,0x55};
    int ret = 0;
    if(value == 1)  //休眠
    {
        send_data[2]=0x00;
        send_data[3]=0x52;
    }else if(value == 0)    //唤醒
    {
        send_data[2]=0xFF;
        send_data[3]=0xAD;
    }
    for(int i = 0;i < 5;i++)
    {
        ret=write(fd,send_data + i,1);
        if(ret < 0)
        {
            return ERR;
        }
        usleep(250);    //需要每一个字节之间间隔200ms以上
    }
    return OK;
}