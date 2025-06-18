#pragma once
#include <QObject>

class flightTrackingPage : public QObject{
    Q_OBJECT
public:
    static void startTracking();
};