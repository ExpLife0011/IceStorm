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
    _In_        BOOLEAN                     BPersistent
);

_Success_(ERROR_SUCCESS == return)
DWORD
GetAppCtrlStatus(
    _Out_       BOOLEAN                    *PBEnabled
);
