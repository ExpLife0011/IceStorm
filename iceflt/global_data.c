#include "global_data.h"
#include "debug.h"
#include "os_version.h"

#define DEFAULT_MAXIMUM_PROCESS_CAHCE       256

PICE_GLOBAL_DATA     gPData;

_Success_(NT_SUCCESS(return))
NTSTATUS
IceInitConfigRegistry(
    _In_    PUNICODE_STRING                 PUsRegistryPath
    )
{
    if (NULL == PUsRegistryPath)
    {
        LogErrorNt(STATUS_INVALID_PARAMETER_1, "WTF Windwos??? (NULL == PUsRegistryPath)");
        return STATUS_INVALID_PARAMETER_1;
    }

    gPData->PConfigRegistryPath = ExAllocatePoolWithTag(PagedPool, PUsRegistryPath->Length + sizeof(L"\\Parameters"), TAG_ICRG);
    if (NULL == gPData->PConfigRegistryPath)
    {
        LogErrorNt(STATUS_INSUFFICIENT_RESOURCES, "ExAllocatePoolWithTag(PagedPool, %d, FSrg", (DWORD) (PUsRegistryPath->Length + sizeof(L"\\Parameters")));
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    
    memcpy(gPData->PConfigRegistryPath, PUsRegistryPath->Buffer, PUsRegistryPath->Length);
    memcpy(((PUCHAR) gPData->PConfigRegistryPath) + PUsRegistryPath->Length, L"\\Parameters", sizeof(L"\\Parameters"));
    
    return STATUS_SUCCESS;
}

VOID
IceInitSystemVersion(
    VOID
)
{ 
    gPData->DwOSVersion |= (IsWindows7OrLater()  ? OS_Windows7OrLater  : 0);
    gPData->DwOSVersion |= (IsWindows8OrLater()  ? OS_Windows8OrLater  : 0);
    gPData->DwOSVersion |= (IsWindows81OrLater() ? OS_Windows81OrLater : 0);
    gPData->DwOSVersion |= (IsWindows10OrLater() ? OS_Windows10OrLater : 0);
}

_Use_decl_anno_impl_
NTSTATUS
IceInitGlobalData(
    PDRIVER_OBJECT                          PDriverObject,
    PUNICODE_STRING                         PUsRegistryPath
)
{
    NTSTATUS                                ntStatus;

    ntStatus                                = STATUS_SUCCESS;
    
    __try
    {
        gPData = (PICE_GLOBAL_DATA) ExAllocatePoolWithTag(NonPagedPoolNx, sizeof(ICE_GLOBAL_DATA), TAG_ICGD);
        if (NULL == gPData)
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            LogErrorNt(ntStatus, "ExAllocatePoolWithTag(NonPagedPoolNx, %d, 'FSgd')", (DWORD) sizeof(ICE_GLOBAL_DATA));
            __leave;
        }
        RtlZeroMemory(gPData, sizeof(ICE_GLOBAL_DATA));


        /************************************************************************/
        /* Setez versiunea windowsului                                          */
        /************************************************************************/
        IceInitSystemVersion();
        if (0 == gPData->DwOSVersion)
        {
            ntStatus = STATUS_NOT_SUPPORTED;
            LogErrorNt(ntStatus, "This Windows is lower then 7. WTF am I doning here????");
            __leave;
        }

        LogInfo("OS_Windows7OrLater:  %d", IS_ANY_FLAG_ON(gPData->DwOSVersion, OS_Windows7OrLater));
        LogInfo("OS_Windows8OrLater:  %d", IS_ANY_FLAG_ON(gPData->DwOSVersion, OS_Windows8OrLater));
        LogInfo("OS_Windows81OrLater: %d", IS_ANY_FLAG_ON(gPData->DwOSVersion, OS_Windows81OrLater));
        LogInfo("OS_Windows10OrLater: %d", IS_ANY_FLAG_ON(gPData->DwOSVersion, OS_Windows10OrLater));
        

        /************************************************************************/
        /* Initializez resursele                                                */
        /************************************************************************/
        ExInitializeResourceLite(&gPData->EMainResource);


        /************************************************************************/
        /* setez in gPData cheia de registry Parametrs                          */
        /************************************************************************/
        ntStatus = IceInitConfigRegistry(PUsRegistryPath);
        if (NULL == gPData)
        {
            LogErrorNt(ntStatus, "InitConfigRegistry(%wZ)", PUsRegistryPath);
            __leave;
        }

        gPData->DwSystemProcessId = (DWORD) (SIZE_T) PsGetCurrentProcessId();

        gPData->BProcessCallbackSet     = FALSE;
        gPData->PDriverObject           = PDriverObject;
    }
    __finally
    {
        if (!NT_SUCCESS(ntStatus))
        {
            if (NULL != gPData)
            {
                ExFreePoolWithTag(gPData, TAG_ICGD);
                gPData = NULL;
            }
        }
    }

    return ntStatus;
}

VOID
IceCleanupGlobalData(
    VOID
)
{
    if (NULL == gPData)
    {
        return;
    }

    ExDeleteResourceLite(&gPData->EMainResource);

    if (NULL != gPData->PConfigRegistryPath)
    {
        ExFreePoolWithTag(gPData->PConfigRegistryPath, TAG_ICRG);
        gPData->PConfigRegistryPath = NULL;
    }
}

_Use_decl_anno_impl_
NTSTATUS
IceLoadSettings(
    VOID
)
{
    ULONG                                   i = 0;
    NTSTATUS                                ntStatus = STATUS_SUCCESS;

#pragma warning(push)
#pragma warning(disable : 4204)
    struct
    {
        PWSTR Name;
        PVOID Pointer;
        ULONG DefaultValue;
    } pSettings[] = {
        { L"AllowManualUnload",     &(gPData->IceSettings.UlAllowUnload),           0 },
        { L"AppCtrlScan",           &(gPData->IceSettings.BtEnableAppCtrlScan),     0 },
        { L"FSScan",                &(gPData->IceSettings.BtEnableAppCtrlScan),     0 }
    };
    RTL_QUERY_REGISTRY_TABLE pQueryTable[sizeof(pSettings) / sizeof(pSettings[0]) + 1] = { 0 };
#pragma warning(pop)    


    for (i = 0; i < sizeof(pSettings) / sizeof(pSettings[0]); i++)
    {
        pQueryTable[i].Name              = pSettings[i].Name;
        pQueryTable[i].EntryContext      = pSettings[i].Pointer;
        pQueryTable[i].Flags             = RTL_QUERY_REGISTRY_DIRECT | 0x80000000;
        pQueryTable[i].DefaultData       = &pSettings[i].DefaultValue;
        pQueryTable[i].DefaultType       = REG_DWORD;
        pQueryTable[i].DefaultLength     = sizeof(ULONG);
    }

    ntStatus = RtlQueryRegistryValues(RTL_REGISTRY_OPTIONAL, gPData->PConfigRegistryPath, pQueryTable, NULL, NULL);
    if (ntStatus == STATUS_OBJECT_NAME_NOT_FOUND)
    {
        for (i = 0; i < sizeof(pSettings) / sizeof(pSettings[0]); i++)
        {
            *(PULONG) pQueryTable[i].EntryContext = pSettings[i].DefaultValue;
        }

        ntStatus = STATUS_SUCCESS; // chiar daca nu s-a gasit chiea incarcarea a avut succes
    }

    return STATUS_SUCCESS;
}