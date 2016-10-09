#-------------------------------------------------
#
# Project created by QtCreator 2016-10-04T16:15:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = overclock_2
TEMPLATE = app


SOURCES += main.cpp\
        hostwindow.cpp \
    adl_device.cpp

HEADERS  += hostwindow.h \
    adl_device.h \
    adl_define.h

FORMS    += hostwindow.ui
