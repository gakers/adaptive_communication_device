#-------------------------------------------------
#
# Project created by QtCreator 2017-09-15T21:08:45
#
#-------------------------------------------------

QT       += gui
QT       += widgets texttospeech

CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = txt2speech
TEMPLATE = app


SOURCES += main.cpp \
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

DISTFILES += \
    dict.txt
