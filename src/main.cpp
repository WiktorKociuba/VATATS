#include <QApplication>
#include <QPushButton>
#include <windows.h>
#include <iostream>
#include <SimConnect.h>
#include <QLabel>
#include <QTimer>
#include <QWebChannel>
#include <QWebEngineView>
#include <QObject>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QVector>
#include <QVariantList>
#include "PathProvider.h"
#include <QWebEngineSettings>
#include <random>

HANDLE hSimConnect = nullptr;
QVector<QPair<double,double>> aircraftPts;

struct AircraftPosition{ //Struct to store position info of the aircraft
    double latitude;
    double longitude;
    double altitude;
};

AircraftPosition currentAircraftPosition;

//Callback to receive requested position data from Simconnect
//It translates the received data, so that it can extract the position info
//Also checks if the id matches the id of the request
void CALLBACK getAircraftLocation(SIMCONNECT_RECV* pData, DWORD, void*){
    if(pData->dwID == SIMCONNECT_RECV_ID_SIMOBJECT_DATA){
        SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;
        if(pObjData->dwRequestID == 0){
            currentAircraftPosition = *(AircraftPosition*)&pObjData->dwData;
            aircraftPts.push_back({currentAircraftPosition.latitude,currentAircraftPosition.longitude});
        }
    }
}

void requestAircraftLocation(HANDLE hSimConnect){
    HRESULT hr;
    //Add the msfs variables to a client object definition
    hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "PLANE LATITUDE", "degrees");
    hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "PLANE LONGITUDE", "degrees");
    hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "PLANE ALTITUDE", "feet");
    //Request the data
    hr = SimConnect_RequestDataOnSimObject(hSimConnect, 0, 0, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND);
}

//Connect to the simulator with SimConnect
bool ConnectToMSFS(){
    if(SUCCEEDED(SimConnect_Open(&hSimConnect, "VATATS", nullptr, 0, 0, 0))){
        std::cout << "connected" << std::endl;
        return true;
    }
    else{
        std::cout << "failed" << std::endl;
        return false;
    }
}

int main(int argc, char** argv){
    QApplication app(argc,argv);
    //Retry the connection
    for(int i = 0; !ConnectToMSFS() && i < 5; i++){
        Sleep(5000);
    }
    requestAircraftLocation(hSimConnect);

    QMainWindow mainWindow;
    QWebEngineView* view = new QWebEngineView;
    
    QWebChannel * channel = new QWebChannel(view->page());
    PathProvider* pathProvider = new PathProvider;
    channel->registerObject("pathProvider", pathProvider);
    view->page()->setWebChannel(channel);
    
    view->load(QUrl("qrc:/src/map.html"));
    view->settings()->setAttribute(static_cast<QWebEngineSettings::WebAttribute>(14), true);
    mainWindow.setCentralWidget(view);
    mainWindow.show();
    aircraftPts.push_back({51.0,16.0});
    QTimer timer;
    //Request the data every 1000ms and then update the GUI
    QObject::connect(&timer, &QTimer::timeout, [&](){
        SimConnect_CallDispatch(hSimConnect, getAircraftLocation, nullptr);
        pathProvider->setPoints(aircraftPts);
    });
    timer.start(2000);
    return app.exec();
}