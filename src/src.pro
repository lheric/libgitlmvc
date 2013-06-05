#-------------------------------------------------
#
# Project created by QtCreator 2012-11-02T17:27:25
#
#-------------------------------------------------

QT       += core
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

Debug:   TARGET = GitlMVCd
Release: TARGET = GitlMVC

DESTDIR = $${OUT_PWD}/..

CONFIG   += console
CONFIG   -= app_bundle
CONFIG += staticlib
TEMPLATE = lib

SOURCES += \
    gitlfrontcontroller.cpp \
    gitlcommandparameter.cpp \
    gitlabstractcommand.cpp

HEADERS += \
    gitlfrontcontroller.h \
    gitlcommandrespond.h \
    gitlcommandrequest.h \
    gitlcommandparameter.h \
    gitlabstractcommand.h

INCLUDEPATH    += ../include
