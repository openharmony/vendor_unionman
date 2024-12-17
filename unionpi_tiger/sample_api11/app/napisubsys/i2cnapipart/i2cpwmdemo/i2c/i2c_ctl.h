/*
 * Copyright (c) 2023 Unionman Technology Co., Ltd.
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
#ifndef __I2C_CTL_H_
#define __I2C_CTL_H_

#include <cstddef>
#include <cstdint>
#include <i2c_if.h>
namespace I2c {
    /// @brief 命令
    namespace cmd {
        /// @brief 关闭周期测量
        constexpr uint16_t stopPeriodicAcquisition = 0x3093;
        /// @brief 软重启
        constexpr uint16_t softReset = 0x30A2;
        /// @brief 获取测量数据
        constexpr uint16_t getData = 0xE000;
    }

    /// @brief 测量模式
    namespace mode {
        /// @brief 高精准度，每秒测量4次
        constexpr uint16_t highFrequency4Hz = 0x2334;
        /// @brief 中精准度，每秒测量4次
        constexpr uint16_t mediumFrequency4Hz = 0x2322;
        /// @brief 低精准度，每秒测量4次
        constexpr uint16_t lowFrequency4Hz = 0x2329;

        /// @brief 高精准度，每秒测量2次
        constexpr uint16_t highFrequency2Hz = 0x2236;
        /// @brief 中精准度，每秒测量2次
        constexpr uint16_t mediumFrequency2Hz = 0x2220;
        /// @brief 低精准度，每秒测量2次
        constexpr uint16_t lowFrequency2Hz = 0x222B;

        /// @brief 高精准度，每秒测量1次
        constexpr uint16_t highFrequency1Hz = 0x2130;
        /// @brief 中精准度，每秒测量1次
        constexpr uint16_t mediumFrequency1Hz = 0x2126;
        /// @brief 低精准度，每秒测量1次
        constexpr uint16_t lowFrequency1Hz = 0x212D;

        /// @brief 高精准度，2秒测量1次
        constexpr uint16_t highFrequencyHalfHz = 0x2032;
        /// @brief 中精准度，2秒测量1次
        constexpr uint16_t mediumFrequencyHalfHz = 0x2024;
        /// @brief 低精准度，2秒测量1次
        constexpr uint16_t lowFrequencyHalfHz = 0x202F;
    }

    /// @brief 测量结果
    struct HumTem {
        /// @brief 摄氏度
        float temperatureC;
        /// @brief 湿度
        float humidity;
        /// @brief 华氏度
        float temperatureF;
    };

    class I2cCtl {
    public:
        I2cCtl(uint16_t i2cNumber, uint8_t address) noexcept;
        /// @brief 软重启设备
        void restart();
        /// @brief 设置测量模式
        /// @param  mode 测量模式
        /// @return 是否成功
        bool setMode(uint16_t mode);
        /// @brief 发送命令
        /// @param cmd 命令
        /// @return 是否成功
        bool sendCmd(uint16_t cmd);
        /// @brief  发送命令并接收数据
        /// @param cmd 命令
        /// @param data 用于接收数据的数组引用
        /// @return 是否成功
        template <typename T, size_t size>
        bool sendCmdAndRev(uint16_t cmd, T (&data)[size]);
        /// @brief 读取测量数据
        /// @return 测量数据
        HumTem readData();
        ~I2cCtl();

    protected:
        DevHandle handle;
        uint8_t address;
        static uint16_t swapBit(uint16_t src);
        static uint8_t crc(uint8_t* src);
    };
} // namespace I2c

#endif
