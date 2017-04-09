#include "icecommon.h"
#include "debug2.h"
#include "import_icefltum.h"

PFUNC_IcInitConnectionToIceFlt              IcInitConnectionToIceFlt    = NULL;
PFUNC_IcUninitConnectionToIceFlt            IcUninitConnectionToIceFlt  = NULL;
PFUNC_IcSendSetOption                       IcSendSetOption             = NULL;
PFUNC_IcStartAppCtrlScan                    IcStartAppCtrlScan          = NULL;
PFUNC_IcStopAppCtrlScan                     IcStopAppCtrlScan           = NULL;

HMODULE                                     gHIcefltUM                  = NULL;

_Use_decl_anno_impl_
NTSTATUS
IcImportIcefltUmAPI(
    VOID
)
{
    NTSTATUS                                ntStatus;

    ntStatus                                = STATUS_SUCCESS;

    __try
    {
        gHIcefltUM = LoadLibraryW(ICEFLTUM_DLL);
        if (NULL == gHIcefltUM)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"LoadLibraryW(\"%s\")", ICEFLTUM_DLL);
            __leave;
        }

        IcInitConnectionToIceFlt = (PFUNC_IcInitConnectionToIceFlt) GetProcAddress(gHIcefltUM, "IcInitConnectionToIceFlt");
        if (NULL == IcInitConnectionToIceFlt)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"GetProcAddress(\"IcInitConnection\") failed");
            LogErrorWin(GetLastError(), L"GetProcAddress(\"IcInitConnection\") failed");
            __leave;
        }

        IcUninitConnectionToIceFlt = (PFUNC_IcUninitConnectionToIceFlt) GetProcAddress(gHIcefltUM, "IcUninitConnectionToIceFlt");
        if (NULL == IcInitConnectionToIceFlt)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"GetProcAddress(\"IcUninitConnectionToIceFlt\") failed");
            LogErrorWin(GetLastError(), L"GetProcAddress(\"IcUninitConnectionToIceFlt\") failed");
            __leave;
        }

        IcSendSetOption = (PFUNC_IcSendSetOption) GetProcAddress(gHIcefltUM, "IcSendSetOption");
        if (NULL == IcInitConnectionToIceFlt)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"GetProcAddress(\"IcSendSetOption\") failed");
            LogErrorWin(GetLastError(), L"GetProcAddress(\"IcSendSetOption\") failed");
            __leave;
        }

        IcStartAppCtrlScan = (PFUNC_IcStartAppCtrlScan) GetProcAddress(gHIcefltUM, "IcStartAppCtrlScan");
        if (NULL == IcStartAppCtrlScan)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"GetProcAddress(\"IcStartAppCtrlScan\") failed");
            LogErrorWin(GetLastError(), L"GetProcAddress(\"IcStartAppCtrlScan\") failed");
            __leave;
        }

        IcStopAppCtrlScan = (PFUNC_IcStartAppCtrlScan) GetProcAddress(gHIcefltUM, "IcStopAppCtrlScan");
        if (NULL == IcStopAppCtrlScan)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"GetProcAddress(\"IcStopAppCtrlScan\") failed");
            LogErrorWin(GetLastError(), L"GetProcAddress(\"IcStopAppCtrlScan\") failed");
            __leave;
        }
    }
    __finally
    {
        if (!NT_SUCCESS(ntStatus))
        {
            IcFreeIcefltUmAPI();
        }
    }

    return ntStatus;
}

_Use_decl_anno_impl_
VOID
IcFreeIcefltUmAPI(
    VOID
)
{
    IcInitConnectionToIceFlt    = NULL;
    IcUninitConnectionToIceFlt  = NULL;
    IcSendSetOption             = NULL;
    IcStartAppCtrlScan          = NULL;
    IcStopAppCtrlScan           = NULL;

    if (NULL != gHIcefltUM)
    {
        FreeLibrary(gHIcefltUM);
        gHIcefltUM = NULL;
    }
}
