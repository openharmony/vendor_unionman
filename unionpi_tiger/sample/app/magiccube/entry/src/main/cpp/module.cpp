/*
 * Copyright 2023 Unionman Technology Co., Ltd.
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

#include <vertex_manger.h>
#include "app_napi.h"
#include "log.h"
#include "napi_manager.h"
#include "native_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * function for module exports
 */
static napi_value ExportAxis(napi_env env)
{
    napi_value axis;
    napi_create_object(env, &axis);
    napi_value x, y, z;
    napi_create_uint32(env, static_cast<uint32_t>(Axis::X), &x);
    napi_create_uint32(env, static_cast<uint32_t>(Axis::Y), &y);
    napi_create_uint32(env, static_cast<uint32_t>(Axis::Z), &z);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("X", x),
        DECLARE_NAPI_STATIC_PROPERTY("Y", y),
        DECLARE_NAPI_STATIC_PROPERTY("Z", z),
    };
    NAPI_CALL(env, napi_define_properties(env, axis, sizeof(desc) / sizeof(desc[0]), desc));
    return axis;
}

static napi_value ExportDirection(napi_env env)
{
    napi_value direction;
    napi_create_object(env, &direction);
    napi_value left, middle, right;
    napi_create_uint32(env, static_cast<uint32_t>(Direction::Left), &left);
    napi_create_uint32(env, static_cast<uint32_t>(Direction::Middle), &middle);
    napi_create_uint32(env, static_cast<uint32_t>(Direction::Right), &right);
    napi_property_descriptor desc[] = { DECLARE_NAPI_STATIC_PROPERTY("Left", left),
        DECLARE_NAPI_STATIC_PROPERTY("Middle", middle), DECLARE_NAPI_STATIC_PROPERTY("Right", right) };
    NAPI_CALL(env, napi_define_properties(env, direction, sizeof(desc) / sizeof(desc[0]), desc));
    return direction;
}

static napi_value Init(napi_env env, napi_value exports)
{
    LOGE("Init");
    napi_property_descriptor desc[] = { DECLARE_NAPI_STATIC_PROPERTY("Axis", ExportAxis(env)),
        DECLARE_NAPI_STATIC_PROPERTY("Direction", ExportDirection(env)),
        DECLARE_NAPI_FUNCTION("getContext", NapiManager::GetContext),
        DECLARE_NAPI_STATIC_FUNCTION("updateAngle", AppNapi::UpdateAngle),
        DECLARE_NAPI_STATIC_FUNCTION("twist", AppNapi::Twist) };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    bool ret = NapiManager::GetInstance()->Export(env, exports);
    if (!ret) {
        LOGE("Init failed");
    }

    return exports;
}

/*
 * Napi Module define
 */
static napi_module appNapiModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "magiccube",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

/*
 * Module register function
 */
extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&appNapiModule);
}

#ifdef __cplusplus
}
#endif