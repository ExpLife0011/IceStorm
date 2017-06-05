#pragma once

#include <windows.h>

_Success_(return != FALSE)
BOOLEAN
StartClient(
    _In_z_  PCHAR           PIpAddr,
    _In_z_  PCHAR           PPort
);


VOID
StopClient(
    VOID
);