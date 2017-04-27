#include "debug.h"
#include "process_list.h"

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, IceProLstInitialize)
    #pragma alloc_text(PAGE, IceProLstUninitialize)
    #pragma alloc_text(PAGE, IceProLstClearAll)
    #pragma alloc_text(PAGE, IceProLstAddProcess)
#endif

typedef struct _ICE_PROCESS_LIST
{
    KGUARDED_MUTEX                          Mutex;
    volatile WORD                           WMaxCount;
    volatile WORD                           WCount;
    PLIST_ENTRY                             PHead;
} ICE_PROCESS_LIST, *PICE_PROCESS_LIST;

typedef struct _ICE_PROCESS_INFO_NODE
{
    PWCHAR                                  PPath;
    LIST_ENTRY                              Link;
} ICE_PROCESS_INFO_NODE, *PICE_PROCESS_INFO_NODE;

PICE_PROCESS_LIST                           gPProcCache;

_Use_decl_anno_impl_
NTSTATUS
IceProLstInitialize(
    WORD                                    WMaximumCacheSize
)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    PAGED_CODE();

    __try
    {
        gPProcCache = ExAllocatePoolWithTag(PagedPool, sizeof(ICE_PROCESS_LIST), TAG_ICPL);
        if (NULL == gPProcCache)
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            LogErrorNt(ntStatus, "ExAllocatePoolWithTag(PagedPool, %d, 'ICpl') for process cache", sizeof(ICE_PROCESS_LIST));
            __leave;
        }
        RtlZeroMemory(gPProcCache, sizeof(ICE_PROCESS_LIST));

        KeInitializeGuardedMutex(&gPProcCache->Mutex);

        gPProcCache->PHead = ExAllocatePoolWithTag(PagedPool, sizeof(LIST_ENTRY), TAG_ICPH);
        if (NULL == gPProcCache->PHead)
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            LogErrorNt(ntStatus, "ExAllocatePoolWithTag(PagedPool, %d, 'ICph') for process cache", sizeof(LIST_ENTRY));
            __leave;
        }
        InitializeListHead(gPProcCache->PHead);

        gPProcCache->WMaxCount = WMaximumCacheSize;
    }
    __finally
    {
        if (!NT_SUCCESS(ntStatus))
        {
            IceProLstUninitialize();
        }
    }
    
    return ntStatus;
}

_Use_decl_anno_impl_
VOID
IceProLstUninitialize(
    VOID
)
{
    PAGED_CODE();

    if (NULL == gPProcCache)
    {
        return;
    }
    
    KeAcquireGuardedMutex(&gPProcCache->Mutex);
    if (NULL != gPProcCache->PHead)
    {
        // parcurgere lista.. bla bla bla

        ExFreePoolWithTag(gPProcCache->PHead, TAG_ICPH);
        gPProcCache->PHead = NULL;
    }
    gPProcCache->WCount = 0;
    gPProcCache->WMaxCount = 0;
    KeReleaseGuardedMutex(&gPProcCache->Mutex);

    ExFreePoolWithTag(gPProcCache, TAG_ICPL);
    gPProcCache = NULL;
}

_Use_decl_anno_impl_
VOID
IceProLstClearAll(
    VOID
)
{
    PAGED_CODE();

    if (NULL == gPProcCache)
    {
        return;
    }
}

_Use_decl_anno_impl_
NTSTATUS
IceProLstAddProcess(
    PWCHAR                                  PPath
)
{
    NTSTATUS                ntStatus        = STATUS_SUCCESS;
    PICE_PROCESS_INFO_NODE  pProcInfoNode   = NULL;

    PAGED_CODE();
    
    pProcInfoNode;

    if (NULL == gPProcCache)
    {
        return STATUS_DEVICE_NOT_READY;
    }

    if (NULL == PPath)
    {
        ntStatus = STATUS_INVALID_PARAMETER_1;
        LogErrorNt(ntStatus, "(NULL == PPath)");
        return ntStatus;
    }

    __try
    {
        //pProcInfoNode = ExAllocatePoolWithTag()


    }
    __finally
    {

    }

    return ntStatus;
}