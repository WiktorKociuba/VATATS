#include <QApplication>
#include "globals.h"
#include "cpdlc.h"

tracking* g_mainWindow = nullptr;
bridgeToMSFS* g_bridgeToMSFSInstance = nullptr;
QVector<int> pointTimes;
QString g_callsign, g_hoppieSecret, g_vatsimCID;
int messageId;
QVector<QString> g_messages;

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
    return app.exec();
}