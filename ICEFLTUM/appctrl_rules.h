#pragma once

#include "global_data.h"
#include "icecommon.h"

_Success_(return == ERROR_SUCCESS)
DWORD
InitAppCtrlRules(
    VOID
);

_Success_(return == ERROR_SUCCESS)
DWORD
UninitAppCtrlRules(
    VOID
);

_Success_(return == ERROR_SUCCESS)
DWORD
GetAppCtrlScanResult(
    _In_    PICE_APP_CTRL_SCAN_REQUEST_PACKET       PScanRequest,
    _Inout_ PICE_APP_CTRL_SCAN_RESULT_PACKET        PResultPack
);

_Success_(ERROR_SUCCESS == return)
DWORD
AddAppCtrlDenyRule(
    _In_z_      PWCHAR                      PFilePath,
    _In_        DWORD                       DwPid,
    _Inout_opt_ DWORD                      *PDwRuleId
);

_Success_(ERROR_SUCCESS == return)
DWORD
AddAppCtrlAllowRule(
    _In_z_      PWCHAR                      PFilePath,
    _In_        DWORD                       DwPid,
    _Inout_opt_ DWORD                      *PDwRuleId
);

_Success_(ERROR_SUCCESS == return)
DWORD
DeleteAppCtrlDenyRule(
    _In_        DWORD                       DwRuleId
);

_Success_(ERROR_SUCCESS == return)
DWORD
DeleteAppCtrlAllowRule(
    _In_        DWORD                       DwRuleId
);

_Success_(ERROR_SUCCESS == return)
DWORD
UpdateAppCtrlDenyRule(
    _In_        DWORD                       DwRuleId,
    _In_opt_z_  PWCHAR                      PFilePath,
    _In_opt_    DWORD                       DwPid
);

_Success_(ERROR_SUCCESS == return)
DWORD
UpdateAppCtrlAllowRule(
    _In_        DWORD                       DwRuleId,
    _In_opt_z_  PWCHAR                      PFilePath,
    _In_opt_    DWORD                       DwPid
);

_Success_(ERROR_SUCCESS == return)
DWORD
GetAppCtrlRules(
    _In_        BOOLEAN                     BGetAllowRules,
    _In_        BOOLEAN                     BGetDenyRules,
    _Inout_     PIC_APPCTRL_RULE           *PPRules,
    _Inout_     DWORD                      *PDwLength
);

_Success_(ERROR_SUCCESS == return)
VOID
FreeAppCtrlRulesList(
    _Inout_     PIC_APPCTRL_RULE           PRules,
    _In_        DWORD                      DwLength
);
