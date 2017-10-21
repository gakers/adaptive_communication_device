#-------------------------------------------------
#
# Project created by QtCreator 2017-09-15T21:08:45
#
#-------------------------------------------------

QT       += gui
QT       += widgets

CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = txt2speech
TEMPLATE = app


SOURCES += main.cpp \
        mainwindow.cpp \
    corpus.cpp

HEADERS  += mainwindow.h \
    corpus.h

FORMS    += mainwindow.ui

DISTFILES += \
    dict.txt \
    google-books-common-words.txt \
    phrase_list.txt

INSTALLS    = target
target.files = txt2speech
target.path = /home/pi
