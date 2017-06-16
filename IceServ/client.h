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

typedef enum _IC_SERVER_COMMAND
{
    IcServerCommand_Error               = 0x0,

    IcServerCommand_Ping                = 0x1,

    IcServerCommand_GetAppCtrlStatus    = 0x2,
    IcServerCommand_GetFSScanStatus     = 0x4,

    IcServerCommand_SetAppCtrlStatus    = 0x8,
    IcServerCommand_SetFSScanStatus     = 0x10,

    IcServerCommand_GetAppCtrlEvents    = 0x20,
    IcServerCommand_GetAppCtrlRules     = 0x40,
    IcServerCommand_AddAppCtrlRule      = 0x80,
    IcServerCommand_UpdateAppCtrlRule   = 0x100,
    IcServerCommand_DeleteAppCtrlRule   = 0x200,

    IcServerCommand_GetFSScanEvents     = 0x400,
    IcServerCommand_GetFSScanRules      = 0x800,
    IcServerCommand_AddFSScanRule       = 0x1000,
    IcServerCommand_UpdateFSScanRule    = 0x2000,
    IcServerCommand_DeleteFSScanRule    = 0x4000,
    
    IcServerCommand_SetOption           = 0x8000,

} IC_SERVER_COMMAND, *PIC_SERVER_COMMAND;


typedef enum _IC_SERVER_COMMAND_RESULT
{
    IcServerCommandResult_Error = 0x0,
    IcServerCommandResult_Success = 0x1

} IC_SERVER_COMMAND_RESULT, *PIC_SERVER_COMMAND_RESULT;


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


_Success_(return != IcServerCommand_Error)
IC_SERVER_COMMAND
GetCommandFromServer(
    VOID
);
