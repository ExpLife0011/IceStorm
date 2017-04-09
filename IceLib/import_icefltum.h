#ifndef __IMPORT_ICELTUM_H__
#define __IMPORT_ICELTUM_H__


#include <windows.h>
#include <ntstatus.h>

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
    _In_z_      DWORD                       DwValue
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


extern PFUNC_IcInitConnectionToIceFlt       IcInitConnectionToIceFlt;
extern PFUNC_IcUninitConnectionToIceFlt     IcUninitConnectionToIceFlt;
extern PFUNC_IcSendSetOption                IcSendSetOption;
extern PFUNC_IcStartAppCtrlScan             IcStartAppCtrlScan;
extern PFUNC_IcStopAppCtrlScan              IcStopAppCtrlScan;

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
