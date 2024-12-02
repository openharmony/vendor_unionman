#ifndef __UM_GPIO_H__
#define __UM_GPIO_H__

#define UM_GPIO_EXPORT "/sys/class/gpio/export"// 定义导出GPIO的文件路径
#define UM_GPIO_UNEXPORT "/sys/class/gpio/unexport"
#define UM_GPIO_PEX "/sys/class/gpio/gpio" // 定义GPIO设备文件的路径前缀

// hilog
#undef LOG_DOMAIN
#undef LOG_TAG
#define LOG_DOMAIN 0  // 标识业务领域，范围0x0~0xFFFFF
#define LOG_TAG "GPIO_TEST"

// gpios
#define UM_GPIO_01 380 /* GPIO.7 */
#define UM_GPIO_02 381 /* GPIO.0 */
#define UM_GPIO_03 382 /* GPIO.2 */
#define UM_GPIO_04 383 /* GPIO.3 */
#define UM_GPIO_05 384 /* GPIO.21 */
#define UM_GPIO_06 385 /* GPIO.22 */
#define UM_GPIO_07 386 /* GPIO.23 */
#define UM_GPIO_08 387 /* GPIO.24 */
#define UM_GPIO_09 388 /* GPIO.25 */
#define UM_GPIO_10 389 /* GPIO.1 */
#define UM_GPIO_11 390 /* GPIO.4 */
#define UM_GPIO_12 391 /* GPIO.5 */
#define UM_GPIO_13 392 /* GPIO.6 */
#define UM_GPIO_14 393 /* GPIO.30 */
#define UM_GPIO_15 394 /* GPIO.26 */
#define UM_GPIO_16 395 /* GPIO.27 */

// spi cs
#define UM_GPIO_SPI_CS 486 /* SS0 */  // 定义SPI片选信号对应的编号为486

// direction
#define UM_GPIO_DIRECTION_IN 0// 定义GPIO方向为输入
#define UM_GPIO_DIRECTION_OUT 1

// is export
#define UM_GPIO_NOT_EXPORT 0// 定义GPIO未导出的状态
#define UM_GPIO_EXPORTED 1

// errno
#define UM_GPIO_ERR (-1)// 定义GPIO操作错误码
#define UM_GPIO_NOT_EXPROT_ERROR (-2)

// value high - low level
#define UM_GPIO_LOW_LEVE 0
#define UM_GPIO_HIGH_LEVE 1// 定义GPIO高电平

/**
 * set gpio export设置GPIO导出
 * @param gpioNum gpioNum
 * @param bExport export,0:not export 1:export
 */
int UM_GPIO_Export(int gpioNum, int bExport);

/**
 * set gpio direction置GPIO方向
 * @param gpioNum gpioNum
 * @param direction direction,0:in 1:out
 */
int UM_GPIO_SetDirection(int gpioNum, int direction);

/**
 * set gpio value设置GPIO值
 * @param gpioNum gpioNum
 * @param value value,0:low 1:high
 */
int UM_GPIO_SetValue(int gpioNum, int value);

/**
 * check gpio export or not检查GPIO是否已导出
 * @param gpioNum gpioNum
 * @param *value export,0:not export 1:exported
 */
int UM_GPIO_IsExport(int gpioNum, int *value);

/**
 * get gpio direction获取GPIO方向
 * @param gpioNum gpioNum
 * @param *value direction,0:in 1:out
 */
int UM_GPIO_GetDirection(int gpioNum, int *value);

/**
 * get gpio value获取GPIO值
 * @param gpioNum gpioNum
 * @param *value value,0:low 1:high
 */
int UM_GPIO_GetValue(int gpioNum, int *value);

#endif /* __UM_GPIO_H__ */
