#ifndef TRACKING_H
#define TRACKING_H

#include <QWidget>
#include <QWebEngineView>
#include <QWebChannel>
#include <QTimer>
#include <QListWidgetItem>
#include "src/PathProvider.h"
#include <QtCharts/QChartView>
#include <QTreeWidgetItem>
#include <QPdfView>
#include <QPdfDocument>

namespace Ui {
class tracking;
}

class tracking : public QWidget
{
    Q_OBJECT

public:
    explicit tracking(QWidget *parent = nullptr);
    ~tracking();
    Ui::tracking *ui;
    QWebEngineView* mapView = nullptr;
    QWebChannel* webChannel = nullptr;
    PathProvider* pathProvider = nullptr;
    QTimer* trackingTimer = nullptr;
    void populateSaveDD();
    QChartView* heightChartView = nullptr;
    void showHeightProfile(const QVector<double>& heightData);
    void displayDuration(int duration);
    void updateCallsignLabel(const QString& callsign);
    void connectedHoppie();
    void disconnectedHoppie();
    void updateMessageList();
    void updateCurrentStation();
    void logonConfirmed();
    void disableChartfoxAuthorize();
    QPdfView* pdfView = nullptr;
    QPdfDocument* pdfDoc = nullptr;
public slots:
    void updateLandingDataDisplay();
    void logoffATC();
private slots:
    void onFlightTrackingClicked();  
    void onChartsClicked();
    void onCPDLCClicked();
    void onSimbriefClicked();
    void onStartTrackingClicked();
    void onShowLastClicked();  
    void onStopTrackingClicked();
    void onClearMapClicked();
    void onSaveDDChanged(const QString& saveName);
    void onSettingsClicked();
    void onSaveSimConfClicked();
    void onRequestsClicked();
    void onPredepClicked();
    void onHoppieConnectClicked();
    void onVatsimCIDClicked();
    void onHoppieSecretClicked();
    void onHoppieDisconnectClicked();
    void onMessageClicked(QListWidgetItem* item);
    void logonATC();
    void onTelexClicked();
    void onInforeqClicked();
    void onChartsSearchClicked();
    void onChartfoxAuthorizeClicked();
    void onChartItemClicked(QTreeWidgetItem* item, int column);
signals:
    void landingDataUpdated();
};

#endif // TRACKING_H
