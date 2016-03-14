#-------------------------------------------------
#
# Project created by QtCreator 2016-01-28T23:32:13
#
#-------------------------------------------------

QT       += core gui \
         multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hw4b
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    gameboard.cpp \
    instructions.cpp

HEADERS  += mainwindow.h \
    gameboard.h \
    instructions.h

FORMS    += mainwindow.ui \
    gameboard.ui \
    instructions.ui

RESOURCES += \
    images.qrc
