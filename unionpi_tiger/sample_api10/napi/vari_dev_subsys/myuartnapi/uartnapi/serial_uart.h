
#ifndef _SERIAL_UART_H_
#define _SERIAL_UART_H_
#include <string>
using std::string;
int uart_init(int fd);
string data_proce(int *recv);

#endif