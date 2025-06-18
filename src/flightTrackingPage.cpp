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

void flightTrackingPage::startTracking(){
    bridgeToMSFS bridgeToMSFSInstance;
    for(int i = 0; !bridgeToMSFSInstance.ConnectToMSFS() && i < 5; i++){
        Sleep(5000);
    }
    bridgeToMSFSInstance.requestAircraftLocation(bridgeToMSFSInstance.hSimConnect);
    tracking* trackingWindow = new tracking();
    trackingWindow->show();

    QWebEngineView *view = new QWebEngineView(trackingWindow->ui->mapWidget);
    view->setUrl(QUrl("qrc:/src/map.html"));

    if(trackingWindow->ui->mapWidget->layout()){
        trackingWindow->ui->mapWidget->layout()->addWidget(view);
    }
    else{
        view->setGeometry(trackingWindow->ui->mapWidget->rect());
        view->show();
    }

    QWebChannel *channel = new QWebChannel(view->page());
    PathProvider *pathProvider = new PathProvider;
    channel->registerObject("pathProvider", pathProvider);
    view->page()->setWebChannel(channel);
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, [&](){SimConnect_CallDispatch(bridgeToMSFSInstance.hSimConnect, bridgeToMSFS::getAircraftLocation, nullptr);
        
        pathProvider->setPoints(bridgeToMSFSInstance.aircraftPts);
    });
    timer.start(2000);
}