#pragma once

#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS

#include "icecommon.h"
#include "..\Contrib\SQLite\Source\sqlite3.h"
#include "global_data.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

typedef DWORD SQL_ERROR;

extern sqlite3 *gPDB;

_Success_(return == ERROR_SUCCESS)
SQL_ERROR
DBUninit(
    VOID
);

_Success_(return == ERROR_SUCCESS)
SQL_ERROR
DBInit(
    VOID
);

_Success_(return == ERROR_SUCCESS)
DWORD
DbContainsAppCtrlDenyRule(
    _In_    PICE_APP_CTRL_SCAN_REQUEST_PACKET   PScanRequest,
    _Inout_ BOOLEAN                            *PBHasDenyRule,
    _Inout_ DWORD                              *PDwDenyTimestamp
);

_Success_(return == ERROR_SUCCESS)
DWORD
DbContainsAppCtrlAllowRule(
    _In_    PICE_APP_CTRL_SCAN_REQUEST_PACKET   PScanRequest,
    _Inout_ BOOLEAN                            *PBHasAllowRule,
    _Inout_ DWORD                              *PDwAllowTimestamp
);

_Success_(ERROR_SUCCESS == return)
DWORD
DbAddAppCtrlRule(
    _In_z_      PWCHAR                      PFilePath,
    _In_        DWORD                       DwPid,
    _In_        BOOLEAN                     BIsDenyRule,
    _Inout_opt_ DWORD                      *PDwRuleId
);

_Success_(ERROR_SUCCESS == return)
DWORD
DbDeleteAppCtrlRule(
    _In_        DWORD                       DwRuleId,
    _In_        BOOLEAN                     BIsDenyRule
);

_Success_(ERROR_SUCCESS == return)
DWORD
DbUpdateAppCtrlRule(
    _In_        DWORD                       DwRuleId,
    _In_z_      PWCHAR                      PFilePath,
    _In_        DWORD                       DwPid,
    _In_        BOOLEAN                     BIsDenyRule
);

_Success_(ERROR_SUCCESS == return)
DWORD
DbGetAppCtrlRules(
    _In_        BOOLEAN                     BGetAllowRules,
    _In_        BOOLEAN                     BGetDenyRules,
    _Inout_     PIC_APPCTRL_RULE           *PPRules,
    _Inout_     DWORD                      *PDwLength
);

_Success_(ERROR_SUCCESS == return)
VOID
DbFreeAppCtrlRulesList(
    _Inout_     PIC_APPCTRL_RULE           PRules,
    _In_        DWORD                      DwLength
);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
