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
#include "crypto.h"

#include <cmath>
#include <cstdint>
#include <cstring>
#include <openssl/evp.h>

ArrayBuffer AesEcbEncrypt(const ArrayBuffer& key, const ArrayBuffer& plaintext)
{
    int lm;
    uint8_t iv[16] = { 0 };
    int len;
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx);
    size_t blockLen = plaintext.getLength() / 16 + 1;
    ArrayBuffer ciphertext(blockLen * 16);
    lm = EVP_EncryptInit_ex(ctx, EVP_Aes_128_ecb(), nullptr, key.getData(), iv);
    lm = EVP_EncryptUpdate(ctx, ciphertext.getData(), &len, plaintext.getData(), plaintext.getLength());
    lm = EVP_EncryptFinal_ex(ctx, ciphertext.getData() + len, &len);
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext;
}

ArrayBuffer AesEcbDecrypt(const ArrayBuffer& key, const ArrayBuffer& ciphertext)
{
    uint8_t iv[16] = { 0 };
    int len;
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx);
    size_t blockLen = ciphertext.getLength() / 16;
    ArrayBuffer plaintext(blockLen * 16);
    memset(plaintext.getData(), 0, plaintext.getLength());
    EVP_DecryptInit_ex(ctx, EVP_Aes_128_ecb(), nullptr, key.getData(), iv);
    EVP_DecryptUpdate(ctx, plaintext.getData(), &len, ciphertext.getData(), ciphertext.getLength());

    EVP_DecryptFinal_ex(ctx, plaintext.getData() + len, &len);
    EVP_CIPHER_CTX_free(ctx);
    return plaintext;
}
