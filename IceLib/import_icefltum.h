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
DWORD(*PFUNC_IcAddAppCtrlDenyRule) (
    _In_opt_z_  PWCHAR                      PFilePath,
    _In_opt_    DWORD                       DwPid,
    _Inout_opt_ DWORD                      *PDwRuleId
    );

typedef
_Success_(ERROR_SUCCESS == return)
DWORD (*PFUNC_IcAddAppCtrlAllowRule) (
    _In_opt_    PWCHAR                      PFilePath,
    _In_opt_    DWORD                       DwPid,
    _Inout_opt_ DWORD                      *PDwRuleId
    );

typedef
_Success_(ERROR_SUCCESS == return)
DWORD(*PFUNC_IcDeleteAppCtrlDenyRule) (
    _In_        DWORD                       DwRuleId
    );

typedef
_Success_(ERROR_SUCCESS == return)
DWORD (*PFUNC_IcDeleteAppCtrlAllowRule) (
    _In_        DWORD                       DwRuleId
    );

typedef
_Success_(ERROR_SUCCESS == return)
DWORD (*PFUNC_IcUpdateAppCtrlDenyRule) (
    _In_        DWORD                       DwRuleId,
    _In_opt_z_  PWCHAR                      PFilePath,
    _In_opt_    DWORD                       DwPid
);

typedef
_Success_(ERROR_SUCCESS == return)
DWORD (*PFUNC_IcUpdateAppCtrlAllowRule) (
    _In_        DWORD                       DwRuleId,
    _In_opt_z_  PWCHAR                      PFilePath,
    _In_opt_    DWORD                       DwPid
);


typedef
_Success_(ERROR_SUCCESS == return)
DWORD (*PFUNC_IcGetAppCtrlRules) (
    _In_        BOOLEAN                     BGetAllowRules,
    _In_        BOOLEAN                     BGetDenyRules,
    _Inout_     PIC_APPCTRL_RULE           *PPRules,
    _Inout_     DWORD                      *PDwLength
    );

typedef
_Success_(ERROR_SUCCESS == return)
VOID (*PFUNC_IcFreeAppCtrlRulesList) (
    _Inout_     PIC_APPCTRL_RULE           PRules,
    _In_        DWORD                      DwLength
);


extern PFUNC_IcInitConnectionToIceFlt       IcInitConnectionToIceFlt;
extern PFUNC_IcUninitConnectionToIceFlt     IcUninitConnectionToIceFlt;
extern PFUNC_IcSendSetOption                IcSendSetOption;
extern PFUNC_IcStartAppCtrlScan             IcStartAppCtrlScan;
extern PFUNC_IcStopAppCtrlScan              IcStopAppCtrlScan;
extern PFUNC_IcAddAppCtrlDenyRule           IcAddAppCtrlDenyRule;
extern PFUNC_IcAddAppCtrlAllowRule          IcAddAppCtrlAllowRule;
extern PFUNC_IcDeleteAppCtrlDenyRule        IcDeleteAppCtrlDenyRule;
extern PFUNC_IcDeleteAppCtrlAllowRule       IcDeleteAppCtrlAllowRule;
extern PFUNC_IcUpdateAppCtrlDenyRule        IcUpdateAppCtrlDenyRule;
extern PFUNC_IcUpdateAppCtrlAllowRule       IcUpdateAppCtrlAllowRule;
extern PFUNC_IcGetAppCtrlRules              IcGetAppCtrlRules;
extern PFUNC_IcFreeAppCtrlRulesList         IcFreeAppCtrlRulesList;


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
