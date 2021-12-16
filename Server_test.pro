QT += core
QT -= gui
QT += network
QT += testlib

CONFIG += c++14

TARGET = Server_test
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    serverClientTest.cpp \
    client.cpp

HEADERS += \
    client.h \
    hcommon.h \
    serverClientTest.h
