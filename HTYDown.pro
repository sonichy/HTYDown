#-------------------------------------------------
#
# Project created by QtCreator 2017-01-18T14:54:09
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HTYDown
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dialognew.cpp \
    form.cpp

HEADERS  += mainwindow.h \
    dialognew.h \
    form.h

FORMS    += mainwindow.ui \
    dialognew.ui \
    form.ui

RESOURCES += \
    files.qrc
