QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = K-Neighbours_DetNum
TEMPLATE = app

INCLUDEPATH += ./data

SOURCES += main.cpp \
        mainwindow.cpp drawarea.cpp \

HEADERS  += mainwindow.h drawarea.h r2geom.h pi.h \

FORMS    += mainwindow.ui

CONFIG(release, debug|release): DEFINES += NDEBUG
