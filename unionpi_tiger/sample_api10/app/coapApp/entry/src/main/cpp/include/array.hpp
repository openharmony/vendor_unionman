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
#include <cstddef>
#include <cstdint>
#include <tuple>

template<typename T>
class Array {
public:
    inline Array(T* data, size_t length) noexcept : data(data), length(length) {}

    inline Array() noexcept : data(nullptr), length(0) {}

    inline Array(const Array<T>& a) noexcept : length(a.length)
    {
        data = new T[length];
        memcpy(data, a.data, length);
    }

    inline Array(Array<T>&& a) noexcept : data(a.data), length(a.length)
    {
        a.data = nullptr;
        a.length = 0;
    }

    inline Array& operator=(const Array<T>& a) noexcept
    {
        if (data != nullptr) {
            delete data;
        }

        data = new T[length];
        length = a.length;
        memcpy(data, a.data, length);
        return *this;
    }

    inline Array& operator=(Array<T>&& a) noexcept
    {
        if (data != nullptr) {
            delete data;
        }
        data = a.data;
        length = a.length;
        a.data = nullptr;
        a.length = 0;
        return *this;
    }

    inline Array(size_t length) noexcept : length(length)
    {
        data = new T[length];
    }

    inline ~Array() noexcept
    {
        if (data != nullptr) {
            delete data;
        }
    }

    inline T* getData()
    {
        return data;
    }

    inline const T* getData() const
    {
        return data;
    }

    inline size_t& getLength()
    {
        return length;
    }

    inline const size_t& getLength() const
    {
        return length;
    }

    // 导出内部缓冲区指针以及长度,同时移交所有权
    inline std::tuple<T*, size_t> exportAndMove()
    {
        std::tuple<T*, size_t> result = std::make_tuple(data, length);
        data = nullptr;
        length = 0;
        return result;
    }

protected:
    T* data;
    size_t length;
};

using ArrayBuffer = Array<uint8_t>;