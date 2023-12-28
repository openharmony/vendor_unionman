#-------------------------------------------------
#
# Project created by QtCreator 2018-05-09T10:31:11
#
#-------------------------------------------------

QT       += core gui network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LANUpgrade
TEMPLATE = app

SOURCES += main.cpp\
        MainWindow.cpp \
    StbItemWidget.cpp \
    JQHttpServer.cpp \
    JQNet.cpp \
    logtextwidget.cpp

HEADERS  += MainWindow.h \
    StbItemWidget.h \
    JQHttpServer.h \
    JQNet.h \
    ProductConfig.h \
    logtextwidget.h

FORMS    += mainwindow.ui \
    logtextwidget.ui \
    stb_item_widget.ui

UI_DIR = ui_gen

RESOURCES += \
    res.qrc

RC_FILE += logo.rc
