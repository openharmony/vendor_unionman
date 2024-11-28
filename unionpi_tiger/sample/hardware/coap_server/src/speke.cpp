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
#include "speke.h"

#include <iomanip>
#include <iostream>
#include <openssl/hmac.h>
#include <openssl/kdf.h>
#include <openssl/rand.h>
#include <sstream>

constexpr unsigned int p = 87683;

Speke::Speke(std::string pin) : PIN(pin), challenge1(16), salt(8), sessionKey1(16), sessionKey2(16)
{
    RAND_bytes(challenge1.getData(), challenge1.getLength());

    RAND_bytes(salt.getData(), salt.getLength());

    ArrayBuffer pinBin(PIN.length());
    memcpy(pinBin.getData(), PIN.c_str(), PIN.length());

    ArrayBuffer sk1Bin(16);
    RAND_bytes(sk1Bin.getData(), sk1Bin.getLength());

    secret = hkdf(pinBin, salt, "hichain_speke_base_info", 32);
    base = boost::multiprecision::powm(cppint_from_byte_array(secret), 2, p);
    sk1 = cppint_from_byte_array(sk1Bin);
    pk1 = boost::multiprecision::powm(base, sk1, p);
}

SpekeResponse1 Speke::step1()
{
    SpekeResponse1 resp;

    resp.challenge = bytesToHexStr(challenge1);
    resp.salt = bytesToHexStr(salt);
    resp.pk = bytesToHexStr(byte_array_from_cppint(pk1));

    return resp;
}

std::string Speke::step2(SpekeRequest2 req)
{
    ArrayBuffer challenge2 = hexStrToBytes(req.challenge);
    boost::multiprecision::cpp_int pk2 = cppint_from_byte_array(hexStrToBytes(req.pk));
    std::string kcfData2 = req.kcfData;

    boost::multiprecision::cpp_int sharedSecret = boost::multiprecision::powm(pk2, sk1, p);
    ArrayBuffer sessionKey = hkdf(byte_array_from_cppint(sharedSecret), salt, "hichain_speke_sessionkey_info", 32);
    memcpy(sessionKey1.getData(), sessionKey.getData(), 16);
    memcpy(sessionKey2.getData(), sessionKey.getData() + 16, 16);

    ArrayBuffer msg2(32);
    memcpy(msg2.getData(), challenge2.getData(), 16);
    memcpy(msg2.getData() + 16, challenge1.getData(), 16);
    std::string my_kcfData2 = bytesToHexStr(hmac(sessionKey2, msg2, 32));
    if (my_kcfData2 == kcfData2) {
        std::cout << "speke auth" << std::endl;
        this->dataEncKey = hkdf(sessionKey1, salt, "hichain_return_key", 16);
        ArrayBuffer msg1(32);
        memcpy(msg1.getData(), challenge1.getData(), 16);
        memcpy(msg1.getData() + 16, challenge2.getData(), 16);
        ArrayBuffer kcfData1 = hmac(sessionKey2, msg1, 32);
        return bytesToHexStr(kcfData1);
    } else {
        std::cout << "speke not auth" << std::endl;
        return "";
    }
}

ArrayBuffer Speke::hexStrToBytes(std::string hexStr)
{
    size_t len = hexStr.length() / 2;
    ArrayBuffer bytes(len);
    for (int i = 0; i < len; i++) {
        std::string byteStr = hexStr.substr(i * 2, 2);
        bytes.getData()[i] = static_cast<uint8_t>(std::stoi(byteStr, 0, 16));
    }
    return bytes;
}

std::string Speke::bytesToHexStr(const ArrayBuffer& bytes)
{
    std::stringstream hexStrStream;
    for (int i = 0; i < bytes.getLength(); i++) {
        hexStrStream << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(bytes.getData()[i]);
    }
    return hexStrStream.str();
}

ArrayBuffer Speke::hkdf(const ArrayBuffer& ikm, const ArrayBuffer& salt, const std::string& info, size_t keyLen)
{
    ArrayBuffer key(keyLen);
    EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, nullptr);
    EVP_PKEY_derive_init(pctx);
    EVP_PKEY_CTX_hkdf_mode(pctx, EVP_PKEY_HKDEF_MODE_EXTRACT_AND_EXPAND);
    EVP_PKEY_CTX_set_hkdf_md(pctx, EVP_sha256());
    EVP_PKEY_CTX_set1_hkdf_key(pctx, ikm.getData(), ikm.getLength());
    EVP_PKEY_CTX_set1_hkdf_salt(pctx, salt.getData(), salt.getLength());
    EVP_PKEY_CTX_add1_hkdf_info(pctx, reinterpret_cast<const uint8_t*>(info.c_str()), info.size());
    EVP_PKEY_derive(pctx, key.getData(), &key.getLength());
    return key;
}

ArrayBuffer Speke::hmac(const ArrayBuffer& key, const ArrayBuffer& msg, size_t macLen)
{
    ArrayBuffer mac(macLen);
    HMAC(EVP_sha256(), key.getData(), key.getLength(), msg.getData(), msg.getLength(), mac.getData(),
        reinterpret_cast<unsigned int*>(&mac.getLength()));
    return mac;
}

boost::multiprecision::cpp_int Speke::cppint_from_byte_array(const ArrayBuffer& bytes)
{
    boost::multiprecision::cpp_int result = 0;
    for (int i = 0; i < bytes.getLength(); i++) {
        result <<= 8;
        result |= bytes.getData()[i];
    }
    return result;
}

ArrayBuffer Speke::byte_array_from_cppint(boost::multiprecision::cpp_int bigint)
{
    boost::multiprecision::cpp_int temp = bigint;
    size_t maxLen = bigint.backend().size() * sizeof(boost::multiprecision::limb_type);
    ArrayBuffer bytes(maxLen);
    memset(bytes.getData(), 0, maxLen);
    int i;
    for (i = 0; i < maxLen; i++) {
        uint8_t dataByte = (temp & 0xFF).convert_to<uint8_t>();
        if (dataByte == 0) {
            break;
        }
        bytes.getData()[i] = dataByte;
        temp >>= 8;
    }
    std::reverse(bytes.getData(), bytes.getData() + i);
    bytes.getLength() = i;
    return bytes;
}
