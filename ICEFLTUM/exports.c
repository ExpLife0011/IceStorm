#include "exports.h"
#include "appctrl_scan.h"
#include "driver_io.h"
#include "appctrl_rules.h"

_Use_decl_anno_impl_
DWORD
IcInitConnectionToIceFlt(
    VOID
)
{
    return InitConnectionToIceFlt();
}

_Use_decl_anno_impl_
DWORD
IcUninitConnectionToIceFlt(
    VOID
)
{
    return UninitConnectionToIceFlt();
}

_Use_decl_anno_impl_
DWORD
IcSendSetOption(
    DWORD                                   DwOption,
    DWORD                                   DwValue
)
{
    return SendSetOption(DwOption, DwValue);
}

_Use_decl_anno_impl_
DWORD
IcStartAppCtrlScan(
    VOID
)
{
    return StartAppCtrlScan();
}

_Use_decl_anno_impl_
DWORD
IcStopAppCtrlScan(
    VOID
)
{
    return StopAppCtrlScan();
}

_Use_decl_anno_impl_
DWORD
IcAddAppCtrlDenyRule(
    PWCHAR                                  PFilePath,
    DWORD                                   DwPid,
    DWORD                                  *PDwRuleId
)
{
    if (NULL == PFilePath && 0 == DwPid)
    {
        return ERROR_INVALID_PARAMETER;
    }

    return AddAppCtrlDenyRule(PFilePath, DwPid, PDwRuleId);
}

_Use_decl_anno_impl_
DWORD
IcAddAppCtrlAllowRule(
    PWCHAR                                  PFilePath,
    DWORD                                   DwPid,
    DWORD                                  *PDwRuleId
)
{
    if (NULL == PFilePath && 0 == DwPid)
    {
        return ERROR_INVALID_PARAMETER;
    }
    
    return AddAppCtrlAllowRule(PFilePath, DwPid, PDwRuleId);
}

_Use_decl_anno_impl_
DWORD
IcDeleteAppCtrlDenyRule(
    DWORD                                   DwRuleId
)
{
    if (DwRuleId == 0)
    {
        return ERROR_INVALID_PARAMETER;
    }

    return DeleteAppCtrlDenyRule(DwRuleId);
}

_Use_decl_anno_impl_
DWORD
IcDeleteAppCtrlAllowRule(
    DWORD                                   DwRuleId
)
{
    if (DwRuleId == 0)
    {
        return ERROR_INVALID_PARAMETER;
    }

    return DeleteAppCtrlAllowRule(DwRuleId);
}

_Use_decl_anno_impl_
DWORD
IcUpdateAppCtrlDenyRule(
    DWORD                                   DwRuleId,
    PWCHAR                                  PFilePath,
    DWORD                                   DwPid
)
{
    if (DwRuleId == 0 || (PFilePath == NULL && DwPid == 0))
    {
        return ERROR_INVALID_PARAMETER;
    }

    return UpdateAppCtrlDenyRule(DwRuleId, PFilePath, DwPid);
}

_Use_decl_anno_impl_
DWORD
IcUpdateAppCtrlAllowRule(
    DWORD                                   DwRuleId,
    PWCHAR                                  PFilePath,
    DWORD                                   DwPid
)
{
    if (DwRuleId == 0 || (PFilePath == NULL && DwPid == 0))
    {
        return ERROR_INVALID_PARAMETER;
    }

    return UpdateAppCtrlAllowRule(DwRuleId, PFilePath, DwPid);
}

_Use_decl_anno_impl_
DWORD
IcGetAppCtrlRules(
    BOOLEAN                                 BGetAllowRules,
    BOOLEAN                                 BGetDenyRules,
    PIC_APPCTRL_RULE                       *PPRules,
    DWORD                                  *PDwLength
)
{
    if (
        (!BGetDenyRules && !BGetDenyRules) ||
        (NULL == PPRules) ||
        (NULL == PDwLength)
        )
    {
        return ERROR_INVALID_PARAMETER;
    }

    return GetAppCtrlRules(BGetAllowRules, BGetDenyRules, PPRules, PDwLength);
}

_Use_decl_anno_impl_
VOID
IcFreeAppCtrlRulesList(
    PIC_APPCTRL_RULE                        PRules,
    DWORD                                   DwLength
)
{
    if (NULL == PRules || 0 == DwLength)
    {
        return;
    }

    FreeAppCtrlRulesList(PRules, DwLength);
}

