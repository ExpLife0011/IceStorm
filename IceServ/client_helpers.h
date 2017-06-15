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
ClSendString(
    _In_opt_z_  PWCHAR          PString
);

_Success_(return == 0)
DWORD
ClSendDWORD(
    _In_        DWORD           DwValue
);

_Success_(return == 0)
DWORD
ClRecvMessage(
    _Inout_     PBYTE           PBuffer,
    _In_        DWORD           DwMexBufferSize
);

_Success_(return == 0)
DWORD
ClGetNextMessageSize(
    _Inout_     DWORD           *PDwNextSize
);

_Success_(return == ERROR_SUCCESS)
DWORD
ClRecvMessageWithoutSize(
    _Inout_bytecap_(DwBufferSize)   PBYTE           PBuffer,
    _In_                            DWORD           DwBufferSize
);

_Success_(return == ERROR_SUCCESS)
DWORD
ClRecvDWORD(
    _Inout_     DWORD           *PDwBufferSize
);
