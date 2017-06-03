#-------------------------------------------------
#
# Project created by QtCreator 2017-06-02T12:04:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Project_PoC
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    gausskernel.cpp

HEADERS  += mainwindow.h \
    gausskernel.h

FORMS    += mainwindow.ui \
    gausskernel.ui
