# CAN

详细代码及使用可参考[sample/can](https://gitee.com/umspark/can)目录

## 工具简介

canutils 工具包内含 5 个独立的程序：canconfig、candump、canecho、cansend、cansequence。程序的功能简述如下：

| 程序名         | 功能                                        |
| ----------- | ----------------------------------------- |
| canconfig   | 用于配置CAN总线接口的参数，主要是波特率和模式                  |
| candump     | 从CAN总线接口接收数据并以十六进制形式打印到标准输出，也可以输出到指定文件    |
| canecho     | 把从CAN总线接口接收到的所有数据重新发送到CAN总线接口             |
| cansend     | 往指定的CAN总线接口发送指定的数据                        |
| cansequence | 往指定的CAN总线接口自动重复递增数字，也可以指定接收模式并校验检查接收的递增数字 |

> 工具常见用法：
> 
> candump canX                                 // 接收can总线发来数据
> 
> ifconfig canX down                           // 关闭can设备，以便配置
> 
> ip link set canX up type can bitrate 250000  // 设置can波特率 (需配合ip工具)
> 
> conconfig canX bitrate + 波特率
> 
> canconfig canX start                         // 启动can设备
> 
> canconfig canX ctrlmode loopback on          // 回环测试
> 
> canconfig canX restart                       // 重启can设备
> 
> canconfig canX stop                          // 停止can设备
> 
> canecho canX                                 // 查看can设备总线状态
> 
> cansend canX --identifier=ID+数据            // 发送数据
> 
> candump canX --filter=ID:mask                // 使用滤波器接收ID匹配的数据

## 接口介绍 (libsocketcan)

```
int can_do_restart(const char *name);
int can_do_stop(const char *name);
int can_do_start(const char *name);

int can_set_restart_ms(const char *name, __u32 restart_ms);
int can_set_bittiming(const char *name, struct can_bittiming *bt);
int can_set_ctrlmode(const char *name, struct can_ctrlmode *cm);
int can_set_bitrate(const char *name, __u32 bitrate);
int can_set_bitrate_samplepoint(const char *name, __u32 bitrate, __u32 sample_point);

int can_get_restart_ms(const char *name, __u32 *restart_ms);
int can_get_bittiming(const char *name, struct can_bittiming *bt);
int can_get_ctrlmode(const char *name, struct can_ctrlmode *cm);
int can_get_state(const char *name, int *state);
int can_get_clock(const char *name, struct can_clock *clock);
int can_get_bittiming_const(const char *name, struct can_bittiming_const *btc);
int can_get_berr_counter(const char *name, struct can_berr_counter *bc);
```
