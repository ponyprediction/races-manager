#-------------------------------------------------
#
# Project created by QtCreator 2015-05-19T11:32:12
#
#-------------------------------------------------

QT       += core sql network

QT       -= gui

TARGET = races-manager
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    manager.cpp \
    util.cpp

DISTFILES += \
    help.txt \
    conf.xml \
    columns.txt

HEADERS += \
    manager.hpp \
    util.hpp
