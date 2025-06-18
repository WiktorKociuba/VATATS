#pragma once
#include <QObject>
#include <QVector>
#include <QPair>
#include <QString>
#include <windows.h>
#include <SimConnect.h>

class bridgeToMSFS : public QObject{
    Q_OBJECT
public:
    static HANDLE hSimConnect;
    static QVector<QPair<double,double>> aircraftPts;
    static QString saveName;
    struct AircraftPosition{
        double latitude;
        double longitude;
        double altitude;
    };
    static AircraftPosition currentAircraftPosition;
    static void CALLBACK getAircraftLocation(SIMCONNECT_RECV* pData, DWORD, void*);
    void requestAircraftLocation(HANDLE hSimConnect);
    bool ConnectToMSFS();
};