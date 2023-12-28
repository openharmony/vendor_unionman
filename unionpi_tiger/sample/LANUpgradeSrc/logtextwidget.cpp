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


#include "logtextwidget.h"
#include "MainWindow.h"
#include "ui_logtextwidget.h"
#include <QMessageBox>
#include <QScrollBar>
#include <QTimer>

LogTextWidget::LogTextWidget(QWidget* parent, QWidget* stbItemWidget)
    :QWidget(parent), ui(new Ui::LogTextWidget)
{
    this->ItemWidget = stbItemWidget;
    ui->setupUi(this);
}

LogTextWidget::~LogTextWidget()
{
    delete ui;
}

void LogTextWidget::on_logTextClosepushButton_clicked()
{
    this->close();
    this->deleteLater();
}

void LogTextWidget::setLogText(QString logContent)
{
    QString styleSheet = "QTextBrowser {"
                         "  color: black;"
                         "  font-family: Arial;"
                         "  font-size: 12pt;"
                         "  font-weight: normal;"
                         "}";
    ui->textBrowser->setStyleSheet(styleSheet);

    ui->textBrowser->clear();

    QTimer* timer = new QTimer(this);
    int index = 0;

    this->logLoadingIsCancelled = false;

    QMessageBox* loadingBox = new QMessageBox(this);

    loadingBox->setWindowTitle("提示");
    loadingBox->setText("日志文件正在加载，请稍候...    ");

    QFont font;
    font.setPointSize(10L);
    font.setWeight(QFont::Normal);
    loadingBox->setFont(font);

    loadingBox->setStandardButtons(QMessageBox::NoButton);
    loadingBox->setIcon(QMessageBox::Information);

    QPushButton* cancelButton = new QPushButton(loadingBox);
    cancelButton->setText("取消");
    loadingBox->addButton(cancelButton, QMessageBox::RejectRole);

    connect(cancelButton, &QPushButton::clicked, [=]() {
        this->logLoadingIsCancelled = true;
        timer->stop();
        loadingBox->close();
        loadingBox->deleteLater();
        timer->deleteLater();
    });

    loadingBox->show();
    QCoreApplication::processEvents();

    QTimer::connect(timer, &QTimer::timeout, [=]() mutable {
        if (index < logContent.length() && !logLoadingIsCancelled) {
            QString text = logContent.mid(index, 200000);
            ui->textBrowser->insertPlainText(text);

            QScrollBar* verticalScrollBar = ui->textBrowser->verticalScrollBar();
            verticalScrollBar->setValue(verticalScrollBar->maximum());

            index += text.length();
        } else {
            timer->stop();
            loadingBox->close();
            loadingBox->deleteLater();
            timer->deleteLater();
        }
    });

    timer->setInterval(1);
    timer->start();
}

void LogTextWidget::on_logTextclearPushButton_clicked()
{
    QMessageBox::StandardButton btn
        = QMessageBox::question(this, "提示", "确认清除日志信息?", QMessageBox::Yes | QMessageBox::No);
    if (btn == QMessageBox::Yes) {
        StbItemWidget* curItemWidget = static_cast<StbItemWidget*>(this->ItemWidget);

        if (!curItemWidget) {
            return;
        }

        QString logFilePath = curItemWidget->getLogPath();

        QFile file(logFilePath);
        if (!file.exists()) {
            return;
        }

        curItemWidget->logMutexLock();
        if (file.open(QIODevice::WriteOnly)) {
            QString file_head_str = curItemWidget->getLogHead();
            file.write(file_head_str.toStdString().data());
            file.flush();
            file.close();
        }
        if (file.open(QIODevice::ReadOnly)) {
            QString fileContent = QString(file.readAll());
            this->setLogText(fileContent);
            file.close();
        }
        curItemWidget->logMutexUnlock();
    }
}

void LogTextWidget::on_logTextrefreshPushButton_clicked()
{
    StbItemWidget* curItemWidget = static_cast<StbItemWidget*>(this->ItemWidget);

    if (!curItemWidget) {
        return;
    }

    QString logFilePath = curItemWidget->getLogPath();

    QFile file(logFilePath);
    if (!file.exists()) {
        return;
    }

    curItemWidget->logMutexLock();
    if (file.exists()) {
        if (file.open(QIODevice::ReadOnly)) {
            QString fileContent = QString(file.readAll());
            this->setLogText(fileContent);
            file.close();
        }
    }
    curItemWidget->logMutexUnlock();
}
