#include "tracking.h"
#include "ui_tracking.h"
#include <QWebEngineView>
#include "src/flightTrackingPage.h"
#include "src/globals.h"
#include "src/readPoints.h"
#include <tuple>
#include <QLayout>

tracking::tracking(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::tracking)
{
    ui->setupUi(this);
    setWindowTitle("VATATS");
    mapView = new QWebEngineView(ui->mapWidget);
    mapView->setUrl(QUrl("qrc:/src/map.html"));
    if(ui->mapWidget->layout()){
        ui->mapWidget->layout()->addWidget(mapView);
    }
    else{
        mapView->setGeometry(ui->mapWidget->rect());
        mapView->show();
    }

    webChannel = new QWebChannel(mapView->page());
    pathProvider = new PathProvider;
    webChannel-> registerObject("pathProvider", pathProvider);
    mapView->page()->setWebChannel(webChannel);

    trackingTimer = new QTimer(this);

    //navbar
    connect(ui->flightTrackingPB, &QPushButton::clicked, this, &tracking::onFlightTrackingClicked);
    connect(ui->chartsPB, &QPushButton::clicked, this, &tracking::onChartsClicked);
    connect(ui->flightradarPB, &QPushButton::pressed, this, &tracking::onFlightradarClicked);
    connect(ui->simbriefPB, &QPushButton::clicked, this, &tracking::onSimbriefClicked);


    //page 1
    connect(ui->startTrackingPB, &QPushButton::clicked, this, &tracking::onStartTrackingClicked);
    connect(ui->showLastPB, &QPushButton::clicked, this, &tracking::onShowLastClicked);
    connect(ui->stopTrackingPB, &QPushButton::clicked, this, &tracking::onStopTrackingClicked);
    connect(ui->clearMapPB, &QPushButton::clicked, this, &tracking::onClearMapClicked);
}

tracking::~tracking()
{
    delete ui;
}

void tracking::onFlightTrackingClicked(){
    ui->stackedWidget->setCurrentIndex(0);
}

void tracking::onChartsClicked(){
    ui->stackedWidget->setCurrentIndex(1);
}

void tracking::onFlightradarClicked(){
    ui->stackedWidget->setCurrentIndex(2);
}

void tracking::onSimbriefClicked(){
    ui->stackedWidget->setCurrentIndex(3);
}

void tracking::onShowLastClicked(){
    QVector<std::tuple<double,double,double>> points = readPoints::readPointsDB(readPoints::getLastSaveName());
    QVector<QPair<double,double>> result;
    for(int i = 0; i < points.size(); i++){
        result.push_back({std::get<0>(points[i]),std::get<1>(points[i])});
    }
    flightTrackingPage::displayPastRoute(result);
}

void tracking::onStartTrackingClicked(){
    ui->startTrackingPB->setEnabled(false);
    ui->stopTrackingPB->setEnabled(true);
    flightTrackingPage::startTracking(g_mainWindow);
}

void tracking::onStopTrackingClicked(){
    ui->stopTrackingPB->setEnabled(false);
    ui->startTrackingPB->setEnabled(true);
    if(trackingTimer->isActive())
        trackingTimer->stop();
}

void tracking::onClearMapClicked(){
    g_mainWindow->pathProvider->setPoints({});
}