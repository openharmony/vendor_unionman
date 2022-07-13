// uart test

#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include "serial_uart.h"

/*
baud rate
=============
B600
B1200
B9600
B19200
B38400
B57600
B115200
B230400
B460800
B921600
B1000000
B2000000
B3000000
B4000000
*/

/**
 *@brief set uart port baud rate
 *@param fd[in] file descriptor of uart port
 *@param baud[in] baud rate like B9600
 *@return none
 */

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

void set_baud(int fd, int baud)
{
    int status;
    struct termios opt;

    tcgetattr(fd, &opt);
    tcflush(fd, TCIOFLUSH);
    cfsetispeed(&opt, baud);
    cfsetospeed(&opt, baud);
    status = tcsetattr(fd, TCSANOW, &opt);
    if (status != 0) {
        perror("tcsetattr fd1");
        return;
    }

    tcflush(fd, TCIOFLUSH);
}

/**
 *@brief set word length, parity and stop bits
 *@param fd[in] file descriptor of uart port
 *@param databits[in] bits of word, valid value 5/6/7/8
 *@param stopbits[in] stop bits, valid value 1 or 2
 *@param parity[in]  parity should be one of None,Even,Odd,Mark,Space
 */

int set_params(int fd, int databits, int stopbits, int parity)
{
    struct termios options;

    if (tcgetattr(fd, &options) != 0) {
        perror("tcgetattr fail");
        return -1;
    }

    options.c_iflag = 0;
    options.c_oflag = 0;

    // setup data bits
    options.c_cflag &= ~CSIZE;
    switch (databits) {
        case 5:
            options.c_cflag |= CS5;
            break;

        case 6:
            options.c_cflag |= CS6;
            break;

        case 7:
            options.c_cflag |= CS7;
            break;

        case 8:
            options.c_cflag |= CS8;
            break;

        default:
            return -1;
    }

    // parity
    switch (parity) {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB; // Clear parity enable/
            options.c_iflag &= ~INPCK;  // disable input parity checking/
            break;

        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB); // odd parity checking
            options.c_iflag |= INPCK;             // enable parity checking
            break;

        case 'e':
        case 'E':
            options.c_cflag |= PARENB;  // Enable parity /
            options.c_cflag &= ~PARODD; // even parity/
            options.c_iflag |= INPCK;   // enable parity checking /
            break;

        case 'M':
        case 'm': // mark parity/
            options.c_cflag |= PARENB | CMSPAR | PARODD;
            options.c_iflag |= INPCK; // enable parity checking /
            break;

        case 'S':
        case 's': // space parity/
            options.c_cflag |= PARENB | CMSPAR;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK; // enable parity checking /
            break;

        default:
            return -1;
    }

    // stop bits/
    switch (stopbits) {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;

        case 2:
            options.c_cflag |= CSTOPB;
            break;

        default:
            return -1;
    }

    // cts rts /
    options.c_cflag &= ~CRTSCTS;
    options.c_lflag = 0;
    options.c_cc[VTIME] = 10;
    options.c_cc[VMIN] = 1;

    // Set input parity option /
    tcflush(fd, TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        return -1;
    }

    return 0;
}

speed_t getBaudrate(int baudrate)
{
    switch (baudrate) {
        case 0:
            return B0;
        case 50:
            return B50;
        case 75:
            return B75;
        case 110:
            return B110;
        case 134:
            return B134;
        case 150:
            return B150;
        case 200:
            return B200;
        case 300:
            return B300;
        case 600:
            return B600;
        case 1200:
            return B1200;
        case 1800:
            return B1800;
        case 2400:
            return B2400;
        case 4800:
            return B4800;
        case 9600:
            return B9600;
        case 19200:
            return B19200;
        case 38400:
            return B38400;
        case 57600:
            return B57600;
        case 115200:
            return B115200;
        case 230400:
            return B230400;
        case 460800:
            return B460800;
        case 500000:
            return B500000;
        case 921600:
            return B921600;
        case 1000000:
            return B1000000;
        case 1152000:
            return B1152000;
        case 1500000:
            return B1500000;
        case 2000000:
            return B2000000;
        case 2500000:
            return B2500000;
        case 3000000:
            return B3000000;
        case 3500000:
            return B3500000;
        case 4000000:
            return B4000000;
        default:
            return 1152000;
    }
    return 1152000;
}

int uartDevInit(int fd, int uartBaud)
{
    set_baud(fd, getBaudrate(uartBaud));
    // uart param /
    if (set_params(fd, 8, 1, 'n')) {
        printf("Set uart parameters fail.\n");
        return -1;
    }
    return 0;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
