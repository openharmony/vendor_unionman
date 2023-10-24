/*
 * Copyright (c) 2023 Unionman Technology Co., Ltd.
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

#ifndef DTO_INFO_H
#define DTO_INFO_H
#include <string>
class Info {
public:
    Info();
    ~Info();
    /// @brief 第几个版本
    std::string versionCode;
    /// @brief 版本名
    std::string versionName;
    /// @brief 下载更新包的url
    std::string url;
    /// @brief 更新包的sha256
    std::string verifyInfo;
    /// @brief 更新包的大小
    size_t fileSize;
    /// @brief 更新内容
    std::string description;
};
#endif
