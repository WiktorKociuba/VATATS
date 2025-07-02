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
    static QVector<double> altitudeData;
    struct AircraftPosition{
        double latitude;
        double longitude;
        double altitude;
    };
    struct LandingForce{
        double verticalSpeed;
        double gForce;
    };
    struct simData{
        double latitude;
        double longitude;
        double altitude;
        double verticalSpeed;
        double gForce;
        uint32_t touchdownState;
    };
    static AircraftPosition currentAircraftPosition;
    static LandingForce currentLandingForce;
    static simData currentSimData; 
    static void CALLBACK getAircraftLocation(SIMCONNECT_RECV* pData, DWORD, void*);
    void requestAircraftLocation(HANDLE hSimConnect);
    bool ConnectToMSFS();
    void closeConnection();
};