QT += core
QT -= gui
QT += network
QT += testlib

CONFIG += c++11

TARGET = Server_test
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    serverClientTest.cpp

HEADERS += \
    serverclientconnection.h
