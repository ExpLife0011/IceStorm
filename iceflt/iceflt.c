#include "debug.h"
#include "iceflt.h"
#include "registrationdata.h"
#include "global_data.h"
#include "process_routines.h"
#include "communication.h"
#include "acquireforsection.h"

DRIVER_INITIALIZE DriverEntry;

NTSTATUS
DriverEntry(
    _In_        PDRIVER_OBJECT              PDriverObject,
    _In_        PUNICODE_STRING             PUsRegistryPath
);

_Success_(NT_SUCCESS(return))
NTSTATUS
IceUnload(
    _Unreferenced_parameter_ FLT_FILTER_UNLOAD_FLAGS Flags
);

VOID
IceInitSystemVersion(
    VOID
);

_Success_(NT_SUCCESS(return))
NTSTATUS
IceInitConfigRegistry(
    _In_    PUNICODE_STRING                 PUsRegistryPath
);

_Use_decl_anno_impl_
BOOLEAN
IceVerifyWindowsVersion(
    RTL_OSVERSIONINFOEXW                    *PVersionInfo,
    ULONGLONG                               UllConditionMask
);

BOOLEAN
IsWindows7OrLater(
    VOID
);

BOOLEAN
IsWindows8OrLater(
    VOID
);

BOOLEAN
IsWindows81OrLater(
    VOID
);

BOOLEAN
IsWindows10OrLater(
    VOID
);


#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, IceUnload)
#pragma alloc_text(INIT, IceInitConfigRegistry)
#pragma alloc_text(INIT, IceInitGlobalData)
#pragma alloc_text(INIT, IceLoadSettings)
#pragma alloc_text(INIT, IceInitSystemVersion)
#pragma alloc_text(INIT, IsWindows7OrLater)
#pragma alloc_text(INIT, IsWindows8OrLater)
#pragma alloc_text(INIT, IsWindows81OrLater)
#pragma alloc_text(INIT, IsWindows10OrLater)
#pragma alloc_text(INIT, IceVerifyWindowsVersion)
#pragma alloc_text(INIT, DriverEntry)
#endif    

extern PULONG InitSafeBootMode;

VOID
IceDriverCleanup(
    VOID
)
{
    LogInfo("IceDriverCleanup called");

    if (NULL == gPData)
    {
        return;
    }

    IceCleanupCommPorts();

    IceCleanupLoadImage();

    IceCleanupProcessCalback();

    if (NULL != gPData->PFilter)
    {
        FltUnregisterFilter(gPData->PFilter);
        gPData->PFilter = NULL;
    }
                
    IceCleanupGlobalData();

    ExFreePoolWithTag(gPData, TAG_ICGD);
    gPData = NULL;
}

NTSTATUS
DriverEntry(
    _In_        PDRIVER_OBJECT              PDriverObject,
    _In_        PUNICODE_STRING             PUsRegistryPath
)
{
    NTSTATUS                                ntStatus;

    ntStatus                                = STATUS_SUCCESS;

    //DebugBreak();
    
    if (*InitSafeBootMode > 0)
    {
        return STATUS_NOT_SAFE_MODE_DRIVER;
    }

    LogInfo("\n\n----->>> HELLO <<<-----\n\n");
    LogInfo("Driver is starting");
    LogInfo("PUsRegistryPath: %wZ", PUsRegistryPath);
    LogInfo("PDriverObject->DriverName: %wZ", &PDriverObject->DriverName);

    __try
    {
        ExInitializeDriverRuntime(DrvRtPoolNxOptIn);

        ntStatus = IceInitGlobalData(PDriverObject, PUsRegistryPath);
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "IceInitGlobalData failed");
            __leave;
        }

        ntStatus = IceLoadSettings();
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "IceLoadSettings failed");
            __leave;
        }
        
        if (IS_ANY_FLAG_ON(gPData->DwOSVersion, OS_Windows8OrLater))
        {
            ntStatus = FltRegisterFilter(PDriverObject, (PFLT_REGISTRATION) &FilterRegistration_WIN8, &gPData->PFilter);
            LogInfo("FltRegisterFilter for Windos 8 or later returnd %d", ntStatus);
        }
        else
        {
            ntStatus = FltRegisterFilter(PDriverObject, (PFLT_REGISTRATION) &FilterRegistration_WIN7, &gPData->PFilter);
            LogInfo("FltRegisterFilter for Windos 7 returnd %d", ntStatus);
        }
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "FltRegisterFilter failed");
            __leave;
        }

        ntStatus = FltStartFiltering(gPData->PFilter);
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "FltStartFiltering failed");
            __leave;
        }
        
        ntStatus = IceInitCommPorts();
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "IceInitCommPorts failed");
            __leave;
        }

        ntStatus = IceRegisterProcessCallback();
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "IceRegisterProcessCallback failed");
            __leave;
        }

        ntStatus = IceRegisterLoadImageCallback();
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "IceRegisterLoadImageCallback failed");
            __leave;
        }

        InterlockedIncrement(&gPData->LnInit);
    }
    __finally
    {
        if (!NT_SUCCESS(ntStatus))
        {
            IceDriverCleanup();
        }
    }

    return ntStatus;
}

_Success_(NT_SUCCESS(return))
NTSTATUS
IceUnload(
    _Unreferenced_parameter_ FLT_FILTER_UNLOAD_FLAGS                 UlFlags
)
{
    PAGED_CODE();

    if ((gPData->IceSettings.UlAllowUnload != 1) && !(UlFlags & FLTFL_FILTER_UNLOAD_MANDATORY))
    { 
        LogInfo("--->> UlAllowUnload is not 1, Driver should not unload... in the final version");
        //return STATUS_FLT_DO_NOT_DETACH;
    }

    gPData->BUnloading = TRUE;

    LogInfo("Driver unloading with flags: 0x%x", UlFlags);

    InterlockedDecrement(&gPData->LnInit);

    IceDriverCleanup();

    LogInfo("\n\n----->>> BYE BYE <<<-----\n\n");
    return STATUS_SUCCESS;
}
