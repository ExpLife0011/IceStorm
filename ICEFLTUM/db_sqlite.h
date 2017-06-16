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
    _Inout_     PIC_APPCTRL_RULE            PRules,
    _In_        DWORD                       DwLength
);

_Success_(ERROR_SUCCESS == return)
DWORD
DbGetFSScanDeniedFlags(
    _In_        IC_FS_RULE                 *PRule,
    _Inout_     ULONG                      *PUlDeniedFlags
);

_Success_(ERROR_SUCCESS == return)
DWORD
DbAddFSScanRule(
    _In_        IC_FS_RULE                 *PRule,
    _Inout_opt_ DWORD                      *PDwRuleId
);

_Success_(ERROR_SUCCESS == return)
DWORD
DbDeleteFSScanRule(
    _In_        DWORD                       DwRuleId
);

_Success_(ERROR_SUCCESS == return)
DWORD
DbUpdateFSScanRule(
    _In_        DWORD                       DwRuleId,
    _In_        IC_FS_RULE                 *PRule
);

_Success_(ERROR_SUCCESS == return)
DWORD
DbGetFSScanRules(
    _Inout_     PIC_FS_RULE                *PPRules,
    _Inout_     DWORD                      *PDwLength
);

_Success_(ERROR_SUCCESS == return)
VOID
DbFreeFSScanRulesList(
    _Inout_     PIC_FS_RULE                 PRules,
    _In_        DWORD                       DwLength
);

_Success_(ERROR_SUCCESS == return)
DWORD
DbAddFSEvent(
    _In_        PIC_FS_EVENT                PEvent
);

_Success_(ERROR_SUCCESS == return)
DWORD
DbAddAppEvent(
    _In_        PIC_APPCTRL_EVENT           PEvent
);

_Success_(ERROR_SUCCESS == return)
DWORD
DbGetFSEvents(
    _Out_       PIC_FS_EVENT               *PPEvents,
    _In_        DWORD                      *PDwLength,
    _In_        DWORD                       DwFirstId
);

VOID
DbFreeFSEventsList(
    _Inout_     PIC_FS_EVENT                PEvents,
    _In_        DWORD                       DwLength
);

_Success_(ERROR_SUCCESS == return)
DWORD
DbGetAppCtrlEvents(
    _Out_       PIC_APPCTRL_EVENT          *PPEvents,
    _In_        DWORD                      *PDwLength,
    _In_        DWORD                       DwFirstId
);

VOID
DbFreeAppCtrlEventsList(
    _Inout_     PIC_APPCTRL_EVENT           PEvents,
    _In_        DWORD                       DwLength
);

_Success_(ERROR_SUCCESS == return)
DWORD
DBSetAppCtrlScanStatus(
    _In_        DWORD                       DwStatus
);

_Success_(ERROR_SUCCESS == return)
DWORD
DBSetFSScanStatus(
    _In_        DWORD                       DwStatus
);

_Success_(ERROR_SUCCESS == return)
DWORD
DbGetScanStatus(
    _Out_       DWORD                      *PDwAppCtrlStatus,
    _Out_       DWORD                      *PDwFSScanStatus
);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
