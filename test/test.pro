#-------------------------------------------------
#
# Project created by QtCreator 2013-05-28T02:26:05
#
#-------------------------------------------------

QT       += core testlib

QT       -= gui

TARGET = TestGitlEvtBus
CONFIG   += console
CONFIG   += c++11
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH    += ../../libgitlmvc/src \
                  ../libgitlevtbus/src

SOURCES += \
    testcase.cpp



LIBS += -L$${OUT_PWD}/.. -L$${OUT_PWD}/../libgitlevtbus

CONFIG(debug, debug|release){
    LIBS += -lGitlMVCd -lGitlEvtBusd
}
CONFIG(release, debug|release){
    LIBS += -lGitlMVC  -lGitlEvtBus
}
