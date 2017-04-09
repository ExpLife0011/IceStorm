#include "exports.h"
#include "appctrl_scan.h"
#include "driver_io.h"

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
