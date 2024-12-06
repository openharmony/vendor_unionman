/*
* 引入必要的头文件
*/
#include <string.h>   // 字符串操作相关的函数
#include <stdlib.h>   // 通用工具函数（如malloc和free）
#include <stdio.h>    // 标准输入输出
#include <unistd.h>   // Unix标准功能（如access）
#include <signal.h>   // 信号处理
#include "hilog/log.h" // 日志记录功能
#include "securec.h"  // 安全字符串操作函数
#include "um_gpio.h"  // GPIO相关定义

/*
 函数 UM_GPIO_Export
功能：导出或取消导出指定的GPIO编号。
   参数：
  gpioNum - 要导出的GPIO编号
   bExport - 是否导出（1为导出，0为取消导出）
 返回值：
   成功返回0，失败返回错误码UM_GPIO_ERR
*/
int UM_GPIO_Export(int gpioNum, int bExport)
{
    int ret = -1;
    char buffer[256] = {0};

    // 根据bExport的值构建对应的shell命令
    if (bExport) {
        (void) snprintf_s(buffer, sizeof(buffer), sizeof(buffer), "echo %d > %s", gpioNum, UM_GPIO_EXPORT);
    } else {
        (void) snprintf_s(buffer, sizeof(buffer), sizeof(buffer), "echo %d > %s", gpioNum, UM_GPIO_UNEXPORT);
    }

    // 设置信号处理器
    sighandler_t old_handler;
    old_handler = signal(SIGCHLD, SIG_DFL);
    
    // 执行shell命令
    ret = system(buffer);
    if (ret < 0) {
        HILOG_ERROR(LOG_CORE, "set gpio%{public}d %{public}s failed", gpioNum, bExport == 1 ? "export" : "unexport");
        return UM_GPIO_ERR; // 返回错误
    }
    (void) signal(SIGCHLD, old_handler); // 恢复信号处理器
    return ret;
}

/*
* 函数 UM_GPIO_SetDirection
* 功能：设置指定GPIO的方向为输入或输出。
* 参数：
*   gpioNum - 要设置方向的GPIO编号
*   direction - GPIO方向（UM_GPIO_DIRECTION_IN或UM_GPIO_DIRECTION_OUT）
* 返回值：
*   成功返回0，失败返回错误码
*/
int UM_GPIO_SetDirection(int gpioNum, int direction)
{
    // Declare variables
    int ret_sprintf_s = -1;
    char gpio_file_name[128];

    // 清零GPIO文件名数组
    (void) memset_s(gpio_file_name, sizeof(gpio_file_name), 0, sizeof(gpio_file_name));
    
    // 构造GPIO方向文件名
    ret_sprintf_s = snprintf_s(gpio_file_name, sizeof(gpio_file_name), sizeof(gpio_file_name), "%s%d/direction",
                               UM_GPIO_PEX, gpioNum);
     
    if (access(gpio_file_name, F_OK) != 0) { // 检查GPIO是否已导出
        HILOG_ERROR(LOG_CORE, "gpio%{public}d not export", gpioNum);
        return UM_GPIO_NOT_EXPROT_ERROR; // 返回未导出错误
    }

    // 打开GPIO方向文件以写入
    FILE *fp = fopen(gpio_file_name, "r+");
    if (fp == NULL) {
        HILOG_ERROR(LOG_CORE, "open %{public}s%{public}d/direction failed", UM_GPIO_PEX, gpioNum);
        return UM_GPIO_ERR; // 打开文件失败
    }

    // 根据方向写入信息
    if (direction == UM_GPIO_DIRECTION_IN) {
        fprintf(fp, "%s", "in");
    } else if (direction == UM_GPIO_DIRECTION_OUT) {
        fprintf(fp, "%s", "out");
    }

    // 关闭文件
    (void) fclose(fp);
    return 0; // 返回成功
}

/*
* 函数 UM_GPIO_SetValue
* 功能：设置指定GPIO的值。
* 参数：
*   gpioNum - 要设置值的GPIO编号
*   value - GPIO值（UM_GPIO_LOW_LEVE或UM_GPIO_HIGH_LEVE）
* 返回值：
*   成功返回0，失败返回错误码
*/
int UM_GPIO_SetValue(int gpioNum, int value)
{
    int ret_sprintf_s = -1;
    char gpio_file_name[128];

    // 清零GPIO文件名数组
    (void) memset_s(gpio_file_name, sizeof(gpio_file_name), 0, sizeof(gpio_file_name));
    
    // 构造GPIO值文件名
    ret_sprintf_s = snprintf_s(gpio_file_name, sizeof(gpio_file_name), sizeof(gpio_file_name), "%s%d/value",
                               UM_GPIO_PEX, gpioNum);
    
    if (access(gpio_file_name, F_OK) != 0) { // 检查GPIO是否已导出
        HILOG_ERROR(LOG_CORE, "gpio%{public}d not export", gpioNum);
        return UM_GPIO_NOT_EXPROT_ERROR; // 返回未导出错误
    }

    // 打开GPIO值文件以写入
    FILE *fp = fopen(gpio_file_name, "r+");
    if (fp == NULL) {
        HILOG_ERROR(LOG_CORE, "open %{public}s%{public}d/value failed", UM_GPIO_PEX, gpioNum);
        return UM_GPIO_ERR; // 打开文件失败
    }

    // 根据值写入信息
    if (value == UM_GPIO_LOW_LEVE) {
        fprintf(fp, "%s", "0");
    } else if (value == UM_GPIO_HIGH_LEVE) {
        fprintf(fp, "%s", "1");
    }

    // 关闭文件
    (void) fclose(fp);
    return 0; // 返回成功
}

/*
* 函数 UM_GPIO_IsExport
* 功能：检查指定GPIO是否已导出。
* 参数：
*   gpioNum - 要检查的GPIO编号
*   value - 输出参数，返回GPIO的导出状态
* 返回值：
*   成功返回0，失败返回错误码
*/
int UM_GPIO_IsExport(int gpioNum, int *value)
{
    int ret_sprintf_s = -1;

    if (value == NULL) {
        return UM_GPIO_ERR; // 检查输出参数是否有效
    }

    char gpio_file_name[128];
    (void) memset_s(gpio_file_name, sizeof(gpio_file_name), 0, sizeof(gpio_file_name));
    ret_sprintf_s = snprintf_s(gpio_file_name, sizeof(gpio_file_name), sizeof(gpio_file_name), "%s%d/value",
                               UM_GPIO_PEX, gpioNum);

    if (access(gpio_file_name, F_OK) != 0) { // 检查GPIO是否已导出
        HILOG_INFO(LOG_CORE, "gpio%{public}d not export", gpioNum);
        *value = UM_GPIO_NOT_EXPORT; // 设置输出值
    } else {
        *value = UM_GPIO_EXPORTED; // 设置输出值
    }
    return 0; // 返回成功
}

/*
* 函数 UM_GPIO_GetDirection
* 功能：获取指定GPIO的方向。
* 参数：
*   gpioNum - 要获取方向的GPIO编号
*   value - 输出参数，返回GPIO的方向
* 返回值：
*   成功返回0，失败返回错误码
*/
int UM_GPIO_GetDirection(int gpioNum, int *value)
{
    int ret = 0;
    int ret_sprintf_s = -1;

    if (value == NULL) {
        return UM_GPIO_ERR; // 检查输出参数是否有效
    }

    char gpio_file_name[128];
    (void) memset_s(gpio_file_name, sizeof(gpio_file_name), 0, sizeof(gpio_file_name));
    ret_sprintf_s = snprintf_s(gpio_file_name, sizeof(gpio_file_name), sizeof(gpio_file_name), "%s%d/direction",
                               UM_GPIO_PEX, gpioNum);

    if (access(gpio_file_name, F_OK) != 0) { // 检查GPIO是否已导出
        HILOG_ERROR(LOG_CORE, "gpio%{public}d not export", gpioNum);
        return UM_GPIO_NOT_EXPROT_ERROR; // 返回未导出错误
    }

    // 打开GPIO方向文件以读取
    FILE *fp = fopen(gpio_file_name, "r");
    if (fp == NULL) {
        HILOG_ERROR(LOG_CORE, "read %{public}s%{public}d/direction failed", UM_GPIO_PEX, gpioNum);
        return UM_GPIO_ERR; // 打开文件失败
    }

    char buffer[20] = {0};
    (void) fread(buffer, sizeof(buffer), 1, fp); // 读取内容
    (void) fclose(fp);
    
    // 解析方向
    if (strstr(buffer, "out") != NULL) {
        *value = UM_GPIO_DIRECTION_OUT;
    } else if (strstr(buffer, "in") != NULL) {
        *value = UM_GPIO_DIRECTION_IN;
    } else {
        ret = UM_GPIO_ERR; // 解析错误
    }
    return ret; // 返回结果
}

/*
* 函数 UM_GPIO_GetValue
* 功能：获取指定GPIO的值。
* 参数：
*   gpioNum - 要获取值的GPIO编号
*   value - 输出参数，返回GPIO的值
* 返回值：
*   成功返回0，失败返回错误码
*/
int UM_GPIO_GetValue(int gpioNum, int *value)
{
    int ret = 0;
    int ret_sprintf_s = -1;

    if (value == NULL) {
        return UM_GPIO_ERR; // 检查输出参数是否有效
    }

    char gpio_file_name[128];
    (void) memset_s(gpio_file_name, sizeof(gpio_file_name), 0, sizeof(gpio_file_name));
    ret_sprintf_s = snprintf_s(gpio_file_name, sizeof(gpio_file_name), sizeof(gpio_file_name), "%s%d/value",
                               UM_GPIO_PEX, gpioNum);

    if (access(gpio_file_name, F_OK) != 0) { // 检查GPIO是否已导出
        HILOG_ERROR(LOG_CORE, "gpio%{public}d not export", gpioNum);
        return UM_GPIO_NOT_EXPROT_ERROR; // 返回未导出错误
    }

    // 打开GPIO值文件以读取
    FILE *fp = fopen(gpio_file_name, "r");
    if (fp == NULL) {
        HILOG_ERROR(LOG_CORE, "read %{public}s%{public}d/value failed", UM_GPIO_PEX, gpioNum);
        return UM_GPIO_ERR; // 打开文件失败
    }

    char buffer[20] = {0};
    (void) fread(buffer, sizeof(buffer), 1, fp); // 读取内容
    (void) fclose(fp);

    // 解析值
    if (strstr(buffer, "0") != NULL) {
        *value = UM_GPIO_LOW_LEVE;
    } else if (strstr(buffer, "1") != NULL) {
        *value = UM_GPIO_HIGH_LEVE;
    } else {
        ret = UM_GPIO_ERR; // 解析错误
    }
    return ret; // 返回结果
}
