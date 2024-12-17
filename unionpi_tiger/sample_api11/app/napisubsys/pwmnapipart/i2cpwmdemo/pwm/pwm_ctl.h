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
#ifndef __PWM_CTL_H_
#define __PWM_CTL_H_

#include <cstdint>
#include <string>
#include <pwm_if.h>
namespace Pwm {
    namespace Channel {
        constexpr uint32_t pwm1 = 0;
        constexpr uint32_t pwm2 = 2;
    }

    namespace Polarity {
        constexpr uint8_t Normal = PWM_NORMAL_POLARITY;
        constexpr uint8_t Inversed = PWM_INVERTED_POLARITY;
    }

    class PwmCtl {
    public:
        /**
         * @brief 构造pwm控制对象
         * @param channel PWM插槽,pwm1:PWM插槽1 pwm2:PWM插槽2
         */
        explicit PwmCtl(uint32_t channel);

        ~PwmCtl();
        /**
         * @brief 设置是否启用
         * @param isEnable 是否启用
         */
        void setEnable(bool isEnable);

        /**
         * @brief 获取启用状态
         * @return 是否启用
         */
        bool getEnable();

        /**
         * @brief 设置PWM周期时间
         * @param period PWM周期时间(单位纳秒)
         */
        void setPeriod(uint32_t period);

        /**
         * @brief 获取PWM周期时间
         * @return PWM周期时间(单位纳秒)
         */
        uint32_t getPeriod();

        /**
         * @brief 设置PWM高电平时间
         * @param dutyCycle PWM高电平时间(单位纳秒)
         */
        void setDutyCycle(uint32_t dutyCycle);

        /**
         * @brief 获取PWM高电平时间
         * @return dutyCycle PWM高电平时间(单位纳秒)
         */
        uint32_t getDutyCycle();

        /**
         * @brief 设置舵机方向
         * @param polarity 舵机方向,Normal:正转 Inversed:反转
         */
        void setPolarity(uint8_t polarity);

        /**
         * @brief 获取舵机方向
         * @return 舵机方向,Normal:正转 Inversed:反转
         */
        uint8_t getPolarity();

    protected:
        DevHandle handle = nullptr;
    };
}

#endif