#include "exports.h"
#include "appctrl_scan.h"
#include "driver_io.h"
#include "appctrl_rules.h"
#include "fs_scan.h"
#include "fs_rules.h"
#include "debug2.h"

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
    BOOLEAN                                 BPersistent
)
{
    return StopAppCtrlScan(BPersistent);
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
    BOOLEAN                         BPersistent
)
{
    return StopFSScan(BPersistent);
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
    if ((DwRuleId == 0) || (NULL == PProcessPath && 0 == DwPid && NULL == PFilePath))
    {
        LogInfo(L"DwRuleId: %d, PProcessPath: %d, PFilePath: %d", DwRuleId, PProcessPath, PFilePath);
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
IcFreeFSScanList(
    PIC_FS_RULE                             PRules,
    DWORD                                   DwLength
)
{
    if (NULL == PRules || 0 == DwLength)
    {
        return;
    }

    FreeFSScanList(PRules, DwLength);
}

_Use_decl_anno_impl_
DWORD
IcGetFSEvents(
    PIC_FS_EVENT                           *PPEvents,
    DWORD                                  *PDwLength,
    DWORD                                   DwFirstId
)
{
    if ((NULL == PPEvents) || (NULL == PDwLength))
    {
        return ERROR_INVALID_PARAMETER;
    }

    return GetFSEvents(PPEvents, PDwLength, DwFirstId);
}

_Use_decl_anno_impl_
VOID
IcFreeFSEventsList(
    PIC_FS_EVENT                            PEvents,
    DWORD                                   DwLength
)
{
    if (NULL == PEvents || 0 == DwLength)
    {
        return;
    }

    FreeFSEventsList(PEvents, DwLength);
}

_Use_decl_anno_impl_
DWORD
IcGetAppCtrlEvents(
    PIC_APPCTRL_EVENT                      *PPEvents,
    DWORD                                  *PDwLength,
    DWORD                                   DwFirstId
)
{
    if (NULL == PPEvents || NULL == PDwLength)
    {
        return ERROR_INVALID_PARAMETER;
    }

    return GetAppCtrlEvents(PPEvents, PDwLength, DwFirstId);
}

_Use_decl_anno_impl_
VOID
IcFreeAppCtrlEventsList(
    PIC_APPCTRL_EVENT                       PEvents,
    DWORD                                   DwLength
)
{
    if (NULL == PEvents || 0 == DwLength)
    {
        return;
    }

    FreeAppCtrlEventsList(PEvents, DwLength);
}

_Use_decl_anno_impl_
DWORD
IcGetAppCtrlStatus(
    BOOLEAN                                *PBEnabled
)
{
    if (NULL == PBEnabled)
    {
        return ERROR_INVALID_PARAMETER;
    }

    return GetAppCtrlStatus(PBEnabled);
}

_Use_decl_anno_impl_
DWORD
IcGetFSscanStatus(
    BOOLEAN                                *PBEnabled
)
{
    if (NULL == PBEnabled)
    {
        return ERROR_INVALID_PARAMETER;
    }
    return GetFSscanStatus(PBEnabled);
}
