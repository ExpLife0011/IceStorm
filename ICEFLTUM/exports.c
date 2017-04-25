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
