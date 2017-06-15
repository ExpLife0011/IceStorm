#pragma once

#include <windows.h>

#define MAX_PROCESSOR_NAME  50
#define MAX_OS_NAME         75

typedef struct _IC_MACHINE_INFO
{
    WCHAR   PMachineName[MAX_COMPUTERNAME_LENGTH + 2];
    WCHAR   PArchitecture[MAX_PROCESSOR_NAME];
    WCHAR   POS[MAX_OS_NAME];
    DWORD   DwNrOfProcessors;

} IC_MACHINE_INFO, *PIC_MACHINE_INFO;

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

VOID
SendMachineInfo(
    _In_ IC_MACHINE_INFO *PMachineInfo
);