#pragma once

#include "global_data.h"
#include "icecommon.h"

_Success_(return == ERROR_SUCCESS)
DWORD
InitFSScanRules(
    VOID
);

_Success_(return == ERROR_SUCCESS)
DWORD
UninitFSScanRules(
    VOID
);

_Success_(return == ERROR_SUCCESS)
DWORD
GetFSScanResult(
    _In_    ULONG                           UlOriginalFlags,
    _In_    IC_FS_RULE                     *PRule,
    _Inout_ ICE_FS_SCAN_RESULT_PACKET      *PResultPack
);
