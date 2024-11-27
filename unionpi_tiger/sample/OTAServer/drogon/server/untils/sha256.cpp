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

#include "sha256.h"
#include <fstream>
#include <iomanip>
#include <openssl/sha.h>
#include <sstream>
#include <string>


std::string sha256(const std::string& filePath, size_t fileSize)
{
    SHA256_CTX ctx;
    SHA256_Init(&ctx);

    unsigned char sha256Sum[32] = { 0 };
    std::ifstream file;
    file.open(filePath, std::ios::binary | std::ios::in);
    size_t readByteSize = fileSize;
    while (readByteSize > 0) {
        char block[64] = { 0 };
        size_t nRead = readByteSize >= 64 ? 64 : readByteSize;
        file.read(block, nRead);
        SHA256_Update(&ctx, block, nRead);
        readByteSize -= nRead;
    }
    SHA256_Final(sha256Sum, &ctx);
    return bytesToHexStr(sha256Sum, 32);
}

std::string bytesToHexStr(const unsigned char* bytes, size_t len)
{
    std::stringstream hexStrStream;
    for (int i = 0; i < len; i++) {
        hexStrStream << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(bytes[i]);
    }
    return hexStrStream.str();
}