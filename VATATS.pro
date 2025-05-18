TEMPLATE = app
TARGET = VATATS

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += src/main.cpp

INCLUDEPATH += include

LIBS += -LC:/Users/User/Documents/GitHub/VATATS/libs/SimConnect/lib -lSimConnect
INCLUDEPATH += C:/Users/User/Documents/GitHub/VATATS/libs/SimConnect/include