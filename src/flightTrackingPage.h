#pragma once
#include <QObject>

class tracking;

class flightTrackingPage : public QObject{
    Q_OBJECT
public:
    static void startTracking(tracking*);
    static void displayPastRoute(QVector<QPair<double,double>>);
};