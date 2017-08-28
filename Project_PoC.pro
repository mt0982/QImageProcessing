#-------------------------------------------------
#
# Project created by QtCreator 2017-06-02T12:04:21
#
#-------------------------------------------------

QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Project_PoC
TEMPLATE = app

LIBS += -lfftw3

SOURCES += main.cpp\
        mainwindow.cpp \
    gausskernel.cpp \
    staticfilter.cpp \
    canny.cpp \
    morfologicalfilter.cpp \
    Pattern/facadeimage.cpp \
    houghtransform.cpp \
    fftw.cpp \
    histogram.cpp

HEADERS  += mainwindow.h \
    gausskernel.h \
    staticfilter.h \
    canny.h \
    morfologicalfilter.h \
    Pattern/facadeimage.h \
    houghtransform.h \
    fftw.h \
    histogram.h

FORMS    += mainwindow.ui \
    gausskernel.ui \
    staticfilter.ui \
    canny.ui \
    morfologicalfilter.ui \
    houghtransform.ui \
    fftw.ui \
    histogram.ui
