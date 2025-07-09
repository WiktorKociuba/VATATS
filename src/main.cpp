#include <QApplication>
#include <QRandomGenerator>
#include "globals.h"
#include "cpdlc.h"
#include "readPoints.h"
#include "chartfox.h"
#include "vatsimMap.h"

tracking* g_mainWindow = nullptr;
bridgeToMSFS* g_bridgeToMSFSInstance = nullptr;
QVector<int> pointTimes;
QString g_callsign, g_hoppieSecret, g_vatsimCID;
int messageId;
QVector<cpdlc::hoppieMessage> g_messages;
cpdlc* myCpdlc = new cpdlc(g_mainWindow);
QString g_currentStation, g_chartfoxToken;
QVector<chartfox::chartData> g_currentCharts;
QVector<vatsimMap::pilotData> g_currentPilotData;
QVector<vatsimMap::ATCData> g_currentATCData;
QVector<vatsimMap::atisData> g_currentAtisData;
QVector<vatsimMap::prefileData> g_currentPrefileData;
QVector<vatsimMap::extraData> g_facilities;
QVector<vatsimMap::extraData> g_ratings;
QVector<vatsimMap::extraData> g_pilotRatings;
QVector<vatsimMap::extraData> g_militaryRatings;
bool ifVatsim;

int main(int argc, char** argv){
    QApplication app(argc,argv);
    pointTimes = {};
    g_currentCharts = {};
    g_messages = {};
    g_currentPilotData = {};
    g_currentATCData = {};
    g_currentAtisData = {};
    g_currentPrefileData = {};
    g_facilities = {};
    g_ratings = {};
    g_pilotRatings = {};
    g_militaryRatings = {};
    ifConnected = false;
    ifVatsim = false;
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
    myChartfox->retrieveToken();
    readPoints::getHoppieVatsim();
    return app.exec();
}