#pragma once

#include "global_data.h"
#include "icecommon.h"

_Success_(ERROR_SUCCESS == return)
DWORD
StartFSScan(
    VOID
);

_Success_(ERROR_SUCCESS == return)
DWORD
StopFSScan(
    VOID
);

_Success_(ERROR_SUCCESS == return)
DWORD
GetFSscanStatus(
    _Out_       BOOLEAN                    *PBEnabled
);
