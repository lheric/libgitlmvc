#-------------------------------------------------
#
# Project created by QtCreator 2012-11-02T17:27:25
#
#-------------------------------------------------

QT       += core
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DESTDIR = $${OUT_PWD}/..

# c++11 enalbed
CONFIG   += c++11
CONFIG   += console
CONFIG   += staticlib
CONFIG   -= app_bundle

TEMPLATE = lib

LIBS += -L$${OUT_PWD}/../libgitlevtbus


CONFIG(debug, debug|release){
    TARGET = GitlMVCd
    LIBS += -lGitlEvtBusd
}
CONFIG(release, debug|release){
    TARGET = GitlMVC
    LIBS += -lGitlEvtBus
}


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


