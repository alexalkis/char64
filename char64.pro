QT += core gui widgets

CONFIG += c++11

win32:RC_FILE = myapp.rc

TARGET = char64
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    mainwindow.cpp \
    chardisplay.cpp \
    colorlisteditor.cpp \
    chardialog.cpp \
    charmapwidget.cpp \
    palettebutton.cpp \
    popupwisget.cpp

FORMS += \
    mainwindow.ui \
    chardialog.ui

HEADERS += \
    mainwindow.h \
    chardisplay.h \
    colorlisteditor.h \
    chardialog.h \
    charmapwidget.h \
    palettebutton.h \
    popupwisget.h

RESOURCES += \
    resources.qrc

DISTFILES += \
    myapp.rc \
    Readme.md
