#pragma once
#define NOMINMAX
#include "tracking.h"
#include <windows.h>
#include "bridgeToMSFS.h"

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
extern QVector<QString> g_messages;