# Copyright 2024 Unionman Technology Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(coapApp)
set(CMAKE_CXX_STANDARD 17)
set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

link_directories(${NATIVERENDER_ROOT_PATH}/libs)

add_library(speke SHARED src/speke.cpp src/bignum_napi.cpp src/crypto.cpp)
target_link_libraries(speke PUBLIC libace_napi.z.so)

target_link_libraries(speke PUBLIC libcrypto_openssl.z.so)
target_link_libraries(speke PUBLIC libssl_openssl.z.so)