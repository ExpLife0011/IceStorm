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
IcAddAppCtrlDenyRule(
    _In_opt_z_  PWCHAR                      PFilePath,
    _In_opt_    DWORD                       DwPid,
    _Inout_opt_ DWORD                      *PDwRuleId
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcAddAppCtrlAllowRule(
    _In_opt_z_  PWCHAR                      PFilePath,
    _In_opt_    DWORD                       DwPid,
    _Inout_opt_ DWORD                      *PDwRuleId
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcDeleteAppCtrlDenyRule(
    _In_        DWORD                       DwRuleId
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcDeleteAppCtrlAllowRule(
    _In_        DWORD                       DwRuleId
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcUpdateAppCtrlDenyRule(
    _In_        DWORD                       DwRuleId,
    _In_opt_z_  PWCHAR                      PFilePath,
    _In_opt_    DWORD                       DwPid
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcUpdateAppCtrlAllowRule(
    _In_        DWORD                       DwRuleId,
    _In_opt_z_  PWCHAR                      PFilePath,
    _In_opt_    DWORD                       DwPid
);

ICEFLTUM_API
_Success_(ERROR_SUCCESS == return)
DWORD
IcGetAppCtrlRules(
    _In_        BOOLEAN                     BGetAllowRules,
    _In_        BOOLEAN                     BGetDenyRules,
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

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
