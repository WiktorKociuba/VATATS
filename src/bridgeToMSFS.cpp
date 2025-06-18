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


HANDLE bridgeToMSFS::hSimConnect = nullptr;
QVector<QPair<double,double>> bridgeToMSFS::aircraftPts;
QString bridgeToMSFS::saveName = QString("save%1.sqlite").arg(QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));

bridgeToMSFS::AircraftPosition bridgeToMSFS::currentAircraftPosition;

//Callback to receive requested position data from Simconnect
//It translates the received data, so that it can extract the position info
//Also checks if the id matches the id of the request
void CALLBACK bridgeToMSFS::getAircraftLocation(SIMCONNECT_RECV* pData, DWORD, void*){
    savePoints savePointsInstance;
    if(pData->dwID == SIMCONNECT_RECV_ID_SIMOBJECT_DATA){
        SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;
        if(pObjData->dwRequestID == 0){
            currentAircraftPosition = *(AircraftPosition*)&pObjData->dwData;
            aircraftPts.push_back({currentAircraftPosition.latitude,currentAircraftPosition.longitude});
            savePointsInstance.sendPointsToSQL(saveName, currentAircraftPosition.latitude, currentAircraftPosition.longitude, currentAircraftPosition.altitude);
        }
    }
}

void bridgeToMSFS::requestAircraftLocation(HANDLE hSimConnect){
    HRESULT hr;
    //Add the msfs variables to a client object definition
    hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "PLANE LATITUDE", "degrees");
    hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "PLANE LONGITUDE", "degrees");
    hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "PLANE ALTITUDE", "feet");
    //Request the data
    hr = SimConnect_RequestDataOnSimObject(hSimConnect, 0, 0, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND);
}

//Connect to the simulator with SimConnect
bool bridgeToMSFS::ConnectToMSFS(){
    if(SUCCEEDED(SimConnect_Open(&hSimConnect, "VATATS", nullptr, 0, 0, 0))){
        std::cout << "connected" << std::endl;
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "saveindex_connection");
            db.setDatabaseName("saves.sqlite");
            if(!db.open()){
                qWarning() << "Cannot open database: " << db.lastError().text();
            }
            QSqlQuery query(db);
            if(!query.exec("CREATE TABLE IF NOT EXISTS saves (id INTEGER PRIMARY KEY AUTOINCREMENT, saveName TEXT)")){
                qWarning() << "Create table failed: " << query.lastError().text();
            }
            query.prepare("INSERT INTO saves (saveName) VALUES (?)");
            query.addBindValue(saveName);
            if(!query.exec()){
                qWarning() << "Insert failed: " << query.lastError().text();
            }
            db.close();
        }
        QSqlDatabase::removeDatabase("saveindex_connection");
        return true;
    }
    else{
        std::cout << "failed" << std::endl;
        return false;
    }
}