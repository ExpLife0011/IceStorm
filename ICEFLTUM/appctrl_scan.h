#pragma once

#include "global_data.h"
#include "icecommon.h"

_Success_(ERROR_SUCCESS == return)
DWORD
SendSetOption(
    _In_          DWORD                       DwOption,
    _In_z_        DWORD                       DwValue
);

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
