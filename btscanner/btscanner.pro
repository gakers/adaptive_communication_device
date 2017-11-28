TARGET = btscanner

QT = core bluetooth widgets
TEMPLATE = app

SOURCES = \
    main.cpp \
    device.cpp \
    service.cpp

HEADERS = \
    device.h \
    service.h

FORMS = \
    device.ui \
    service.ui

target.path = /home/gakers/Qt5.9.0/Examples/Qt-5.9/btscanner
INSTALLS += target
