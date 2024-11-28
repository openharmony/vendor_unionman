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
#include <chrono>
#include <coap3/coap.h>
#include <coap3/libcoap.h>
#include <iostream>
#include <nlohmann/json.hpp>

#include "crypto.h"
#include "speke.h"

Speke speke("1234");

int main()
{
    coap_context_t* context = nullptr;
    coap_address_t address;
    coap_resource_t *startRes = nullptr, *finishRes = nullptr, *operateRes;
    coap_str_const_t* startUrl = coap_make_str_const("ohos/v1/device/spekeStart");
    coap_str_const_t* finishUrl = coap_make_str_const("ohos/v1/device/spekeFinish");
    coap_str_const_t operateUrl;
    operateUrl.length = 22;
    operateUrl.s = (const uint8_t*)"ohos/v1/device/operate";
    coap_startup();
    coap_address_init(&address);
    address.addr.sin.sin_family = AF_INET;
    address.addr.sin.sin_addr.s_addr = INADDR_ANY;
    address.addr.sin.sin_port = htons(5683);
    context = coap_new_context(nullptr);
    coap_new_endpoint(context, &address, COAP_PROTO_UDP);
    startRes = coap_resource_init(startUrl, 0);
    finishRes = coap_resource_init(finishUrl, 0);
    operateRes = coap_resource_init(&operateUrl, 0);
    coap_register_handler(startRes, COAP_REQUEST_POST,
        [](coap_resource_t* url_res, coap_session_t* session, const coap_pdu_t* request, const coap_string_t* query,
            coap_pdu_t* response) {
            SpekeResponse1 resp = speke.step1();
            nlohmann::json respJson { { "challenge", resp.challenge }, { "salt", resp.salt }, { "pk", resp.pk } };
            std::string text = respJson.dump();
            uint8_t buf[2];
            coap_pdu_set_type(response, COAP_MESSAGE_CON);
            coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
            coap_add_option(response, COAP_OPTION_CONTENT_FORMAT,
                coap_encode_var_safe(buf, sizeof(buf), COAP_MEDIATYPE_APPLICATION_JSON), buf);
            coap_add_data(response, text.length(), reinterpret_cast<const uint8_t*>(text.c_str()));
        });
    coap_register_handler(finishRes, COAP_REQUEST_POST,
        [](coap_resource_t* url_res, coap_session_t* session, const coap_pdu_t* request, const coap_string_t* query,
            coap_pdu_t* response) {
            SpekeRequest2 spekeReq;
            const char* str;
            size_t strLen;
            coap_get_data(request, &strLen, reinterpret_cast<const uint8_t**>(&str));
            nlohmann::json requestJson = nlohmann::json::parse(str, str + strLen);

            spekeReq.challenge = requestJson["challenge"];
            spekeReq.pk = requestJson["pk"];
            spekeReq.kcfData = requestJson["kcfData"];

            std::string kcfData1 = speke.step2(spekeReq);
            std::cout << "dataEncKey: " << Speke::bytesToHexStr(speke.dataEncKey) << std::endl;
            nlohmann::json respJson { { "kcfData", kcfData1 } };
            std::string text = respJson.dump();
            uint8_t buf[2];
            coap_pdu_set_type(response, COAP_MESSAGE_CON);
            coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
            coap_add_option(response, COAP_OPTION_CONTENT_FORMAT,
                coap_encode_var_safe(buf, sizeof(buf), COAP_MEDIATYPE_APPLICATION_JSON), buf);
            coap_add_data(response, text.length(), reinterpret_cast<const uint8_t*>(text.c_str()));
        });
    coap_register_handler(operateRes, COAP_REQUEST_POST,
        [](coap_resource_t* url_res, coap_session_t* session, const coap_pdu_t* request, const coap_string_t* query,
            coap_pdu_t* response) {
            const char* str;
            size_t strLen;
            coap_get_data(request, &strLen, reinterpret_cast<const uint8_t**>(&str));
            ArrayBuffer ciph = Speke::hexStrToBytes(std::string(str, strLen));
            ArrayBuffer plain = aesEcbDecrypt(speke.dataEncKey, ciph);
            nlohmann::json requestJson = nlohmann::json::parse(plain.getData(), plain.getData() + strLen);
            std::cout << requestJson.dump() << std::endl;
            if (requestJson["operateCode"].get<int>() == 1) {
                system("aa start -a ohos.samples.distributedcalc.MainAbility -b "
                       "ohos.samples.distributedcalc");
            }
            std::string text = "success";
            uint8_t buf[2];
            coap_pdu_set_type(response, COAP_MESSAGE_CON);
            coap_pdu_set_code(response, COAP_RESPONSE_CODE_CONTENT);
            coap_add_option(response, COAP_OPTION_CONTENT_FORMAT,
                coap_encode_var_safe(buf, sizeof(buf), COAP_MEDIATYPE_APPLICATION_JSON), buf);
            coap_add_data(response, text.length(), reinterpret_cast<const uint8_t*>(text.c_str()));
        });
    coap_add_resource(context, startRes);
    coap_add_resource(context, finishRes);
    coap_add_resource(context, operateRes);
    while (true) {
        coap_io_process(context, 0);
    }
    coap_free_context(context);
    coap_cleanup();
}
