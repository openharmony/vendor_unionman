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
#include "pwm_ctl.h"
namespace Pwm {
    PwmCtl::PwmCtl(uint32_t channel)
    {
        DevHandle handle = PwmOpen(channel);
        this->handle = handle;
        PwmConfig conf = {
            .duty = 0,
            // 硬件文档指定的周期为50hz,即周期时间为20000000纳秒
            .period = 20000000,
            .number = 0,
            .polarity = Polarity::Normal,
            .status = PWM_ENABLE_STATUS
        };
        PwmSetConfig(handle, &conf);
    }

    PwmCtl::~PwmCtl()
    {
        PwmDisable(handle);
        PwmClose(handle);
    }

    void PwmCtl::setEnable(bool isEnable)
    {
        if (isEnable) {
            PwmEnable(handle);
        } else {
            PwmDisable(handle);
        }
    }

    bool PwmCtl::getEnable()
    {
        PwmConfig conf;
        PwmGetConfig(handle, &conf);
        return conf.status == PWM_ENABLE_STATUS;
    }

    void PwmCtl::setPeriod(uint32_t period)
    {
        PwmSetPeriod(handle, period);
    }

    uint32_t PwmCtl::getPeriod()
    {
        PwmConfig conf;
        PwmGetConfig(handle, &conf);
        return conf.period;
    }

    void PwmCtl::setDutyCycle(uint32_t dutyCycle)
    {
        PwmSetDuty(handle, dutyCycle);
    }

    uint32_t PwmCtl::getDutyCycle()
    {
        PwmConfig conf;
        PwmGetConfig(handle, &conf);
        return conf.duty;
    }

    void PwmCtl::setPolarity(uint8_t polarity)
    {
        PwmSetPolarity(handle, polarity);
    }

    uint8_t PwmCtl::getPolarity()
    {
        PwmConfig conf;
        PwmGetConfig(handle, &conf);
        return conf.polarity;
    }
}
