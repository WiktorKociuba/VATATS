TEMPLATE = app
TARGET = VATATS

QT = core gui
QT += webchannel webenginewidgets sql charts pdf pdfwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS += src/PathProvider.h src/flightTrackingPage.h src/bridgeToMSFS.h src/savePoints.h src/savePoints.h src/readPoints.h src/globals.h src/cpdlc.h src/chartfox.h src/wordwrapdelegate.h src/zoomablePdfView.h \
    src/cpdlcrequests.h \
    src/inforeqdialog.h \
    src/logon.h \
    src/messageresponse.h \
    src/predepgui.h \
    src/telexdialog.h \
    tracking.h
SOURCES += src/main.cpp src/PathProvider.cpp src/flightTrackingPage.cpp src/bridgeToMSFS.cpp src/savePoints.cpp src/readpoints.cpp src/cpdlc.cpp src/chartfox.cpp \
    src/cpdlcrequests.cpp \
    src/inforeqdialog.cpp \
    src/logon.cpp \
    src/messageresponse.cpp \
    src/predepgui.cpp \
    src/telexdialog.cpp \
    tracking.cpp
RESOURCES += resources.qrc

INCLUDEPATH += include

LIBS += -LC:/Users/User/Documents/GitHub/VATATS/libs/SimConnect/lib -lSimConnect
INCLUDEPATH += C:/Users/User/Documents/GitHub/VATATS/libs/SimConnect/include

FORMS += \
    src/cpdlcrequests.ui \
    src/inforeqdialog.ui \
    src/logon.ui \
    src/messageresponse.ui \
    src/predepgui.ui \
    src/telexdialog.ui \
    tracking.ui
