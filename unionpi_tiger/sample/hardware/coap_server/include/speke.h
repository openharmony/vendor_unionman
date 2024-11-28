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
#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include <cstdint>
#include <string>

#include "array.hpp"

struct SpekeRequest2 {
    std::string challenge;
    std::string kcfData;
    std::string pk;
};

struct SpekeResponse1 {
    std::string challenge;
    std::string salt;
    std::string pk;
};

class Speke {
public:
    Speke(std::string pin);
    SpekeResponse1 step1();
    std::string step2(SpekeRequest2 req);

    static ArrayBuffer hexStrToBytes(std::string hexStr);
    static std::string bytesToHexStr(const ArrayBuffer& bytes);

    ArrayBuffer dataEncKey;

protected:
    std::string PIN;
    ArrayBuffer challenge1, salt, secret, sessionKey1, sessionKey2;
    boost::multiprecision::cpp_int base, sk1, pk1;

    static ArrayBuffer hkdf(const ArrayBuffer& ikm, const ArrayBuffer& salt, const std::string& info, size_t keyLen);
    static ArrayBuffer hmac(const ArrayBuffer& key, const ArrayBuffer& msg, size_t macLen);
    static boost::multiprecision::cpp_int cppint_from_byte_array(const ArrayBuffer& bytes);
    static ArrayBuffer byte_array_from_cppint(boost::multiprecision::cpp_int bigint);
};
