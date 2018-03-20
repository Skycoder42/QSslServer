QT += core gui widgets network

TARGET = Sample
TEMPLATE = app

include(../qsslserver.pri)

HEADERS += \
		mainwindow.h \
    server.h

SOURCES += \
		main.cpp \
		mainwindow.cpp \
    server.cpp

FORMS += \
		mainwindow.ui

RESOURCES += \
    sample.qrc
