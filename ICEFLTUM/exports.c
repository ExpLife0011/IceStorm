#include "exports.h"
#include "appctrl_scan.h"
#include "driver_io.h"
#include "appctrl_rules.h"
#include "fs_scan.h"
#include "fs_rules.h"

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
IcAddAppCtrlRule(
    IC_STRING_MATCHER                       MatcherProcessPath,
    PWCHAR                                  PProcessPath,
    DWORD                                   DwPid,
    IC_STRING_MATCHER                       MatcherParentPath,
    PWCHAR                                  PParentPath,
    DWORD                                   DwParentPid,
    ICE_SCAN_VERDICT                        Verdict,
    DWORD                                  *PDwRuleId
)
{
    if (NULL == PProcessPath && 0 == DwPid && NULL == PParentPath && 0 == DwParentPid)
    {
        return ERROR_INVALID_PARAMETER;
    }

    return AddAppCtrlRule(MatcherProcessPath, PProcessPath, DwPid, MatcherParentPath, PParentPath, DwParentPid, Verdict, PDwRuleId);
}

_Use_decl_anno_impl_
DWORD
IcDeleteAppCtrlRule(
    DWORD                                   DwRuleId
)
{
    if (DwRuleId == 0)
    {
        return ERROR_INVALID_PARAMETER;
    }

    return DeleteAppCtrlRule(DwRuleId);
}

_Use_decl_anno_impl_
DWORD
IcUpdateAppCtrlRule(
    DWORD                                   DwRuleId,
    IC_STRING_MATCHER                       MatcherProcessPath,
    PWCHAR                                  PProcessPath,
    DWORD                                   DwPid,
    IC_STRING_MATCHER                       MatcherParentPath,
    PWCHAR                                  PParentPath,
    DWORD                                   DwParentPid,
    ICE_SCAN_VERDICT                        Verdict
)
{
    if ((DwRuleId == 0) || (NULL == PProcessPath && 0 == DwPid && NULL == PParentPath && 0 == DwParentPid))
    {
        return ERROR_INVALID_PARAMETER;
    }

    return UpdateAppCtrlRule(DwRuleId, MatcherProcessPath, PProcessPath, DwPid, MatcherParentPath, PParentPath, DwParentPid, Verdict);
}

_Use_decl_anno_impl_
DWORD
IcGetAppCtrlRules(
    PIC_APPCTRL_RULE                       *PPRules,
    DWORD                                  *PDwLength
)
{
    if ((NULL == PPRules) || (NULL == PDwLength))
    {
        return ERROR_INVALID_PARAMETER;
    }

    return GetAppCtrlRules(PPRules, PDwLength);
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

_Use_decl_anno_impl_
DWORD
IcStartFSScan(
    VOID
)
{
    return StartFSScan();
}

_Use_decl_anno_impl_
DWORD
IcStopFSScan(
    VOID
)
{
    return StopFSScan();
}

_Use_decl_anno_impl_
DWORD
IcAddFSScanRule(
    IC_STRING_MATCHER               MatcherProcessPath,
    PWCHAR                          PProcessPath,
    DWORD                           DwPid,
    IC_STRING_MATCHER               MatcherFilePath,
    PWCHAR                          PFilePath,
    ULONG                           UlDeniedOperations,
    DWORD                          *PDwRuleId
)
{
    if (NULL == PProcessPath && 0 == DwPid && NULL == PFilePath)
    {
        return ERROR_INVALID_PARAMETER;
    }

    return AddFSScanRule(MatcherProcessPath, PProcessPath, DwPid, MatcherFilePath, PFilePath, UlDeniedOperations, PDwRuleId);
}

_Use_decl_anno_impl_
DWORD
IcDeleteFSScanRule(
    DWORD                                   DwRuleId
)
{
    if (DwRuleId == 0)
    {
        return ERROR_INVALID_PARAMETER;
    }

    return DeleteFSScanRule(DwRuleId);
}

_Use_decl_anno_impl_
DWORD
IcUpdateFSScanRule(
    DWORD                           DwRuleId,
    IC_STRING_MATCHER               MatcherProcessPath,
    PWCHAR                          PProcessPath,
    DWORD                           DwPid,
    IC_STRING_MATCHER               MatcherFilePath,
    PWCHAR                          PFilePath,
    ULONG                           UlDeniedOperations
)
{
    if ((DwRuleId == 0) || (NULL == PProcessPath && 0 == DwPid && NULL == PProcessPath))
    {
        return ERROR_INVALID_PARAMETER;
    }

    return UpdateFSScanRule(DwRuleId, MatcherProcessPath, PProcessPath, DwPid, MatcherFilePath, PFilePath, UlDeniedOperations);
}

_Use_decl_anno_impl_
DWORD
IcGetFSScanRules(
    PIC_FS_RULE                            *PPRules,
    DWORD                                  *PDwLength
)
{
    if ((NULL == PPRules) || (NULL == PDwLength))
    {
        return ERROR_INVALID_PARAMETER;
    }

    return GetFSScanRules(PPRules, PDwLength);
}

_Use_decl_anno_impl_
VOID
IcFreeAppFSScanList(
    PIC_FS_RULE                             PRules,
    DWORD                                   DwLength
)
{
    if (NULL == PRules || 0 == DwLength)
    {
        return;
    }

    FreeAppFSScanList(PRules, DwLength);
}
