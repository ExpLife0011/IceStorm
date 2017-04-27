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
DbGetAppCtrlVerdict(
    _In_    IC_APPCTRL_RULE                    *PRule,
    _Out_   ICE_SCAN_VERDICT                   *PVerdict
);

_Success_(ERROR_SUCCESS == return)
DWORD
DbAddAppCtrlRule(
    _In_        IC_APPCTRL_RULE            *PRule,
    _Inout_opt_ DWORD                      *PDwRuleId
);

_Success_(ERROR_SUCCESS == return)
DWORD
DbDeleteAppCtrlRule(
    _In_        DWORD                       DwRuleId
);

_Success_(ERROR_SUCCESS == return)
DWORD
DbUpdateAppCtrlRule(
    _In_        DWORD                       DwRuleId,
    _In_        IC_APPCTRL_RULE            *PRule
);

_Success_(ERROR_SUCCESS == return)
DWORD
DbGetAppCtrlRules(
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
