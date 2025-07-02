#define NOMINMAX
#include "tracking.h"
#include "ui_tracking.h"
#include <QWebEngineView>
#include "src/flightTrackingPage.h"
#include "src/globals.h"
#include "src/readPoints.h"
#include "src/bridgeToMSFS.h"
#include <tuple>
#include <QLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

QString saveNameChosen = nullptr;

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
    webChannel->registerObject("pathProvider", pathProvider);
    mapView->page()->setWebChannel(webChannel);

    trackingTimer = new QTimer(this);

    //heightChart

    QChartView* chartView = new QChartView(new QChart(), ui->heightChart);
    chartView->setRenderHint(QPainter::Antialiasing);
    if(!ui->heightChart->layout()){
        QVBoxLayout* layout = new QVBoxLayout(ui->heightChart);
        layout->setContentsMargins(0,0,0,0);
        ui->heightChart->setLayout(layout);
    }
    ui->heightChart->layout()->addWidget(chartView);
    this->heightChartView = chartView;
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
    connect(ui->chooseSaveDD, &QComboBox::currentTextChanged, this, &tracking::onSaveDDChanged);
    connect(this, &tracking::landingDataUpdated, this, &tracking::updateLandingDataDisplay);
}

tracking::~tracking()
{
    delete ui;
}

void tracking::onFlightTrackingClicked(){
    ui->stackedWidget->setCurrentIndex(0);
    tracking::populateSaveDD();
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
    QVector<std::tuple<double,double,double>> points = readPoints::readPointsDB(saveNameChosen);
    QVector<QPair<double,double>> result;
    QVector<double> altitude;
    for(int i = 0; i < points.size(); i++){
        result.push_back({std::get<0>(points[i]),std::get<1>(points[i])});
        altitude.push_back(std::get<2>(points[i]));
    }
    flightTrackingPage::displayPastRoute(result);
    tracking::showHeightProfile(altitude);
}

void tracking::onStartTrackingClicked(){
    ui->startTrackingPB->setEnabled(false);
    ui->stopTrackingPB->setEnabled(true);
    ui->showLastPB->setEnabled(false);
    initialized = false;
    wasOnGround = TRUE;
    flightTrackingPage::startTracking(g_mainWindow);
}

void tracking::onStopTrackingClicked(){
    ui->stopTrackingPB->setEnabled(false);
    ui->showLastPB->setEnabled(true);
    ui->startTrackingPB->setEnabled(true);
    trackingTimer->stop();
    bridgeToMSFS::closeConnection();
    tracking::displayDuration(readPoints::getFlightTime());
    tracking::populateSaveDD();
}

void tracking::onClearMapClicked(){
    g_mainWindow->pathProvider->setPoints({});
    tracking::showHeightProfile({});
    ui->landingSpeedLabel->setText("");
}

void tracking::populateSaveDD(){
    ui->chooseSaveDD->clear();
    QVector<QString> saveNames = readPoints::getLastSaveName();
    for(int i = 0; i < saveNames.size(); i++){
        ui->chooseSaveDD->addItem(saveNames[i]);
    }
}

void tracking::onSaveDDChanged(const QString& saveName){
    saveNameChosen = saveName;
}
void tracking::showHeightProfile(const QVector<double>& heightData){
    QChart* chart = heightChartView->chart();
    chart->removeAllSeries();
    QLineSeries *series = new QLineSeries();
    int idx = 0;
    for(int i = 0; i < heightData.size(); i++){
        series->append(idx++,heightData[i]);
    }
    chart->addSeries(series);
    chart->createDefaultAxes();
    QAbstractAxis* axisX = chart->axes(Qt::Horizontal).first();
    axisX->setLabelsVisible(false);
    chart->axes(Qt::Vertical).first()->setTitleText("Altitude (ft)");
    chart->legend()->hide();
}

void tracking::updateLandingDataDisplay(){
    QString text = QString("Landing vertical speed: %1 ft/min, G-force: %2 G")
        .arg(bridgeToMSFS::currentLandingForce.verticalSpeed, 0, 'f', 2)
        .arg(bridgeToMSFS::currentLandingForce.gForce, 0, 'f', 2);
    ui->landingSpeedLabel->setText(text);
}

void tracking::displayDuration(int duration){
    QString text = QString("Time of the flight: %1:%2")
        .arg((duration/60)/60)
        .arg((duration/60)%60);
    ui->durationLabel->setText(text);
}