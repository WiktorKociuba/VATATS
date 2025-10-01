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
#include <QTextStream>
#include <QMessageBox>
#include <QFile>
#include "src/cpdlcrequests.h"
#include "src/predepgui.h"
#include "src/cpdlc.h"
#include "src/messageresponse.h"
#include "src/savePoints.h"
#include "src/logon.h"
#include "src/telexdialog.h"
#include "src/inforeqdialog.h"
#include "src/chartfox.h"
#include <QTreeWidgetItem>
#include <QMap>
#include "src/wordwrapdelegate.h"
#include <QPdfView>
#include <QPdfDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTemporaryFile>
#include "src/zoomablePdfView.h"


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
    vatsimView = new QWebEngineView(ui->vatsimWidget);
    vatsimView->setUrl(QUrl("qrc:/src/vatsimMap.html"));
    if(ui->vatsimWidget->layout()){
        ui->vatsimWidget->layout()->addWidget(vatsimView);
    }
    else{
        vatsimView->setGeometry(ui->vatsimWidget->rect());
        vatsimView->show();
    }
    webChannel = new QWebChannel(mapView->page());
    pathProvider = new PathProvider;
    webChannel->registerObject("pathProvider", pathProvider);
    mapView->page()->setWebChannel(webChannel);
    vatsimChannel = new QWebChannel(vatsimView->page());
    vatsimProvider = new PathProvider;
    vatsimChannel->registerObject("pathProvider", vatsimProvider);
    vatsimView->page()->setWebChannel(vatsimChannel);

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
    connect(ui->CPDLCPB, &QPushButton::pressed, this, &tracking::onCPDLCClicked);
    connect(ui->settings, &QPushButton::clicked, this, &tracking::onSettingsClicked);
    connect(ui->vatsimPB, &QPushButton::clicked, this, &tracking::onVatsimClicked);

    //tracking
    connect(ui->startTrackingPB, &QPushButton::clicked, this, &tracking::onStartTrackingClicked);
    connect(ui->showLastPB, &QPushButton::clicked, this, &tracking::onShowLastClicked);
    connect(ui->stopTrackingPB, &QPushButton::clicked, this, &tracking::onStopTrackingClicked);
    connect(ui->clearMapPB, &QPushButton::clicked, this, &tracking::onClearMapClicked);
    connect(ui->chooseSaveDD, &QComboBox::currentTextChanged, this, &tracking::onSaveDDChanged);
    connect(this, &tracking::landingDataUpdated, this, &tracking::updateLandingDataDisplay);

    //charts
    connect(ui->searchChartsPB, &QPushButton::clicked, this, &tracking::onChartsSearchClicked);
    connect(ui->chartsList, &QTreeWidget::itemClicked, this, &tracking::onChartItemClicked);
    ui->chartsList->setColumnWidth(0,200);
    ui->chartsList->setItemDelegate(new WordWrapDelegate(ui->chartsList));
    ui->chartsList->header()->hide();
    pdfView = new ZoomablePdfView(ui->chartView);
    pdfDoc = new QPdfDocument(this);
    QVBoxLayout* pdfLayout = new QVBoxLayout(ui->chartView);
    pdfLayout->setContentsMargins(0,0,0,0);
    pdfLayout->addWidget(pdfView);
    pdfView->setDocument(pdfDoc);

    //cpdlc
    connect(ui->reqPB, &QPushButton::clicked, this, &tracking::onRequestsClicked);
    connect(ui->predepPB, &QPushButton::clicked, this, &tracking::onPredepClicked);
    connect(ui->connectHoppie, &QPushButton::clicked, this, &tracking::onHoppieConnectClicked);
    connect(ui->CIDsave, &QPushButton::clicked, this, &tracking::onVatsimCIDClicked);
    connect(ui->saveHoppie, &QPushButton::clicked, this, &tracking::onHoppieSecretClicked);
    connect(ui->disconnectHoppie, &QPushButton::clicked, this, &tracking::onHoppieDisconnectClicked);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &tracking::onMessageClicked);
    connect(ui->logonPB, &QPushButton::clicked, this, &tracking::logonATC);
    connect(ui->logoffPB, &QPushButton::clicked, this, &tracking::logoffATC);
    connect(ui->telexPB, &QPushButton::clicked, this, &tracking::onTelexClicked);
    connect(ui->inforeqPB, &QPushButton::clicked, this, &tracking::onInforeqClicked);

    //settings
    connect(ui->saveSimConf, &QPushButton::clicked, this, &tracking::onSaveSimConfClicked);
    connect(ui->chartfoxAuthorizePB, &QPushButton::clicked, this, &tracking::onChartfoxAuthorizeClicked);

    QTimer::singleShot(200,this,[this](){
        QResizeEvent* event = new QResizeEvent(this->size(), this->size());
        this->resizeEvent(event);
        delete event;
    });
    tracking::populateSaveDD();
}

tracking::~tracking()
{
    delete ui;
}

void tracking::onFlightTrackingClicked(){
    ui->stackedWidget->setCurrentIndex(0);
    tracking::populateSaveDD();
    QTimer::singleShot(200, this, [this](){
        QResizeEvent* event = new QResizeEvent(this->size(), this->size());
        this->resizeEvent(event);
        delete event;
    });
}

void tracking::onChartsClicked(){
    ui->stackedWidget->setCurrentIndex(1);
}

void tracking::onCPDLCClicked(){
    ui->stackedWidget->setCurrentIndex(2);
}

void tracking::onSimbriefClicked(){
    ui->stackedWidget->setCurrentIndex(3);
}

void tracking::onVatsimClicked(){
    ui->stackedWidget->setCurrentIndex(4);
    vatsimMap* updateVatsimMap = new vatsimMap();
    QTimer* vatsimUpdate = new QTimer();
    vatsimUpdate->setInterval(20000);
    connect(vatsimUpdate, &QTimer::timeout, [updateVatsimMap](){
        updateVatsimMap->requestData();
    });
    updateVatsimMap->requestData();
    vatsimUpdate->start();
    QTimer::singleShot(200,this,[this](){
        QResizeEvent* event = new QResizeEvent(this->size(), this->size());
        this->resizeEvent(event);
        delete event;
    });
}

void tracking::onSettingsClicked(){
    ui->stackedWidget->setCurrentIndex(5);
}

void tracking::onShowLastClicked(){
    QVector<std::tuple<double,double,double>> points = readPoints::readPointsDB(saveNameChosen);
    QVector<QPair<double,double>> result;
    QVector<double> altitude;
    for(int i = 0; i < points.size(); i++){
        result.push_back({std::get<0>(points[i]),std::get<1>(points[i])});
        altitude.push_back(std::get<2>(points[i]));
    }
    mapView->page()->runJavaScript("resetMapCentering();");
    flightTrackingPage::displayPastRoute(result);
    tracking::showHeightProfile(altitude);
}

void tracking::onStartTrackingClicked(){
    ui->startTrackingPB->setEnabled(false);
    ui->stopTrackingPB->setEnabled(true);
    ui->showLastPB->setEnabled(false);
    initialized = false;
    wasOnGround = TRUE;
    mapView->page()->runJavaScript("resetMapCentering();");
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

void tracking::onRequestsClicked(){
    cpdlcrequests dlg(this);
    dlg.exec();
}

void tracking::onPredepClicked(){
    predepgui dlg(this);
    dlg.exec();
}

void tracking::onHoppieConnectClicked(){
    cpdlc::connectToNetwork();
}

void tracking::onHoppieDisconnectClicked(){
    g_callsign = "";
    ui->callsignLabel->setText("Callsign:");
    tracking::disconnectedHoppie();
    myCpdlc->stopPolling();
}

void tracking::onClearMapClicked(){
    mapView->page()->runJavaScript("resetMapCentering();");
    g_mainWindow->pathProvider->setPoints({});
    tracking::showHeightProfile({});
    ui->landingSpeedLabel->setText("");
}

void tracking::onMessageClicked(QListWidgetItem* item){
    int row = ui->listWidget->row(item);
    if(row >= 0 && row < g_messages.size()){
        cpdlc::hoppieMessage msg = g_messages[row];
        messageResponse dlg(this);
        dlg.setupWindow(msg);
        dlg.exec();
    }
}

void tracking::onTelexClicked(){
    telexdialog dlg(this);
    dlg.exec();
}

void tracking::onInforeqClicked(){
    inforeqdialog dlg(this);
    dlg.exec();
}

void tracking::onChartsSearchClicked(){
    chartfox* chartfoxSearch = new chartfox();
    QString icao = ui->icaoChartsLine->text();
    connect(chartfoxSearch, &chartfox::updateCharts, [this](){
        ui->chartsList->clear();
        QMap<QString, QTreeWidgetItem*> listContent;
        for(const auto& chart : g_currentCharts){
            if(!listContent.contains(chart.type)){
                QTreeWidgetItem* category = new QTreeWidgetItem(ui->chartsList);
                category->setText(0, chart.type);
                listContent[chart.type] = category;
            }
            QTreeWidgetItem* chartItem = new QTreeWidgetItem();
            chartItem->setText(0, chart.name);
            chartItem->setData(0, Qt::UserRole, chart.source);
            chartItem->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
            chartItem->setFlags(chartItem->flags());
            listContent[chart.type]->addChild(chartItem);
        }
        ui->chartsList->expandAll();
        ui->chartsList->viewport()->update();
        ui->chartsList->updateGeometry();
    });
    chartfoxSearch->getCharts(icao);
}

void tracking::onChartfoxAuthorizeClicked(){
    chartfox* chartfoxAuthorize = new chartfox();
    connect(chartfoxAuthorize, &chartfox::chartfoxAuthorized, [this](){
        ui->chartfoxAuthorizePB->setEnabled(false);
    });
    chartfoxAuthorize->authorizeChartfox();
}

void tracking::onChartItemClicked(QTreeWidgetItem* item, int column){
    if(item->parent() == nullptr)
        return;
    QString sourceUrl = item->data(0, Qt::UserRole).toString();
    if(sourceUrl.isEmpty()){
        QMessageBox::warning(this, "Chart Error", "No source URL available!");
        column = 0;
        return;
    }
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->get(QNetworkRequest(QUrl(sourceUrl)));
    connect(reply, &QNetworkReply::finished, this, [=](){
        if(reply->error() == QNetworkReply::NoError){
            QByteArray pdfData = reply->readAll();
            QTemporaryFile* temp = new QTemporaryFile(this);
            if(temp->open()){
                temp->write(pdfData);
                temp->flush();
                pdfDoc->load(temp->fileName());
                pdfView->setDocument(pdfDoc);
                pdfView->setZoomMode(QPdfView::ZoomMode::FitInView);
            }
            else{
                QMessageBox::warning(this, "Chart Error", "Failed to download PDF!");
            }
            reply->deleteLater();
        }
    });
}

//////////////////////////////////////////////

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

void tracking::onSaveSimConfClicked(){
    QString ip = ui->ipLineEdit->text().trimmed();
    QString port = ui->portLineEdit->text().trimmed();
    if(ip.isEmpty())
        ip = "0.0.0.0";
    if(port.isEmpty())
        port = "500";
    QString xml = QString(
        "<SimBase.Document Type=\"SimConnect\" version=\"1,0\">\n"
        "   <SimConnect.Comm>\n"
        "       <Comm>\n"
        "           <Protocol>IPv4</Protocol>\n"
        "           <Scope>global</Scope>\n"
        "           <Address>%1</Address>\n"
        "           <Port>%2</Port>\n"
        "           <MaxClients>64</MaxClients>\n"
        "           <MaxRecvSize>4096</MaxRecvSize>\n"
        "           <DisableNagle>False</DisableNagle>\n"
        "       </Comm>\n"
        "   </SimConnect.Comm>\n"
        "</SimBase.Document>\n"
    ).arg(ip,port);
    ui->SimConfOut->setPlainText(xml);
    ui->SimConfOut->setEnabled(true);
    QString cfg = QString(
        "[SimConnect]\n"
        "Protocol=IPv4\n"
        "Address=%1\n"
        "Port=%2\n"
    ).arg(ip,port);
    QFile file("SimConnect.cfg");
    if(QFile::exists("SimConnect.cfg")){
        QFile::remove("SimConnect.cfg");
    }
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&file);
        out << cfg;
        file.close();
    }
    else{
        QMessageBox::warning(this,"Error","Could not write SimConnect.cfg!");
    }
}

void tracking::updateCallsignLabel(const QString& callsign){
    ui->callsignLabel->setText("Callsign: " + callsign);
}

void tracking::connectedHoppie(){
    ui->connectHoppie->setEnabled(false);
    ui->disconnectHoppie->setEnabled(true);
    ui->logonPB->setEnabled(true);
    ui->telexPB->setEnabled(true);
    ui->inforeqPB->setEnabled(true);
    ui->logoffPB->setEnabled(false);
}

void tracking::disconnectedHoppie(){
    if(ui->logoffPB->isEnabled()){
        this->logoffATC();
    }
    ui->connectHoppie->setEnabled(true);
    ui->disconnectHoppie->setEnabled(false);
    ui->logoffPB->setEnabled(false);
    ui->logonPB->setEnabled(false);
    ui->inforeqPB->setEnabled(false);
    ui->telexPB->setEnabled(false);
}

void tracking::onVatsimCIDClicked(){
    g_vatsimCID = ui->vatsimCIDInput->text().trimmed();
    savePoints::saveVatsimCid();
}

void tracking::onHoppieSecretClicked(){
    g_hoppieSecret = ui->hoppieSecretInput->text().trimmed();
    savePoints::saveHoppieSecret();
}

void tracking::updateMessageList(){
    ui->listWidget->clear();
    for(const cpdlc::hoppieMessage& msg : g_messages){
        ui->listWidget->addItem(msg.packet);
    }
}

void tracking::logonATC(){
    logon dlg(this);
    dlg.exec();
}

void tracking::logonConfirmed(){
    ui->logonPB->setEnabled(false);
    ui->logoffPB->setEnabled(true);
    ui->reqPB->setEnabled(true);
}

void tracking::logoffATC(){
    ui->logonPB->setEnabled(true);
    ui->logoffPB->setEnabled(false);
    ui->reqPB->setEnabled(false);
    cpdlc* cpdlcLogoff = new cpdlc(g_mainWindow);
    QString packet = QString("/data2/%1//N/LOGOFF").arg(QString::number(messageId));
    messageId++;
    cpdlcLogoff->sendMessage(g_hoppieSecret,g_callsign,g_currentStation,"cpdlc",packet);
    g_currentStation = "";
    this->updateCurrentStation();
}

void tracking::updateCurrentStation(){
    ui->currentStationLabel->setText("Current station: " + g_currentStation);
}

void tracking::disableChartfoxAuthorize(){
    ui->chartfoxAuthorizePB->setEnabled(false);
}

void tracking::updateVatsimMap(){
    QVariantList planes = vatsimProvider->getVatsimPlanes();
    QJsonDocument doc = QJsonDocument::fromVariant(planes);
    QString js = QString("showVatsimPlanes(%1);").arg(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
    vatsimView->page()->runJavaScript(js);
}

void tracking::resizeEvent(QResizeEvent* event){
    QWidget::resizeEvent(event);
    if(mapView){
        QWidget* pageWidget = ui->stackedWidget->widget(0);
        if (pageWidget && pageWidget->layout()) {
            QSize availableSize = pageWidget->size();

            int mapHeight = availableSize.height() - 50 - 171 - 50;
            int mapWidth = availableSize.width() - 20;

            mapView->setMinimumSize(mapWidth, mapHeight);
            mapView->resize(mapWidth, mapHeight);
            mapView->updateGeometry();

            QString resizeScript = QString(
                "document.getElementById('map').style.width = '%1px';"
                "document.getElementById('map').style.height = '%2px';"
                "if (typeof map !== 'undefined') { map.invalidateSize(true); }"
            ).arg(mapWidth).arg(mapHeight);
            
            mapView->page()->runJavaScript(resizeScript);
        }
    }
    if(vatsimView){
        QWidget* vatsimPageWidget = ui->stackedWidget->widget(4);
        if (vatsimPageWidget && vatsimPageWidget->layout()) {
            QSize availableSize = vatsimPageWidget->size();

            int mapHeight = availableSize.height() - 20;
            int mapWidth = availableSize.width() - 20;

            vatsimView->setMinimumSize(mapWidth, mapHeight);
            vatsimView->resize(mapWidth, mapHeight);
            vatsimView->updateGeometry();

            QString resizeScript = QString(
                "document.getElementById('map').style.width = '%1px';"
                "document.getElementById('map').style.height = '%2px';"
                "if (typeof map !== 'undefined') { map.invalidateSize(true); }"
            ).arg(mapWidth).arg(mapHeight);
            
            vatsimView->page()->runJavaScript(resizeScript);
        }
    }
}