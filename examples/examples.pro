#-------------------------------------------------
#
# Project created by QtCreator 2013-08-03T00:09:19
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = examples
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app


SOURCES += main.cpp

HEADERS  +=

FORMS    += mainwindow.ui


INCLUDEPATH    += ../../libgitlmvc/src \
                  ../../libgitlevtbus/src


LIBS += -L$${OUT_PWD}/.. -L$${OUT_PWD}/../libgitlevtbus
Debug:   LIBS += -lGitlMVCd -lGitlEvtBusd
Release: LIBS += -lGitlMVC  -lGitlEvtBus
