#pragma once
#define NOMINMAX
#include "tracking.h"
#include <windows.h>

extern tracking* g_mainWindow;
static BOOL wasOnGround;
static bool initialized;
static bool ifConnected;