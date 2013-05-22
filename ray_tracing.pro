#-------------------------------------------------
#
# Project created by QtCreator 2013-05-16T22:53:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ray_tracing
TEMPLATE = app

CONFIG += c++11
LIBS += -lpng

SOURCES += main.cpp\
        mainwindow.cpp \
    Texture.cpp

HEADERS  += mainwindow.h \
    Object.hpp \
    Ray.hpp \
    Vector.hpp \
    Material.hpp \
    Color.hpp \
    Texture.hpp

FORMS    += mainwindow.ui
