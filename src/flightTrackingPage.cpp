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
#include "savePoints.h"

void flightTrackingPage::startTracking(tracking* trackingWindow){
    savePoints::createNewSave();
    
    for(int i = 0; !g_bridgeToMSFSInstance->ConnectToMSFS() && i < 5; i++){
        Sleep(5000);
    }
    g_bridgeToMSFSInstance->requestAircraftLocation(g_bridgeToMSFSInstance->hSimConnect, SIMCONNECT_PERIOD_SECOND);
    wasOnGround = TRUE;
    static bool nearGround = false;

    QObject::connect(trackingWindow->trackingTimer, &QTimer::timeout, [](){
        SimConnect_CallDispatch(g_bridgeToMSFSInstance->hSimConnect, bridgeToMSFS::getAircraftLocation, nullptr);
    });
    trackingWindow->trackingTimer->start(5000);

    QObject::connect(g_bridgeToMSFSInstance, &bridgeToMSFS::SimDataUpdated, [trackingWindow](){
        qDebug() << "called";
        savePoints savePointsInstance;
        static QElapsedTimer saveTimer;
        static bool timerStarted = false;
        if(!timerStarted){
            saveTimer.start();
            timerStarted = true;
        }
        bool onGround = bridgeToMSFS::currentSimData.simOnGround;
        double altAboveGround = bridgeToMSFS::currentSimData.altAboveGround;
        if(!onGround && altAboveGround < 10.0 && !nearGround){
            g_bridgeToMSFSInstance->requestAircraftLocation(g_bridgeToMSFSInstance->hSimConnect, SIMCONNECT_PERIOD_SIM_FRAME);
            nearGround = true;
            trackingWindow->trackingTimer->setInterval(50);
            trackingWindow->trackingTimer->start();
        }
        else if((onGround || altAboveGround >=10.0) && nearGround){
            g_bridgeToMSFSInstance->requestAircraftLocation(g_bridgeToMSFSInstance->hSimConnect, SIMCONNECT_PERIOD_SECOND);
            nearGround = false;
            trackingWindow->trackingTimer->setInterval(5000);
            trackingWindow->trackingTimer->start();
        }
        if(saveTimer.elapsed() >= 4900){
            savePoints::savePointsTime();
            bridgeToMSFS::aircraftPts.push_back({bridgeToMSFS::currentAircraftPosition.latitude,bridgeToMSFS::currentAircraftPosition.longitude});
            savePointsInstance.sendPointsToSQL(bridgeToMSFS::saveName, bridgeToMSFS::currentAircraftPosition.latitude, bridgeToMSFS::currentAircraftPosition.longitude, bridgeToMSFS::currentAircraftPosition.altitude);
            bridgeToMSFS::altitudeData.push_back(bridgeToMSFS::currentAircraftPosition.altitude);
            trackingWindow->pathProvider->setPoints(g_bridgeToMSFSInstance->aircraftPts);
            trackingWindow->showHeightProfile(bridgeToMSFS::altitudeData);
            saveTimer.restart();
        }
        if(wasOnGround && !onGround){
            savePoints::saveTakeoffTime();
        }
        if(!wasOnGround && onGround){
            savePoints::saveLandingTime();
            bridgeToMSFS::currentLandingForce.gForce = bridgeToMSFS::currentSimData.gForce;
            bridgeToMSFS::currentLandingForce.verticalSpeed = bridgeToMSFS::currentSimData.verticalSpeed;
            QMetaObject::invokeMethod(g_mainWindow, "updateLandingDataDisplay", Qt::QueuedConnection);
        }
        wasOnGround = onGround;
    });
    
}

void flightTrackingPage::displayPastRoute(QVector<QPair<double,double>> points){
    tracking* trackingWindow = g_mainWindow;
    
    if(trackingWindow->trackingTimer->isActive())
        trackingWindow->trackingTimer->stop();

    trackingWindow->pathProvider->setPoints(points);
}

void flightTrackingPage::returnToSavedPosition(QString saveName){

}