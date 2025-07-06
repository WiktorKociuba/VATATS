#include <QApplication>
#include "globals.h"
#include "cpdlc.h"
#include "readPoints.h"

tracking* g_mainWindow = nullptr;
bridgeToMSFS* g_bridgeToMSFSInstance = nullptr;
QVector<int> pointTimes;
QString g_callsign, g_hoppieSecret, g_vatsimCID;
int messageId;
QVector<cpdlc::hoppieMessage> g_messages;
cpdlc* myCpdlc = new cpdlc(g_mainWindow);

int main(int argc, char** argv){
    QApplication app(argc,argv);
    pointTimes = {};
    g_messages = {};
    ifConnected = false;
    g_mainWindow = new tracking();
    g_bridgeToMSFSInstance = new bridgeToMSFS();
    g_callsign = "";
    g_hoppieSecret = "";
    g_vatsimCID = "";
    messageId = 1;
    g_mainWindow->show();
    readPoints::getHoppieVatsim();
    return app.exec();
}