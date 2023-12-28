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

#ifndef STBITEMWIDGET_H
#define STBITEMWIDGET_H

#include <QWidget>

#include <QFile>
#include <QMutex>
#include <QNetworkReply>
#include <QTcpSocket>
#include <logtextwidget.h>

#include <QFileDialog>
#include <QInputDialog>
#include <QPlainTextEdit>
#include <QProcess>
#include <QPushButton>
#include <QThread>
#include <QVBoxLayout>

namespace Ui {
    class StbItemWidget;
}

typedef enum {
    DEV_STATUS_IDEL = 0,
    DEV_STATUS_UPGRADE_DOWNLOADING,
    DEV_STATUS_UPGRADE_CHECKING,
    DEV_STATUS_UPGRADE_UPDATING,

    DEV_STATUS_UPGRADE_ERR = 100,
    DEV_STATUS_UPGRADE_ERR_DOWNLOAD = DEV_STATUS_UPGRADE_ERR,
    DEV_STATUS_UPGRADE_ERR_CHECK,
    DEV_STATUS_UPGRADE_ERR_UPDATE,

    DEV_STATUS_OFFLINE = 1000,
} DEV_STATUS_E;

class FilePusherThread : public QThread {
    Q_OBJECT

public:
    explicit FilePusherThread(QObject* parent = nullptr)
    {
    }
    void SetFilePath(const QString& filePath)
    {
        this->m_filePath = filePath;
    }
    void SetTargetPath(const QString& targetPath)
    {
        this->m_targetPath = targetPath;
    }
    void SetIp(const QString& ip)
    {
        this->m_ip = ip;
    }
    void CancelPush();

signals:
    void PushMessage(const QString& result);
    void PushFinished(const QString& result);

protected:
    void Run() override;

private:
    QProcess pushProcess;
    QString m_filePath;
    QString m_targetPath;
    QString m_ip;
    bool pushProcessCancal = false;
};

class FilePusher : public QWidget {
    Q_OBJECT

public:
    FilePusher(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setFixedSize(400L, 200L);

        selectButton = new QPushButton("选择文件", this);
        connect(selectButton, &QPushButton::clicked, this, &FilePusher::selectFile);
        selectButton->setFixedSize(80L, 30L);

        filePathLineEdit = new QLineEdit(this);
        filePathLineEdit->setPlaceholderText("选择文件路径");
        filePathLineEdit->setFixedSize(280L, 25L);

        QHBoxLayout* fileLayout = new QHBoxLayout();
        fileLayout->addWidget(selectButton);
        fileLayout->addWidget(filePathLineEdit);

        targetPathLineEdit = new QLineEdit(this);
        targetPathLineEdit->setPlaceholderText("设备目标路径");
        targetPathLineEdit->setFixedSize(280L, 25L);

        pushButton = new QPushButton("推送", this);
        connect(pushButton, &QPushButton::clicked, this, &FilePusher::pushFile);
        connect(&m_filePusherThread, &FilePusherThread::PushMessage, this, &FilePusher::handlePushMessage);
        connect(&m_filePusherThread, &FilePusherThread::PushFinished, this, &FilePusher::handlePushFinished);

        pushButton->setFixedSize(80L, 30L);

        QHBoxLayout* pushLayout = new QHBoxLayout();
        pushLayout->addWidget(pushButton);
        pushLayout->addWidget(targetPathLineEdit);

        resultTextEdit = new QPlainTextEdit(this);
        resultTextEdit->setFrameStyle(QFrame::NoFrame);
        resultTextEdit->setReadOnly(true);

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addLayout(fileLayout);
        layout->addLayout(pushLayout);
        layout->addWidget(resultTextEdit);
    }
    void setIp(QString ip);

private slots:
    void selectFile();
    void pushFile();
    void handlePushMessage(const QString& result);
    void handlePushFinished(const QString& result);

private:
    QLineEdit* filePathLineEdit;
    QLineEdit* targetPathLineEdit;
    QPlainTextEdit* resultTextEdit;
    QPushButton* selectButton;
    QPushButton* pushButton;
    FilePusherThread m_filePusherThread;
    QString ip;
};

class StbItemWidget : public QWidget {
    Q_OBJECT

public:
    explicit StbItemWidget(QWidget* parent = nullptr);
    ~StbItemWidget();
    void setIndex(int index);
    void setMainInfo(QString mac, QString sn, QString hwv, QString swv, QString dm);
    void setIp(QString ip, bool isStatic = false);
    void setStatus(DEV_STATUS_E status);
    void setStatusCase1();
    void setStatusCase2();
    void setStatusCase3();
    void setStatusCase4();
    void setStatusCase5();
    void setStatusCase6();
    void setStatusCase7();
    void setUptime(QString uptime);
    void setTemperature(QString temperature);
    void setIpcameraPss(QString ipcameraPss);
    void writeLog(QString info);
    DEV_STATUS_E getStatus();
    qint64 updateTime;
    QTcpSocket* socket = nullptr;
    QString getIp()
    {
        return ip;
    }
    QString getTemperature()
    {
        return temperature;
    }
    QString getLogHead();
    QString getLogPath();
    QString generateLogInfo();
    void logMutexLock();
    void logMutexUnlock();

protected:
    void mouseDoubleClickEvent(QMouseEvent* event);
protected slots:
    void requestFinished(QNetworkReply* reply);
    void openMenu(const QPoint& pos);
    void playVideo();
    void openADBShell();
    void checkLog();
    void openLogDirectory();
    void burnSN();
    void reboot();
    void enterFactoryMode();
    void restoreFactorySetting();
    void pushFile();
    void openConfigWeb();

private:
    int httpRequest(QString req);
    Ui::StbItemWidget* ui;
    QWidget* parent;
    DEV_STATUS_E status = DEV_STATUS_IDEL;
    QString ip;
    QString sn;
    QString mac;
    QString hwv;
    QString swv;
    QString temperature;
    QString ipcameraPss;
    QString deviceModel;
    QString upTime;
    bool isStaticIP = false;
    QMutex logMutex;
    FilePusher filePusher;
};

#endif // STBITEMWIDGET_H
