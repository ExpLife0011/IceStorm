#pragma once


#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>

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

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
