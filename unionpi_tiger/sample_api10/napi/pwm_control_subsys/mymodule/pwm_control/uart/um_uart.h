
#ifndef __UM_UART_H__
#define __UM_UART_H__

#ifdef __cplusplus
extern "C"
{
#endif 

#define RECV_HEAD 0xaa
#define RECV_END 0x55
#define EVT_RIGHT 0x01
#define EVT_LEFT 0x02
#define EVT_BACK 0x03
#define EVT_FORWARD 0x04
#define EVT_PULLUP 0x05
#define EVT_PULLDOWN 0x06
#define EVT_PULLREMOVE 0x07
#define EVT_TOUCH1 0x21
#define EVT_TOUCH2 0x22
#define EVT_TOUCH3 0x23
#define EVT_TOUCH4 0x24
#define EVT_TOUCH5 0x25
#define OK 0
#define ERR (-1)

/*
 * 作用：初始化串口
 * 参数：无
 */
int um_init_uart(void);
/*
 * 作用：获取串口数据
 * 参数：无
 * 返回值：串口数据
 */
int um_get_uart(void);
/*
 * 作用：设置传感器休眠
 * 参数：1 -> 进入休眠 ， 0 -> 退出休眠
 * 返回值：0 -> 成功， -1 -> 失败
 */
int um_sleep_set(int value);

#ifdef __cplusplus
}
#endif 

#endif // endif __UM_PWM_H__