#-------------------------------------------------
#
# Project created by QtCreator 2018-04-09T09:06:34
#
#-------------------------------------------------

QT       += core gui sql

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PhonemeDetectionDemo
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += E:\opencv\opencv\build-sources-Desktop_Qt_5_9_2_MinGW_32bit-Default\install\include\opencv2 \
               E:\opencv\opencv\build-sources-Desktop_Qt_5_9_2_MinGW_32bit-Default\install\include\opencv  \
               E:\opencv\opencv\build-sources-Desktop_Qt_5_9_2_MinGW_32bit-Default\install\include

LIBS += E:\opencv\opencv\build-sources-Desktop_Qt_5_9_2_MinGW_32bit-Default\install\x86\mingw\lib\libopencv_*


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    csqlopt.cpp \
    DetectMusicalScale.cpp \
    ReadWav.cpp \
    stdafx.cpp \
    cthreaddetectphoneme.cpp \
    cconnectdialog.cpp \
    tinyxml2.cpp

HEADERS += \
        mainwindow.h \
    csqlopt.h \
    DetectMusicalScale.h \
    ReadWav.h \
    stdafx.h \
    targetver.h \
    cthreaddetectphoneme.h \
    cconnectdialog.h \
    tinyxml2.h

FORMS += \
        mainwindow.ui \
    cconnectdialog.ui
