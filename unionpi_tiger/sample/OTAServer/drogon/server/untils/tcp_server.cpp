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

#include "tcp_server.h"
#include <fstream>
#include <iostream>
using namespace boost::asio;
using namespace std::placeholders;
constexpr int listen_port = 5022;
TcpServer::TcpServer(boost::asio::io_context& context)
    : io_context(context),
      endpoint(ip::tcp::v4(), listen_port),
      accepter(context, endpoint),
      ssl_context(ssl::context::tlsv12_server)
{
    ssl_context.set_options(ssl::verify_none | ssl::context::no_compression);
    ssl_context.use_certificate_file("config/serverCert.cer", ssl::context::file_format::pem);
    ssl_context.use_private_key_file("config/serverKey.pem", ssl::context::file_format::pem);
    ssl_socket = new ssl::stream<ip::tcp::socket>(context, ssl_context);
    accepter.async_accept(ssl_socket->lowest_layer(), std::bind(&TcpServer::accept_handler, this, _1));
}

TcpServer::~TcpServer()
{
    delete ssl_socket;
}

void TcpServer::accept_handler(const boost::system::error_code& ec)
{
    ssl_socket->async_handshake(ssl::stream_base::server, std::bind(&TcpServer::handshake_handler, this, _1));
}
void TcpServer::handshake_handler(const boost::system::error_code& ec)
{
    std::string jsonInfo = readjsonFile("config/serverInfo.json");
    ssl_socket->async_write_some(buffer(jsonInfo), std::bind(&TcpServer::write_handler, this, _1, _2));
}
void TcpServer::write_handler(const boost::system::error_code& ec, std::size_t bytes_transferred)
{
    ssl_socket->shutdown();
    ssl_socket->lowest_layer().close();
    delete ssl_socket;
    ssl_socket = new ssl::stream<ip::tcp::socket>(io_context, ssl_context);
    accepter.async_accept(ssl_socket->lowest_layer(), std::bind(&TcpServer::accept_handler, this, _1));
}
std::string TcpServer::readjsonFile(const std::string path)
{
    std::string jsonSource;
    std::fstream jsonFile;
    std::stringstream jsonStream;
    jsonFile.open(path);
    jsonStream << jsonFile.rdbuf();
    jsonSource = jsonStream.str();
    return jsonSource;
}
