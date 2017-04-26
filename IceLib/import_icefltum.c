#include "icecommon.h"
#include "debug2.h"
#include "import_icefltum.h"

PFUNC_IcInitConnectionToIceFlt              IcInitConnectionToIceFlt    = NULL;
PFUNC_IcUninitConnectionToIceFlt            IcUninitConnectionToIceFlt  = NULL;
PFUNC_IcSendSetOption                       IcSendSetOption             = NULL;
PFUNC_IcStartAppCtrlScan                    IcStartAppCtrlScan          = NULL;
PFUNC_IcStopAppCtrlScan                     IcStopAppCtrlScan           = NULL;
PFUNC_IcAddAppCtrlDenyRule                  IcAddAppCtrlDenyRule        = NULL;
PFUNC_IcAddAppCtrlAllowRule                 IcAddAppCtrlAllowRule       = NULL;
PFUNC_IcDeleteAppCtrlDenyRule               IcDeleteAppCtrlDenyRule     = NULL;
PFUNC_IcDeleteAppCtrlAllowRule              IcDeleteAppCtrlAllowRule    = NULL;
PFUNC_IcUpdateAppCtrlDenyRule               IcUpdateAppCtrlDenyRule     = NULL;
PFUNC_IcUpdateAppCtrlAllowRule              IcUpdateAppCtrlAllowRule    = NULL;
PFUNC_IcGetAppCtrlRules                     IcGetAppCtrlRules           = NULL;
PFUNC_IcFreeAppCtrlRulesList                IcFreeAppCtrlRulesList      = NULL;

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

        IcAddAppCtrlDenyRule = (PFUNC_IcAddAppCtrlDenyRule) GetProcAddress(gHIcefltUM, "IcAddAppCtrlDenyRule");
        if (NULL == IcAddAppCtrlDenyRule)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"GetProcAddress(\"IcAddAppCtrlDenyRule\") failed");
            LogErrorWin(GetLastError(), L"GetProcAddress(\"IcAddAppCtrlDenyRule\") failed");
            __leave;
        }

        IcAddAppCtrlAllowRule = (PFUNC_IcAddAppCtrlDenyRule) GetProcAddress(gHIcefltUM, "IcAddAppCtrlAllowRule");
        if (NULL == IcAddAppCtrlAllowRule)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"GetProcAddress(\"IcAddAppCtrlAllowRule\") failed");
            LogErrorWin(GetLastError(), L"GetProcAddress(\"IcAddAppCtrlAllowRule\") failed");
            __leave;
        }

        IcDeleteAppCtrlDenyRule = (PFUNC_IcDeleteAppCtrlDenyRule) GetProcAddress(gHIcefltUM, "IcDeleteAppCtrlDenyRule");
        if (NULL == IcDeleteAppCtrlDenyRule)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"GetProcAddress(\"IcDeleteAppCtrlDenyRule\") failed");
            LogErrorWin(GetLastError(), L"GetProcAddress(\"IcDeleteAppCtrlDenyRule\") failed");
            __leave;
        }

        IcDeleteAppCtrlAllowRule = (PFUNC_IcDeleteAppCtrlAllowRule) GetProcAddress(gHIcefltUM, "IcDeleteAppCtrlAllowRule");
        if (NULL == IcDeleteAppCtrlAllowRule)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"GetProcAddress(\"IcDeleteAppCtrlAllowRule\") failed");
            LogErrorWin(GetLastError(), L"GetProcAddress(\"IcDeleteAppCtrlAllowRule\") failed");
            __leave;
        }

        IcUpdateAppCtrlDenyRule = (PFUNC_IcUpdateAppCtrlDenyRule) GetProcAddress(gHIcefltUM, "IcUpdateAppCtrlDenyRule");
        if (NULL == IcUpdateAppCtrlDenyRule)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"GetProcAddress(\"IcUpdateAppCtrlDenyRule\") failed");
            LogErrorWin(GetLastError(), L"GetProcAddress(\"IcUpdateAppCtrlDenyRule\") failed");
            __leave;
        }

        IcUpdateAppCtrlAllowRule = (PFUNC_IcUpdateAppCtrlAllowRule) GetProcAddress(gHIcefltUM, "IcUpdateAppCtrlAllowRule");
        if (NULL == IcUpdateAppCtrlAllowRule)
        {
            ntStatus = NTSTATUS_FROM_WIN32(GetLastError());
            LogErrorNt(ntStatus, L"GetProcAddress(\"IcUpdateAppCtrlAllowRule\") failed");
            LogErrorWin(GetLastError(), L"GetProcAddress(\"IcUpdateAppCtrlAllowRule\") failed");
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
    IcAddAppCtrlDenyRule = NULL;
    IcAddAppCtrlAllowRule = NULL;
    IcDeleteAppCtrlDenyRule = NULL;
    IcDeleteAppCtrlAllowRule = NULL;
    IcUpdateAppCtrlDenyRule = NULL;
    IcUpdateAppCtrlAllowRule = NULL;
    IcGetAppCtrlRules = NULL;
    IcFreeAppCtrlRulesList = NULL;

    if (NULL != gHIcefltUM)
    {
        FreeLibrary(gHIcefltUM);
        gHIcefltUM = NULL;
    }
}
