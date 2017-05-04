#include "acquireforsection.h"
#include "debug.h"

VOID
IceLoadImageNotifyRoutine(
    _In_opt_    PUNICODE_STRING     FullImageName,
    _In_        HANDLE              ProcessId,
    _In_        PIMAGE_INFO         ImageInfo
);

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, IceLoadImageNotifyRoutine)
    #pragma alloc_text(INIT, IceRegisterLoadImageCallback)
#endif

VOID
IceLoadImageNotifyRoutine(
    _In_opt_    PUNICODE_STRING     PFullImageName,
    _In_        HANDLE              HProcessId,                // pid into which image is being mapped
    _In_        PIMAGE_INFO         PImageInfo
)
{
    PImageInfo, HProcessId, PFullImageName;

    PAGED_CODE();

    //LogInfo("IceLoadImageNotifyRoutine >> %wZ, %d", PFullImageName, (ULONG) (ULONG_PTR) HProcessId);
}

_Use_decl_anno_impl_
FLT_PREOP_CALLBACK_STATUS
IcePreAcquireForSection(
    PFLT_CALLBACK_DATA      PData,
    PCFLT_RELATED_OBJECTS   PFltObjects,
    PVOID                  *PCompletionContext
)
{
    PCompletionContext, PFltObjects, PData;
    //LogInfo(">> IcePreAcquireForSection: %wZ", &PData->Iopb->TargetFileObject->FileName);

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

NTSTATUS
IceRegisterLoadImageCallback(
    VOID
)
{
    return PsSetLoadImageNotifyRoutine(IceLoadImageNotifyRoutine);
}

VOID
IceCleanupLoadImage(
    VOID
)
{
    PsRemoveLoadImageNotifyRoutine(IceLoadImageNotifyRoutine);
}
