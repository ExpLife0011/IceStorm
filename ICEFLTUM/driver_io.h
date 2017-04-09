#pragma once


#include "global_data.h"
#include "icecommon.h"

_Success_(STATUS_SUCCESS == return)
DWORD
IcReplyScanMessage(
    _In_    HANDLE                          HPort,
    _In_    PBYTE                           PReadBuffer,
    _In_    PICE_APP_CTRL_SCAN_RESULT_PACKET    PResultPack
);

_Success_(STATUS_SUCCESS == return)
DWORD
IcReadScanMessage(
    _In_    PBYTE                           PBuffer,
    _Inout_ DWORD                          *PDwWritten,
    _In_    HANDLE                          HPort,
    _In_    HANDLE                          HStopEvent
);
