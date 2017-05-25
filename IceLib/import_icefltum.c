#include "icecommon.h"
#include "debug2.h"
#include "import_icefltum.h"

PFUNC_IcInitConnectionToIceFlt              IcInitConnectionToIceFlt    = NULL;
PFUNC_IcUninitConnectionToIceFlt            IcUninitConnectionToIceFlt  = NULL;
PFUNC_IcSendSetOption                       IcSendSetOption             = NULL;
PFUNC_IcStartAppCtrlScan                    IcStartAppCtrlScan          = NULL;
PFUNC_IcStopAppCtrlScan                     IcStopAppCtrlScan           = NULL;
PFUNC_IcAddAppCtrlRule                      IcAddAppCtrlRule            = NULL;
PFUNC_IcDeleteAppCtrlRule                   IcDeleteAppCtrlRule         = NULL;
PFUNC_IcUpdateAppCtrlRule                   IcUpdateAppCtrlRule         = NULL;
PFUNC_IcGetAppCtrlRules                     IcGetAppCtrlRules           = NULL;
PFUNC_IcFreeAppCtrlRulesList                IcFreeAppCtrlRulesList      = NULL;
PFUNC_IcStartFSScan                         IcStartFSScan               = NULL;
PFUNC_IcStopFSScan                          IcStopFSScan                = NULL;

HMODULE                                     gHIcefltUM                  = NULL;

_Use_decl_anno_impl_
NTSTATUS
IcImportIcefltUmAPI(
    VOID
)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

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

        IcAddAppCtrlRule = (PFUNC_IcAddAppCtrlRule) GetProcAddress(gHIcefltUM, "IcAddAppCtrlRule");
        if (NULL == IcAddAppCtrlRule)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"GetProcAddress(\"IcAddAppCtrlRule\") failed");
            LogErrorWin(GetLastError(), L"GetProcAddress(\"IcAddAppCtrlRule\") failed");
            __leave;
        }

        IcDeleteAppCtrlRule = (PFUNC_IcDeleteAppCtrlRule) GetProcAddress(gHIcefltUM, "IcDeleteAppCtrlRule");
        if (NULL == IcDeleteAppCtrlRule)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"GetProcAddress(\"IcDeleteAppCtrlRule\") failed");
            LogErrorWin(GetLastError(), L"GetProcAddress(\"IcDeleteAppCtrlRule\") failed");
            __leave;
        }

        IcUpdateAppCtrlRule = (PFUNC_IcUpdateAppCtrlRule) GetProcAddress(gHIcefltUM, "IcUpdateAppCtrlRule");
        if (NULL == IcUpdateAppCtrlRule)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"GetProcAddress(\"IcUpdateAppCtrlRule\") failed");
            LogErrorWin(GetLastError(), L"GetProcAddress(\"IcUpdateAppCtrlRule\") failed");
            __leave;
        }

        IcGetAppCtrlRules = (PFUNC_IcGetAppCtrlRules) GetProcAddress(gHIcefltUM, "IcGetAppCtrlRules");
        if (NULL == IcGetAppCtrlRules)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"GetProcAddress(\"IcGetAppCtrlRules\") failed");
            LogErrorWin(GetLastError(), L"GetProcAddress(\"IcGetAppCtrlRules\") failed");
            __leave;
        }

        IcFreeAppCtrlRulesList = (PFUNC_IcFreeAppCtrlRulesList) GetProcAddress(gHIcefltUM, "IcFreeAppCtrlRulesList");
        if (NULL == IcFreeAppCtrlRulesList)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"GetProcAddress(\"IcFreeAppCtrlRulesList\") failed");
            LogErrorWin(GetLastError(), L"GetProcAddress(\"IcFreeAppCtrlRulesList\") failed");
            __leave;
        }

        IcStartFSScan = (PFUNC_IcStartFSScan) GetProcAddress(gHIcefltUM, "IcStartFSScan");
        if (NULL == IcStartFSScan)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"GetProcAddress(\"IcStartFSScan\") failed");
            LogErrorWin(GetLastError(), L"GetProcAddress(\"IcStartFSScan\") failed");
            __leave;
        }

        IcStopFSScan = (PFUNC_IcStopFSScan) GetProcAddress(gHIcefltUM, "IcStopFSScan");
        if (NULL == IcStopFSScan)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"GetProcAddress(\"IcStopFSScan\") failed");
            LogErrorWin(GetLastError(), L"GetProcAddress(\"IcStopFSScan\") failed");
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
    IcInitConnectionToIceFlt = NULL;
    IcUninitConnectionToIceFlt = NULL;
    IcSendSetOption = NULL;
    IcStartAppCtrlScan = NULL;
    IcStopAppCtrlScan = NULL;
    IcAddAppCtrlRule = NULL;
    IcDeleteAppCtrlRule = NULL;
    IcUpdateAppCtrlRule = NULL;
    IcGetAppCtrlRules = NULL;
    IcFreeAppCtrlRulesList = NULL;
    IcStartFSScan = NULL;
    IcStopFSScan = NULL;

    if (NULL != gHIcefltUM)
    {
        FreeLibrary(gHIcefltUM);
        gHIcefltUM = NULL;
    }
}
