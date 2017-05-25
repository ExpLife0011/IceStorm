#ifndef __IMPORT_ICELTUM_H__
#define __IMPORT_ICELTUM_H__


#include <windows.h>
#include <ntstatus.h>
#include "icecommon.h"

typedef 
_Success_(ERROR_SUCCESS == return)
DWORD(*PFUNC_IcInitConnectionToIceFlt) (
    VOID
    );

typedef
_Success_(ERROR_SUCCESS == return)
DWORD(*PFUNC_IcUninitConnectionToIceFlt) (
    VOID
    );

typedef
_Success_(ERROR_SUCCESS == return)
DWORD(*PFUNC_IcSendSetOption) (
    _In_        DWORD                       DwOption,
    _In_        DWORD                       DwValue
    );

typedef
_Success_(ERROR_SUCCESS == return)
DWORD(*PFUNC_IcStartAppCtrlScan) (
    VOID
    );

typedef
_Success_(ERROR_SUCCESS == return)
DWORD(*PFUNC_IcStopAppCtrlScan) (
    VOID
    );

typedef
_Success_(ERROR_SUCCESS == return)
DWORD(*PFUNC_IcAddAppCtrlRule) (
    _In_        IC_STRING_MATCHER           MatcherProcessPath,
    _In_opt_z_  PWCHAR                      PProcessPath,
    _In_opt_    DWORD                       DwPid,
    _In_        IC_STRING_MATCHER           MatcherParentPath,
    _In_opt_z_  PWCHAR                      PParentPath,
    _In_opt_    DWORD                       DwParentPid,
    _In_        ICE_SCAN_VERDICT            Verdict,
    _Inout_opt_ DWORD                      *PDwRuleId
);

typedef
_Success_(ERROR_SUCCESS == return)
DWORD(*PFUNC_IcDeleteAppCtrlRule) (
    _In_        DWORD                       DwRuleId
    );

typedef
_Success_(ERROR_SUCCESS == return)
DWORD (*PFUNC_IcUpdateAppCtrlRule) (
    _In_        DWORD                       DwRuleId,
    _In_        IC_STRING_MATCHER           MatcherProcessPath,
    _In_opt_z_  PWCHAR                      PProcessPath,
    _In_opt_    DWORD                       DwPid,
    _In_        IC_STRING_MATCHER           MatcherParentPath,
    _In_opt_z_  PWCHAR                      PParentPath,
    _In_opt_    DWORD                       DwParentPid,
    _In_        ICE_SCAN_VERDICT            Verdict
);

typedef
_Success_(ERROR_SUCCESS == return)
DWORD (*PFUNC_IcGetAppCtrlRules) (
    _Inout_     PIC_APPCTRL_RULE           *PPRules,
    _Inout_     DWORD                      *PDwLength
    );

typedef
_Success_(ERROR_SUCCESS == return)
VOID (*PFUNC_IcFreeAppCtrlRulesList) (
    _Inout_     PIC_APPCTRL_RULE           PRules,
    _In_        DWORD                      DwLength
);

typedef
_Success_(ERROR_SUCCESS == return)
DWORD(*PFUNC_IcStartFSScan) (
    VOID
    );

typedef
_Success_(ERROR_SUCCESS == return)
DWORD(*PFUNC_IcStopFSScan) (
    VOID
    );

extern PFUNC_IcInitConnectionToIceFlt       IcInitConnectionToIceFlt;
extern PFUNC_IcUninitConnectionToIceFlt     IcUninitConnectionToIceFlt;
extern PFUNC_IcSendSetOption                IcSendSetOption;
extern PFUNC_IcStartAppCtrlScan             IcStartAppCtrlScan;
extern PFUNC_IcStopAppCtrlScan              IcStopAppCtrlScan;
extern PFUNC_IcAddAppCtrlRule               IcAddAppCtrlRule;
extern PFUNC_IcDeleteAppCtrlRule            IcDeleteAppCtrlRule;
extern PFUNC_IcUpdateAppCtrlRule            IcUpdateAppCtrlRule;
extern PFUNC_IcGetAppCtrlRules              IcGetAppCtrlRules;
extern PFUNC_IcFreeAppCtrlRulesList         IcFreeAppCtrlRulesList;
extern PFUNC_IcStartFSScan                  IcStartFSScan;
extern PFUNC_IcStopFSScan                   IcStopFSScan;

_Success_(NT_SUCCESS(return))
NTSTATUS
IcImportIcefltUmAPI(
    VOID
);

VOID
IcFreeIcefltUmAPI(
    VOID
);


#endif // !__IMPORT_ICEFLTUM_H__
