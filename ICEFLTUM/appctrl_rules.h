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
