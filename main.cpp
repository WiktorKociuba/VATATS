#include <windows.h>
#include <SimConnect.h>
#include <iostream>

HANDLE hSimConnect = nullptr;

struct AircraftPosition{
    double latitude;
    double longitude;
    double altitude;
};

void getAircraftLocation(HANDLE hSimConnect){
    HRESULT hr;

    //Add the msfs variables to a client object definition
    hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "PLANE LATITUDE", "degrees");
    hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "PLANE LONGITUDE", "degrees");
    hr = SimConnect_AddToDataDefinition(hSimConnect, 0, "PLANE ALTITUDE", "feet");

    //Request the data
    hr = SimConnect_RequestDataOnSimObject(hSimConnect, 0, 0, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SECOND);

    SIMCONNECT_RECV* pData;
    DWORD cbData;

    //Receive the data until the connection is not closed every second (specified with the SIMCONNECT_PERIOD_SECOND)
    while(SUCCEEDED(SimConnect_CallDispatch(hSimConnect, [](SIMCONNECT_RECV* pData, DWORD cbData, void* pContext){
        if(pData->dwID == SIMCONNECT_RECV_ID_SIMOBJECT_DATA){
            SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;
            if(pObjData->dwRequestID == 0){
                AircraftPosition* pos = (AircraftPosition*)&pObjData->dwData;
                std::cout << "Latitude: " << pos->latitude << ", Longitude: " << pos->longitude << ", Altitude: " << pos->altitude << " feet" << std::endl;
            }
        }
    }, nullptr))){
        
    }
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

int main(){
    //Retry the connection
    while(!ConnectToMSFS()){
        Sleep(5000);
    }
    getAircraftLocation(hSimConnect);

    SimConnect_Close(hSimConnect);
    return 0;
}