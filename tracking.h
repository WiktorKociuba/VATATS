#ifndef TRACKING_H
#define TRACKING_H

#include <QWidget>
#include <QWebEngineView>
#include <QWebChannel>
#include <QTimer>
#include "src/PathProvider.h"
#include <QtCharts/QChartView>

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
public slots:
    void updateLandingDataDisplay();
private slots:
    void onFlightTrackingClicked();  
    void onChartsClicked();
    void onFlightradarClicked();
    void onSimbriefClicked();
    void onStartTrackingClicked();
    void onShowLastClicked();  
    void onStopTrackingClicked();
    void onClearMapClicked();
    void onSaveDDChanged(const QString& saveName);
    void onSettingsClicked();
    void onSaveSimConfClicked();
signals:
    void landingDataUpdated();
};

#endif // TRACKING_H
