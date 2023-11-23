/* Copyright 2023 Unionman Technology Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "gpio_ctl.h"
namespace Gpio {
    GpioCtl::GpioCtl(GpioName gpio, Direction direction)
        : gpio(gpio)
    {
        GpioSetDir(static_cast<uint32_t>(gpio), static_cast<uint32_t>(direction));
    }

    Val GpioCtl::read()
    {
        uint16_t data;
        GpioRead(static_cast<uint32_t>(gpio), &data);
        return static_cast<Val>(data);
    }

    bool GpioCtl::write(Val data)
    {
        return GpioWrite(static_cast<uint32_t>(gpio), static_cast<uint16_t>(data)) == HDF_SUCCESS;
    }
} // namespace Gpio