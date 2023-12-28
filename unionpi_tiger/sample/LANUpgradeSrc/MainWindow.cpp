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

#include "MainWindow.h"
#include "ProductConfig.h"
#include "StbItemWidget.h"
#include "ui_mainwindow.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QHostAddress>
#include <QHostInfo>
#include <QInputDialog>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMenu>
#include <QMessageLogger>
#include <QNetworkInterface>
#include <QProcess>
#include <QStringList>

QEvent::Type MyEvent::MY_EVENT_TYPE = static_cast<QEvent::Type>(QEvent::registerEventType());

#define MSG_TYPE_HTTP_SESSION_NEW (1)
#define MSG_TYPE_HTTP_SESSION_DELETE (2)
#define MSG_TYPE_DEVICE_FOUND (3)
#define MSG_TYPE_DISCOVER_ERROR (4)

MainWindow::MainWindow(QWidget* parent)
    :QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);

    updateLocalIpComboBox();

    timerId = startTimer(1000L);
}

MainWindow::~MainWindow()
{
    if (timerId != -1) {
        killTimer(timerId);
        timerId = -1;
    }

    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    qDebug() << "resizeEvent";
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    qDebug() << "Stop server when exit app...";
    stopHttpServer();
}

void MainWindow::customEvent(QEvent* event)
{
    if (event->type() == MyEvent::MY_EVENT_TYPE) {
        MyEvent* e = static_cast<MyEvent*>(event);

        switch (e->msgType) {
            case MSG_TYPE_HTTP_SESSION_NEW: {
                qDebug() << "download begin. client ip:" << e->args["ip"];

                StbItemWidget* item = getStbItemWidgetByIp(e->args["ip"]);
                if (item != nullptr) {
                    item->setStatus(DEV_STATUS_UPGRADE_DOWNLOADING);
                }
                break;
            }
            case MSG_TYPE_HTTP_SESSION_DELETE: {
                qDebug() << "download end. client ip:" << e->args["ip"];

                StbItemWidget* item = getStbItemWidgetByIp(e->args["ip"]);
                if (item != nullptr) {
                    item->setStatus(DEV_STATUS_UPGRADE_CHECKING);
                }
                break;
            }
            case MSG_TYPE_DEVICE_FOUND: {
                qDebug() << "device found: ip=" << e->args["ip"] << ", sn:" << e->args["sn"];
                StbItemWidget* item = takeStbItemWidget(e->args["mac"]);
                if (item != nullptr) {
                    item->setIp(e->args["ip"], e->args["static_ip"] == "1");
                    item->setMainInfo(e->args["mac"], e->args["sn"], e->args["hw_version"], e->args["sw_version"],
                        e->args["device_model"]);
                    item->setTemperature(e->args["temperature"]);
                    item->setIpcameraPss(e->args["ipcamera_pss"]);
                    item->setUptime(e->args["uptime"]);
                    item->setStatus((DEV_STATUS_E)e->args["upgrade_status"].toInt());

                    item->updateTime = QDateTime::currentMSecsSinceEpoch();
                }
                break;
            }
            case MSG_TYPE_DISCOVER_ERROR: {
                if (!(QMessageBox::information(this, tr("错误"), tr("启动 DISCOVER 服务失败"), tr("退出应用")))) {
                    QApplication* app;
                    app->exit(0);
                }
            }
            default:
                break;
        }
    }
}

void MainWindow::handleMD5CalculationFinished(const QString& md5)
{
    if (!md5.isEmpty()) {
        qDebug() << "MD5: " + md5;
        packageInfo.md5 = md5;
        ui->lineEdit_upgradefileMD5->setText(md5);
        ui->startButton->setEnabled(true);
        ui->md5Enable->setEnabled(true);
        ui->selPackageButton->setEnabled(true);
        ui->hwVersionEdit->setEnabled(true);
        ui->swVersionEdit->setEnabled(true);
        ui->deviceModelEdit->setEnabled(true);
    } else {
        qDebug() << "Failed to calculate MD5";
        QMessageBox errorBox;

        errorBox.setWindowTitle("错误");
        errorBox.setText("升级包MD5值计算失败！ ");

        QFont font;
        font.setPointSize(10L);
        font.setWeight(QFont::Normal);
        errorBox.setFont(font);

        errorBox.setIcon(QMessageBox::Critical);
        errorBox.exec();
    }
    ui->startButton->setEnabled(true);
}

void MainWindow::calculateAndDisplayFileSize(const QString& filePath)
{
    QFile file(filePath);
    qint64 size = file.size();
    QString sizeStr;
    qint64 sizeK = size / 1024;
    qint64 sizeM = sizeK / 1024;
    if (sizeM > 0) {
        sizeStr = QString::asprintf("%lldM (%lld)", sizeM, size);
    } else if (sizeK > 0) {
        sizeStr = QString::asprintf("%lldK (%lld)", sizeK, size);
    } else {
        sizeStr = QString::asprintf("%lldB", size);
    }

    MD5CalculationThread* md5Calculationthread = new MD5CalculationThread(filePath, this);
    connect(md5Calculationthread, &MD5CalculationThread::md5CalculationFinished, this,
        &MainWindow::handleMD5CalculationFinished);
    md5Calculationthread->start();

    ui->lineEdit_upgradefileMD5->setText("MD5值计算中...");
    ui->startButton->setEnabled(false);
    ui->md5Enable->setEnabled(false);
    ui->selPackageButton->setEnabled(false);
    ui->hwVersionEdit->setEnabled(false);
    ui->swVersionEdit->setEnabled(false);
    ui->deviceModelEdit->setEnabled(false);

    ui->lineEdit_upgradefileSize->setText(sizeStr);

    ui->packagePathEdit->setStyleSheet("background-color: rgb(240, 240, 240);color: rgb(0, 0, 255);");

    if (ui->hwVersionEdit->text().isEmpty() || ui->swVersionEdit->text().isEmpty()) {
        ui->hwVersionEdit->setText("");
        ui->swVersionEdit->setText("");
    }
}

void MainWindow::on_selPackageButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(nullptr, "选择升级包", "", "*.pkg");
    if (filePath != nullptr && !filePath.isEmpty()) {
        ui->packagePathEdit->setText(filePath);
        calculateAndDisplayFileSize(filePath);

        QFileInfo fileInfo(filePath);
        QString fileName = fileInfo.fileName();
        QString hwVersion = fileName.section('_', 2, 2);
        QString swVersion = fileName.section('_', 3, 3);
        QString deviceModel = fileName.section('_', 4, 4);
        if (fileName.contains("UPDATE", Qt::CaseSensitive) && (hwVersion != nullptr && !hwVersion.isEmpty())
            && (swVersion != nullptr && !swVersion.isEmpty()) && (deviceModel != nullptr && !deviceModel.isEmpty())) {
            ui->hwVersionEdit->setText(hwVersion);
            ui->swVersionEdit->setText(swVersion);
            ui->deviceModelEdit->setText(deviceModel);
        } else {
            QMessageBox msgBox;
            msgBox.setWindowTitle("警告");
            msgBox.setText("所选文件可能并非升级包！");

            QFont font;
            font.setPointSize(10L);
            font.setWeight(QFont::Normal);
            msgBox.setFont(font);

            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();

            ui->hwVersionEdit->setText("");
            ui->swVersionEdit->setText("");
            ui->deviceModelEdit->setText("");
        }
    }
}

void MainWindow::disableUIElements()
{
    // Disable relevant UI elements
    ui->startButton->setEnabled(false);
    ui->md5Enable->setEnabled(false);
    ui->stopButton->setEnabled(true);

    ui->selPackageButton->setEnabled(false);
    ui->localIpComboBox->setEnabled(false);
    ui->refrehIpButton->setEnabled(false);
    ui->hwVersionEdit->setEnabled(false);
    ui->swVersionEdit->setEnabled(false);
    ui->deviceModelEdit->setEnabled(false);

    mUpgradeStartFlag = true;
}

void MainWindow::on_startButton_clicked()
{
    MainWindow* mw = static_cast<MainWindow*>(parent());
    QString url;
    if (hostIpAddress.isEmpty()) {
        QMessageBox::information(
            this, "提示", "本机IP地址不能为空，请检查电脑网络连接，并点击刷新", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    QString packageFile = ui->packagePathEdit->text();
    QString swVersion = ui->swVersionEdit->text();
    QString hwVersion = ui->hwVersionEdit->text();
    QString deviceModel = ui->deviceModelEdit->text();
    if (packageFile == nullptr || packageFile.isEmpty() || !QFile::exists(packageFile)) {
        QMessageBox::information(this, "提示", "请选择升级包", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    if (swVersion == nullptr || swVersion.isEmpty()) {
        QMessageBox::information(this, "提示", "请输入正确的软件版本号", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    if (hwVersion == nullptr || hwVersion.isEmpty()) {
        QMessageBox::information(this, "提示", "请输入正确的硬件版本号", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    packageInfo.filename = packageFile;
    packageInfo.swVersion = swVersion;
    packageInfo.hwVersion = hwVersion;
    packageInfo.deviceModel = deviceModel;
    mPackageFilePath = packageFile;

    if (!startHttpServer()) {
        QMessageBox::information(this, "提示", "启动HTTP服务器失败，请尝试重启应用", QMessageBox::Ok, QMessageBox::Ok);
        return;
    }

    mUpgradeRequestUrlPath = QString::asprintf("/package-%d.pkg", QDateTime::currentDateTime().toTime_t());

    QString address = (QHostAddress(hostIpAddress.toLatin1().data()).protocol() == QAbstractSocket::IPv6Protocol)
        ? QString("[%1]").arg(hostIpAddress)
        : hostIpAddress;
    url = QString::asprintf("http://%s:%d%s?sw_version=%s&hw_version=%s", address.toLatin1().data(), HTTP_SERVER_PORT,
        mUpgradeRequestUrlPath.toLatin1().data(), swVersion.toLatin1().data(), hwVersion.toLatin1().data());

    if (!deviceModel.isEmpty()) {
        url.append("&device_model=");
        url.append(deviceModel.toLatin1().data());
    }

    if (this->md5Enable) {
        url.append("&md5=");
        url.append(this->packageInfo.md5.toLatin1().data());
    }

    mDiscoverThread->startUpgrade(url);
    disableUIElements();
}

void MainWindow::on_stopButton_clicked()
{
    int result = QMessageBox::warning(this, "警告", "确认停止升级？", QMessageBox::Ok, QMessageBox::Cancel);
    if (result == QMessageBox::Cancel) {
        return;
    }

    ui->listWidget->clear();
    stbItemMap.clear();

    mDiscoverThread->stopUpgrade();

    qDebug() << "Stop server...";
    stopHttpServer();

    ui->startButton->setEnabled(true);
    ui->md5Enable->setEnabled(true);
    ui->stopButton->setEnabled(false);

    ui->selPackageButton->setEnabled(true);
    ui->localIpComboBox->setEnabled(true);
    ui->refrehIpButton->setEnabled(true);
    ui->hwVersionEdit->setEnabled(true);
    ui->swVersionEdit->setEnabled(true);
    ui->deviceModelEdit->setEnabled(true);

    mUpgradeStartFlag = false;
}

void MainWindow::on_localIpComboBox_currentIndexChanged(const QString& arg1)
{
    qDebug() << "sel host ip:" << arg1;
    hostIpAddress = arg1;
    QHostAddress ipAddress(hostIpAddress);
    subnetMask = getSubnetMask(ipAddress);
    stopMonitor();
    startMonitor();
}

void MainWindow::on_refrehIpButton_clicked()
{
    updateLocalIpComboBox();
}

bool MainWindow::jsonToMap(QString jsonStr, QMap<QString, QString>& map)
{
    qDebug() << "jsonToMap:" << jsonStr;
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8(), &jsonError);
    if (jsonDoc.isNull() || jsonError.error != QJsonParseError::NoError) {
        qDebug() << "parse json from client falided.";
        return false;
    }
    QJsonObject obj = jsonDoc.object();
    foreach (QString key, obj.keys()) {
        map[key] = obj[key].toString();
    }
    return true;
}

bool DiscoverThread::startUpgrade(QString url)
{
    mUpgradeUrl = url;
    mUpgradeFlag = true;
    return true;
}

bool DiscoverThread::stopUpgrade()
{
    mUpgradeFlag = false;
    return true;
}

bool DiscoverThread::exitThread()
{
    mExitFlag = true;
    QThread::wait();
    return true;
}

void DiscoverThread::setDirectConnectSetup(QString ip, QString mask)
{
    this->mDirectConnectSetIp = ip;
    this->mDirectConnectSetSubnetMask = mask;
}

void DiscoverThread::setDirectConnectSetupFlag(bool directConnectSetupFlag)
{
    this->mDirectConnectSetupFlag = directConnectSetupFlag;
}

void DiscoverThread::handleDatagram(const QByteArray &datagram, const QHostAddress &clientAddress, quint16 clientPort)
{
    QMap<QString, QString> map;
    QString jsonStr(datagram);
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8(), &jsonError);
    if (jsonDoc.isNull() || jsonError.error != QJsonParseError::NoError) {
        qDebug() << "parse json from client failed.";
    } else {
        QJsonObject obj = jsonDoc.object();
        for (const QString &key : obj.keys()) {
            map[key] = obj[key].toString();
        }
        MainWindow* mw = static_cast<MainWindow*>(parent());
        QApplication::postEvent(mw, new MyEvent(MSG_TYPE_DEVICE_FOUND, map));
    }
}

void DiscoverThread::run()
{
    MainWindow* mw = static_cast<MainWindow*>(parent());
    QHostAddress hostAddress(mw->hostIpAddress);
    qint64 lastMulticastTime = 0;
    QUdpSocket sock;

    qDebug() << "Enter Discover thread. host ip:" << mw->hostIpAddress;

    sock.open(QUdpSocket::ReadWrite);

    if (!sock.bind(hostAddress, MULTICAST_LOCAL_PORT, QUdpSocket::ShareAddress)) {
        qDebug() << "bind fail! ip:" << mw->hostIpAddress << " port:" << MULTICAST_LOCAL_PORT;
        qDebug() << "error " << sock.errorString();
        sock.close();
        QMap<QString, QString> map;
        QApplication::postEvent(mw, new MyEvent(MSG_TYPE_DISCOVER_ERROR, map));
        return;
    }

    while (!mExitFlag) {
        qint64 curTime = QDateTime::currentMSecsSinceEpoch();

        if (sock.hasPendingDatagrams()) {
            char buf[1024];
            QHostAddress clientAddress;
            quint16 clientPort;
            memset_s(buf, sizeof(buf), 0, sizeof(buf));
            if (sock.readDatagram(buf, sizeof(buf) - 1, &clientAddress, &clientPort) > 0) {
                handleDatagram(buf, clientAddress, clientPort);
            }
        } else if (curTime > lastMulticastTime + 1000L) {
            QString msgDiscover = "DISCOVER IPCAMERA";
            QHostAddress multiAddr;
            if (QHostAddress(mw->hostIpAddress).protocol() == QAbstractSocket::IPv6Protocol) {
                multiAddr.setAddress(MULTICAST_IP_ADDRESS_V6);
            } else {
                multiAddr.setAddress(MULTICAST_IP_ADDRESS_V4);
            }

            if (mUpgradeFlag && !mUpgradeUrl.isEmpty()) {
                QString msg = "UPGRADE IPCAMERA " + mUpgradeUrl;
                sock.writeDatagram(msg.toLatin1().data(), msg.length(), multiAddr, MULTICAST_PORT);
            }

            if (this->mDirectConnectSetupFlag && !this->mDirectConnectSetIp.isEmpty()) {
                QString msgSetIp = "DIRECT CONNECTION SET IP " + this->mDirectConnectSetIp + " MASK "
                    + this->mDirectConnectSetSubnetMask;
                sock.writeDatagram(msgSetIp.toLatin1().data(), msgSetIp.length(), multiAddr, MULTICAST_PORT);
                this->mDirectConnectSetupFlag = false;
            }

            sock.writeDatagram(msgDiscover.toLatin1().data(), msgDiscover.length(), multiAddr, MULTICAST_PORT);

            lastMulticastTime = curTime;
        }
    }

    qDebug() << "Exit Discover thread";
}

void MainWindow::timerEvent(QTimerEvent* e)
{
    foreach (StbItemWidget* item, stbItemMap) {
        if (item == nullptr) {
            continue;
        }
        if (QDateTime::currentMSecsSinceEpoch() - item->updateTime >= OFFLINE_TIMEOUT) {
            if (item->getStatus() != DEV_STATUS_UPGRADE_UPDATING
                && item->getStatus() != DEV_STATUS_UPGRADE_DOWNLOADING /*兼容老的下位机程序*/) {
                item->setStatus(DEV_STATUS_OFFLINE);
            }
        } else {
            if (item->getStatus() == DEV_STATUS_OFFLINE) {
                item->setStatus(DEV_STATUS_IDEL);
            }
        }
    }
}

QStringList MainWindow::getLocalIPs()
{
    QStringList ipList;

    QString localHostName = QHostInfo::localHostName();
    qDebug() << "LocalHostName:" << localHostName;
    // 获取IP地址
    QHostInfo info = QHostInfo::fromName(localHostName);
    qDebug() << "IP Address:" << info.addresses();

    foreach (QHostAddress address, info.addresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol)
            ipList.push_front(address.toString().replace("::ffff:", ""));
        else if (address.protocol() == QAbstractSocket::IPv6Protocol && !address.toString().startsWith("fe80::"))
            ipList.push_back(address.toString());
    }

    return ipList;
}

QString MainWindow::getSubnetMask(const QHostAddress& ipAddress)
{
    QString subnetMask;

    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    foreach (const QNetworkInterface& interface, interfaces) {
        QList<QNetworkAddressEntry> addressEntries = interface.addressEntries();
        foreach (const QNetworkAddressEntry& entry, addressEntries) {
            if (entry.ip() == ipAddress) {
                subnetMask = entry.netmask().toString();
                break;
            }
        }

        if (!subnetMask.isEmpty())
            break;
    }

    return subnetMask;
}

bool MainWindow::startHttpServer()
{
    if (mHttpServer != nullptr) {
        return true;
    }

    quint16 httpServerPort = HTTP_SERVER_PORT;

    mHttpServer = new JQHttpServer::TcpServerManage(128L);

    mHttpServer->setHttpAcceptedCallback([this](const QPointer<JQHttpServer::Session>& session) {
        QTcpSocket* sock = static_cast<QTcpSocket*>(session->getIODevice());
        QString clientAddress = sock->peerAddress().toString();
        if (clientAddress.startsWith("::ffff:")) {
            clientAddress.remove(0, 7L);
        }
        qDebug() << "new http request form" << clientAddress << ", " << session->requestUrl();

        if (session->requestUrlPath() != mUpgradeRequestUrlPath) {
            qDebug() << "path invalid:" << session->requestUrlPath();
            return;
        }

        QMap<QString, QString> query = session->requestUrlQuery();

        query.insert("ip", clientAddress);
        connect(session.data(), &QObject::destroyed, [this, session, query]() {
            disconnect(session.data());
            QApplication::postEvent(this, new MyEvent(MSG_TYPE_HTTP_SESSION_DELETE, query));
        });

        QApplication::postEvent(this, new MyEvent(MSG_TYPE_HTTP_SESSION_NEW, query));

        qDebug() << "Ready to serve file: " + mPackageFilePath;
        session->replyFile(this->mPackageFilePath);
    });

    if (!mHttpServer->listen(QHostAddress(hostIpAddress), httpServerPort)) {
        qDebug() << "http listen failed. port:" << httpServerPort;
        delete mHttpServer;
        mHttpServer = nullptr;
        return false;
    }

    qDebug() << "Start http server on PORT:" << httpServerPort;

    return true;
}

void MainWindow::stopHttpServer()
{
    if (mHttpServer != nullptr) {
        delete mHttpServer;
        mHttpServer = nullptr;
        qDebug() << "Stop http server";
    }
}

void MainWindow::tcpReadyRead()
{
    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());
    QByteArray array = socket->readAll();
    QString jsonStr(array);
    qDebug() << "TCP  server receive msg:" << jsonStr;

    QMap<QString, QString> map;
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonStr.toUtf8(), &jsonError);
    if (jsonDoc.isNull() || jsonError.error != QJsonParseError::NoError) {
        qDebug() << "parse json from client falided.";
    } else {
        QJsonObject obj = jsonDoc.object();
        foreach (QString key, obj.keys()) {
            map[key] = obj[key].toString();
        }
        QApplication::postEvent(this, new MyEvent(MSG_TYPE_DEVICE_FOUND, map));
    }
}

void MainWindow::tcpDisconnection()
{
    QTcpSocket* socket = (QTcpSocket*)this->sender();

    qDebug() << "disconnection ip:" << socket->peerAddress().toString() << " port:" << socket->peerPort();
    socket->close();
}

void MainWindow::newTcpConnection()
{
    QTcpSocket* socket = mTcpServer->nextPendingConnection();
    qDebug() << "new TCP connection from ip:" << socket->peerAddress().toString() << " port:" << socket->peerPort();

    connect(socket, SIGNAL(readyRead()), this, SLOT(tcpReadyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(tcpDisconnection()));
}

bool MainWindow::startMonitor()
{
    mDiscoverThread = new DiscoverThread(this);

    mDiscoverThread->start();

    mTcpServer = new QTcpServer(this);
    mTcpServer->listen(QHostAddress(hostIpAddress), TCP_SERVER_PORT);
    connect(mTcpServer, SIGNAL(newConnection()), this, SLOT(newTcpConnection()));

    return true;
}

void MainWindow::stopMonitor()
{
    if (mDiscoverThread) {
        mDiscoverThread->exitThread();
    }

    if (mTcpServer != nullptr) {
        mTcpServer->close();
        mTcpServer = nullptr;
    }
}

void MainWindow::updateLocalIpComboBox()
{
    QStringList ips = getLocalIPs();
    ui->localIpComboBox->clear();
    ui->localIpComboBox->addItems(ips);
    if (ips.isEmpty()) {
        hostIpAddress = "";
    }
}

StbItemWidget* MainWindow::takeStbItemWidget(QString mac)
{
    QString key;

    key = mac;

    if (key.isEmpty()) {
        qDebug() << "Failed: No key.";
        return nullptr;
    }

    StbItemWidget* item = stbItemMap[key];
    if (item == nullptr) {
        item = new StbItemWidget(this);
        item->setVisible(true);
        item->setStatus(DEV_STATUS_IDEL);
        QListWidgetItem* litem = new QListWidgetItem(ui->listWidget);
        ui->listWidget->addItem(litem);
        ui->listWidget->setItemWidget(litem, item);
        item->setIndex(ui->listWidget->count());
        stbItemMap[key] = item;
    }

    return item;
}

StbItemWidget* MainWindow::getStbItemWidgetByIp(QString ip)
{
    if (ip.isEmpty()) {
        qDebug() << "Failed: ip is empty.";
        return nullptr;
    }

    for (StbItemWidget* item : stbItemMap) {
        if (item->getIp() == ip) {
            return item;
        }
    }

    return nullptr;
}

QMap<QString, QString> MainWindow::parsePropertyText(QString text)
{
    QMap<QString, QString> map;

    QStringList lines = text.split("\n");
    for (QString line : lines) {
        QStringList kv = line.split("=");
        if (kv.count() == 2L) {
            map[kv[0]] = kv[1];
        }
    }

    return map;
}

void MainWindow::showErrorMessage(const QString &title, const QString &message)
{
    QMessageBox msgBox;
    QFont font;
    font.setPointSize(10L);
    font.setWeight(QFont::Normal);
    msgBox.setFont(font);
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();
}

void MainWindow::validateAndConfirmIpAddressChange(const QString& ip, quint32 networkAddress)
{
    if (ip.isEmpty()) {
        showErrorMessage("错误", "IP地址不能为空！");
        openDirectConnectionSetupPage();
    } else {
        QHostAddress getIpAddress(ip);
        if (getIpAddress.isNull() || getIpAddress.protocol() != QAbstractSocket::IPv4Protocol) {
            showErrorMessage("错误", "无效的IP地址！");
            openDirectConnectionSetupPage();
        } else if ((getIpAddress.toIPv4Address() & networkAddress) != networkAddress) {
            showErrorMessage("错误", "设置的IP地址与本地IP不在同一网段！");
            openDirectConnectionSetupPage();
        } else {
            msgBox.setWindowTitle("提示");
            QString msgText = "确认将直连设备IP设为: <b><font color='red'>" + ip + " </font></b> ?   ";
            msgBox.setTextFormat(Qt::RichText);
            msgBox.setText(msgText);

            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            msgBox.setDefaultButton(QMessageBox::No);

            msgBox.setIcon(QMessageBox::Question);

            if (msgBox.exec() == QMessageBox::Yes) {
                this->mDiscoverThread->setDirectConnectSetup(ip, this->subnetMask);
                this->mDiscoverThread->setDirectConnectSetupFlag(true);
            } else {
                openDirectConnectionSetupPage();
            }
        }
    }
}

void MainWindow::openDirectConnectionSetupPage()
{
    qDebug() << "Open Direct Connection Setup Page.";
    bool getInfo;

    QHostAddress ipAddress(this->hostIpAddress);
    if (ipAddress.isNull() || ipAddress.protocol() != QAbstractSocket::IPv4Protocol) {
        qDebug() << "Invalid IP address";
        return;
    }

    quint32 ipValue = ipAddress.toIPv4Address();
    quint32 subnetMaskValue = QHostAddress(this->subnetMask).toIPv4Address();

    quint32 networkAddress = ipValue & subnetMaskValue;

    quint32 hostMask = ~subnetMaskValue;

    quint32 hostValue = (ipValue & hostMask) + 1;

    if ((hostValue & hostMask) == hostMask || (hostValue & hostMask) == 0) {
        hostValue = networkAddress + 1;
    }

    QHostAddress newIpAddress(networkAddress | (hostValue & hostMask));

    QString ip;
    DirectConnectionSetupDialog directConnectionSetupDialog;
    directConnectionSetupDialog.setIpAddressLineEdit(newIpAddress.toString());
    directConnectionSetupDialog.setSubnetMaskLineEdit(this->subnetMask);
    if (directConnectionSetupDialog.exec() == QDialog::Accepted) {
        ip = directConnectionSetupDialog.getIpAddress();
    }

    getInfo = directConnectionSetupDialog.getAccepted();

    qDebug() << "ip: " << ip << ", getInfo: " << getInfo;

    if (!getInfo) {
        return
    }
    validateAndConfirmIpAddressChange(ip, networkAddress);
}

void MainWindow::openNetworkAdapterSettings()
{
    QString program = "control.exe";
    QStringList arguments;
    arguments << "ncpa.cpl";

    QProcess::startDetached(program, arguments);
}

void MainWindow::on_toolMenuButton_clicked()
{
    QMenu menu;

    connect(menu.addAction("直连配网 "), &QAction::triggered, this, &MainWindow::openDirectConnectionSetupPage);
    connect(menu.addAction("更改适配器选项 "), &QAction::triggered, this, &MainWindow::openNetworkAdapterSettings);

    menu.exec(QCursor::pos());
    qDebug() << "exit menu.";
}

void MainWindow::on_md5Enable_stateChanged(int arg1)
{
    this->md5Enable = (arg1 == Qt::Checked) ? true : false;
}
