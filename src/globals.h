#pragma once
#define NOMINMAX
#include "tracking.h"
#include <windows.h>
#include "bridgeToMSFS.h"
#include "cpdlc.h"

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