TEMPLATE = app
TARGET = VATATS

QT = core gui
QT += webchannel webenginewidgets sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS += src/PathProvider.h src/flightTrackingPage.h src/bridgeToMSFS.h src/savePoints.h src/savePoints.h src/readPoints.h \
    tracking.h
SOURCES += src/main.cpp src/PathProvider.cpp src/flightTrackingPage.cpp src/bridgeToMSFS.cpp src/savePoints.cpp src/savePoints.cpp src/readpoints.cpp \
    tracking.cpp
RESOURCES += resources.qrc

INCLUDEPATH += include

LIBS += -LC:/Users/User/Documents/GitHub/VATATS/libs/SimConnect/lib -lSimConnect
INCLUDEPATH += C:/Users/User/Documents/GitHub/VATATS/libs/SimConnect/include

FORMS += \
    tracking.ui
