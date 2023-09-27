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
#include <napi/native_api.h>
#include <napi/native_node_api.h>
#include "i2c/i2c_ctl.h"

constexpr uint8_t address = 0x44;
constexpr uint16_t i2cNumber = 5;

static I2c::I2cCtl i2c(i2cNumber, address);

struct asyncPromiseData {
    napi_async_work asyncWork;
    napi_deferred deferred;
    I2c::HumTem data;
};

static napi_value getDataAsync(napi_env env, napi_callback_info cb)
{
    asyncPromiseData* pd = new asyncPromiseData;
    napi_value promise;
    napi_create_promise(env, &pd->deferred, &promise);
    napi_value resourceName;
    napi_create_string_utf8(env, "getDataAsync", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, nullptr, resourceName,
        [](napi_env env, void* data) {
            asyncPromiseData* pd = reinterpret_cast<asyncPromiseData*>(data);
            pd->data = i2c.readData();
        },
        [](napi_env env, napi_status status, void* data) {
            asyncPromiseData* pd = reinterpret_cast<asyncPromiseData*>(data);
            napi_value obj, temc, temf, hum;
            napi_create_object(env, &obj);

            napi_create_double(env, pd->data.temperatureC, &temc);
            napi_set_named_property(env, obj, "temperatureC", temc);
            napi_create_double(env, pd->data.temperatureF, &temf);
            napi_set_named_property(env, obj, "temperatureF", temf);
            napi_create_double(env, pd->data.humidity, &hum);
            napi_set_named_property(env, obj, "humidity", hum);

            napi_resolve_deferred(env, pd->deferred, obj);
            napi_delete_async_work(env, pd->asyncWork);
            delete pd;
        }, pd, &pd->asyncWork);
    napi_queue_async_work(env, pd->asyncWork);
    return promise;
}

static napi_value registerI2cCtl_Apis(napi_env env, napi_value exports)
{
    i2c.restart();
    i2c.setMode(I2c::mode::highFrequency4Hz);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getDataAsync", getDataAsync),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

NAPI_MODULE(i2c_ctl, registerI2cCtl_Apis)