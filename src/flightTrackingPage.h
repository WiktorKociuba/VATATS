#pragma once
#include <QObject>
#include <QElapsedTimer>

class tracking;

class flightTrackingPage : public QObject{
    Q_OBJECT
public:
    static void startTracking(tracking*);
    static void displayPastRoute(QVector<QPair<double,double>>);
    static void returnToSavedPosition(QString saveName);
};