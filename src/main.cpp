#include <QApplication>
#include <QRandomGenerator>
#include "globals.h"
#include "cpdlc.h"
#include "readPoints.h"
#include "chartfox.h"

tracking* g_mainWindow = nullptr;
bridgeToMSFS* g_bridgeToMSFSInstance = nullptr;
QVector<int> pointTimes;
QString g_callsign, g_hoppieSecret, g_vatsimCID;
int messageId;
QVector<cpdlc::hoppieMessage> g_messages;
cpdlc* myCpdlc = new cpdlc(g_mainWindow);
QString g_currentStation, g_chartfoxToken;
QVector<chartfox::chartData> g_currentCharts;

int main(int argc, char** argv){
    QApplication app(argc,argv);
    pointTimes = {};
    g_currentCharts = {};
    g_messages = {};
    ifConnected = false;
    g_mainWindow = new tracking();
    g_bridgeToMSFSInstance = new bridgeToMSFS();
    g_callsign = "";
    g_hoppieSecret = "";
    g_vatsimCID = "";
    g_currentStation = "";
    g_chartfoxToken = "";
    messageId = QRandomGenerator::global()->bounded(1,9000);
    g_mainWindow->show();
    chartfox* myChartfox = new chartfox();
    myChartfox->authorizeChartfox();
    readPoints::getHoppieVatsim();
    return app.exec();
}