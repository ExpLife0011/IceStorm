#include "process_routines.h"
#include "debug.h"
#include "global_data.h"
#include "ice_app_ctrl_scan.h"
#include "ice_user_common.h"


VOID
IceStartProcessCallback(
    _Inout_     PEPROCESS                   PProcess,
    _In_        HANDLE                      HProcessId,
    _Inout_opt_ PPS_CREATE_NOTIFY_INFO      PCreateInfo
);

VOID
IceStopProcessCallback(
    _Inout_     PEPROCESS                   PProcess,
    _In_        HANDLE                      HProcessId
);

VOID
IceCreateProcessCallback(
    _Inout_     PEPROCESS                   PProcess,
    _In_        HANDLE                      HProcessId,
    _Inout_opt_ PPS_CREATE_NOTIFY_INFO      PCreateInfo
);

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, IceStartProcessCallback)
    #pragma alloc_text(PAGE, IceStopProcessCallback)
    #pragma alloc_text(PAGE, IceCreateProcessCallback)
    #pragma alloc_text(INIT, IceRegisterProcessCallback)
#endif

_Use_decl_anno_impl_
VOID
IceStartProcessCallback(
    PEPROCESS                               PProcess,
    HANDLE                                  HProcessId,
    PPS_CREATE_NOTIFY_INFO                  PCreateInfo
)
{
    NTSTATUS    ntStatus        = STATUS_SUCCESS;;
    NTSTATUS    ntScanResult    = STATUS_SUCCESS;;

    PAGED_CODE();

    LogInfo("Process %wZ (%d) starting", PCreateInfo->ImageFileName, (ULONG)(ULONG_PTR) HProcessId);

    if (gPData->IceSettings.BtEnableAppCtrlScan == 0)
    {
        LogInfo("AppCtrl scanning is disabled.");
        return;
    }

    __try
    {
        LogInfo("Sending scan request to user mode for process: %wZ (%d)", PCreateInfo->ImageFileName, (ULONG)(ULONG_PTR) HProcessId);
        ntStatus = IceAppCtrlScanProcess(PProcess, HProcessId, PCreateInfo, &ntScanResult);
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "IceAppCtrlScanProcess(%wZ, %d) failed. Result will not be added to cache", PCreateInfo->ImageFileName, (ULONG) (ULONG_PTR) HProcessId);
            ntScanResult = STATUS_SUCCESS;
            __leave;
        }
    }
    __finally
    {

    }

    if (ntScanResult == IceScanVerdict_Deny) ntScanResult = STATUS_ACCESS_DENIED;
    LogInfo("[AppCtrl] Process %wZ (%d) scan result: %d (%s)", PCreateInfo->ImageFileName, (ULONG)(ULONG_PTR) HProcessId, ntScanResult, ntScanResult ? "DENY" : "ALLOW");
    PCreateInfo->CreationStatus = ntScanResult;
}

_Use_decl_anno_impl_
VOID
IceStopProcessCallback(
    PEPROCESS                               PProcess,
    HANDLE                                  HProcessId
)
{
    PAGED_CODE();
    UNREFERENCED_PARAMETER(PProcess);
    UNREFERENCED_PARAMETER(HProcessId);

    LogInfo("Process %d stoping", (ULONG)(ULONG_PTR) HProcessId);
}

// https://msdn.microsoft.com/en-us/library/ff542860(v=vs.85).aspx
_Use_decl_anno_impl_
VOID
IceCreateProcessCallback(
    PEPROCESS                               PProcess,
    HANDLE                                  HProcessId,
    PPS_CREATE_NOTIFY_INFO                  PCreateInfo
    )
{
    PAGED_CODE();

    if (gPData->BUnloading)
    {
        LogInfo("Process %s (%d) while driver is unloading", (NULL != PCreateInfo) ? "STARTING" : "STOPPING", (ULONG)(ULONG_PTR) HProcessId);
        return;
    }

    // procesul se opreste
    if (NULL == PCreateInfo)
    {
        IceStopProcessCallback(PProcess, HProcessId);
        return;
    }

    IceStartProcessCallback(PProcess, HProcessId, PCreateInfo);
}

_Use_decl_anno_impl_
NTSTATUS
IceRegisterProcessCallback(
    VOID
    )
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    ntStatus = PsSetCreateProcessNotifyRoutineEx(IceCreateProcessCallback, FALSE);
    if (!NT_SUCCESS(ntStatus))
    {
        LogErrorNt(ntStatus, "PsSetCreateProcessNotifyRoutineEx - Failed to set process start callback");
    }
    gPData->BProcessCallbackSet = NT_SUCCESS(ntStatus);

    return ntStatus;
}

VOID
IceCleanupProcessCalback(
    VOID
)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    if (gPData->BProcessCallbackSet)
    {
        ntStatus = PsSetCreateProcessNotifyRoutineEx(IceCreateProcessCallback, TRUE);
        if (!NT_SUCCESS(ntStatus))
        {
            LogWarningNt(ntStatus, "PsSetCreateProcessNotifyRoutineEx - Failed to Remove process start callback");
        }
        else
        {
            gPData->BProcessCallbackSet = FALSE;
        }
    }
}
