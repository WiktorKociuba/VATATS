#pragma once
#define NOMINMAX
#include "tracking.h"
#include <windows.h>
#include "bridgeToMSFS.h"
#include "cpdlc.h"
#include "chartfox.h"
#include "vatsimMap.h"

extern tracking* g_mainWindow;
static BOOL wasOnGround;
static bool initialized;
static bool ifConnected;
extern bridgeToMSFS* g_bridgeToMSFSInstance;
extern QVector<int> pointTimes;
extern QString g_callsign;
extern QString g_hoppieSecret;
extern int messageId;
extern QString g_vatsimCID;
extern QVector<cpdlc::hoppieMessage> g_messages;
extern cpdlc* myCpdlc;
extern QString g_currentStation;
extern QString g_chartfoxToken;
extern QVector<chartfox::chartData> g_currentCharts;
extern QVector<vatsimMap::pilotData> g_currentPilotData;
extern QVector<vatsimMap::ATCData> g_currentATCData;
extern QVector<vatsimMap::atisData> g_currentAtisData;
extern QVector<vatsimMap::prefileData> g_currentPrefileData;
extern bool ifVatsim;
extern QVector<vatsimMap::extraData> g_facilities;
extern QVector<vatsimMap::extraData> g_ratings;
extern QVector<vatsimMap::extraData> g_pilotRatings;
extern QVector<vatsimMap::extraData> g_militaryRatings;