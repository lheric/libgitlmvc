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

LIBS += -L$${OUT_PWD}/../libgitlevtbus

Debug:   LIBS += -lGitlEvtBusd
Release: LIBS += -lGitlEvtBus

HEADERS += \
    gitlfrontcontroller.h \
    gitlcommandparameter.h \
    gitlabstractcommand.h \
    gitlmvcconst.h \
    gitlview.h \
    gitlupdateuievt.h \
    gitlivkcmdevt.h \
    gitlmodel.h

SOURCES += \
    gitlfrontcontroller.cpp \
    gitlcommandparameter.cpp \
    gitlview.cpp \
    gitlupdateuievt.cpp \
    gitlivkcmdevt.cpp


