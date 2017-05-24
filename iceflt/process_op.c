#include "process_op.h"
#include "debug.h"
#include "tags.h"
#include "defines.h"

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, IceGetProcessPathByHandle)
    #pragma alloc_text(PAGE, IceGetProcessPathByPid)
#endif

_Use_decl_anno_impl_
NTSTATUS
IceGetProcessPathByHandle(
    HANDLE                              HProcess,
    UNICODE_STRING                    **PPPath
)
{
    NTSTATUS        ntStatus        = STATUS_SUCCESS;
    ULONG           ulBufferLength  = 0;
    UNICODE_STRING *pUSPath         = NULL;

    PAGED_CODE();

    if (NULL == HProcess || NULL == PPPath)
    {
        return STATUS_INVALID_PARAMETER;
    }
    *PPPath = NULL;

    __try
    {
        ntStatus = ZwQueryInformationProcess(HProcess, ProcessImageFileName, 0, 0, &ulBufferLength);
        if (ntStatus != STATUS_INFO_LENGTH_MISMATCH)
        {
            LogErrorNt(ntStatus, "ZwQueryInformationProcess(%d, ProcessImageFileName) instead of STATUS_INFO_LENGTH_MISMATCH", (DWORD) HProcess);
            if (NT_SUCCESS(ntStatus)) ntStatus = STATUS_UNSUCCESSFUL;
        }

        pUSPath = ExAllocatePoolWithTag(PagedPool, ulBufferLength, TAG_ICPP);
        if (pUSPath == NULL)
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            LogErrorNt(ntStatus, "ExAllocatePoolWithTag(%d, TAG_ICPP) for %d", ulBufferLength, (DWORD) HProcess);
            __leave;
        }
        RtlZeroMemory(pUSPath, ulBufferLength);

        ntStatus = ZwQueryInformationProcess(HProcess, ProcessImageFileName, pUSPath, ulBufferLength, &ulBufferLength);
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "ZwQueryInformationProcess(%d)", (DWORD) HProcess);
            __leave;
        }
        *PPPath = pUSPath;
    }
    __finally
    {
        if (!NT_SUCCESS(ntStatus) && pUSPath != NULL)
        {
            ExFreePoolWithTag(pUSPath, TAG_ICPP);
            pUSPath = NULL;
            *PPPath = NULL;
        }
    }

    return ntStatus;
}

_Use_decl_anno_impl_
NTSTATUS
IceGetProcessPathByPid(
    HANDLE                              HProcessId,
    UNICODE_STRING                    **PPPath
)
{
    NTSTATUS            ntStatus    = STATUS_SUCCESS;
    HANDLE              hProcess    = NULL;
    CLIENT_ID           cid         = { 0 };
    OBJECT_ATTRIBUTES   attr        = { 0 };

    PAGED_CODE();

    __try
    {
        cid.UniqueProcess = HProcessId;
        InitializeObjectAttributes(&attr, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

        ntStatus = ZwOpenProcess(&hProcess, STANDARD_RIGHTS_READ | SYNCHRONIZE, &attr, &cid);
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "ZwOpenProcess(%d)", (DWORD) HProcessId);
            __leave;
        }

        ntStatus = IceGetProcessPathByHandle(hProcess, PPPath);
    }
    __finally
    {
        if (hProcess != NULL)
        {
            ZwClose(hProcess);
            hProcess = NULL;
        }
    }

    return ntStatus;
}
