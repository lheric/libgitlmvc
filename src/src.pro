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

# c++11 enalbed
CONFIG += c++11
QMAKE_CXXFLAGS = -std=c++11 -stdlib=libc++ -mmacosx-version-min=10.7
QMAKE_LFLAGS =   -std=c++11 -stdlib=libc++ -mmacosx-version-min=10.7

CONFIG   += console
CONFIG   += staticlib
CONFIG   -= app_bundle

TEMPLATE = lib

LIBS += -L$${OUT_PWD}/../libgitlevtbus

Debug:   LIBS += -lGitlEvtBusd
Release: LIBS += -lGitlEvtBus

HEADERS += \
    gitlfrontcontroller.h \
    gitlabstractcommand.h \
    gitlmvcconst.h \
    gitlview.h \
    gitlupdateuievt.h \
    gitlivkcmdevt.h \
    gitlmodel.h \
    gitlmtfrontcontroller.h

SOURCES += \
    gitlfrontcontroller.cpp \
    gitlview.cpp \
    gitlupdateuievt.cpp \
    gitlivkcmdevt.cpp \
    gitlmtfrontcontroller.cpp


