/* Copyright 2022 Unionman Technology Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#undef LOG_TAG
#define LOG_TAG "spi_test"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "hilog/log.h"
#include "um_spi.h"

// 日期长度
static int g_data_len = 17;
// 内容最大长度
static int g_value_len = 200;
// 日期加内容最大总长度
static int g_sum_len = 217;
// 存sum_list的首地址
static int g_sum_list_address = 0x0000;
// 存日期和内容的首地址
static int g_flash_address = 0x1000;

static int g_gpio = 486;
// 从flash中读取sum_list的值
static int get_sum_list(void)
{
    uint16_t maxlen = 11;
    uint32_t read_address = g_sum_list_address;
    uint8_t tx_read_buff[maxlen], read_buff[maxlen];
    uint32_t result = 0;
    int spi_ret = 0;
    int i = 0, j = 0;

    // 设置spi接口传输
    sysfs_gpio_export(g_gpio, 1);
    // 设置spi接口传输方向为输出
    spi_ret = sysfs_gpio_direction(g_gpio, 1);
    if (spi_ret < 0) {
        return spi_ret;
    }
    spi_buffer_read(read_buff, read_address, maxlen);
    sysfs_gpio_export(g_gpio, 0);

    // 复制出合法的字符串
    do {
        tx_read_buff[i] = read_buff[i];
        i++;
    } while (read_buff[i] != 255L && i < maxlen);

    // uint8_t转换为uint32_t
    for (j = i - 1; j >= 0; --j) {
        result <<= 8L;
        result += tx_read_buff[j];
    }
    return result;
}
// 同步方法实现获取flashid值
static napi_value sync_get_flashid_value(napi_env env, napi_callback_info info)
{
    uint8_t dataID[4];
    napi_value ret = nullptr;
    uint32_t result = 0;
    int spi_ret = 0;
    int i = 0;

    // 设置spi接口传输
    sysfs_gpio_export(g_gpio, 1);
    // 设置spi接口传输方向为输出
    spi_ret = sysfs_gpio_direction(g_gpio, 1);
    if (spi_ret < 0) {
        return ret;
    }
    spi_read_flashid(dataID);
    sysfs_gpio_export(g_gpio, 0);

    // uint8_t转换为uint32_t
    for (i = 3L; i >= 0; --i) {
        result <<= 8L;
        result += dataID[i];
    }
    NAPI_CALL(env, napi_create_uint32(env, result, &ret));
    return ret;
}
// 同步方法实现获取deviceid值
static napi_value sync_get_deviceid_value(napi_env env, napi_callback_info info)
{
    uint8_t dataID[6];
    uint32_t result = 0;
    napi_value ret = nullptr;
    int spi_ret = 0;
    int i = 0;

    // 设置spi接口传输
    sysfs_gpio_export(g_gpio, 1);
    // 设置spi接口传输方向为输出
    spi_ret = sysfs_gpio_direction(g_gpio, 1);
    if (spi_ret < 0) {
        return ret;
    }
    spi_read_deviceid(dataID);
    sysfs_gpio_export(g_gpio, 0);
    // uint8_t转换为uint32_t
    for (i = 5L; i >= 4L; --i) {
        result <<= 8L;
        result += dataID[i];
    }
    NAPI_CALL(env, napi_create_uint32(env, result, &ret));
    return ret;
}

static napi_value sync_write_buffer_value(napi_env env, napi_callback_info info)
{
    // 接收参数
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    napi_valuetype valuetype[3];
    int value_length = 0;
    char *value;
    char *date;

    int sum_list = 0, spi_ret = 0, i = 0;
    size_t size = 0, time_size = g_data_len;
    napi_value ret = nullptr;
    uint32_t write_address = g_flash_address;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));

    // 1 验证参数个数
    NAPI_ASSERT(env, argc >= 3L, "wrong number of argument\n");

    // 2 验证参数类型
    for (i = 0; i < 3L; i++) {
        NAPI_CALL(env, napi_typeof(env, args[i], &valuetype[i]));
    }
    NAPI_ASSERT(env, valuetype[0] == napi_number && valuetype[1] == napi_string && valuetype[2L] == napi_string,
                "Wrong argument type. Numbers expected.");

    // 3 接收参数
    // 3-1 接收value的长度参数：value_length
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &value_length));
    NAPI_ASSERT(env, value_length <= g_value_len, "value is too long");

    // 3-2 接收写入的内容参数：value
    size = (size_t)value_length;
    value = (char *)malloc(size * sizeof(char));
    NAPI_CALL(env, napi_get_value_string_utf8(env, args[1], value, size, &size));

    // 3-3接收写入的时间参数：date
    date = (char *)malloc(time_size * sizeof(char));
    NAPI_CALL(env, napi_get_value_string_utf8(env, args[2L], date, time_size, &time_size));

    // 设置spi接口传输
    sysfs_gpio_export(g_gpio, 1);
    // 设置spi接口传输方向为输出
    spi_ret = sysfs_gpio_direction(g_gpio, 1);
    if (spi_ret < 0) {
        return ret;
    }
    sum_list = get_sum_list();
    if (sum_list < 0) {
        return ret;
    }
    // 更新sum_list
    sum_list++;
    spi_sector_erase(g_sum_list_address);
    spi_buffer_write((uint8_t *)&sum_list, g_sum_list_address, sizeof(sum_list));

    // 添加date和value
    write_address = write_address + (sum_list - 1) * g_sum_len;
    spi_buffer_write((uint8_t *)date, write_address, time_size);
    write_address = write_address + g_data_len;
    spi_buffer_write((uint8_t *)value, write_address, value_length);
    sysfs_gpio_export(g_gpio, 0);

    // 回传value
    NAPI_CALL(env, napi_create_string_utf8(env, value, size, &ret));

    // 释放申请的动态空间
    free(value);
    free(date);

    return ret;
}

static napi_value sync_read_value(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_valuetype valuetype0;
    int item = 0;

    const int maxlen = g_sum_len;
    uint16_t len = maxlen;
    int spi_ret = 0;
    uint8_t tx_read_buff[maxlen], read_buff[maxlen];
    uint32_t read_address = g_flash_address;
    napi_value ret = nullptr;
    int i = 0;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    // 验证参数个数
    NAPI_ASSERT(env, argc >= 1, "wrong number of argument\n");
    // 验证参数类型
    NAPI_CALL(env, napi_typeof(env, args[0], &valuetype0));
    NAPI_ASSERT(env, valuetype0 == napi_number, "Wrong argument type. Numbers expected.");

    // 接收value2的长度value1参数
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &item));
    // 初始化读的初始地址
    read_address = read_address + (item * g_sum_len) + g_data_len;

    // 设置spi接口传输
    sysfs_gpio_export(g_gpio, 1);
    // 设置spi接口传输方向为输出
    spi_ret = sysfs_gpio_direction(g_gpio, 1);
    if (spi_ret < 0) {
        return ret;
    }
    spi_buffer_read(read_buff, read_address, len);
    sysfs_gpio_export(g_gpio, 0);

    // 复制出合法的字符串
    do {
        tx_read_buff[i] = read_buff[i];
        i++;
    } while (read_buff[i] != 255L && i <= g_value_len);
    const char *cdataID = (char *)tx_read_buff;
    NAPI_CALL(env, napi_create_string_utf8(env, cdataID, i, &ret));
    return ret;
}

static napi_value sync_read_date(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_valuetype valuetype0;
    int item = 0;

    const int maxlen = g_data_len;
    uint32_t read_address = g_flash_address;
    uint8_t tx_read_buff[maxlen], read_buff[maxlen];
    napi_value ret = nullptr;
    int i = 0;
    int spi_ret = 0;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    // 验证参数个数
    NAPI_ASSERT(env, argc >= 1, "wrong number of argument\n");
    // 验证参数类型
    NAPI_CALL(env, napi_typeof(env, args[0], &valuetype0));
    NAPI_ASSERT(env, valuetype0 == napi_number, "Wrong argument type. Numbers expected.");

    // 接收value2的长度value1参数
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &item));
    // 初始化读的初始地址
    read_address = read_address + item * g_sum_len;

    // 设置spi接口传输
    sysfs_gpio_export(g_gpio, 1);
    // 设置spi接口传输方向为输出
    spi_ret = sysfs_gpio_direction(g_gpio, 1);
    if (spi_ret < 0) {
        return ret;
    }
    spi_buffer_read(read_buff, read_address, maxlen);
    sysfs_gpio_export(g_gpio, 0);
    // 复制出合法的字符串
    do {
        tx_read_buff[i] = read_buff[i];
        i++;
    } while (read_buff[i] != 255L && i < maxlen);
    const char *cdataID = (char *)tx_read_buff;
    NAPI_CALL(env, napi_create_string_utf8(env, cdataID, i, &ret));
    return ret;
}

static napi_value sync_read_sum_list(napi_env env, napi_callback_info info)
{
    napi_value ret = nullptr;
    int sum_list = 0;
    sum_list = get_sum_list();
    if (sum_list < 0) {
        return ret;
    }
    NAPI_CALL(env, napi_create_int32(env, sum_list, &ret));
    return ret;
}

static napi_value sync_all_erase(napi_env env, napi_callback_info info)
{
    int sum_list = 0;
    int sectorErase = 0;
    int valueArea = 0;
    uint32_t valueAddress = g_sum_list_address;
    uint8_t writebuff[1];
    // 单个扇区的大小
    uint32_t area = g_flash_address;
    napi_value ret = nullptr;
    int result = 0;
    int spi_ret = 0;
    int i = 0;

    // 读取sum_list数据
    sum_list = get_sum_list();
    // 算出整个记录范围
    valueArea = g_sum_len * sum_list;
    // 算出要扇区擦除的个数
    sectorErase = valueArea / area + 1;

    // 设置spi接口传输
    sysfs_gpio_export(g_gpio, 1);
    // 设置spi接口传输方向为输出
    spi_ret = sysfs_gpio_direction(g_gpio, 1);
    if (spi_ret < 0) {
        return ret;
    }
    // 开始擦除
    for (i = 0; i <= sectorErase; i++) {
        result = spi_sector_erase(valueAddress);
        if (result < 0) {
            return ret;
        }
        valueAddress += area;
    }
    // 修改sum_list的值
    writebuff[0] = 0;
    spi_buffer_write(writebuff, g_sum_list_address, 1);
    sysfs_gpio_export(g_gpio, 0);
    // 成功擦除返回1
    result = 1;
    NAPI_CALL(env, napi_create_int32(env, result, &ret));
    return ret;
}

static napi_value sync_get_MB_Free(napi_env env, napi_callback_info info)
{
    int sum_list = 0;
    int sum_MB = 0X3FFFFF;
    int sum_list_MB = 0X1000;
    int result = 0;
    napi_value ret = nullptr;
    sum_list = get_sum_list();
    if (sum_list < 0) {
        return ret;
    }
    int t = sum_list * g_sum_len + sum_list_MB;
    result = (sum_MB - t) * 100L / sum_MB;
    NAPI_CALL(env, napi_create_int32(env, result, &ret));
    return ret;
}
/*
 * 注册接口
 */
static napi_value registerUm_SpiApis(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {

        // 声明函数供ets调用
        DECLARE_NAPI_FUNCTION("sync_get_flashid_value", sync_get_flashid_value),

        DECLARE_NAPI_FUNCTION("sync_get_deviceid_value", sync_get_deviceid_value),

        DECLARE_NAPI_FUNCTION("sync_write_buffer_value", sync_write_buffer_value),

        DECLARE_NAPI_FUNCTION("sync_read_date", sync_read_date),

        DECLARE_NAPI_FUNCTION("sync_read_value", sync_read_value),

        DECLARE_NAPI_FUNCTION("sync_read_sum_list", sync_read_sum_list),

        DECLARE_NAPI_FUNCTION("sync_all_erase", sync_all_erase),

        DECLARE_NAPI_FUNCTION("sync_get_MB_Free", sync_get_MB_Free),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

/*
 * 模块定义
 */
static napi_module um_spiModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = registerUm_SpiApis,
    // 模块名
    .nm_modname = "um_spi",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

/*
 * 注册模块
 */
extern "C" __attribute__((constructor)) void RegisterUm_SpiModule(void)
{
    // 接口注册函数
    napi_module_register(&um_spiModule);
}