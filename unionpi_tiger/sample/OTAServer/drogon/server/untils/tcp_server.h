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

#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <string>
class TcpServer {
public:
	explicit TcpServer(boost::asio::io_context& context);
    ~TcpServer();

protected:
    boost::asio::ip::tcp::endpoint endpoint;
    boost::asio::ip::tcp::acceptor accepter;
    boost::asio::io_context& io_context;
    boost::asio::ssl::context ssl_context;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket>* ssl_socket;
    void accept_handler(const boost::system::error_code& ec);
    void handshake_handler(const boost::system::error_code& ec);
    void write_handler(const boost::system::error_code& ec, std::size_t bytes_transferred);
    std::string readjsonFile(const std::string path);
};
#endif