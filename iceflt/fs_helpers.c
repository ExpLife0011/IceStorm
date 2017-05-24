#include "fs_helpers.h"
#include "global_data.h"
#include "defines.h"
#include "debug.h"

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, IceIsCsvDlEcpPresent)
    #pragma alloc_text(PAGE, IceIsPrefetchEcpPresent)
    //#pragma alloc_text(PAGE, IcePreCreateCsvfs)
    //#pragma alloc_text(PAGE, IceAddCsvRevisionECP)
    #pragma alloc_text(PAGE, IceGetFileEncrypted)
    #pragma alloc_text(PAGE, IceIsStreamAlternate)
#endif

_Use_decl_anno_impl_
BOOLEAN
IceIsCsvDlEcpPresent(
    PFLT_CALLBACK_DATA  PData
)
{
    NTSTATUS    ntStatus    = STATUS_SUCCESS;
    PECP_LIST   pEcpList    = NULL;
    PVOID       pEcpContext = NULL;
    
    PAGED_CODE();

    ntStatus = FltGetEcpListFromCallbackData(gPData->PFilter, PData, &pEcpList);
    if (!NT_SUCCESS(ntStatus) || (pEcpList == NULL))
    {
        return FALSE;
    }

    ntStatus = FltFindExtraCreateParameter(gPData->PFilter, pEcpList, &GUID_ECP_CSV_DOWN_LEVEL_OPEN, &pEcpContext, NULL);
    return NT_SUCCESS(ntStatus);
}

_Use_decl_anno_impl_
BOOLEAN
IceIsPrefetchEcpPresent(
    PFLT_CALLBACK_DATA  PData
)
{
    NTSTATUS    ntStatus        = STATUS_SUCCESS;
    PECP_LIST   pEcpList        = NULL;
    PVOID       pEcpContext     = NULL;
    
    PAGED_CODE();

    ntStatus = FltGetEcpListFromCallbackData(gPData->PFilter, PData, &pEcpList);
    if (!NT_SUCCESS(ntStatus) || (pEcpList == NULL))
    {
        return FALSE;
    }
    
    ntStatus = FltFindExtraCreateParameter(gPData->PFilter, pEcpList, &GUID_ECP_PREFETCH_OPEN, &pEcpContext, NULL);
    if (!NT_SUCCESS(ntStatus))
    {
        return FALSE;
    }

    if (!FltIsEcpFromUserMode(gPData->PFilter, pEcpContext))
    {
        return TRUE;
    }

    return FALSE;
}
//
//_Use_decl_anno_impl_
//NTSTATUS
//IcePreCreateCsvfs (
//    PFLT_CALLBACK_DATA      PData,
//    PCFLT_RELATED_OBJECTS   PFltObjects
//)
//{
//    NTSTATUS                ntStatus            = STATUS_SUCCESS;
//    PICE_INSTANCE_CONTEXT   pInstanceContext    = NULL;
//    
//    PAGED_CODE();
//    
//    ntStatus = FltGetInstanceContext(PFltObjects->Instance, &pInstanceContext);
//    if (!NT_SUCCESS(ntStatus) || NULL == pInstanceContext)
//    {
//        LogErrorNt(ntStatus, "FltGetInstanceContext");
//        return ntStatus;
//    }
//
//    if (pInstanceContext->VolumeFSType == FLT_FSTYPE_CSVFS) 
//    {
//        IceAddCsvRevisionECP(PData);
//    }
//
//    FltReleaseContext(pInstanceContext);
//    
//    return ntStatus;
//}
//
//_Use_decl_anno_impl_
//NTSTATUS 
//IceAddCsvRevisionECP(
//    PFLT_CALLBACK_DATA      PData
//)
//{
//    NTSTATUS                                ntStatus    = STATUS_SUCCESS;
//    PECP_LIST                               pEcpList    = NULL;
//    PCSV_QUERY_FILE_REVISION_ECP_CONTEXT    pEcpContext = NULL;
//
//    PAGED_CODE();
//    
//    LogInfo("AvAddCsvRevisionECP: Entered");
//    
//    __try
//    {
//        ntStatus = FltGetEcpListFromCallbackData(gPData->PFilter, PData, &pEcpList);
//        if (!NT_SUCCESS(ntStatus))
//        {
//            LogErrorNt(ntStatus, "FltGetEcpListFromCallbackData");
//            __leave;
//        }
//
//
//        if (pEcpList == NULL)
//        {
//            ntStatus = FltAllocateExtraCreateParameterList(gPData->PFilter, 0, &pEcpList);
//            if (!NT_SUCCESS(ntStatus))
//            {
//                LogErrorNt(ntStatus, "FltAllocateExtraCreateParameterList");
//                __leave;
//            }
//            
//            ntStatus = FltSetEcpListIntoCallbackData(gPData->PFilter, PData, pEcpList);
//            if (!NT_SUCCESS(ntStatus))
//            {
//                LogErrorNt(ntStatus, "FltSetEcpListIntoCallbackData");
//                FltFreeExtraCreateParameterList(gPData->PFilter, pEcpList);
//                __leave;
//            }
//        }
//        else
//        {
//            ntStatus = FltFindExtraCreateParameter(gPData->PFilter, pEcpList, &GUID_ECP_CSV_QUERY_FILE_REVISION, NULL, NULL);
//            if (ntStatus != STATUS_NOT_FOUND)
//            {
//                __leave;
//            }
//        }
//
//        ntStatus = FltAllocateExtraCreateParameter(
//            gPData->PFilter,
//            &GUID_ECP_CSV_QUERY_FILE_REVISION,
//            sizeof(CSV_QUERY_FILE_REVISION_ECP_CONTEXT),
//            0,
//            NULL,
//            TAG_ICSC,
//            &pEcpContext
//        );
//        if (!NT_SUCCESS(ntStatus))
//        {
//            LogErrorNt(ntStatus, "FltAllocateExtraCreateParameter");
//            __leave;
//        }
//        RtlZeroMemory(pEcpContext, sizeof(CSV_QUERY_FILE_REVISION_ECP_CONTEXT));
//
//        ntStatus = FltInsertExtraCreateParameter(gPData->PFilter, pEcpList, pEcpContext);
//        if (!NT_SUCCESS(ntStatus))
//        {
//            LogErrorNt(ntStatus, "FltInsertExtraCreateParameter");
//            FltFreeExtraCreateParameter(gPData->PFilter, pEcpContext);
//            __leave;
//        }
//    }
//    __finally
//    {
//        LogInfo("AvAddCsvRevisionECP : Leave, status: 0x%X", ntStatus);
//    }
//
//    return ntStatus;
//}


_Use_decl_anno_impl_
NTSTATUS
IceGetFileEncrypted(
    PFLT_INSTANCE   PInstance,
    PFILE_OBJECT    PFileObject,
    PBOOLEAN        PBEncrypted
)
{
    NTSTATUS                ntStatus    = STATUS_SUCCESS;
    FILE_BASIC_INFORMATION  basicInfo   = { 0 };

    ntStatus = FltQueryInformationFile(PInstance, PFileObject, &basicInfo, sizeof(FILE_BASIC_INFORMATION), FileBasicInformation, NULL);
    if (NT_SUCCESS(ntStatus)) 
    {
        *PBEncrypted = BooleanFlagOn(basicInfo.FileAttributes, FILE_ATTRIBUTE_ENCRYPTED);
    }

    return ntStatus;
}

_Use_decl_anno_impl_
BOOLEAN
IceIsStreamAlternate(
    PFLT_CALLBACK_DATA  PData
)
{
    NTSTATUS                    ntStatus    = STATUS_SUCCESS;
    BOOLEAN                     bAlternate  = FALSE;
    PFLT_FILE_NAME_INFORMATION  pNameInfo   = NULL;

    PAGED_CODE();
    
    __try
    {
        ntStatus = FltGetFileNameInformation(PData, FLT_FILE_NAME_OPENED | FLT_FILE_NAME_QUERY_ALWAYS_ALLOW_CACHE_LOOKUP, &pNameInfo);
        if (!NT_SUCCESS(ntStatus))
        {
            __leave;
        }

        ntStatus = FltParseFileNameInformation(pNameInfo);
        if (!NT_SUCCESS(ntStatus))
        {
            __leave;
        }

        if (pNameInfo && pNameInfo->ParentDir.Buffer && pNameInfo->FinalComponent.Buffer && pNameInfo->Stream.Buffer)
        {
            LogInfo("Dir: %wZ, FinalComponent: %wZ, Stream: %wZ, sLen: %d",
                pNameInfo->ParentDir, pNameInfo->FinalComponent, pNameInfo->Stream, pNameInfo->Stream.Length);
        }

        bAlternate = (pNameInfo->Stream.Length > 0);
    }
    __finally
    {
        if (pNameInfo != NULL)
        {
            FltReleaseFileNameInformation(pNameInfo);
            pNameInfo = NULL;
        }
    }

    return bAlternate;
}
