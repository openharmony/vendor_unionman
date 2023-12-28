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

#ifndef JQHTTPSERVER_H
#define JQHTTPSERVER_H

#ifndef QT_NETWORK_LIB
#error("Please add network in pro file")
#endif

#ifndef QT_CONCURRENT_LIB
#error("Please add concurrent in pro file")
#endif

// C++ lib import
#include <functional>

// Qt lib import
#include <QHostAddress>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QPointer>
#include <QSet>
#include <QSharedPointer>
#include <QUrl>
#include <QVector>

class QIODevice;
class QThreadPool;
class QHostAddress;
class QTimer;
class QImage;
class QTcpServer;
class QLocalServer;
class QSslCertificate;
class QSslKey;
class QSslConfiguration;

namespace JQHttpServer {

    class Session : public QObject {
        Q_OBJECT
        Q_DISABLE_COPY(Session)

    public:
        Session(const QPointer<QIODevice>& tcpSocket);

        ~Session();

        inline void setHandleAcceptedCallback(const std::function<void(const QPointer<Session>&)>& callback)
        {
            handleAcceptedCallback_ = callback;
        }

        inline QString requestMethod() const
        {
            return requestMethod_;
        }

        inline QString requestUrl() const
        {
            return requestUrl_;
        }

        inline QString requestCrlf() const
        {
            return requestCrlf_;
        }

        inline QMap<QString, QString> requestHeader() const
        {
            return requestHeader_;
        }

        inline QByteArray requestBody() const
        {
            return requestBody_;
        }

        QString requestUrlPath() const;

        QStringList requestUrlPathSplitToList() const;

        QMap<QString, QString> requestUrlQuery() const;

        inline QIODevice* GetIODevice() const
        {
            return ioDevice_.data();
        }

    public slots:
        void ReplyText(const QString& replyData, const int& httpStatusCode = 200);

        void ReplyRedirects(const QUrl& targetUrl, const int& httpStatusCode = 200);

        void ReplyJsonObject(const QJsonObject& jsonObject, const int& httpStatusCode = 200);

        void ReplyJsonArray(const QJsonArray& jsonArray, const int& httpStatusCode = 200);

        void ReplyFile(const QString& filePath, const int& httpStatusCode = 200);

        void ReplyImage(const QImage& image, const int& httpStatusCode = 200);

        void ReplyImage(const QString& imageFilePath, const int& httpStatusCode = 200);

        void ReplyBytes(const QByteArray& bytes, const int& httpStatusCode = 200);

        void ReplyOptions();

    private:
        void InspectionBufferSetup1();

        void InspectionBufferSetup2();

    private:
        QPointer<QIODevice> ioDevice_;
        std::function<void(const QPointer<Session>&)> handleAcceptedCallback_;
        QSharedPointer<QTimer> timerForClose_;

        QByteArray buffer_;

        QString requestMethod_;
        QString requestUrl_;
        QString requestCrlf_;

        QMap<QString, QString> requestHeader_;
        bool headerAcceptedFinish_ = false;
        qint64 contentLength_ = -1;
        bool alreadyReply_ = false;

        QByteArray requestBody_;

        qint64 waitWrittenByteCount_ = 0;
        QSharedPointer<QIODevice> ioDeviceForReply_;
    };

    class AbstractManage : public QObject {
        Q_OBJECT
        Q_DISABLE_COPY(AbstractManage)

    public:
        AbstractManage(const int& handleMaxThreadCount);

        virtual ~AbstractManage();

        inline void setHttpAcceptedCallback(
            const std::function<void(const QPointer<Session>& session)>& httpAcceptedCallback)
        {
            httpAcceptedCallback_ = httpAcceptedCallback;
        }

        inline QSharedPointer<QThreadPool> handleThreadPool()
        {
            return handleThreadPool_;
        }

        inline QSharedPointer<QThreadPool> serverThreadPool()
        {
            return serverThreadPool_;
        }

        virtual bool IsRunning() = 0;

    protected Q_SLOTS:
        bool Begin();

        void Close();

    protected:
        virtual bool OnStart() = 0;

        virtual void OnFinish() = 0;

        bool StartServerThread();

        void StopHandleThread();

        void StopServerThread();

        void NewSession(const QPointer<Session>& session);

        void HandleAccepted(const QPointer<Session>& session);

    signals:
        void readyToClose();

    protected:
        QSharedPointer<QThreadPool> serverThreadPool_;
        QSharedPointer<QThreadPool> handleThreadPool_;

        QMutex mutex_;

        std::function<void(const QPointer<Session>& session)> httpAcceptedCallback_;

        QSet<Session*> availableSessions_;
    };

    class TcpServerManage : public AbstractManage {
        Q_OBJECT
        Q_DISABLE_COPY(TcpServerManage)

    public:
        TcpServerManage(const int& handleMaxThreadCount = 2);

        ~TcpServerManage() override;

        bool Listen(const QHostAddress& address, const quint16& port);

        bool IsRunning() override;

    private:
        bool OnStart() override;

        void OnFinish() override;

    private:
        QPointer<QTcpServer> tcpServer_;

        QHostAddress listenAddress_ = QHostAddress::Any;
        quint16 listenPort_ = 0;
    };

#ifndef QT_NO_SSL
    class SslServerHelper;

    class SslServerManage : public AbstractManage {
        Q_OBJECT
        Q_DISABLE_COPY(SslServerManage)

    public:
        SslServerManage(const int& handleMaxThreadCount = 2);

        ~SslServerManage() override;

        bool Listen(const QHostAddress& address, const quint16& port, const QString& crtFilePath,
            const QString& keyFilePath, const QList<QPair<QString, bool>>& caFileList = {} // [ { filePath, isPem } ]
        );

        bool IsRunning() override;

    private:
        bool OnStart() override;

        void OnFinish() override;

    private:
        QPointer<SslServerHelper> tcpServer_;

        QHostAddress listenAddress_ = QHostAddress::Any;
        quint16 listenPort_ = 0;

        QSharedPointer<QSslConfiguration> sslConfiguration_;
    };
#endif

    class LocalServerManage : public AbstractManage {
        Q_OBJECT
        Q_DISABLE_COPY(LocalServerManage)

    public:
        LocalServerManage(const int& handleMaxThreadCount);

        ~LocalServerManage() override;

        bool Listen(const QString& name);

        bool IsRunning() override;

    private:
        bool OnStart() override;

        void OnFinish() override;

    private:
        QPointer<QLocalServer> localServer_;

        QString listenName_;
    };

}

#endif // JQHTTPSERVER_H_
