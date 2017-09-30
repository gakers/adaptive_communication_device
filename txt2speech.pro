#-------------------------------------------------
#
# Project created by QtCreator 2017-09-15T21:08:45
#
#-------------------------------------------------

QT       += gui
CONFIG += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = txt2speech
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    textbutton.cpp \
    globals.cpp

HEADERS  += mainwindow.h \
    textbutton.h \
    globals.h

FORMS    += mainwindow.ui
