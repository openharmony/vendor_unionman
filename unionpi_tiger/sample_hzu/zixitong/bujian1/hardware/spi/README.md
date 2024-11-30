# SPI

SPI（Serial Peripheral Interface，串行外设接口）是一种同步串行数据传输标准，是一种高速的，全双工，同步的通信总线，在很多器件中被广泛应用。

SPI NAND FLASH，从名字上就直接解释了该类型存储设备的基本情况----内部以nand flash作为存储介质，对外为SPI类型的接口。

详细代码参考`sample/spi`目录

图1 spi使用流程图
![](../figures/spi_flow.png)
图2 spi实物连接图
![](../figures/spi_usage.png)

## 宏定义及接口

```c
#define LOW 0
#define WIP_Flag 0x01

#define DIR_MAXSIZ 256
#define EXP_MAXSIZ 256
#define IN 0
#define OUT 1

#define UM_GPIO_SPI_CS 486 /* SS0 */
#define SPI_FLASH_PageSize 256
#define SPI_FLASH_PerWritePageSize 256

#define FLASH_ADDRESS_HIGHT 0xFF0000
#define FLASH_ADDRESS_MIDDLE 0xFF00
#define FLASH_ADDRESS_LOW 0xFF
// return
#define SPI_SUCCESS 0
// error return
#define SPI_OPEN_DEV_FAIL (-1)
#define SPI_NOT_OPENED_FAIL (-2)
#define SPI_SET_MODE_FAIL (-3)
#define SPI_GET_MODE_FAIL (-4)
#define SPI_PARAM_FAIL (-5)
#define SPI_TRANSFER_FAIL (-6)
#define SPI_SET_DIRECTION_FAIL (-7)
#define SPI_OPEN_DIRECTION_FAIL (-8)
#define SPI_OPEN_VALUE_FAIL (-9)
#define SPI_READ_VALUE_FAIL (-10)
#define SPI_WRITE_VALUE_FAIL (-11)
#define SET_WRITEENABLE_FAIL (-12)
#define SPI_OPEN_DEVICE_FAIL (-13)
#define READ_STATUS_REGISTER_FAIL (-14)
#define READ_FLASHID_FAIL (-15)
#define READ_DEVICEID_FAIL (-16)
#define SPI_SET_FLASH_WRITE_ENABLE_FAIL (-17)
#define SPI_FLASH_SECTOR_ERASE_FAIL (-18)
#define SPI_FLASH_PAGE_WRITE_FAIL (-19)
#define SPI_FLASH_BUFFER_READ_FAIL (-20)
#define SET_GPIO_FAIL (-21)
#define SPI_VALUE_NUMBER_WRONG (-22)
#define SPI_SET_FLASH_WRITE_ENABLE (-23)

#define FLASH_WriteAddress 0x00000
#define FLASH_ReadAddress FLASH_WriteAddress
#define FLASH_SectorToErase FLASH_WriteAddress

#define DEFAULT_DEV "/dev/spidev0.0"


/*
 函数功能：打开或关闭40_ext上的spi接口
bExport：为0不输出，不为0输出
*/
void sysfs_gpio_export(int s32GpioNum, int bExport);
/*
函数功能：设置Pin号接口的传输方向
Dir：决定传输方向，为IN则输入，为OUT则输出
*/
int sysfs_gpio_direction(int Pin, int Dir);
/*
函数功能：将value的值写到Pin接口
*/
int sysfs_gpio_write(int Pin, int value);
/*
函数功能：初始化SPI接口
*/
int spi_open(void);
/*
函数功能：读取从机的flashID
*/
int spi_read_flashid(void);
/*
函数功能：读取从机的deviceID
*/
int spi_read_deviceid(void);
/*
函数功能：读取从机的状态寄存器的值s7-s0
dataID：存放读取到的状态寄存器中的值
len：传输数值的长度
*/
int read_status_register(uint8_t *data, int len);
/*
函数功能：写使能命令，在对flash进行擦除和写入前都需要先进行该操作
*/
int spi_flash_writeenable(void);
/*
函数功能：输出dataID[first]--dataID[last]
*/
int spi_read_data(int first, int last, uint8_t *data);
/*
函数功能：spi全双工传输数据
        需要注意，双向传输数据时，读和写的数据长度相等
txbuf：发送的命令 
rxbuf：接受的数据
len：长度
*/
int spi_transfer_fullduplex(uint8_t *txbuf, int len, uint8_t *rxbuf);
/*
函数功能：等待写入/擦除空闲，擦除/写入flash前使用
*/
void spi_wait_writeend(void);
/*
函数功能：擦除SectorAddr指定扇区
*/
int spi_sector_erase(uint32_t SectorAddr);
/*
函数功能：将NumByteToWrite大小的pBuffer中的内容写到WriteAddr指定扇区,最大只读一页
pBuffer：写入的数据
WriteAddr：写入的地址
NumByteToWrite：pBuffer的大小
*/
int spi_page_write(uint8_t *Buffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
/*
函数功能：将NumByteToRead大小的pBuffer中的内容写到ReadAddr指定扇区
pBuffer：读出的数据
ReadAddr：读的地址
NumByteToRead：pBuffer的大小
*/
int spi_buffer_read(uint8_t *Buffer, uint32_t ReadAddr, uint16_t NumByteToRead);
/*
函数功能：将NumByteToRead大小的pBuffer中的内容写到ReadAddr指定扇区
pBuffer：读出的数据
ReadAddr：读的地址
NumByteToRead：pBuffer的大小
*/
int spi_buffer_write(uint8_t *Buffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
/*
函数功能：当首地址不是256的倍数且要写的内容小于一页时，spi_buffer_write（）函数会调用本函数进行相应处理
pBuffer：读出的数据
ReadAddr：读的地址
NumByteToRead：pBuffer的大小
*/
int spi_multi_pages_write(uint8_t *Buffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
/*
函数功能：当首地址不是256的倍数且要写的内容大于一页时，spi_buffer_write（）函数会调用本函数进行相应处理
pBuffer：读出的数据
ReadAddr：读的地址
NumByteToRead：pBuffer的大小
*/
int spi_less_page_write(uint8_t *Buffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
```

## 参考资料

W25Q32JV数据手册
