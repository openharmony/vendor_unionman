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
#include<thread>
#include<untils/tcp_server.h>
static void runUpdateServer()
{
    boost::asio::io_context io_context;
    TcpServer tcpServer(io_context);
    io_context.run();
}
int main()
{
    std::thread thread(runUpdateServer);
    drogon::app().loadConfigFile("config.json").run();
    thread.detach();
    return 0;
}
