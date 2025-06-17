TEMPLATE = app
TARGET = VATATS

QT = core gui
QT += webchannel webenginewidgets sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS += src/PathProvider.h src/savePoints.h
SOURCES += src/main.cpp src/PathProvider.cpp src/savePoints.cpp
RESOURCES += resources.qrc

INCLUDEPATH += include

LIBS += -LC:/Users/User/Documents/GitHub/VATATS/libs/SimConnect/lib -lSimConnect
INCLUDEPATH += C:/Users/User/Documents/GitHub/VATATS/libs/SimConnect/include
