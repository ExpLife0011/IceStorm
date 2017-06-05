#pragma once

#include <windows.h>

_Success_(return == 0)
DWORD
ClSendMessage(
    _In_        PBYTE           PBuffer,
    _In_        DWORD           DwBufferSize
);

_Success_(return == 0)
DWORD
ClRecvMessage(
    _Inout_     PBYTE           PBuffer,
    _In_        DWORD           DwBufferSize,
    _Out_opt_   DWORD          *PDwRecvSize
);
