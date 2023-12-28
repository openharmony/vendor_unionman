/*
 * Copyright (c) 2022 Unionman Technology Co., Ltd.
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "JQHttpServer.h"
#include "StbItemWidget.h"
#include <QCloseEvent>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFile>
#include <QFormLayout>
#include <QLineEdit>
#include <QList>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QTimerEvent>
#include <QUdpSocket>

namespace Ui {
    class MainWindow;
}

class MyEvent : public QEvent {
public:
    explicit MyEvent(int m)
        :QEvent(MY_EVENT_TYPE), msgType(m)
    {
    }
    MyEvent(int m, QMap<QString, QString> a)
        :QEvent(MY_EVENT_TYPE), msgType(m), args(a)
    {
    }

public:
    int msgType;
    QMap<QString, QString> args;

public:
    static Type MY_EVENT_TYPE;
};

class UpgradePackageInfo {
public:
    QString filename;
    QString md5;
    QString swVersion;
    QString hwVersion;
    QString deviceModel;
};

class DiscoverThread : public QThread {
    Q_OBJECT
public:
    DiscoverThread(QObject* parent)
        : QThread(parent)
    {
    }
    bool startUpgrade(QString url);
    bool stopUpgrade();
    bool exitThread();
    void setDirectConnectSetupFlag(bool directConnectSetupFlag);
    void setDirectConnectSetup(QString ip, QString mask);
signals:
    void threadResult();

protected:
    void run();
    void handleDatagram(const QByteArray &datagram, const QHostAddress &clientAddress, quint16 clientPort);
private slots:

private:
    bool mExitFlag = false;
    bool mUpgradeFlag = false;
    bool mDirectConnectSetupFlag = false;
    QString mDirectConnectSetIp;
    QString mDirectConnectSetSubnetMask;
    QString mUpgradeUrl = "";
};

class MD5CalculationThread : public QThread {
    Q_OBJECT

public:
    explicit MD5CalculationThread(const QString& filePath, QObject* parent = nullptr)
        :QThread(parent), mFilePath(filePath)
    {
    }

signals:
    void md5CalculationFinished(const QString& md5);

protected:
    void run() override
    {
        QFile file(mFilePath);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open package file";
            emit md5CalculationFinished("");
            return;
        }

        QCryptographicHash hash(QCryptographicHash::Md5);

        QByteArray data;
        while (!file.atEnd()) {
            data = file.read(8192L);
            hash.addData(data);
        }

        file.close();
        QByteArray md5 = hash.result().toHex();
        emit md5CalculationFinished(QString(md5));
    }

private:
    QString mFilePath;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    QString mPackageFilePath;
    QString mPackageFileMd5;
    QUdpSocket* mUdpSocket = nullptr;
    QString hostIpAddress;
    QString subnetMask;

protected:
    void closeEvent(QCloseEvent* e);
    void customEvent(QEvent* event);
    void resizeEvent(QResizeEvent* event);
    void openDirectConnectionSetupPage();
    void showErrorMessage(const QString &title, const QString &message);
    void openNetworkAdapterSettings();
    void handleMD5CalculationFinished(const QString& md5);
    QString getSubnetMask(const QHostAddress& ipAddress);

private slots:
    void disableUIElements();
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_selPackageButton_clicked();
    void calculateAndDisplayFileSize(const QString& filePath);

    void on_localIpComboBox_currentIndexChanged(const QString& arg1);

    void on_refrehIpButton_clicked();
    void newTcpConnection();
    void tcpReadyRead();
    void tcpDisconnection();

    void on_toolMenuButton_clicked();

    void on_md5Enable_stateChanged(int arg1);

private:
    void timerEvent(QTimerEvent* e);
    QStringList getLocalIPs();
    bool startHttpServer();
    void stopHttpServer();
    bool startMonitor();
    void stopMonitor();
    bool jsonToMap(QString jsonStr, QMap<QString, QString>& map);
    QString readFileFromZip(QString zippath, QString filepath);
    bool hasFileInZip(QString zippath, QString filepath);
    StbItemWidget* takeStbItemWidget(QString mac);
    StbItemWidget* getStbItemWidgetByIp(QString ip);
    void updateLocalIpComboBox();
    QMap<QString, QString> parsePropertyText(QString text);

    Ui::MainWindow* ui;
    QMap<QString, StbItemWidget*> stbItemMap;
    int timerId = -1;
    UpgradePackageInfo packageInfo;
    QString mUpgradeRequestUrlPath;
    DiscoverThread* mDiscoverThread = nullptr;
    JQHttpServer::TcpServerManage* mHttpServer = nullptr;
    bool mUpgradeStartFlag = false;
    QTcpServer* mTcpServer = nullptr;
    bool md5Enable = true;
};

class DirectConnectionSetupDialog : public QDialog {
public:
    explicit DirectConnectionSetupDialog(QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle("直连配网界面");

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        QFormLayout* formLayout = new QFormLayout;
        mainLayout->addLayout(formLayout);

        ipAddressLineEdit = new QLineEdit(this);
        subnetMaskLineEdit = new QLineEdit(this);
        ipAddressLineEdit->setMinimumSize(150L, 25L);
        subnetMaskLineEdit->setMinimumSize(150L, 25L);
        subnetMaskLineEdit->setEnabled(false);

        formLayout->addRow("IP地址:", ipAddressLineEdit);
        formLayout->addRow("子网掩码:", subnetMaskLineEdit);

        QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        mainLayout->addWidget(buttonBox);
        mainLayout->setSpacing(10L);

        connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

        setFixedSize(260L, 120L);
    }

    QString getIpAddress() const
    {
        return ipAddressLineEdit->text();
    }
    QString getSubnetMask() const
    {
        return subnetMaskLineEdit->text();
    }
    void setIpAddressLineEdit(QString ipAddress)
    {
        ipAddressLineEdit->setText(ipAddress);
    }
    void setSubnetMaskLineEdit(QString subnetMask)
    {
        subnetMaskLineEdit->setText(subnetMask);
    }
    bool getAccepted() const
    {
        return this->accepted;
    }

private:
    QLineEdit* ipAddressLineEdit;
    QLineEdit* subnetMaskLineEdit;

    bool accepted = false;
    void accept() override
    {
        accepted = true;
        QDialog::accept();
    }
    void reject() override
    {
        accepted = false;
        QDialog::reject();
    }
};

#endif // MAINWINDOW_H
