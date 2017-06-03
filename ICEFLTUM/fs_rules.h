#pragma once

#include "global_data.h"
#include "icecommon.h"

_Success_(return == ERROR_SUCCESS)
DWORD
InitFSScanRules(
    VOID
);

_Success_(return == ERROR_SUCCESS)
DWORD
UninitFSScanRules(
    VOID
);

_Success_(return == ERROR_SUCCESS)
DWORD
GetFSScanResult(
    _In_    ULONG                           UlOriginalFlags,
    _In_    IC_FS_RULE                     *PRule,
    _Inout_ ICE_FS_SCAN_RESULT_PACKET      *PResultPack
);

_Success_(ERROR_SUCCESS == return)
DWORD
AddFSScanRule(
    _In_        IC_STRING_MATCHER           MatcherProcessPath,
    _In_z_      PWCHAR                      PProcessPath,
    _In_        DWORD                       DwPid,
    _In_        IC_STRING_MATCHER           MatcherFilePath,
    _In_z_      PWCHAR                      PFilePath,
    _In_        ULONG                       UlDeniedOperations,
    _Inout_     DWORD                      *PDwRuleId
);

_Success_(ERROR_SUCCESS == return)
DWORD
DeleteFSScanRule(
    _In_        DWORD                       DwRuleId
);

_Success_(ERROR_SUCCESS == return)
DWORD
UpdateFSScanRule(
    _In_        DWORD                       DwRuleId,
    _In_        IC_STRING_MATCHER           MatcherProcessPath,
    _In_z_      PWCHAR                      PProcessPath,
    _In_        DWORD                       DwPid,
    _In_        IC_STRING_MATCHER           MatcherFilePath,
    _In_z_      PWCHAR                      PFilePath,
    _In_        ULONG                       UlDeniedOperations
);

_Success_(ERROR_SUCCESS == return)
DWORD
GetFSScanRules(
    _Inout_     PIC_FS_RULE                *PPRules,
    _Inout_     DWORD                      *PDwLength
);

_Success_(ERROR_SUCCESS == return)
VOID
FreeFSScanList(
    _Inout_     PIC_FS_RULE                 PRules,
    _In_        DWORD                       DwLength
);

_Success_(ERROR_SUCCESS == return)
DWORD
GetFSEvents(
    _Out_       PIC_FS_EVENT               *PPEvents,
    _In_        DWORD                      *PDwLength,
    _In_        DWORD                       DwFirstId
);

VOID
FreeFSEventsList(
    _Inout_     PIC_FS_EVENT                PEvents,
    _In_        DWORD                       DwLength
);
