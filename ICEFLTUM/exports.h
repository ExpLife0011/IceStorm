#pragma once

#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>

#include "global_data.h"

#ifdef ICEFLTUM_EXPORTS
#define ICEFLTUM_API __declspec(dllexport) 
#else
#define ICEFLTUM_API __declspec(dllimport) 
#endif

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcInitConnectionToIceFlt(
    VOID
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcUninitConnectionToIceFlt(
    VOID
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcSendSetOption(
    _In_          DWORD                       DwOption,
    _In_          DWORD                       DwValue
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcStartAppCtrlScan(
    VOID
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcStopAppCtrlScan(
    VOID
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcAddAppCtrlRule(
    _In_        IC_STRING_MATCHER           MatcherProcessPath,
    _In_opt_z_  PWCHAR                      PProcessPath,
    _In_opt_    DWORD                       DwPid,
    _In_        IC_STRING_MATCHER           MatcherParentPath,
    _In_opt_z_  PWCHAR                      PParentPath,
    _In_opt_    DWORD                       DwParentPid,
    _In_        ICE_SCAN_VERDICT            Verdict,
    _Inout_opt_ DWORD                      *PDwRuleId
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcDeleteAppCtrlRule(
    _In_        DWORD                       DwRuleId
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcUpdateAppCtrlRule(
    _In_        DWORD                       DwRuleId,
    _In_        IC_STRING_MATCHER           MatcherProcessPath,
    _In_opt_z_  PWCHAR                      PProcessPath,
    _In_opt_    DWORD                       DwPid,
    _In_        IC_STRING_MATCHER           MatcherParentPath,
    _In_opt_z_  PWCHAR                      PParentPath,
    _In_opt_    DWORD                       DwParentPid,
    _In_        ICE_SCAN_VERDICT            Verdict
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcGetAppCtrlRules(
    _Inout_     PIC_APPCTRL_RULE           *PPRules,
    _Inout_     DWORD                      *PDwLength
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
VOID
IcFreeAppCtrlRulesList(
    _Inout_     PIC_APPCTRL_RULE           PRules,
    _In_        DWORD                      DwLength
);


ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcStartFSScan(
    VOID
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcStopFSScan(
    VOID
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcAddFSScanRule(
    _In_        IC_STRING_MATCHER           MatcherProcessPath,
    _In_z_      PWCHAR                      PProcessPath,
    _In_        DWORD                       DwPid,
    _In_        IC_STRING_MATCHER           MatcherFilePath,
    _In_z_      PWCHAR                      PFilePath,
    _In_        ULONG                       UlDeniedOperations,
    _Inout_     DWORD                      *PDwRuleId
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcDeleteFSScanRule(
    _In_        DWORD                       DwRuleId
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcUpdateFSScanRule(
    _In_        DWORD                       DwRuleId,
    _In_        IC_STRING_MATCHER           MatcherProcessPath,
    _In_z_      PWCHAR                      PProcessPath,
    _In_        DWORD                       DwPid,
    _In_        IC_STRING_MATCHER           MatcherFilePath,
    _In_z_      PWCHAR                      PFilePath,
    _In_        ULONG                       UlDeniedOperations
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcGetFSScanRules(
    _Inout_     PIC_FS_RULE                *PPRules,
    _Inout_     DWORD                      *PDwLength
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
VOID
IcFreeFSScanList(
    _Inout_     PIC_FS_RULE                 PRules,
    _In_        DWORD                       DwLength
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcGetFSEvents(
    _Out_       PIC_FS_EVENT               *PPEvents,
    _In_        DWORD                      *PDwLength,
    _In_        DWORD                       DwFirstId
);

ICEFLTUM_API
VOID
IcFreeFSEventsList(
    _Inout_     PIC_FS_EVENT                PEvents,
    _In_        DWORD                       DwLength
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcGetAppCtrlEvents(
    _Out_       PIC_APPCTRL_EVENT          *PPEvents,
    _In_        DWORD                      *PDwLength,
    _In_        DWORD                       DwFirstId
);

ICEFLTUM_API
VOID
IcFreeAppCtrlEventsList(
    _Inout_     PIC_APPCTRL_EVENT           PEvents,
    _In_        DWORD                       DwLength
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcGetAppCtrlStatus(
    _Out_       BOOLEAN                    *PBEnabled
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcGetFSscanStatus(
    _Out_       BOOLEAN                    *PBEnabled
);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
