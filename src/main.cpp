#include <QApplication>
#include "globals.h"
#include "cpdlc.h"

tracking* g_mainWindow = nullptr;
bridgeToMSFS* g_bridgeToMSFSInstance = nullptr;
QVector<int> pointTimes;
QString g_callsign, g_hoppieSecret;

int main(int argc, char** argv){
    QApplication app(argc,argv);
    pointTimes = {};
    ifConnected = false;
    g_mainWindow = new tracking();
    g_bridgeToMSFSInstance = new bridgeToMSFS();
    g_callsign = "";
    g_hoppieSecret = "UdhHzxrRVR3uG6P6N";
    g_mainWindow->show();
    cpdlc* myCpdlc = new cpdlc(g_mainWindow);
    QObject::connect(myCpdlc, &cpdlc::vatsimCallsignFound, [myCpdlc](const QString& callsign){
        g_callsign = callsign;
        myCpdlc->testConnection(g_hoppieSecret, g_callsign);
    });
    myCpdlc->getVatsimCallsign("1778006");
    QObject::connect(myCpdlc, &cpdlc::testResult, [](const bool result){
        qDebug() << "ping: " << result;
    });
    
    return app.exec();
}