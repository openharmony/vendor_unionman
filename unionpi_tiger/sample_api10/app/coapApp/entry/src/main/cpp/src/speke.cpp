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
#include <bits/alltypes.h>
#include <boost/multiprecision/cpp_int.hpp>
#include <hilog/log.h>
#include <iostream>
#include <napi/native_common.h>
#include <node_api.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/kdf.h>
#include <openssl/rand.h>

#include "array.hpp"
#include "bignum_napi.h"
#include "crypto.h"

void getDataFromBinary(napi_env env, napi_value binary, uint8_t** data, size_t* len)
{
    bool result;
    size_t offset;
    napi_is_dataview(env, binary, &result);
    if (result) {
        napi_get_dataview_info(env, binary, len, (void**)data, nullptr, &offset);
        *data += offset;
    } else {
        napi_is_arraybuffer(env, binary, &result);
        if (result) {
            napi_get_arraybuffer_info(env, binary, (void**)data, len);
        }
    }
}

static void HKDF(const uint8_t* ikm, size_t ikmLen, const uint8_t* salt, size_t saltLen, const uint8_t* info,
    size_t infoLen, uint8_t* key, size_t* keyLen)
{
    EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, nullptr);
    EVP_PKEY_derive_init(pctx);
    EVP_PKEY_CTX_hkdf_mode(pctx, EVP_PKEY_HKDEF_MODE_EXTRACT_AND_EXPAND);
    EVP_PKEY_CTX_set_hkdf_md(pctx, EVP_sha256());
    EVP_PKEY_CTX_set1_hkdf_key(pctx, ikm, ikmLen);
    EVP_PKEY_CTX_set1_hkdf_salt(pctx, salt, saltLen);
    EVP_PKEY_CTX_add1_hkdf_info(pctx, info, infoLen);
    EVP_PKEY_derive(pctx, key, keyLen);
}

static napi_value randomBytes(napi_env env, napi_callback_info cb)
{
    size_t argc = 1;
    napi_value argv;
    napi_get_cb_info(env, cb, &argc, &argv, nullptr, nullptr);
    uint32_t byteLen;
    napi_get_value_uint32(env, argv, &byteLen);
    uint8_t* arrayBufferMem;

    napi_value arrayBuffer;
    napi_create_arraybuffer(env, byteLen, (void**)&arrayBufferMem, &arrayBuffer);
    RAND_bytes(arrayBufferMem, byteLen);
    return arrayBuffer;
}

static napi_value hkdf(napi_env env, napi_callback_info cb)
{
    size_t argc = 4;
    napi_value argv[4];
    uint8_t *ikm, *salt, *info, *key;
    size_t ikmLen, saltLen, infoLen, keyLen;
    uint32_t keyLenU;
    napi_get_cb_info(env, cb, &argc, argv, nullptr, nullptr);
    getDataFromBinary(env, argv[0], &ikm, &ikmLen);
    getDataFromBinary(env, argv[1], &salt, &saltLen);
    napi_get_value_string_utf8(env, argv[2], nullptr, 0, &infoLen);
    info = new uint8_t[infoLen + 1];
    napi_get_value_string_utf8(env, argv[2], (char*)info, infoLen + 1, &infoLen);
    napi_get_value_uint32(env, argv[3], &keyLenU);
    keyLen = keyLenU;
    napi_value arrayBuffer;
    napi_create_arraybuffer(env, keyLen, (void**)&key, &arrayBuffer);
    HKDF(ikm, ikmLen, salt, saltLen, info, infoLen, key, &keyLen);
    delete[] info;
    return arrayBuffer;
}

static napi_value hmac(napi_env env, napi_callback_info cb)
{
    size_t argc = 2;
    napi_value argv[2];
    uint8_t *key, *msg, *out;
    size_t keyLen, msgLen;
    unsigned int outLen = 32;
    napi_get_cb_info(env, cb, &argc, argv, nullptr, nullptr);
    getDataFromBinary(env, argv[0], &key, &keyLen);
    getDataFromBinary(env, argv[1], &msg, &msgLen);
    napi_value arrayBuffer;
    napi_create_arraybuffer(env, outLen, (void**)&out, &arrayBuffer);
    HMAC(EVP_sha256(), key, keyLen, msg, msgLen, out, &outLen);
    return arrayBuffer;
}

static napi_value stringToBytes(napi_env env, napi_callback_info cb)
{
    size_t argc = 1;
    napi_value argv, arraybuffer;
    char* str;
    size_t stringLen;
    napi_get_cb_info(env, cb, &argc, &argv, nullptr, nullptr);
    napi_get_value_string_utf8(env, argv, nullptr, 0, &stringLen);
    napi_create_arraybuffer(env, stringLen, (void**)&str, &arraybuffer);
    napi_get_value_string_utf8(env, argv, str, stringLen + 1, &stringLen);
    return arraybuffer;
}

static napi_value encrypt(napi_env env, napi_callback_info cb)
{
    size_t argc = 2;
    napi_value argv[2];
    uint8_t *pkey, *pplain;
    size_t keyLen, plainLen;

    napi_get_cb_info(env, cb, &argc, argv, nullptr, nullptr);
    getDataFromBinary(env, argv[0], &pkey, &keyLen);
    getDataFromBinary(env, argv[1], &pplain, &plainLen);
    ArrayBuffer key(pkey, keyLen), plaintext(pplain, plainLen);
    ArrayBuffer ciphertext = AesEcbEncrypt(key, plaintext);
    napi_value arraybuffer;
    auto [data, len] = ciphertext.exportAndMove();
    napi_create_external_arraybuffer(
        env, data, len, [](napi_env env, void* finalize_data, void* finalize_hint) { delete (uint8_t*)finalize_data; },
        nullptr, &arraybuffer);
    key.exportAndMove();
    plaintext.exportAndMove();
    return arraybuffer;
}

static napi_value decrypt(napi_env env, napi_callback_info cb)
{
    size_t argc = 2;
    napi_value argv[2];
    uint8_t *pkey, *pciph;
    size_t keyLen, ciphLen;

    napi_get_cb_info(env, cb, &argc, argv, nullptr, nullptr);
    getDataFromBinary(env, argv[0], &pkey, &keyLen);
    getDataFromBinary(env, argv[1], &pciph, &ciphLen);
    ArrayBuffer key(pkey, keyLen), ciphertext(pciph, ciphLen);
    ArrayBuffer plaintext = AesEcbDecrypt(key, ciphertext);
    napi_value arraybuffer;
    auto [data, len] = plaintext.exportAndMove();
    napi_create_external_arraybuffer(
        env, data, len, [](napi_env env, void* finalize_data, void* finalize_hint) { delete (uint8_t*)finalize_data; },
        nullptr, &arraybuffer);
    key.exportAndMove();
    ciphertext.exportAndMove();
    return arraybuffer;
}

static napi_value Register_speke_Apis(napi_env env, napi_value exports)
{
    RAND_poll();
    napi_property_descriptor desc[] = { DECLARE_NAPI_STATIC_PROPERTY("Bigint", BignumNapi::defineClass(env)),
        DECLARE_NAPI_STATIC_FUNCTION("randomBytes", randomBytes),
        DECLARE_NAPI_STATIC_FUNCTION("stringToBytes", stringToBytes), DECLARE_NAPI_STATIC_FUNCTION("hkdf", hkdf),
        DECLARE_NAPI_STATIC_FUNCTION("hmac", hmac), DECLARE_NAPI_STATIC_FUNCTION("encrypt", encrypt),
        DECLARE_NAPI_STATIC_FUNCTION("decrypt", decrypt) };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}

NAPI_MODULE(speke, Register_speke_Apis)