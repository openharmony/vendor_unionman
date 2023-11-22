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
#include <napi/native_api.h>
#include <napi/native_node_api.h>
#include "gpio/gpio_ctl.h"
using namespace Gpio;
class GpioNapi {
public:
    static napi_value define(napi_env env)
    {
        napi_value gpioClass;
        napi_property_descriptor desc[] = {
            DECLARE_NAPI_FUNCTION("read", read),
            DECLARE_NAPI_FUNCTION("write", write)
        };
        napi_define_class(
            env, "Gpio", NAPI_AUTO_LENGTH, Constructor, nullptr, sizeof(desc) / sizeof(desc[0]), desc, &gpioClass);
        napi_create_reference(env, gpioClass, 1, &gpioClassRef);
        return gpioClass;
    }

    static napi_value Constructor(napi_env env, napi_callback_info cb)
    {
        napi_value this_var;
        size_t argc = 2L;
        napi_value argv[2L];
        napi_get_cb_info(env, cb, &argc, argv, &this_var, nullptr);
        NAPI_ASSERT(env, argc == 2L, "requires 2 parameter");
        napi_valuetype type;
        napi_typeof(env, argv[0], &type);
        NAPI_ASSERT(env, type == napi_number, "frist parameter type requires number");
        napi_typeof(env, argv[1], &type);
        NAPI_ASSERT(env, type == napi_number, "2nd parameter type requires number");

        uint32_t gpioNum, dir;
        napi_get_value_uint32(env, argv[0], &gpioNum);
        napi_get_value_uint32(env, argv[1], &dir);

        auto* gpio = new GpioCtl(static_cast<GpioName>(gpioNum), static_cast<Direction>(dir));
        napi_wrap(env, this_var, gpio, Destructor, nullptr, nullptr);
        return this_var;
    }

    static void Destructor(napi_env env, void* nativeObject, void* finalize_hint)
    {
        auto* gpio = reinterpret_cast<GpioCtl*>(nativeObject);
        delete gpio;
    }

    static napi_value read(napi_env env, napi_callback_info cb)
    {
        size_t argc = 0;
        napi_value this_var, gpioClass;
        napi_get_cb_info(env, cb, &argc, nullptr, &this_var, nullptr);

        napi_get_reference_value(env, gpioClassRef, &gpioClass);

        bool isInstance;
        napi_instanceof(env, this_var, gpioClass, &isInstance);
        NAPI_ASSERT(env, isInstance, "this read function isn't static function!");

        GpioCtl* gpio;
        napi_unwrap(env, this_var, reinterpret_cast<void**>(&gpio));
        Val data = gpio->read();

        napi_value ret;
        napi_create_uint32(env, static_cast<uint32_t>(data), &ret);
        return ret;
    }

    static napi_value write(napi_env env, napi_callback_info cb)
    {
        size_t argc = 1;
        napi_value ret, this_var, argv, gpioClass;
        napi_get_cb_info(env, cb, &argc, &argv, &this_var, nullptr);

        napi_valuetype type;
        napi_typeof(env, argv, &type);
        NAPI_ASSERT(env, type == napi_number, "frist parameter type requires number");

        napi_get_reference_value(env, gpioClassRef, &gpioClass);

        bool isInstance;
        napi_instanceof(env, this_var, gpioClass, &isInstance);
        NAPI_ASSERT(env, isInstance, "this read function isn't static function!");

        GpioCtl* gpio;
        napi_unwrap(env, this_var, reinterpret_cast<void**>(&gpio));

        uint32_t data;
        napi_get_value_uint32(env, argv, &data);
        gpio->write(static_cast<Val>(data));

        napi_get_undefined(env, &ret);
        return ret;
    }
protected:
    static napi_ref gpioClassRef;
};
napi_ref GpioNapi::gpioClassRef = nullptr;


static napi_value defineGpioName(napi_env env)
{
    napi_value gpioName, gpio1, gpio2, gpio3, gpio4, gpio5, gpio6, gpio7, gpio8, gpio9, gpio10, gpio11, gpio12, gpio13,
        gpio14, gpio15, gpio16;
    napi_create_object(env, &gpioName);

    napi_create_uint32(env, static_cast<uint32_t>(GpioName::GPIO_01), &gpio1);
    napi_create_uint32(env, static_cast<uint32_t>(GpioName::GPIO_02), &gpio2);
    napi_create_uint32(env, static_cast<uint32_t>(GpioName::GPIO_03), &gpio3);
    napi_create_uint32(env, static_cast<uint32_t>(GpioName::GPIO_04), &gpio4);
    napi_create_uint32(env, static_cast<uint32_t>(GpioName::GPIO_05), &gpio5);
    napi_create_uint32(env, static_cast<uint32_t>(GpioName::GPIO_06), &gpio6);
    napi_create_uint32(env, static_cast<uint32_t>(GpioName::GPIO_07), &gpio7);
    napi_create_uint32(env, static_cast<uint32_t>(GpioName::GPIO_08), &gpio8);
    napi_create_uint32(env, static_cast<uint32_t>(GpioName::GPIO_09), &gpio9);
    napi_create_uint32(env, static_cast<uint32_t>(GpioName::GPIO_10), &gpio10);
    napi_create_uint32(env, static_cast<uint32_t>(GpioName::GPIO_11), &gpio11);
    napi_create_uint32(env, static_cast<uint32_t>(GpioName::GPIO_12), &gpio12);
    napi_create_uint32(env, static_cast<uint32_t>(GpioName::GPIO_13), &gpio13);
    napi_create_uint32(env, static_cast<uint32_t>(GpioName::GPIO_14), &gpio14);
    napi_create_uint32(env, static_cast<uint32_t>(GpioName::GPIO_15), &gpio15);
    napi_create_uint32(env, static_cast<uint32_t>(GpioName::GPIO_16), &gpio16);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("GPIO_01", gpio1),
        DECLARE_NAPI_STATIC_PROPERTY("GPIO_02", gpio2),
        DECLARE_NAPI_STATIC_PROPERTY("GPIO_03", gpio3),
        DECLARE_NAPI_STATIC_PROPERTY("GPIO_04", gpio4),
        DECLARE_NAPI_STATIC_PROPERTY("GPIO_05", gpio5),
        DECLARE_NAPI_STATIC_PROPERTY("GPIO_06", gpio6),
        DECLARE_NAPI_STATIC_PROPERTY("GPIO_07", gpio7),
        DECLARE_NAPI_STATIC_PROPERTY("GPIO_08", gpio8),
        DECLARE_NAPI_STATIC_PROPERTY("GPIO_09", gpio9),
        DECLARE_NAPI_STATIC_PROPERTY("GPIO_10", gpio10),
        DECLARE_NAPI_STATIC_PROPERTY("GPIO_11", gpio11),
        DECLARE_NAPI_STATIC_PROPERTY("GPIO_12", gpio12),
        DECLARE_NAPI_STATIC_PROPERTY("GPIO_13", gpio13),
        DECLARE_NAPI_STATIC_PROPERTY("GPIO_14", gpio14),
        DECLARE_NAPI_STATIC_PROPERTY("GPIO_15", gpio15),
        DECLARE_NAPI_STATIC_PROPERTY("GPIO_16", gpio16)
    };
    napi_define_properties(env, gpioName, sizeof(desc) / sizeof(desc[0]), desc);
    return gpioName;
}

static napi_value defineDir(napi_env env)
{
    napi_value dir, input, output, error;
    napi_create_object(env, &dir);

    napi_create_uint32(env, static_cast<uint32_t>(Direction::input), &input);
    napi_create_uint32(env, static_cast<uint32_t>(Direction::output), &output);
    napi_create_uint32(env, static_cast<uint32_t>(Direction::error), &error);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("input", input),
        DECLARE_NAPI_STATIC_PROPERTY("output", output),
        DECLARE_NAPI_STATIC_PROPERTY("error", error)
    };
    napi_define_properties(env, dir, sizeof(desc) / sizeof(desc[0]), desc);
    return dir;
}

static napi_value defineVal(napi_env env)
{
    napi_value val, low, height;
    napi_create_object(env, &val);

    napi_create_uint32(env, static_cast<uint32_t>(Val::low), &low);
    napi_create_uint32(env, static_cast<uint32_t>(Val::height), &height);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("low", low),
        DECLARE_NAPI_STATIC_PROPERTY("height", height)
    };
    napi_define_properties(env, val, sizeof(desc) / sizeof(desc[0]), desc);
    return val;
}

static napi_value registerGpioCtl_Apis(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("GpioName", defineGpioName(env)),
        DECLARE_NAPI_STATIC_PROPERTY("Dir", defineDir(env)),
        DECLARE_NAPI_STATIC_PROPERTY("Val", defineVal(env)),
        DECLARE_NAPI_STATIC_PROPERTY("Gpio", GpioNapi::define(env))
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

NAPI_MODULE(gpio_ctl, registerGpioCtl_Apis)