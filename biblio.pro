#-------------------------------------------------
#
# Project created by QtCreator 2017-01-04T20:35:16
#
#-------------------------------------------------

CONFIG += c++11
QT += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = biblio
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

VERSION = "1.0.1"
DEFINES += "BIBLIO_VERSION=\"\\\"$$VERSION\\\"\""

SOURCES += main.cpp\
        mainwindow.cpp \
    bibtablemodel.cpp \
    bibdata.cpp \
    newdocumentdialog.cpp

HEADERS  += mainwindow.h \
    bibtablemodel.h \
    bibdata.h \
    newdocumentdialog.h

FORMS    += mainwindow.ui \
    newdocumentdialog.ui

RESOURCES += \
    biblio.qrc
