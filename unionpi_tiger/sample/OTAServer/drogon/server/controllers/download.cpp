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

#include <drogon/drogon.h>
#include <fstream>
#include "../untils/picosha2.h"
#include "download.h"
using namespace drogon;
void Download::getFile(
    const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, const std::string& filename)
{
    std::string path(app().getUploadPath());
    path.append("/");
    path.append(filename);
    HttpResponsePtr response = HttpResponse::newFileResponse(path, filename, CT_CUSTOM, "");
    callback(response);
}

void Download::uploadFile(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
    using namespace std;
    MultiPartParser formData;
    if (formData.parse(req) != 0 || formData.getFiles().size() != 1) {
        HttpResponsePtr response = HttpResponse::newHttpResponse();
        response->setBody("the file must only one");
        response->setStatusCode(k403Forbidden);
        callback(response);
    }
    const HttpFile& file = formData.getFiles()[0];
    file.save();
    Info info;
    std::map<std::string, std::string> maps = formData.getParameters();
    info.versionCode = maps["versionCode"];
    info.versionName = maps["versionName"];
    info.url = "http://"s + req->getLocalAddr().toIpPort() + "/api/download/"s + file.getFileName();
    info.fileSize = file.fileLength();
    info.verifyInfo = getFileSha256(app().getUploadPath() + "/"s + file.getFileName());
    info.description = maps["content"];
    addInfoJsonFile("./config/serverInfo.json", info);

    HttpResponsePtr response = HttpResponse::newHttpResponse();
    response->setBody("success!");
    response->setStatusCode(k200OK);
    callback(response);
}

Json::Value Download::readJsonFromFile(const std::string& path)
{
    Json::Value json;
    std::ifstream inputFile(path, std::ios::binary);
    LOG_ERROR_IF(!inputFile.is_open()) << "open updatainfo.json error!";
    Json::CharReaderBuilder readerBuilder;
    std::string error;
    LOG_ERROR_IF(!Json::parseFromStream(readerBuilder, inputFile, &json, &error)) << "fail parse JSON error:" << error;
    inputFile.close();
    return json;
}

void Download::saveJsonToFile(const std::string& path, const Json::Value json)
{
    Json::StreamWriterBuilder writer;
    writer["emitUTF8"] = true;
    std::string jsonString = Json::writeString(writer, json);
    std::ofstream file(path, std::ios::out);
    LOG_ERROR_IF(!file.is_open()) << "open updatainfo.json error!";
    file << jsonString;
    file.close();
}

bool Download::addInfoJsonFile(const std::string& path, const Info& info)
{
    Json::Value json = readJsonFromFile(path);
    Json::Value& checkResults = json["checkResults"];
    Json::Value& descriptInfo = json["descriptInfo"];
    Json::Value checkResultsItem, descriptInfoItem;
    checkResultsItem["versionCode"] = info.versionCode;
    checkResultsItem["packageType"] = 1;
    checkResultsItem["descriptPackageId"] = "abcdefg123456789";
    checkResultsItem["url"] = info.url;
    checkResultsItem["versionName"] = info.versionName;
    checkResultsItem["verifyInfo"] = info.verifyInfo;
    checkResultsItem["size"] = info.fileSize;
    checkResults.append(checkResultsItem);
    descriptInfoItem["descriptionType"] = 0;
    descriptInfoItem["content"] = info.description;
    descriptInfo.append(descriptInfoItem);
    saveJsonToFile(path, json);
    return true;
}

std::string Download::getFileSha256(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    LOG_ERROR_IF(!file.is_open()) << "open " << path << " error!";
    std::vector<picosha2::byte_t> hash(picosha2::k_digest_size);
    picosha2::hash256(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), hash.begin(), hash.end());
    std::string value = picosha2::bytes_to_hex_string(hash.begin(), hash.end());
    return value;
}
