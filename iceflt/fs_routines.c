#include "fs_routines.h"
#include "debug.h"
#include "global_data.h"
#include "fs_helpers.h"
#include "utility.h"
#include "process_op.h"
#include "ice_user_common.h"
#include "ice_fs_scan.h"

volatile LONG counter = 0;

typedef struct _ICE_PRE2POST_CREATE_CONTEXT
{
    ULONG           UlOldCreateDisposition;
    ULONG           UlOldDesiredAccess;
    ULONG           UlOldCreateOptions;

    ULONG           UlNewCreateDisposition;
    ULONG           UlNewDesiredAccess;
    ULONG           UlNewCreateOptions;

    BOOLEAN         BDeny;

} ICE_PRE2POST_CREATE_CONTEXT, *PICE_PRE2POST_CREATE_CONTEXT;

VOID
IceGetFsScanFlags(
    _In_    ULONG       CreateDisposition,
    _In_    ULONG       DesiredAccess,
    _In_    ULONG       CreateOptions,
    _Out_   ULONG      *PFsScanFlags
);
VOID
IceGetNewFileFlags(
    _In_        ULONG       UlPreOpFlags,
    _In_        ULONG       UlNewOpFlags,
    _In_        ULONG       UlDesiredAccess,
    _In_        ULONG       UlCreateOptions,
    _In_        ULONG       UlCreateDisposition,
    _In_        ULONG      *PUlNewDesiredAccess,
    _In_        ULONG      *PUlNewCreateOptions,
    _In_        ULONG      *PUlNewCreateDisposition,
    _Inout_     BOOLEAN    *DenyOp

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
    #pragma alloc_text(PAGE, IceGetNewFileFlags)
#endif


VOID
IceGetNewFileFlags(
    _In_        ULONG       PreOpFlags, 
    _In_        ULONG       NewOpFlags, 
    _In_        ULONG       DesiredAccess, 
    _In_        ULONG       CreateOptions, 
    _In_        ULONG       CreateDisposition,
    _Inout_     ULONG      *NewDesiredAccess, 
    _Inout_     ULONG      *NewCreateOptions, 
    _Inout_     ULONG      *NewCreateDisposition,
    _Inout_     BOOLEAN    *DenyOp
)
{
    ULONG newDesiredAccess          = 0;
    ULONG newCreateOptions          = 0;
    ULONG newCreateDisposition      = 0;
    ULONG synchronize               = 0;
    ULONG readFlags                 = (FILE_READ_DATA | FILE_READ_ATTRIBUTES | FILE_READ_EA | READ_CONTROL | FILE_EXECUTE | 
                                        READ_CONTROL | FILE_GENERIC_READ | FILE_GENERIC_EXECUTE | GENERIC_READ | GENERIC_EXECUTE);
    ULONG writeFlags                = (FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA | FILE_APPEND_DATA | WRITE_DAC | 
                                        WRITE_OWNER | FILE_GENERIC_WRITE | GENERIC_WRITE);
    BOOLEAN requieredAll            = FALSE;
    BOOLEAN deny                    = FALSE;

    PAGED_CODE();

    PreOpFlags;

    //__debugbreak();

    if (
        (!(NewOpFlags & (ICE_FS_FLAG_CREATE | ICE_FS_FLAG_OPEN))) ||
        (!(NewOpFlags & (ICE_FS_FLAG_READ | ICE_FS_FLAG_WRITE | ICE_FS_FLAG_DELETE)))
        )
    {
        *DenyOp = TRUE;
        return;
    }

    synchronize = DesiredAccess & SYNCHRONIZE;
    DesiredAccess &= (~SYNCHRONIZE);

    
    if (DesiredAccess & FILE_ALL_ACCESS)
    {
        newDesiredAccess = (DesiredAccess & (~FILE_ALL_ACCESS));
        
        if (NewOpFlags & ICE_FS_FLAG_READ) newDesiredAccess |= (FILE_GENERIC_READ | FILE_GENERIC_EXECUTE);
        if (NewOpFlags & ICE_FS_FLAG_WRITE) newDesiredAccess |= FILE_GENERIC_WRITE;

        requieredAll = TRUE;
    }
    
    if (DesiredAccess & MAXIMUM_ALLOWED)
    {
        newDesiredAccess = (DesiredAccess & (~MAXIMUM_ALLOWED));

        if (NewOpFlags & ICE_FS_FLAG_READ) newDesiredAccess |= (FILE_GENERIC_READ | FILE_GENERIC_EXECUTE);
        if (NewOpFlags & ICE_FS_FLAG_WRITE) newDesiredAccess |= FILE_GENERIC_WRITE;
        if (NewOpFlags & ICE_FS_FLAG_DELETE) newDesiredAccess |= DELETE;
        
        requieredAll = TRUE;
    }


    if (!requieredAll) newDesiredAccess = DesiredAccess;
    newCreateOptions = CreateOptions;
    newCreateDisposition = CreateDisposition;


    if (!(NewOpFlags & ICE_FS_FLAG_READ)) newDesiredAccess &= (~readFlags);
    
    if (!(NewOpFlags & ICE_FS_FLAG_WRITE))
    {
        newDesiredAccess &= (~writeFlags);

        if (newCreateDisposition == FILE_OVERWRITE) newCreateDisposition = FILE_OPEN;
        if (newCreateDisposition == FILE_OVERWRITE_IF) newCreateDisposition = FILE_OPEN_IF;
        if (newCreateDisposition == FILE_SUPERSEDE) newCreateDisposition = FILE_OPEN_IF;
    }

    if (!(NewOpFlags & ICE_FS_FLAG_DELETE))
    {
        newDesiredAccess &= (~DELETE);

        newCreateOptions &= (~FILE_DELETE_ON_CLOSE);

        if (newCreateDisposition == FILE_SUPERSEDE) newCreateDisposition = FILE_OPEN_IF;
        if (newCreateDisposition == FILE_OVERWRITE) newCreateDisposition = FILE_OPEN;
        if (newCreateDisposition == FILE_OVERWRITE_IF) newCreateDisposition = FILE_OPEN_IF;
    }


    if (!(NewOpFlags & ICE_FS_FLAG_CREATE))
    {
        if (newCreateDisposition == FILE_CREATE)
        {
            deny = TRUE;
        }
        else if (newCreateDisposition == FILE_OPEN_IF)
        {
            newCreateDisposition = FILE_OPEN;
        }
        else if ((newCreateDisposition == FILE_OVERWRITE_IF) || (newCreateDisposition == FILE_SUPERSEDE))
        {
            newCreateDisposition = FILE_OVERWRITE;
        }
    }

    if (!deny && !(NewOpFlags & ICE_FS_FLAG_OPEN))
    {
        if (
            (newCreateDisposition == FILE_OPEN) ||
            (newCreateDisposition == FILE_OVERWRITE)
            )
        {
            deny = TRUE;
        }
        else if (
            (newCreateDisposition == FILE_OPEN_IF) ||
            (newCreateDisposition == FILE_SUPERSEDE) ||
            (newCreateDisposition == FILE_OVERWRITE_IF)
            )
        {
            if (!(NewOpFlags & ICE_FS_FLAG_CREATE)) deny = TRUE;
            else newCreateDisposition = FILE_CREATE;
        }
    }

    if (newDesiredAccess == 0) deny = TRUE;

    *NewDesiredAccess       = (newDesiredAccess | synchronize);
    *NewCreateOptions       = newCreateOptions;
    *NewCreateDisposition   = newCreateDisposition;

    *DenyOp                 = deny;
}

VOID
IceGetFsScanFlags(
    _In_    ULONG       CreateDisposition,
    _In_    ULONG       DesiredAccess,
    _In_    ULONG       CreateOptions,
    _Out_   ULONG      *PFsScanFlags
)
{
    ULONG flags = 0;

    PAGED_CODE();

    DesiredAccess &= (~SYNCHRONIZE);
    
    if (
        (DesiredAccess & (FILE_READ_DATA | FILE_READ_ATTRIBUTES | FILE_READ_EA | READ_CONTROL | FILE_EXECUTE | READ_CONTROL |
                            FILE_GENERIC_READ | FILE_GENERIC_EXECUTE | GENERIC_READ | GENERIC_EXECUTE | GENERIC_ALL | MAXIMUM_ALLOWED | FILE_ALL_ACCESS))
        )
    {
        flags |= ICE_FS_FLAG_READ;
    }

    if (
        (DesiredAccess & (FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA | FILE_APPEND_DATA |
                            WRITE_DAC | WRITE_OWNER | FILE_GENERIC_WRITE | GENERIC_WRITE | GENERIC_ALL | MAXIMUM_ALLOWED | FILE_ALL_ACCESS)) ||
        (CreateDisposition == FILE_OVERWRITE) || (CreateDisposition == FILE_OVERWRITE_IF)
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
        (CreateDisposition == FILE_SUPERSEDE) || 
        (CreateDisposition == FILE_CREATE) || 
        (CreateDisposition == FILE_OPEN_IF) || 
        (CreateDisposition == FILE_OVERWRITE_IF)
        )
    {
        flags |= ICE_FS_FLAG_CREATE;
    }

    if (
        (CreateDisposition == FILE_SUPERSEDE) ||
        (CreateDisposition == FILE_OPEN) ||
        (CreateDisposition == FILE_OPEN_IF) ||
        (CreateDisposition == FILE_OVERWRITE) ||
        (CreateDisposition == FILE_OVERWRITE_IF)
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
    NTSTATUS                        ntStatus                = STATUS_SUCCESS;
    NTSTATUS                        ntRetVal                = FLT_PREOP_SUCCESS_NO_CALLBACK;
    ULONG_PTR                       stackLow                = { 0 };
    ULONG_PTR                       stackHigh               = { 0 };
    PFILE_OBJECT                    pFileObject             = PData->Iopb->TargetFileObject;
    PUNICODE_STRING                 pUSFilePath             = &pFileObject->FileName;
    PEPROCESS                       pECurrentProcess        = NULL;
    HANDLE                          hProcessPid             = 0;
    PUNICODE_STRING                 pUSProcessPath          = NULL;
    PUNICODE_STRING                 pUMFilePath             = NULL;
    PUNICODE_STRING                 pUMProcPath             = NULL;
    ULONG                           ulPreOpFlags            = 0;
    ULONG                           ulNewOpFlags            = 0;
    ULONG                           ulCreateOptions         = 0;
    ULONG                           ulCreateDisposition     = 0;
    ULONG                           ulDesiredAccess         = 0;
    ULONG                           ulNewCreateOptions      = 0;
    ULONG                           ulNewCreateDisposition  = 0;
    ULONG                           ulNewDesiredAccess      = 0;
    BOOLEAN                         bDenyOperation          = FALSE;
    PICE_PRE2POST_CREATE_CONTEXT    pPre2Post               = NULL;

    PAGED_CODE();

    *PPCompletionContext = NULL;

    if (InterlockedCompareExchange(&gPData->LnInit, 1, 1) == 0 || gPData->BUnloading)
    {
        return FLT_PREOP_SUCCESS_NO_CALLBACK;
    }

    ulCreateDisposition = PData->Iopb->Parameters.Create.Options >> 24;
    ulDesiredAccess = PData->Iopb->Parameters.Create.SecurityContext->DesiredAccess;
    ulCreateOptions = PData->Iopb->Parameters.Create.Options;

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
        FlagOn(PData->Iopb->OperationFlags, SL_OPEN_TARGET_DIRECTORY | SL_OPEN_PAGING_FILE) ||
        FlagOn(PFltObjects->FileObject->Flags, FO_VOLUME_OPEN) ||
        FlagOn(ulCreateOptions, FILE_OPEN_BY_FILE_ID) ||
        IceIsCsvDlEcpPresent(PData) ||
        IceIsPrefetchEcpPresent(PData) ||
        (pUSFilePath == NULL || pUSFilePath->Length == 0 || pUSFilePath->Buffer == NULL)
        )
    {
        return FLT_PREOP_SUCCESS_NO_CALLBACK;
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

        ntStatus = IceGetUMFilePath(PData, &pUMFilePath);
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "GetUMFilePath(%wZ)", pUSFilePath);
            __leave;
        }

        IceGetFsScanFlags(ulCreateDisposition, ulDesiredAccess, ulCreateOptions, &ulPreOpFlags);

        ntStatus = IceScanFSPreCreate(
            hProcessPid,
            pUSProcessPath,
            pUMFilePath,
            ulPreOpFlags,
            &ulNewOpFlags
        );
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "IceScanFSPreCreate, file: %wZ, proc: %wZ", pUSFilePath, pUSFilePath);
            __leave;
        }

        if (ulPreOpFlags == ulNewOpFlags) __leave;


        IceGetNewFileFlags(
            ulPreOpFlags, ulNewOpFlags, ulDesiredAccess, ulCreateOptions, ulCreateDisposition,
            &ulNewDesiredAccess, &ulNewCreateOptions, &ulNewCreateDisposition, &bDenyOperation
        );

#if DBG
        ULONG c = InterlockedAdd(&counter, 1);

        LogInfo("%d) *********************************************", c);
        LogInfo("%d) PP: [%wZ]", c, pUSProcessPath);
        LogInfo("%d) PF: [%wZ]", c, pUSFilePath);
        LogInfo("%d) deny: %d", c, bDenyOperation);
        LogInfo("%d) of: %d, nf: %d", c, ulPreOpFlags, ulNewOpFlags);
        LogInfo("%d) of: %d, nf: %d", c, ulPreOpFlags, ulNewOpFlags);
        LogInfo("%d) cd: %d, ncd: %d", c, ulCreateDisposition, ulNewCreateDisposition);
        LogInfo("%d) co: %d, nco: %d", c, ulCreateOptions, ulNewCreateOptions);
        LogInfo("%d) da: %d, nda: %d", c, ulDesiredAccess, ulNewDesiredAccess);
        LogInfo("%d) --------------------------------------------", c);
#endif // DBG


        if (ulCreateDisposition == FILE_OVERWRITE_IF)
        {
            LogWarningNt(PData->IoStatus.Status, "Skipping file: [%wZ], Proc: [%wZ], ", pUSFilePath, pUSProcessPath);

            pPre2Post = ExAllocatePoolWithTag(NonPagedPoolNx, sizeof(ICE_PRE2POST_CREATE_CONTEXT), TAG_IC2P);
            if (NULL == pPre2Post) __leave;


            pPre2Post->UlOldDesiredAccess = ulDesiredAccess;
            pPre2Post->UlOldCreateOptions = ulCreateOptions;
            pPre2Post->UlOldCreateDisposition = ulCreateDisposition;

            pPre2Post->UlNewDesiredAccess = ulNewDesiredAccess;
            pPre2Post->UlNewCreateOptions = ulNewCreateOptions;
            pPre2Post->UlNewCreateDisposition = ulNewCreateDisposition;

            pPre2Post->BDeny = bDenyOperation;

            ntRetVal = FLT_PREOP_SUCCESS_WITH_CALLBACK;
            __leave;
        }

        if (bDenyOperation)
        {
            PData->IoStatus.Status = STATUS_ACCESS_DENIED;
            PData->IoStatus.Information = 0;
            ntRetVal = FLT_PREOP_COMPLETE;
            __leave;
        }
        
        ulNewCreateOptions = (ulNewCreateOptions & 0x00FFFFFF) | (ulNewCreateDisposition << 24);
        PData->Iopb->Parameters.Create.Options = ulNewCreateOptions;
        PData->Iopb->Parameters.Create.SecurityContext->DesiredAccess = ulNewDesiredAccess;
        PData->Iopb->Parameters.Create.SecurityContext->AccessState->RemainingDesiredAccess = ulNewDesiredAccess;

        FltSetCallbackDataDirty(PData);

        ntRetVal = FLT_PREOP_SUCCESS_NO_CALLBACK;
    }
    __finally
    {
        if (pUSProcessPath != NULL)
        {
            ExFreePoolWithTag(pUSProcessPath, TAG_ICPP);
            pUSProcessPath = NULL;
        }

        if (NULL != pUMFilePath)
        {
            ExFreePoolWithTag(pUMFilePath, TAG_ICUP);
            pUMFilePath = NULL;
        }

        if (NULL != pUMProcPath)
        {
            ExFreePoolWithTag(pUMProcPath, TAG_ICUP);
            pUMProcPath = NULL;
        }
    }
    
    *PPCompletionContext = pPre2Post;

    return ntRetVal;
}

_Use_decl_anno_impl_
FLT_POSTOP_CALLBACK_STATUS
IcePostCreate(
    PFLT_CALLBACK_DATA          PData,
    PCFLT_RELATED_OBJECTS       PFltObjects,
    PVOID                       PCompletionContext,
    FLT_POST_OPERATION_FLAGS    Flags
)
{
    PICE_PRE2POST_CREATE_CONTEXT    pPre2Post   = NULL;
    NTSTATUS                        ntRetVal    = FLT_POSTOP_FINISHED_PROCESSING;
    
    PAGED_CODE();

    if (PCompletionContext == NULL) return ntRetVal;

    if (FlagOn(Flags, FLTFL_POST_OPERATION_DRAINING))
    {
        ExFreePoolWithTag(pPre2Post, TAG_IC2P);
        pPre2Post = NULL;
        return ntRetVal;
    }

    
    pPre2Post = PCompletionContext;

    if (pPre2Post->BDeny)
    {
        PData->IoStatus.Status = STATUS_ACCESS_DENIED;
        PData->IoStatus.Information = 0;

        FltCancelFileOpen(PFltObjects->Instance, PFltObjects->FileObject);
        LogInfo("Canceled: %wZ", &PData->Iopb->TargetFileObject->FileName);
    }
    else
    {
        pPre2Post->UlNewCreateOptions = pPre2Post->UlNewCreateOptions | (pPre2Post->UlNewCreateDisposition << 24);
        PData->Iopb->Parameters.Create.Options = pPre2Post->UlNewCreateOptions;
        PData->Iopb->Parameters.Create.SecurityContext->DesiredAccess = pPre2Post->UlNewDesiredAccess;
        PData->Iopb->Parameters.Create.SecurityContext->AccessState->RemainingDesiredAccess = pPre2Post->UlNewDesiredAccess;

        FltSetCallbackDataDirty(PData);
    }

    ExFreePoolWithTag(pPre2Post, TAG_IC2P);
    pPre2Post = NULL;

    return ntRetVal;
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