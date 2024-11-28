/*
 * Copyright (c) 2024 Unionman Technology Co., Ltd.
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
#include "bignum_napi.h"

#include <bits/alltypes.h>
#include <cstdint>
#include <cstring>
#include <napi/native_common.h>
#include <node_api.h>

#include "boost/multiprecision/fwd.hpp"
#include "boost/multiprecision/integer.hpp"

napi_ref BignumNapi::jsClassRef = nullptr;

napi_value BignumNapi::constructor(napi_env env, napi_callback_info cb)
{
    boost::multiprecision::cpp_int* bigint;
    bool result;
    size_t argc = 1;
    napi_value argv, thisArg;
    uint8_t* buffer;
    size_t bufferLen;
    napi_get_cb_info(env, cb, &argc, &argv, &thisArg, nullptr);
    if (argc == 0) {
        return thisArg;
    }

    napi_is_arraybuffer(env, argv, &result);
    if (result) {
        napi_get_arraybuffer_info(env, argv, (void**)&buffer, &bufferLen);
        boost::multiprecision::cpp_int temp = cppint_from_byte_array(buffer, bufferLen);
        bigint = new boost::multiprecision::cpp_int(std::move(temp));
    } else {
        napi_valuetype valueType;
        napi_typeof(env, argv, &valueType);
        switch (valueType) {
            case napi_number:
                uint32_t number;
                napi_get_value_uint32(env, argv, &number);
                bigint = new boost::multiprecision::cpp_int(number);
                break;
            case napi_string:
                char str[128];
                size_t strLen;
                napi_get_value_string_utf8(env, argv, str, 128, &strLen);
                {
                    std::string s(str, strLen);
                    bigint = new boost::multiprecision::cpp_int(s);
                }
                break;
            default:
                return nullptr;
                break;
        }
    }
    napi_wrap(
        env, thisArg, bigint,
        [](napi_env env, void* finalize_data, void* finalize_hint) {
            delete (boost::multiprecision::cpp_int*)finalize_data;
        },
        nullptr, nullptr);
    return thisArg;
}

napi_value BignumNapi::powm(napi_env env, napi_callback_info cb)
{
    boost::multiprecision::cpp_int *thisBigInt, *resultBigint;
    size_t argc = 2;
    napi_value argv[2], thisArg, result, jsClass;
    napi_valuetype expType;
    uint32_t p;
    napi_get_cb_info(env, cb, &argc, argv, &thisArg, nullptr);
    napi_get_value_uint32(env, argv[1], &p);
    napi_unwrap(env, thisArg, (void**)&thisBigInt);
    napi_get_reference_value(env, jsClassRef, &jsClass);

    napi_typeof(env, argv[0], &expType);
    if (expType == napi_number) {
        uint32_t exp;
        napi_get_value_uint32(env, argv[0], &exp);
        boost::multiprecision::cpp_int&& temp = boost::multiprecision::powm(*thisBigInt, exp, p);
        resultBigint = new boost::multiprecision::cpp_int(temp);
        napi_new_instance(env, jsClass, 0, nullptr, &result);
        napi_wrap(
            env, result, resultBigint,
            [](napi_env env, void* finalize_data, void* finalize_hint) {
                delete (boost::multiprecision::cpp_int*)finalize_data;
            },
            nullptr, nullptr);
    } else if (expType == napi_object) {
        boost::multiprecision::cpp_int* expBigInt;
        napi_unwrap(env, argv[0], (void**)&expBigInt);
        boost::multiprecision::cpp_int&& temp = boost::multiprecision::powm(*thisBigInt, *expBigInt, p);
        resultBigint = new boost::multiprecision::cpp_int(temp);
        napi_new_instance(env, jsClass, 0, nullptr, &result);
        napi_wrap(
            env, result, resultBigint,
            [](napi_env env, void* finalize_data, void* finalize_hint) {
                delete (boost::multiprecision::cpp_int*)finalize_data;
            },
            nullptr, nullptr);
    } else {
        return nullptr;
    }
    return result;
}

napi_value BignumNapi::toBuffer(napi_env env, napi_callback_info cb)
{
    boost::multiprecision::cpp_int* thisBigInt;
    napi_value thisArg, arrayBuffer;
    size_t argc = 0;
    napi_get_cb_info(env, cb, &argc, nullptr, &thisArg, nullptr);
    napi_unwrap(env, thisArg, (void**)&thisBigInt);
    uint8_t* buffer;
    size_t bufferLen;
    byte_array_from_cppint(*thisBigInt, &buffer, &bufferLen);
    napi_create_external_arraybuffer(
        env, (void*)buffer, bufferLen, [](napi_env, void*, void*) {}, nullptr, &arrayBuffer);
    return arrayBuffer;
}

napi_value BignumNapi::toNumber(napi_env env, napi_callback_info cb)
{
    boost::multiprecision::cpp_int* thisBigInt;
    napi_value thisArg, result;
    size_t argc = 0;
    napi_get_cb_info(env, cb, &argc, nullptr, &thisArg, nullptr);
    napi_unwrap(env, thisArg, (void**)&thisBigInt);
    napi_create_uint32(env, thisBigInt->convert_to<uint32_t>(), &result);
    return result;
}

napi_value BignumNapi::toString(napi_env env, napi_callback_info cb)
{
    boost::multiprecision::cpp_int* thisBigInt;
    napi_value thisArg, result;
    size_t argc = 0;
    napi_get_cb_info(env, cb, &argc, nullptr, &thisArg, nullptr);
    napi_unwrap(env, thisArg, (void**)&thisBigInt);
    std::string out = thisBigInt->str();
    napi_create_string_utf8(env, out.c_str(), out.length(), &result);
    return result;
}

napi_value BignumNapi::defineClass(napi_env env)
{
    const char* className = "Bigint";
    napi_value jsClass;
    napi_property_descriptor desc[] = { // DECLARE_NAPI_FUNCTION("constructor", BignumNapi::constructor),
        DECLARE_NAPI_FUNCTION("powm", BignumNapi::powm), DECLARE_NAPI_FUNCTION("toBuffer", BignumNapi::toBuffer),
        DECLARE_NAPI_FUNCTION("toNumber", BignumNapi::toNumber), DECLARE_NAPI_FUNCTION("toString", BignumNapi::toString)
    };
    napi_define_class(env, className, NAPI_AUTO_LENGTH, BignumNapi::constructor, nullptr,
        sizeof(desc) / sizeof(desc[0]), desc, &jsClass);
    napi_create_reference(env, jsClass, 1, &jsClassRef);
    return jsClass;
}

boost::multiprecision::cpp_int BignumNapi::cppint_from_byte_array(uint8_t* data, size_t dataLen)
{
    boost::multiprecision::cpp_int result = 0;
    for (int i = 0; i < dataLen; i++) {
        result <<= 8;
        result |= data[i];
    }
    return result;
}

void BignumNapi::byte_array_from_cppint(boost::multiprecision::cpp_int bigint, uint8_t** data, size_t* dataLen)
{
    boost::multiprecision::cpp_int temp = bigint;
    size_t maxLen = bigint.backend().size() * sizeof(boost::multiprecision::limb_type);
    uint8_t* bytes = new uint8_t[maxLen];
    memset(bytes, 0, maxLen);
    int i;
    for (i = 0; i < maxLen; i++) {
        uint8_t dataByte = (temp & 0xFF).convert_to<uint8_t>();
        if (dataByte == 0) {
            break;
        }
        bytes[i] = dataByte;
        temp >>= 8;
    }
    std::reverse(bytes, bytes + i);
    *dataLen = i;
    *data = bytes;
}