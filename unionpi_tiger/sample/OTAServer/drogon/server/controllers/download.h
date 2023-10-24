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

#ifndef CONTROLLERS_DOWNLOAD_H
#define CONTROLLERS_DOWNLOAD_H


#include <drogon/HttpController.h>
#include <json/json.h>
#include <string>
#include "../dto/info.h"
class Download : public drogon::HttpController<Download> {
public:
    void getFile(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        const std::string& filename);
    void uploadFile(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    Json::Value readJsonFromFile(const std::string& path);
    void saveJsonToFile(const std::string& path, const Json::Value json);
    bool addInfoJsonFile(const std::string& path, const Info& info);

    std::string getFileSha256(const std::string& path);

    METHOD_LIST_BEGIN
    ADD_METHOD_TO(Download::getFile, "/api/download/{filename}", drogon::Get);
    ADD_METHOD_TO(Download::uploadFile, "/api/upload", drogon::Post);
    METHOD_LIST_END
};
#endif
