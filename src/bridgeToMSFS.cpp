#define NOMINMAX
#include <QApplication>
#include <QWidget>
#include <QWebEngineView>
#include <QWebChannel>
#include <QUrl>
#include <windows.h>
#include <SimConnect.h>
#include <iostream>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include "ui_tracking.h"
#include "tracking.h"
#include "PathProvider.h"
#include "savePoints.h"
#include "bridgeToMSFS.h"
#include "globals.h"


HANDLE bridgeToMSFS::hSimConnect = nullptr;
QVector<QPair<double,double>> bridgeToMSFS::aircraftPts;
QVector<double> bridgeToMSFS::altitudeData;
QString bridgeToMSFS::saveName = nullptr;

bridgeToMSFS::AircraftPosition bridgeToMSFS::currentAircraftPosition;
bridgeToMSFS::LandingForce bridgeToMSFS::currentLandingForce;
bridgeToMSFS::simData bridgeToMSFS::currentSimData;
//Callback to receive requested position data from Simconnect
//It translates the received data, so that it can extract the position info
//Also checks if the id matches the id of the request
void CALLBACK bridgeToMSFS::getAircraftLocation(SIMCONNECT_RECV* pData, DWORD, void*){
    savePoints savePointsInstance;
    if(pData->dwID == SIMCONNECT_RECV_ID_SIMOBJECT_DATA){
        SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;
        if(pObjData->dwRequestID == 0){
            currentSimData = *(simData*)&pObjData->dwData;
            currentAircraftPosition.latitude = currentSimData.latitude;
            currentAircraftPosition.longitude = currentSimData.longitude;
            currentAircraftPosition.altitude = currentSimData.altitude;
            aircraftPts.push_back({currentAircraftPosition.latitude,currentAircraftPosition.longitude});
            savePointsInstance.sendPointsToSQL(saveName, currentAircraftPosition.latitude, currentAircraftPosition.longitude, currentAircraftPosition.altitude);
            altitudeData.push_back(currentAircraftPosition.altitude);
            
        }
    }
}

void bridgeToMSFS::requestAircraftLocation(HANDLE hSimConnect){
    HRESULT hr;
    //Add the msfs variables to a client object definition
    hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "PLANE LATITUDE", "degrees");
    hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "PLANE LONGITUDE", "degrees");
    hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "PLANE ALTITUDE", "feet");
    hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "VERTICAL SPEED", "feet per minute");
    hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "G FORCE", "GForce");
    hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "TOUCHDOWN STATE", "number");
    //Request the data
    hr = SimConnect_RequestDataOnSimObject(hSimConnect, 0, 0, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND);
}

//Connect to the simulator with SimConnect
bool bridgeToMSFS::ConnectToMSFS(){
    if(SUCCEEDED(SimConnect_Open(&hSimConnect, "VATATS", nullptr, 0, 0, 0))){
        std::cout << "connected" << std::endl;
        return true;
    }
    else{
        std::cout << "failed" << std::endl;
        return false;
    }
}

void bridgeToMSFS::closeConnection(){
    if(hSimConnect){
        SimConnect_Close(hSimConnect);
        hSimConnect = nullptr;
    }
}