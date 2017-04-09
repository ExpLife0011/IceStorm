#pragma once

#include "global_data.h"
#include "icecommon.h"

_Success_(ERROR_SUCCESS == return)
DWORD
StartAppCtrlScan(
    VOID
);

_Success_(ERROR_SUCCESS == return)
DWORD
StopAppCtrlScan(
    VOID
);
