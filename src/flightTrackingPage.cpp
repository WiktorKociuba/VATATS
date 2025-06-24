#include <QObject>
#include <QApplication>
#include <windows.h>
#include <SimConnect.h>
#include "bridgeToMSFS.h"
#include "tracking.h"
#include "ui_tracking.h"
#include <QWebEngineView>
#include "PathProvider.h"
#include <QLayout>
#include <QWebChannel>
#include <QTimer>
#include "flightTrackingPage.h"
#include "globals.h"

void flightTrackingPage::startTracking(tracking* trackingWindow){
    bridgeToMSFS bridgeToMSFSInstance;
    for(int i = 0; !bridgeToMSFSInstance.ConnectToMSFS() && i < 5; i++){
        Sleep(5000);
    }
    bridgeToMSFSInstance.requestAircraftLocation(bridgeToMSFSInstance.hSimConnect);

    if(trackingWindow->trackingTimer->isActive())
        trackingWindow->trackingTimer->stop();
    
    trackingWindow->trackingTimer->disconnect();
    wasOnGround = TRUE;
    
    QObject::connect(trackingWindow->trackingTimer, &QTimer::timeout, [trackingWindow, &bridgeToMSFSInstance](){
        SimConnect_CallDispatch(bridgeToMSFSInstance.hSimConnect, bridgeToMSFS::getAircraftLocation, nullptr);
        bool onGround = (bridgeToMSFS::currentSimData.touchdownState != 0);
        trackingWindow->pathProvider->setPoints(bridgeToMSFSInstance.aircraftPts);
        trackingWindow->showHeightProfile(bridgeToMSFS::altitudeData);
        if(!wasOnGround && onGround){
            bridgeToMSFS::currentLandingForce.gForce = bridgeToMSFS::currentSimData.gForce;
            bridgeToMSFS::currentLandingForce.verticalSpeed = bridgeToMSFS::currentSimData.verticalSpeed;
            QMetaObject::invokeMethod(g_mainWindow, "updateLandingDataDisplay", Qt::QueuedConnection);
        }
        wasOnGround = onGround;
    });
    trackingWindow->trackingTimer->start(2000);
}

void flightTrackingPage::displayPastRoute(QVector<QPair<double,double>> points){
    tracking* trackingWindow = g_mainWindow;
    
    if(trackingWindow->trackingTimer->isActive())
        trackingWindow->trackingTimer->stop();

    trackingWindow->pathProvider->setPoints(points);
}