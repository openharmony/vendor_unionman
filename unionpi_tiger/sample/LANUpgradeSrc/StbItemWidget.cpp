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

#include "StbItemWidget.h"
#include "MainWindow.h"
#include "ui_stb_item_widget.h"
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QInputDialog>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QProcess>
#include <QUrl>

StbItemWidget::StbItemWidget(QWidget* parent)
    :QWidget(parent), ui(new Ui::StbItemWidget)
{
    ui->setupUi(this);
    setLayout(ui->horizontalLayout);
    ui->snLabel->setText("上线");
    ui->uptimeLabel->setText("");
    this->parent = parent;
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &StbItemWidget::customContextMenuRequested, this, &StbItemWidget::openMenu);
}

StbItemWidget::~StbItemWidget()
{
    delete ui;
}

void StbItemWidget::openConfigWeb()
{
    if (ip.contains(":")) {
        QDesktopServices::openUrl(QUrl("http://[" + ip + "]"));
    } else {
        QDesktopServices::openUrl(QUrl("http://" + ip));
    }
}

void StbItemWidget::playVideo()
{
    qDebug() << "Ready to play video.";
    QString urlPath = "/livestream/UM-ONVIF";
    QString cmd = "start \"" + ip + "\"" + " .\\ffplay.exe rtsp://" + ip + urlPath
        + " -x 1280 -y 720 -fflags nobuffer -rtsp_transport tcp";
    system(cmd.toLatin1().data());
}

void StbItemWidget::openADBShell()
{
    QString cmd
        = "start \"" + ip + "\" cmd /K \"hdc -t " + ip + " shell\"";
    system(cmd.toLatin1().data());
}

void StbItemWidget::checkLog()
{
    LogTextWidget* logTextWidget = new LogTextWidget(nullptr, qobject_cast<QWidget*>(this));
    QString logTextWidgetTitle = this->sn == NULL ? "log (" + this->mac + ")" : "log (" + this->sn + ")";
    logTextWidget->setWindowTitle(logTextWidgetTitle);
    logTextWidget->show();

    QString logFilePath = this->getLogPath();

    this->logMutex.lock();
    QFile file(logFilePath);
    if (file.exists()) {
        if (file.open(QIODevice::ReadOnly)) {
            QString fileContent = QString(file.readAll());
            logTextWidget->setLogText(fileContent);
            file.close();
        }
    }
    this->logMutex.unlock();
}

void StbItemWidget::openLogDirectory()
{
    QString localApplicationDirPath = QCoreApplication::applicationDirPath();

    QString logDir = localApplicationDirPath + "/log/" + (this->sn == NULL ? this->mac : this->sn);
    QDir dir(logDir);
    logDir.replace("/", QDir::separator());
    if (!dir.exists()) {
        if (!dir.mkpath(logDir)) {
            QMessageBox errorBox;

            errorBox.setWindowTitle("错误");
            errorBox.setText("日志目录不存在，且尝试创建目录失败！ ");

            QFont font;
            font.setPointSize(10L);
            font.setWeight(QFont::Normal);
            errorBox.setFont(font);

            errorBox.setIcon(QMessageBox::Critical);
            errorBox.exec();
            return;
        }
    }

    QUrl directoryUrl = QUrl::fromLocalFile(logDir);
    QDesktopServices::openUrl(directoryUrl);
}

int StbItemWidget::httpRequest(QString req)
{
    QString url = "";
    if (QHostAddress(ip).protocol() == QAbstractSocket::IPv4Protocol) {
        url = "http://" + ip + ":6060/" + req;
    } else if (QHostAddress(ip).protocol() == QAbstractSocket::IPv6Protocol) {
        url = "http://[" + ip + "]:6060/" + req;
    }

    QNetworkRequest request;
    QNetworkAccessManager* naManager = new QNetworkAccessManager(this);
    QMetaObject::Connection connRet
        = QObject::connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
    Q_ASSERT(connRet);

    qDebug() << "url: " + url;
    request.setUrl(QUrl(url));

    QString testData = "";
    QNetworkReply* reply = naManager->post(request, testData.toUtf8());
    return 0;
}

void StbItemWidget::burnSN()
{
    qDebug() << "Ready to burn sn.";
    bool getInfo;
    QInputDialog* inputDialog = new QInputDialog();
    QString title = "修改序列号";
    QString config = inputDialog->getText(this->parent, title, "输入序列号", QLineEdit::Normal, sn, &getInfo);
    qDebug() << "Config: " << config << ", getInfo: " << getInfo;
    if (getInfo) {
        if (config.isEmpty()) {
            config = "sn=null";
        } else if (!config.contains("=")) {
            config = "sn=" + config;
        }

        httpRequest("SetDevInfo?" + config);
    }
}

void StbItemWidget::reboot()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("提示");
    msgBox.setText("确认重启设备?");

    QFont font;
    font.setPointSize(10L);
    font.setWeight(QFont::Normal);
    msgBox.setFont(font);

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    msgBox.setIcon(QMessageBox::Question);

    if (msgBox.exec() == QMessageBox::Yes) {
        QString logInfo = "设备通过升级工具触发重启！";
        this->writeLog(logInfo);
        qDebug() << "Ready reboot.";
        httpRequest("SetDevInfo?shell=reboot");
    }
}

void StbItemWidget::enterFactoryMode()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("提示");
    msgBox.setText("确认进入工厂模式?");

    QFont font;
    font.setPointSize(10L);
    font.setWeight(QFont::Normal);
    msgBox.setFont(font);

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    msgBox.setIcon(QMessageBox::Question);

    if (msgBox.exec() == QMessageBox::Yes) {
        QString url = "";
        qDebug() << "Ready to enter factory.";
        httpRequest("EnterFactoryMode");
    }
}

void StbItemWidget::restoreFactorySetting()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("提示");
    msgBox.setText("确认恢复出厂设置?");

    QFont font;
    font.setPointSize(10L);
    font.setWeight(QFont::Normal);
    msgBox.setFont(font);

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    msgBox.setIcon(QMessageBox::Question);

    if (msgBox.exec() == QMessageBox::Yes) {
        QString logInfo = "设备通过升级工具触发恢复出厂设置！";
        this->writeLog(logInfo);
        qDebug() << "Ready to restore factory setting.";
        httpRequest("RestoreFactorySetting");
    }
}

void FilePusher::setIp(QString ip)
{
    this->ip = ip;
    QString title = "文件推送界面( " + this->ip + " )";
    setWindowTitle(title);
}

void FilePusher::selectFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择文件", "", "All Files (*)");
    if (!filePath.isEmpty()) {
        filePathLineEdit->setText(filePath);
    }
}

void FilePusher::pushFile()
{
    if (pushButton->text() == "推送") {
        if (!filePathLineEdit->text().isEmpty() && !targetPathLineEdit->text().isEmpty()) {
            m_filePusherThread.SetFilePath(filePathLineEdit->text());
            m_filePusherThread.SetTargetPath(targetPathLineEdit->text());
            m_filePusherThread.SetIp(ip);
            filePathLineEdit->setEnabled(false);
            targetPathLineEdit->setEnabled(false);
            selectButton->setEnabled(false);

            pushButton->setText("取消");

            m_filePusherThread.start();
        } else {
            QMessageBox errBox;
            QFont font;
            font.setPointSize(10L);
            font.setWeight(QFont::Normal);
            errBox.setFont(font);
            errBox.setWindowTitle("错误");
            errBox.setText("文件路径和设备目标目录均不能为空！ ");
            errBox.setIcon(QMessageBox::Critical);
            errBox.exec();
        }
    } else {
        m_filePusherThread.CancelPush();

        filePathLineEdit->setEnabled(true);
        targetPathLineEdit->setEnabled(true);
        selectButton->setEnabled(true);

        pushButton->setText("推送");
    }
}

void FilePusher::handlePushMessage(const QString& result)
{
    resultTextEdit->setPlainText(result);
}

void FilePusher::handlePushFinished(const QString& result)
{
    resultTextEdit->setPlainText(result);

    filePathLineEdit->setEnabled(true);
    targetPathLineEdit->setEnabled(true);
    selectButton->setEnabled(true);

    pushButton->setText("推送");
}

void FilePusherThread::CancelPush()
{
    if (pushProcess.state() == QProcess::Running) {
        pushProcess.kill();
    }
    pushProcessCancal = true;
}

void FilePusherThread::Run()
{
    QString adbPath = ".//adb//adb";
    QStringList arguments;
    arguments << "-s" << this->m_ip << "push" << this->m_filePath << this->m_targetPath;
    QStringList connectArguments;
    connectArguments << "connect" << this->m_ip;

    emit PushMessage("文件推送中...");

    pushProcess.setProgram(adbPath);
    pushProcess.setArguments(connectArguments);
    pushProcess.start();
    pushProcess.waitForFinished();

    if (pushProcessCancal) {
        emit PushMessage("文件推送已取消！");
        pushProcessCancal = false;
        return;
    }

    pushProcess.setArguments(arguments);
    pushProcess.start();
    pushProcess.waitForFinished();

    if (pushProcessCancal) {
        emit PushMessage("文件推送已取消！");
        pushProcessCancal = false;
        return;
    }

    QByteArray output = pushProcess.readAllStandardOutput();
    QString result = QString::fromLocal8Bit(output);
    emit PushFinished(result);
}

void StbItemWidget::pushFile()
{
    this->filePusher.setIp(this->ip);
    filePusher.show();
}

void StbItemWidget::openMenu(const QPoint& pos)
{
    QMenu menu;
    if (status != DEV_STATUS_OFFLINE) {
        if (!sn.isEmpty()) {
            connect(menu.addAction("Open Config Web"), &QAction::triggered, this, &StbItemWidget::openConfigWeb);
        }

        connect(menu.addAction("Play Live Video"), &QAction::triggered, this, &StbItemWidget::playVideo);
        connect(menu.addAction("Burn SN"), &QAction::triggered, this, &StbItemWidget::burnSN);
        connect(menu.addAction("Open HDC Shell"), &QAction::triggered, this, &StbItemWidget::openADBShell);
        connect(menu.addAction("Restore Factory Setting"), &QAction::triggered, this,
            &StbItemWidget::restoreFactorySetting);
        connect(menu.addAction("Push File"), &QAction::triggered, this, &StbItemWidget::pushFile);

        if (!sn.isEmpty()) {
            connect(menu.addAction("Enter Factory Mode"), &QAction::triggered, this, &StbItemWidget::enterFactoryMode);
        }

        connect(menu.addAction("Reboot"), &QAction::triggered, this, &StbItemWidget::reboot);
    }

    connect(menu.addAction("Check Log"), &QAction::triggered, this, &StbItemWidget::checkLog);
    connect(menu.addAction("Open Log Directory"), &QAction::triggered, this, &StbItemWidget::openLogDirectory);

    menu.exec(QCursor::pos());
    qDebug() << "exit menu.";
}

void StbItemWidget::requestFinished(QNetworkReply* reply)
{
    // 获取http状态码
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (statusCode.isValid())
        qDebug() << "status code=" << statusCode.toInt();

    QVariant reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    if (reason.isValid())
        qDebug() << "reason=" << reason.toString();

    QNetworkReply::NetworkError err = reply->error();
    if (err != QNetworkReply::NoError) {
        qDebug() << "Failed: " << reply->errorString();
    } else {
        // 获取返回内容
        qDebug() << reply->readAll();
    }
}

void StbItemWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (sn.isEmpty()) {
            playVideo();
        } else {
            openConfigWeb();
        }
    }
}

void StbItemWidget::setIndex(int index)
{
    QString str = QString::asprintf("%d", index);
    ui->indexLabel->setText(str);
}


void StbItemWidget::setMainInfo(QString mac, QString sn, QString hwv, QString swv, QString dm)
{
    ui->macLabel->setText(mac);
    ui->swLabel->setText(swv);
    ui->hwLabel->setText(hwv);
    ui->snLabel->setText(sn);
    ui->dmLabel->setText(dm);
    this->mac = mac;
    this->sn = sn;
    this->deviceModel = dm;
    this->hwv = hwv;
    this->swv = swv;
}

void StbItemWidget::setIp(QString ip, bool isStatic)
{
    ui->ipLabel->setText(ip + (isStatic ? " (Static)" : ""));
    this->ip = ip;
    this->isStaticIP = isStatic;
}

void StbItemWidget::setTemperature(QString temperature)
{
    this->temperature = temperature;
}

void StbItemWidget::setIpcameraPss(QString ipcameraPss)
{
    this->ipcameraPss = ipcameraPss;
}

QString StbItemWidget::getLogHead()
{
    QString file_head_str = "**************************************************************************\n"
                            "*    Mac:\t\t\t"
        + this->mac
        + "\n"
          "*    Sn:\t\t\t"
        + this->sn
        + "\n"
          "*    Hardware Version:\t"
        + this->hwv
        + "\n"
          "*    Software Version:\t"
        + this->swv
        + "\n"
          "*    Device Model:\t\t"
        + this->deviceModel
        + "\n"
          "**************************************************************************\n\n";
    return file_head_str;
}

void StbItemWidget::logMutexLock()
{
    this->logMutex.lock();
}

void StbItemWidget::logMutexUnlock()
{
    this->logMutex.unlock();
}

QString StbItemWidget::getLogPath()
{
    QString localApplicationDirPath = QCoreApplication::applicationDirPath();

    QString logDir = localApplicationDirPath + "/log/" + (this->sn == NULL ? this->mac : this->sn);
    QDir dir(logDir);
    logDir.replace("/", QDir::separator());
    if (!dir.exists()) {
        if (!dir.mkpath(logDir)) {
            return QString();
        }
    }

    QString mac = this->mac.replace(":", "-");
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    QString logFilePath = this->sn == NULL ? (logDir + "\\" + "log_" + current_date_time + "_" + mac + ".txt")
                                           : (logDir + "\\" + "log_" + current_date_time + "_" + this->sn + ".txt");
    return logFilePath;
}

void StbItemWidget::writeLog(QString info)
{
    if (this->sn == NULL && this->mac == NULL) {
        return;
    }
    QString logFilePath = this->getLogPath();

    QFile file(logFilePath);
    bool file_exists = file.exists();
    this->logMutex.lock();
    if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
        if (!file_exists) {
            QString file_head_str = this->getLogHead();
            file.write(file_head_str.toStdString().data());
        }
        QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QString log_info = "[" + current_date_time + "] " + info + "\r\n";
        file.write(log_info.toStdString().data());
        file.flush();
        file.close();
    }
    this->logMutex.unlock();
}

QString StbItemWidget::generateLogInfo()
{
    qlonglong secs = (qlonglong)this->upTime.toDouble();
    QTime time;
    time.setHMS(0, 0, 0, 0);
    QString timeStr = time.addSecs(int(secs)).toString("hh:mm:ss");
    int days = secs / 24 / 60 / 60;
    QString dt = "";
    if (days > 0) {
        dt += QString::number(days) + "天 ";
    }
    dt += timeStr;

    QString logInfo = "温度： " + this->temperature + ",  Ipcamera_PSS： " + this->ipcameraPss + ",  运行时间： " + dt;

    return logInfo;
}

void StbItemWidget::setStatusCase1()
{
    this->setStyleSheet("color:rgb(25, 130, 59)");
    ui->statusLabel->setText("上线");
    QString logInfo = "设备上线  ( 当前" + this->generateLogInfo() + " )";
    this->writeLog(logInfo);
}

void StbItemWidget::setStatusCase2()
{
    this->setStyleSheet("color:rgb(223, 63, 33)");
    ui->statusLabel->setText("离线");
    if (this->status != DEV_STATUS_OFFLINE) {
        QString logInfo = "设备离线  ( 最后上报" + this->generateLogInfo() + " )";
        this->writeLog(logInfo);
    }
}

void StbItemWidget::setStatusCase3()
{
    this->setStyleSheet("color:rgb(132, 112, 255)");
    ui->statusLabel->setText("正在下载");
    QString logInfo = "设备正在下载升级包  ( 当前" + this->generateLogInfo() + " )";
    this->writeLog(logInfo);
}

void StbItemWidget::setStatusCase4()
{
    this->setStyleSheet("color:rgb(132, 112, 255)");
    ui->statusLabel->setText("正在校验");
    QString logInfo = "设备正在校验升级包  ( 当前" + this->generateLogInfo() + " )";
    this->writeLog(logInfo);
}

void StbItemWidget::setStatusCase5()
{
    this->setStyleSheet("color:rgb(132, 112, 255)");
    ui->statusLabel->setText("正在更新");
    QString logInfo = "设备正在更新  ( 当前" + this->generateLogInfo() + " )";
    this->writeLog(logInfo);
}

void StbItemWidget::setStatusCase6()
{
    this->setStyleSheet("color:rgb(226, 190, 27)");
    ui->statusLabel->setText("下载失败");
    QString logInfo = "设备下载升级包失败  ( 当前" + this->generateLogInfo() + " )";
    this->writeLog(logInfo);
}

void StbItemWidget::setStatus(DEV_STATUS_E myStatus)
{
    switch (myStatus) {
        case DEV_STATUS_IDEL: {
            this->setStatusCase1();
            break;
        }
        case DEV_STATUS_OFFLINE: {
            this->setStatusCase2();
            break;
        }
        case DEV_STATUS_UPGRADE_DOWNLOADING: {
            this->setStatusCase3();
            break;
        }
        case DEV_STATUS_UPGRADE_CHECKING: {
            this->setStatusCase4();
            break;
        }
        case DEV_STATUS_UPGRADE_UPDATING: {
            this->setStatusCase5();
            break;
        }
        case DEV_STATUS_UPGRADE_ERR_DOWNLOAD: {
            this->setStatusCase6();
            break;
        }
        case DEV_STATUS_UPGRADE_ERR_CHECK: {
            this->setStyleSheet("color:rgb(226, 190, 27)");
            ui->statusLabel->setText("校验失败");
            QString logInfo = "设备升级包校验失败  ( 当前" + this->generateLogInfo() + " )";
            this->writeLog(logInfo);
            break;
        }
        case DEV_STATUS_UPGRADE_ERR_UPDATE: {
            this->setStyleSheet("color:rgb(226, 190, 27)");
            ;
            ui->statusLabel->setText("升级失败");
            QString logInfo = "设备升级失败  ( 当前" + this->generateLogInfo() + " )";
            this->writeLog(logInfo);
            break;
        }
        default: {
            this->setStyleSheet("color:rgb(226, 190, 27)");
            ui->statusLabel->setText("未知");
            break;
        }
    }

    this->status = myStatus;
}

DEV_STATUS_E StbItemWidget::getStatus()
{
    return this->status;
}

void StbItemWidget::setUptime(QString uptime)
{
    qlonglong secs = (qlonglong)uptime.toDouble();
    QTime time;
    time.setHMS(0, 0, 0, 0);
    QString timeStr = time.addSecs(int(secs)).toString("hh:mm:ss");
    int days = secs / 24 / 60 / 60;
    QString dt = "";
    if (days > 0) {
        dt += QString::number(days) + "天 ";
    }
    dt += timeStr;
    ui->uptimeLabel->setText(dt);

    if (uptime.toDouble() < this->upTime.toDouble()) {
        QString logInfo = " 设备重启! ";
        this->writeLog(logInfo);
    }
    this->upTime = uptime;
}
