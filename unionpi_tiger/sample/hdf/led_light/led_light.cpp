/* Copyright 2022 Unionman Technology Co., Ltd.
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

#include <cstdlib>
#include <securec.h>
#include "unistd.h"
#include "hdf_base.h"
#include "osal_time.h"
#include "osal_mem.h"
#include "light_if.h"
#include "light_type.h"
#include "hilog/log.h"

int main(int argc, char* argv[])
{
    struct LightInfo *lightInfo = nullptr;
    int32_t i;
    uint32_t count = 0;
    struct LightEffect effect;
    const int32_t onTime = 50; // unit : ms
    const int32_t offTime = 50;

    const struct LightInterface *lightDev = NewLightInterfaceInstance();
    if (lightDev == nullptr) {
        printf("test light get Module instance failed\n\r");
    }

    int32_t ret = lightDev->GetLightInfo(&lightInfo, &count);
    if (ret == HDF_FAILURE) {
        printf("get light information failed\n\r");
    }

    printf("Light count %d\n\r", count);

    effect.lightBrightness = 0x000000FF; /* Only power led is valid */
    effect.flashEffect.flashMode = LIGHT_FLASH_NONE;
    effect.flashEffect.onTime = onTime;
    effect.flashEffect.offTime = offTime;

    for (i = 0; i < count; ++i) {
        printf("TurnOffLight ...\n\r");
        ret = lightDev->TurnOffLight(lightInfo[i].lightId);
        if (ret == HDF_FAILURE) {
            printf("TurnOffLight failed\n\r");
        }

        OsalSleep(1);

        printf("TurnOnLight ...\n\r");
        ret = lightDev->TurnOnLight(lightInfo[i].lightId, &effect);
        if (ret == HDF_FAILURE) {
            printf("TurnOnLight failed\n\r");
        }

        OsalSleep(1);

        printf("TurnOffLight ...\n\r");
        ret = lightDev->TurnOffLight(lightInfo[i].lightId);
        if (ret == HDF_FAILURE) {
            printf("TurnOffLight failed\n\r");
        }

        OsalSleep(1);

        printf("TurnOnLight ...\n\r");
        ret = lightDev->TurnOnLight(lightInfo[i].lightId, &effect);
        if (ret == HDF_FAILURE) {
            printf("TurnOnLight failed\n\r");
        }
    }

    if (lightDev != nullptr) {
        FreeLightInterfaceInstance();
        lightDev = nullptr;
    }

    return HDF_SUCCESS;
}
