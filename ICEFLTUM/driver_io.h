#pragma once

#include "global_data.h"
#include "icecommon.h"

_Success_(ERROR_SUCCESS == return)
DWORD
InitConnectionToIceFlt(
    VOID
);

_Success_(ERROR_SUCCESS == return)
DWORD
UninitConnectionToIceFlt(
    VOID
);

_Success_(ERROR_SUCCESS == return)
DWORD
SendSetOption(
    _In_          DWORD                       DwOption,
    _In_z_        DWORD                       DwValue
);

_Success_(STATUS_SUCCESS == return)
DWORD
ReplyScanMessage(
    _In_    HANDLE                          HPort,
    _In_    PBYTE                           PReadBuffer,
    _In_    PICE_APP_CTRL_SCAN_RESULT_PACKET    PResultPack
);

_Success_(STATUS_SUCCESS == return)
DWORD
ReadScanMessage(
    _In_    PBYTE                           PBuffer,
    _Inout_ DWORD                          *PDwWritten,
    _In_    HANDLE                          HPort,
    _In_    HANDLE                          HStopEvent
);
