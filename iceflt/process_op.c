#include "process_op.h"
#include "debug.h"
#include "tags.h"
#include "defines.h"
#include "global_data.h"

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, IceGetProcessPathByHandle)
    #pragma alloc_text(PAGE, IceGetProcessPathByPid)
    #pragma alloc_text(PAGE, IceGetUMFilePath)
    #pragma alloc_text(PAGE, IceCreateUMPath)
    #pragma alloc_text(PAGE, IceGetUMProcessPath)
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
            LogErrorNt(ntStatus, "ZwQueryInformationProcess(%d, ProcessImageFileName) instead of STATUS_INFO_LENGTH_MISMATCH", (ULONG) (ULONG_PTR) HProcess);
            if (NT_SUCCESS(ntStatus)) ntStatus = STATUS_UNSUCCESSFUL;
        }

        pUSPath = ExAllocatePoolWithTag(PagedPool, ulBufferLength, TAG_ICPP);
        if (pUSPath == NULL)
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            LogErrorNt(ntStatus, "ExAllocatePoolWithTag(%d, TAG_ICPP) for %d", ulBufferLength, (ULONG) (ULONG_PTR) HProcess);
            __leave;
        }
        RtlZeroMemory(pUSPath, ulBufferLength);

        ntStatus = ZwQueryInformationProcess(HProcess, ProcessImageFileName, pUSPath, ulBufferLength, &ulBufferLength);
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "ZwQueryInformationProcess(%d)", (ULONG) (ULONG_PTR) HProcess);
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
            LogErrorNt(ntStatus, "ZwOpenProcess(%d)", (ULONG) (ULONG_PTR) HProcessId);
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

_Use_decl_anno_impl_
NTSTATUS
IceCreateUMPath(
    PUNICODE_STRING             KMName, 
    PUNICODE_STRING             VolumeKMName,
    PUNICODE_STRING             VolumeDosName,
    PUNICODE_STRING            *DosName
)
{
    NTSTATUS            ntStatus        = STATUS_SUCCESS;
    PUNICODE_STRING     pAuxName        = NULL;
    PCHAR               pAuxBuffer      = NULL;
    USHORT              nameSize        = 0;
    USHORT              currentSize     = 0;

    PAGED_CODE();

    if (VolumeKMName->Length > KMName->Length)
    {
        LogInfo("haaaa?????");
        //__debugbreak();
        return STATUS_INVALID_PARAMETER;
    }

    if (VolumeKMName->Length != RtlCompareMemory(KMName->Buffer, VolumeKMName->Buffer, VolumeKMName->Length))
    {
        LogInfo("WTFFFFFFFF?????");
        //__debugbreak();
        return STATUS_INVALID_PARAMETER;
    }

    nameSize = (KMName->Length - VolumeKMName->Length) + VolumeDosName->Length + sizeof(WCHAR);
    
    pAuxName = ExAllocatePoolWithTag(NonPagedPool, nameSize + sizeof(WCHAR) + sizeof(UNICODE_STRING), TAG_ICUP);
    if (NULL == pAuxName)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //pAuxName->MaximumLength = nameSize + sizeof(WCHAR);
    pAuxName->Buffer = (PWSTR) (pAuxName + 1);
    pAuxBuffer = (PCHAR) pAuxName->Buffer;
    

    RtlCopyMemory(pAuxBuffer + currentSize, VolumeDosName->Buffer, VolumeDosName->Length);
    currentSize += VolumeDosName->Length;


    if (((PWCHAR) pAuxBuffer)[(currentSize / sizeof(WCHAR)) - 1] != L'\\' && KMName->Buffer[VolumeKMName->Length / sizeof(WCHAR)] != L'\\')
    {
        RtlCopyMemory(pAuxBuffer + currentSize, L"\\", sizeof(WCHAR));
        currentSize += sizeof(WCHAR);
    }
    
    RtlCopyMemory(
        pAuxBuffer + currentSize, 
        ((PCHAR) KMName->Buffer) + VolumeKMName->Length, 
        KMName->Length - VolumeKMName->Length
        );
    currentSize += (KMName->Length - VolumeKMName->Length);

    pAuxName->Length = currentSize;
    *DosName = pAuxName;

    return ntStatus;
}

_Use_decl_anno_impl_
NTSTATUS
IceGetUMFilePath(
    PFLT_CALLBACK_DATA          PData,
    PUNICODE_STRING            *PPUSUmFilePath
)
{
    NTSTATUS                    ntStatus            = STATUS_SUCCESS;
    PDEVICE_OBJECT              pDevObj             = NULL;
    UNICODE_STRING              usVolumeUMName      = { 0 };
    PUNICODE_STRING             pUSVolumeKMName     = NULL;
    PUNICODE_STRING             pUSFileKMName       = NULL;
    PFLT_FILE_NAME_INFORMATION  pFNI                = NULL;

    PAGED_CODE();

    __try
    {
        pDevObj = PData->Iopb->TargetFileObject->DeviceObject;

        if (KeAreAllApcsDisabled())
        {
            ntStatus = STATUS_UNSUCCESSFUL;
            LogErrorNt(ntStatus, "KeAreAllApcsDisabled");
            __leave;
        }

        ntStatus = IoVolumeDeviceToDosName(pDevObj, &usVolumeUMName);
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "IoVolumeDeviceToDosName");
            __leave;
        }

        ntStatus = FltGetFileNameInformation(PData, FLT_FILE_NAME_OPENED | FLT_FILE_NAME_QUERY_DEFAULT, &pFNI);
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "FltGetFileNameInformation");
            __leave;
        }

        if (pFNI == NULL || pFNI->Volume.Buffer == NULL || pFNI->Volume.Length == 0 || pFNI->Name.Buffer == NULL || pFNI->Name.Length == 0)
        {
            ntStatus = STATUS_UNSUCCESSFUL;
            LogErrorNt(ntStatus, "Incomplete data for file");
            __leave;
        }

        pUSVolumeKMName = &pFNI->Volume;
        pUSFileKMName = &pFNI->Name;
        
        ntStatus = IceCreateUMPath(pUSFileKMName, pUSVolumeKMName, &usVolumeUMName, PPUSUmFilePath);
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "IceCreateUMPath");
            __leave;
        }
    }
    __finally
    {
        if (NULL != usVolumeUMName.Buffer)
        {
            ExFreePool(usVolumeUMName.Buffer);
            usVolumeUMName.Buffer = NULL;
        }

        if (NULL != pFNI)
        {
            FltReleaseFileNameInformation(pFNI);
            pFNI = NULL;
        }

    }

    return ntStatus;
}


NTSTATUS
IceGetUMProcessPath(
    PUNICODE_STRING             PKMProcessPath,
    PUNICODE_STRING            *PPUMProcessPath
)
{
    NTSTATUS                    ntStatus            = STATUS_SUCCESS;
    OBJECT_ATTRIBUTES           objAttr             = { 0 };
    IO_STATUS_BLOCK             ioStatus            = { 0 };
    HANDLE                      hFile               = NULL;
    PFILE_OBJECT                pFileObject         = NULL;
    PFLT_VOLUME                 pVolume             = NULL;
    PUNICODE_STRING             pVolumeName         = NULL;
    ULONG                       ulVolumeNameSize    = 0;
    PUNICODE_STRING             pKMVolumeName       = NULL;

    PAGED_CODE();

    PPUMProcessPath;

    __try
    {
        InitializeObjectAttributes(&objAttr, PKMProcessPath, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

        ntStatus = FltCreateFileEx(
            gPData->PFilter, 0, &hFile, &pFileObject, GENERIC_READ | SYNCHRONIZE, &objAttr,
            &ioStatus, 0, 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE, 0, 0, IO_IGNORE_SHARE_ACCESS_CHECK
        );
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "FltCreateFileEx(%wZ)", PKMProcessPath);
            __leave;
        }

        ntStatus = FltGetVolumeFromFileObject(gPData->PFilter, pFileObject, &pVolume);
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "FltGetVolumeFromFileObject");
            __leave;
        }

        ntStatus = FltGetVolumeName(pVolume, NULL, &ulVolumeNameSize);
        if (ntStatus != STATUS_BUFFER_TOO_SMALL)
        {
            LogErrorNt(ntStatus, "FltGetVolumeName");
            __leave;
        }
        
        pKMVolumeName = ExAllocatePoolWithTag(NonPagedPool, ulVolumeNameSize, TAG_ICVN);
        if (NULL == pKMVolumeName)
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            LogErrorNt(ntStatus, "ExAllocatePoolWithTag");
            __leave;
        }

        pKMVolumeName->Length = (USHORT) ulVolumeNameSize;
        pKMVolumeName->MaximumLength = (USHORT) ulVolumeNameSize;
        pKMVolumeName->Buffer = (PWSTR) (pKMVolumeName + 1);

        ntStatus = FltGetVolumeName(pVolume, pVolumeName, 0);
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "FltGetVolumeName");
            __leave;
        }

        LogInfo("Volume name: %wZ", pVolumeName);

    }
    __finally
    {
        if (NULL != hFile)
        {
            FltClose(hFile);
            hFile = NULL;
        }

        if (NULL != pFileObject)
        {
            ObDereferenceObject(pFileObject);
            pFileObject = NULL;
        }

        if (NULL != pVolume)
        {
            FltObjectDereference(pVolume);
            pVolume = NULL;
        }
    }

    return ntStatus;
}
