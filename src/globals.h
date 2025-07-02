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