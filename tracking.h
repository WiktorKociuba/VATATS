#ifndef TRACKING_H
#define TRACKING_H

#include <QWidget>
#include <QWebEngineView>
#include <QWebChannel>
#include <QTimer>
#include "src/PathProvider.h"

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
private slots:
    void onFlightTrackingClicked();  
    void onChartsClicked();
    void onFlightradarClicked();
    void onSimbriefClicked();
    void onStartTrackingClicked();
    void onShowLastClicked();  
    void onStopTrackingClicked();
    void onClearMapClicked();
};

#endif // TRACKING_H
