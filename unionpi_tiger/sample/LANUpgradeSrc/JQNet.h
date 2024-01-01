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

#ifndef JQLIBRARY_INCLUDE_JQNET_H
#define JQLIBRARY_INCLUDE_JQNET_H

#ifndef QT_NETWORK_LIB
#error("Please add network in pro file")
#endif

// C++ lib import
#include <functional>

// Qt lib import
#include <QHttpMultiPart>
#include <QNetworkAccessManager>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>
#include <QNetworkReply>
#include <QSharedPointer>

namespace JQNet {

    QNetworkAddressEntry getNetworkAddressEntry();

    QPair<QNetworkAddressEntry, QNetworkInterface> getNetworkAddressEntryWithNetworkInterface(const bool& ridVm = true);

    QString getHostName();

#ifdef JQFOUNDATION_LIB
    bool pingIp(const QHostAddress& hostAddress);
#endif

    class HTTP {
        Q_DISABLE_COPY(HTTP)

    public:
        HTTP() = default;

        ~HTTP() = default;

    public:
        inline QNetworkAccessManager& manage()
        {
            return manage_;
        }
        bool get(const QNetworkRequest& request, QByteArray& target, const int& timeout = 30 * 1000);

        void get(const QNetworkRequest& request, const std::function<void(const QByteArray& data)>& onFinished,
            const std::function<void(const QNetworkReply::NetworkError& code, const QByteArray& data)>& onError,
            const int& timeout = 30 * 1000);

        bool deleteResource(const QNetworkRequest& request, QByteArray& target, const int& timeout = 30 * 1000);

        void deleteResource(const QNetworkRequest& request,
            const std::function<void(const QByteArray& data)>& onFinished,
            const std::function<void(const QNetworkReply::NetworkError& code, const QByteArray& data)>& onError,
            const int& timeout = 30 * 1000);

        bool post(
            const QNetworkRequest& request, const QByteArray& body, QByteArray& target, const int& timeout = 30 * 1000);

        bool post(const QNetworkRequest& request, const QSharedPointer<QHttpMultiPart>& multiPart, QByteArray& target,
            const int& timeout = 30 * 1000);

        void post(const QNetworkRequest& request, const QByteArray& body,
            const std::function<void(const QByteArray& data)>& onFinished,
            const std::function<void(const QNetworkReply::NetworkError& code, const QByteArray& data)>& onError,
            const int& timeout = 30 * 1000);

        bool put(
            const QNetworkRequest& request, const QByteArray& body, QByteArray& target, const int& timeout = 30 * 1000);

        bool put(const QNetworkRequest& request, const QSharedPointer<QHttpMultiPart>& multiPart, QByteArray& target,
            const int& timeout = 30 * 1000);

        void put(const QNetworkRequest& request, const QByteArray& body,
            const std::function<void(const QByteArray& data)>& onFinished,
            const std::function<void(const QNetworkReply::NetworkError& code, const QByteArray& data)>& onError,
            const int& timeout = 30 * 1000);

#if !(defined Q_OS_LINUX) && (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
        bool patch(
            const QNetworkRequest& request, const QByteArray& body, QByteArray& target, const int& timeout = 30 * 1000);

        void patch(const QNetworkRequest& request, const QByteArray& body,
            const std::function<void(const QByteArray& data)>& onFinished,
            const std::function<void(const QNetworkReply::NetworkError& code, const QByteArray& data)>& onError,
            const int& timeout = 30 * 1000);
#endif
        static QPair<bool, QByteArray> get(const QString& url, const int& timeout = 30 * 1000);

        static QPair<bool, QByteArray> get(const QNetworkRequest& request, const int& timeout = 30 * 1000);

        static QPair<bool, QByteArray> deleteResource(const QString& url, const int& timeout = 30 * 1000);

        static QPair<bool, QByteArray> deleteResource(const QNetworkRequest& request, const int& timeout = 30 * 1000);

        static QPair<bool, QByteArray> post(const QString& url, const QByteArray& body, const int& timeout = 30 * 1000);

        static QPair<bool, QByteArray> post(
            const QNetworkRequest& request, const QByteArray& body, const int& timeout = 30 * 1000);

        static QPair<bool, QByteArray> post(const QNetworkRequest& request,
            const QSharedPointer<QHttpMultiPart>& multiPart, const int& timeout = 30 * 1000);

        static QPair<bool, QByteArray> put(const QString& url, const QByteArray& body, const int& timeout = 30 * 1000);

        static QPair<bool, QByteArray> put(
            const QNetworkRequest& request, const QByteArray& body, const int& timeout = 30 * 1000);

        static QPair<bool, QByteArray> put(const QNetworkRequest& request,
            const QSharedPointer<QHttpMultiPart>& multiPart, const int& timeout = 30 * 1000);

#if !(defined Q_OS_LINUX) && (QT_VERSION >= QT_VERSION_CHECK(5, 9, 0))
        static QPair<bool, QByteArray> patch(
            const QString& url, const QByteArray& body, const int& timeout = 30 * 1000);

        static QPair<bool, QByteArray> patch(
            const QNetworkRequest& request, const QByteArray& body, const int& timeout = 30 * 1000);
#endif

    private:
        void handle(QNetworkReply* reply, const int& timeout,
            const std::function<void(const QByteArray& data)>& onFinished,
            const std::function<void(const QNetworkReply::NetworkError& code, const QByteArray& data)>& onError,
            const std::function<void()>& onTimeout);

    private:
        QNetworkAccessManager manage_;
    };

}

#endif // JQLIBRARY_INCLUDE_JQNET_H_
