#include "fs_routines.h"
#include "debug.h"
#include "global_data.h"
#include "fs_helpers.h"
#include "utility.h"
#include "process_op.h"
#include "ice_user_common.h"
#include "ice_fs_scan.h"

PCHAR GET_FLAGS_STR(ULONG f)
{
    DWORD dwLen = 500;
    PCHAR s = ExAllocatePoolWithTag(NonPagedPool, dwLen, 'aaaa');
    s[0] = 0;

    if (!f)
    {
        LogInfo("WTF???");
        __debugbreak();
        sprintf_s(s, dwLen, "NONE???");
        return s;
    }
    
    if (f & ICE_FS_FLAG_CREATE) sprintf_s(s, dwLen, "%s_CREATE", s);
    if (f & ICE_FS_FLAG_OPEN) sprintf_s(s, dwLen, "%s_OPEN", s);
    if (f & ICE_FS_FLAG_READ) sprintf_s(s, dwLen, "%s_READ", s);
    if (f & ICE_FS_FLAG_WRITE) sprintf_s(s, dwLen, "%s_WRITE", s);
    if (f & ICE_FS_FLAG_DELETE) sprintf_s(s, dwLen, "%s_DELETE", s);

    return s;
}

VOID
IceGetFsScanFlags(
    _In_    ULONG       CreateDisposition,
    _In_    ULONG       DesiredAccess,
    _In_    ULONG       CreateOptions,
    _Out_   ULONG      *PFsScanFlags
);

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, IcePreCreate)
    #pragma alloc_text(PAGE, IcePostCreate)
    #pragma alloc_text(PAGE, IcePreRead)
    #pragma alloc_text(PAGE, IcePostRead)
    #pragma alloc_text(PAGE, IcePreWrite)
    #pragma alloc_text(PAGE, IcePostWrite)
    #pragma alloc_text(PAGE, IcePreCleanup)
    #pragma alloc_text(PAGE, IcePostCleanup)
    #pragma alloc_text(PAGE, IcePreSetInfo)
    #pragma alloc_text(PAGE, IcePostSetInfo)
    #pragma alloc_text(PAGE, IcePreFileSystemControl)
    #pragma alloc_text(PAGE, IcePostFileSystemControl)
    #pragma alloc_text(PAGE, IcePreMountVolume)
    #pragma alloc_text(PAGE, IceGetFsScanFlags)
#endif

VOID
IceGetFsScanFlags(
    _In_    ULONG       CreateDisposition,
    _In_    ULONG       DesiredAccess,
    _In_    ULONG       CreateOptions,
    _Out_   ULONG      *PFsScanFlags
)
{
    ULONG flags = 0;

    DesiredAccess &= (~SYNCHRONIZE);

    if (
        (DesiredAccess & (FILE_READ_DATA | FILE_READ_ATTRIBUTES | FILE_READ_EA | READ_CONTROL | FILE_EXECUTE | READ_CONTROL |
                            FILE_GENERIC_READ | FILE_GENERIC_EXECUTE | GENERIC_READ | GENERIC_EXECUTE | GENERIC_ALL | MAXIMUM_ALLOWED))
        )
    {
        flags |= ICE_FS_FLAG_READ;
    }

    if (
        (DesiredAccess & (FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA | FILE_APPEND_DATA |
                            WRITE_DAC | WRITE_OWNER | FILE_GENERIC_WRITE | GENERIC_WRITE | GENERIC_ALL | MAXIMUM_ALLOWED))
        )
    {
        flags |= ICE_FS_FLAG_WRITE;
    }

    if (
        (DesiredAccess & (DELETE | MAXIMUM_ALLOWED)) ||
        (CreateDisposition == FILE_SUPERSEDE || CreateDisposition == FILE_OVERWRITE || CreateDisposition == FILE_OVERWRITE_IF) ||
        (CreateOptions & (FILE_DELETE_ON_CLOSE))
        )
    {
        flags |= ICE_FS_FLAG_DELETE;
    }

    if (
        CreateDisposition == FILE_SUPERSEDE || 
        CreateDisposition == FILE_CREATE || 
        CreateDisposition == FILE_OPEN_IF || 
        CreateDisposition == FILE_OVERWRITE ||
        CreateDisposition == FILE_OVERWRITE_IF
        )
    {
        flags |= ICE_FS_FLAG_CREATE;
    }

    if (
        CreateDisposition == FILE_OPEN || 
        CreateDisposition == FILE_OPEN_IF || 
        CreateDisposition == FILE_OVERWRITE_IF
        )
    {
        flags |= ICE_FS_FLAG_OPEN;
    }

    *PFsScanFlags = flags;
}

_Use_decl_anno_impl_
FLT_PREOP_CALLBACK_STATUS
IcePreCreate(
    PFLT_CALLBACK_DATA          PData,
    PCFLT_RELATED_OBJECTS       PFltObjects,
    PVOID                      *PPCompletionContext
)
{
    NTSTATUS                    ntStatus                = STATUS_SUCCESS;
    NTSTATUS                    ntRetVal                = FLT_PREOP_SUCCESS_NO_CALLBACK;
    ULONG_PTR                   stackLow                = { 0 };
    ULONG_PTR                   stackHigh               = { 0 };
    PFILE_OBJECT                pFileObject             = PData->Iopb->TargetFileObject;
    PUNICODE_STRING             pUSFilePath             = &pFileObject->FileName;
    PEPROCESS                   pECurrentProcess        = NULL;
    HANDLE                      hProcessPid             = 0;
    PUNICODE_STRING             pUSProcessPath          = NULL;
    ULONG                       ulPreOpFlags            = 0;
    ULONG                       ulCreateOptions         = 0;
    ULONG                       ulCreateDisposition     = 0;
    ULONG                       ulDesiredAccess         = 0;
    ULONG                       ulNewOpFlags            = 0;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(PPCompletionContext);

    ulCreateDisposition = PData->Iopb->Parameters.Create.Options >> 24;
    ulDesiredAccess = PData->Iopb->Parameters.Create.SecurityContext->DesiredAccess;
    ulCreateOptions = PData->Iopb->Parameters.Create.Options;

    if (InterlockedCompareExchange(&gPData->LnInit, 1, 1) == 0 || gPData->BUnloading)
    {
        return FLT_PREOP_SUCCESS_NO_CALLBACK;
    }

    IoGetStackLimits(&stackLow, &stackHigh);
    if (((ULONG_PTR) pFileObject > stackLow) && ((ULONG_PTR) pFileObject < stackHigh))
    {
        return FLT_PREOP_SUCCESS_NO_CALLBACK;
    }

    if (gPData->PEServiceProcess == NULL || gPData->DwServiceProcessId == 0  || gPData->IceSettings.BtEnableFSScan == 0)
    {
        return FLT_PREOP_SUCCESS_NO_CALLBACK;
    }

    pECurrentProcess = PsGetCurrentProcess();
    if (pECurrentProcess == gPData->PEServiceProcess)
    {
        return FLT_PREOP_SUCCESS_NO_CALLBACK;
    }

    if (
        (pFileObject == NULL) ||
        FlagOn(PData->Iopb->Parameters.Create.Options, FILE_DIRECTORY_FILE) ||
        FlagOn(PData->Iopb->OperationFlags, SL_OPEN_PAGING_FILE) ||
        FlagOn(PFltObjects->FileObject->Flags, FO_VOLUME_OPEN) ||
        IceIsCsvDlEcpPresent(PData) ||
        (pUSFilePath == NULL || pUSFilePath->Length == 0 || pUSFilePath->Buffer == NULL)
        )
    {
        return FLT_PREOP_SUCCESS_NO_CALLBACK;
    }

    if (IceIsPrefetchEcpPresent(PData))
    {
        //SetFlag(pStreamHandleContext.Flags, ICE_FLAG_PREFETCH);
    }

    __try
    {
        hProcessPid = PsGetProcessId(pECurrentProcess);

        ntStatus = IceGetProcessPathByPid(hProcessPid, &pUSProcessPath);
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "IceGetProcessPathByPid: %d", (ULONG) (ULONG_PTR) hProcessPid);
            __leave;
        }

        IceGetFsScanFlags(ulCreateDisposition, ulDesiredAccess, ulCreateOptions, &ulPreOpFlags);

        UNICODE_STRING pus2 = { 0 };
        pus2.Buffer = L"\\test.txt";
        pus2.Length = 18;
        pus2.MaximumLength = 18;
        if (RtlEqualUnicodeString(pUSFilePath, &pus2, TRUE))
        {
            PCHAR s = GET_FLAGS_STR(ulPreOpFlags);
            LogInfo(">>> File: %wZ, Proc: %wZ, pid: %d, flags: %s", pUSFilePath, pUSProcessPath, hProcessPid, s);
            ExFreePoolWithTag(s, 'aaaa');
        }
        
        ntStatus = IceScanFSPreCreate(
            hProcessPid,
            pUSProcessPath,
            pUSFilePath,
            ulPreOpFlags,
            &ulNewOpFlags
        );
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "IceScanFSPreCreate, file: %wZ, proc: %wZ", pUSFilePath, pUSFilePath);
            __leave;
        }

        if (ulPreOpFlags == ulNewOpFlags) __leave;
        
        LogInfo("**** File: %wZ, Pr: %wZ, orig: %d, new: %d", pUSFilePath, pUSProcessPath, ulPreOpFlags, ulNewOpFlags);

        //IcePreCreateCsvfs(PData, PFltObjects);
        ntRetVal = FLT_PREOP_SYNCHRONIZE;
    }
    __finally
    {
        if (pUSProcessPath != NULL)
        {
            ExFreePoolWithTag(pUSProcessPath, TAG_ICPP);
            pUSProcessPath = NULL;
        }
    }
    
    return ntRetVal;
}

FLT_POSTOP_CALLBACK_STATUS
IcePostCreate(
    PFLT_CALLBACK_DATA          PData,
    PCFLT_RELATED_OBJECTS       PFltObjects,
    PVOID                       PCompletionContext,
    FLT_POST_OPERATION_FLAGS    Flags
)
{

    UNREFERENCED_PARAMETER(PCompletionContext);
    UNREFERENCED_PARAMETER(Flags);
    UNREFERENCED_PARAMETER(PData);
    UNREFERENCED_PARAMETER(PFltObjects);

    //LogInfo(">> IcePostCreate: %wZ", pUSFileName);
    
    return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
IcePreRead(
    PFLT_CALLBACK_DATA          PData,
    PCFLT_RELATED_OBJECTS       PFltObjects,
    PVOID                      *PPCompletionContext
)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(PData);
    UNREFERENCED_PARAMETER(PFltObjects);
    UNREFERENCED_PARAMETER(PPCompletionContext);

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}


FLT_POSTOP_CALLBACK_STATUS
IcePostRead(
    PFLT_CALLBACK_DATA          PData,
    PCFLT_RELATED_OBJECTS       PFltObjects,
    PVOID                       PCompletionContext,
    FLT_POST_OPERATION_FLAGS    Flags
)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(PData);
    UNREFERENCED_PARAMETER(PFltObjects);
    UNREFERENCED_PARAMETER(PCompletionContext);
    UNREFERENCED_PARAMETER(Flags);

    return FLT_POSTOP_FINISHED_PROCESSING;
}


FLT_PREOP_CALLBACK_STATUS
IcePreWrite(
    PFLT_CALLBACK_DATA          PData,
    PCFLT_RELATED_OBJECTS       PFltObjects,
    PVOID                      *PPCompletionContext
)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(PData);
    UNREFERENCED_PARAMETER(PFltObjects);
    UNREFERENCED_PARAMETER(PPCompletionContext);

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}


FLT_POSTOP_CALLBACK_STATUS
IcePostWrite(
    PFLT_CALLBACK_DATA          PData,
    PCFLT_RELATED_OBJECTS       PFltObjects,
    PVOID                       PCompletionContext,
    FLT_POST_OPERATION_FLAGS    Flags
)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(PData);
    UNREFERENCED_PARAMETER(PFltObjects);
    UNREFERENCED_PARAMETER(PCompletionContext);
    UNREFERENCED_PARAMETER(Flags);

    return FLT_POSTOP_FINISHED_PROCESSING;
}


FLT_PREOP_CALLBACK_STATUS
IcePreCleanup(
    PFLT_CALLBACK_DATA          PData,
    PCFLT_RELATED_OBJECTS       PFltObjects,
    PVOID                      *PPCompletionContext
)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(PData);
    UNREFERENCED_PARAMETER(PFltObjects);
    UNREFERENCED_PARAMETER(PPCompletionContext);

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}


FLT_POSTOP_CALLBACK_STATUS
IcePostCleanup(
    PFLT_CALLBACK_DATA          PData,
    PCFLT_RELATED_OBJECTS       PFltObjects,
    PVOID                       PCompletionContext,
    FLT_POST_OPERATION_FLAGS    Flags
)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(PData);
    UNREFERENCED_PARAMETER(PFltObjects);
    UNREFERENCED_PARAMETER(PCompletionContext);
    UNREFERENCED_PARAMETER(Flags);

    return FLT_POSTOP_FINISHED_PROCESSING;
}


FLT_PREOP_CALLBACK_STATUS
IcePreSetInfo(
    PFLT_CALLBACK_DATA          PData,
    PCFLT_RELATED_OBJECTS       PFltObjects,
    PVOID                      *PPCompletionContext
)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(PData);
    UNREFERENCED_PARAMETER(PFltObjects);
    UNREFERENCED_PARAMETER(PPCompletionContext);

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}


FLT_POSTOP_CALLBACK_STATUS
IcePostSetInfo(
    PFLT_CALLBACK_DATA          PData,
    PCFLT_RELATED_OBJECTS       PFltObjects,
    PVOID                       PCompletionContext,
    FLT_POST_OPERATION_FLAGS    Flags
)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(PData);
    UNREFERENCED_PARAMETER(PFltObjects);
    UNREFERENCED_PARAMETER(PCompletionContext);
    UNREFERENCED_PARAMETER(Flags);

    return FLT_POSTOP_FINISHED_PROCESSING;
}


FLT_PREOP_CALLBACK_STATUS
IcePreFileSystemControl(
    PFLT_CALLBACK_DATA          PData,
    PCFLT_RELATED_OBJECTS       PFltObjects,
    PVOID                      *PPCompletionContext
)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(PData);
    UNREFERENCED_PARAMETER(PFltObjects);
    UNREFERENCED_PARAMETER(PPCompletionContext);

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}


FLT_POSTOP_CALLBACK_STATUS
IcePostFileSystemControl(
    PFLT_CALLBACK_DATA          PData,
    PCFLT_RELATED_OBJECTS       PFltObjects,
    PVOID                       PCompletionContext,
    FLT_POST_OPERATION_FLAGS    Flags
)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(PData);
    UNREFERENCED_PARAMETER(PFltObjects);
    UNREFERENCED_PARAMETER(PCompletionContext);
    UNREFERENCED_PARAMETER(Flags);

    return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
IcePreMountVolume(
    PFLT_CALLBACK_DATA          PData,
    PCFLT_RELATED_OBJECTS       PFltObjects,
    PVOID                      *PPCompletionContext
)
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(PData);
    UNREFERENCED_PARAMETER(PFltObjects);
    UNREFERENCED_PARAMETER(PPCompletionContext);

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}