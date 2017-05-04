#include "fs_routines.h"
#include "debug.h"
#include "global_data.h"

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
#endif

_Use_decl_anno_impl_
FLT_PREOP_CALLBACK_STATUS
IcePreCreate(
    PFLT_CALLBACK_DATA          PData,
    PCFLT_RELATED_OBJECTS       PFltObjects,
    PVOID                      *PPCompletionContext
)
{
    FLT_PREOP_CALLBACK_STATUS retStatus = FLT_PREOP_SUCCESS_NO_CALLBACK;
    
    PData, PFltObjects, PPCompletionContext;

    PAGED_CODE();

    if (InterlockedCompareExchange(&gPData->LnInit, 1, 1) == 0)
    {
        return retStatus;
    }

    //LogInfo(">> IcePreCreate: %wZ", &Data->Iopb->TargetFileObject->FileName);
    return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

FLT_POSTOP_CALLBACK_STATUS
IcePostCreate(
    PFLT_CALLBACK_DATA          PData,
    PCFLT_RELATED_OBJECTS       PFltObjects,
    PVOID                       PCompletionContext,
    FLT_POST_OPERATION_FLAGS    Flags
)
{
    FLT_POSTOP_CALLBACK_STATUS retStatus = FLT_POSTOP_FINISHED_PROCESSING;

    PData, PFltObjects, PCompletionContext, Flags;

    PAGED_CODE();

    if (InterlockedCompareExchange(&gPData->LnInit, 1, 1) == 0)
    {
        return retStatus;
    }

    //LogInfo(">> IcePostCreate: %wZ", &Data->Iopb->TargetFileObject->FileName);
 
    return retStatus;
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