#include "ice_fs_scan.h"
#include "debug.h"
#include "ice_user_common.h"
#include "global_data.h"
#include "communication.h"

VOID
IceBuildPreCreateFSScanRequest(
    _In_    HANDLE                          HProcessPid,
    _In_    PUNICODE_STRING                 PUSProcessPath,
    _In_    PUNICODE_STRING                 PUSFilePath,
    _In_    ULONG                           UlPreOpFlags,
    _Inout_ PICE_FS_SCAN_REQUEST_PACKET     PScanRequest
);

ULONG
IceGetFSPreCreateScanRequestSize(
    _In_    PUNICODE_STRING                 PUSProcessPath,
    _In_    PUNICODE_STRING                 PUSFilePath
);

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, IceScanFSPreCreate)
    #pragma alloc_text(PAGE, IceBuildPreCreateFSScanRequest)
    #pragma alloc_text(PAGE, IceGetFSPreCreateScanRequestSize)
#endif

ULONG
IceGetFSPreCreateScanRequestSize(
    _In_    PUNICODE_STRING                 PUSProcessPath,
    _In_    PUNICODE_STRING                 PUSFilePath
)
{
    ULONG ulSize = 0;

    PAGED_CODE();

    ulSize += sizeof(ICE_GENERIC_PACKET);
    ulSize += sizeof(ICE_FS_SCAN_REQUEST_PACKET);

    ulSize += PUSProcessPath->Length;
    ulSize += PUSFilePath->Length;

    return ulSize;
}

VOID
IceBuildPreCreateFSScanRequest(
    _In_    HANDLE                          HProcessPid,
    _In_    PUNICODE_STRING                 PUSProcessPath,
    _In_    PUNICODE_STRING                 PUSFilePath,
    _In_    ULONG                           UlPreOpFlags,
    _Inout_ PICE_FS_SCAN_REQUEST_PACKET     PScanRequest
)
{
    PAGED_CODE();

    RtlCopyMemory(PScanRequest->PStrings, PUSProcessPath->Buffer, PUSProcessPath->Length);
    RtlCopyMemory(((PCHAR) PScanRequest->PStrings) + PUSProcessPath->Length, PUSFilePath->Buffer, PUSFilePath->Length);

    PScanRequest->DwPid             = (DWORD) (SIZE_T) HProcessPid;
    PScanRequest->DwProcessPathSize = PUSProcessPath->Length;
    PScanRequest->DwFilePathSize    = PUSFilePath->Length;
    PScanRequest->DwIceFsFlags      = UlPreOpFlags;
}

_Use_decl_anno_impl_
NTSTATUS
IceScanFSPreCreate(
    HANDLE              HProcessPid,
    PUNICODE_STRING     PUSProcessPath,
    PUNICODE_STRING     PUSFilePath,
    ULONG               UlPreOpFlags,
    ULONG              *PUlNewOpFlags
)
{
    NTSTATUS                    ntStatus            = STATUS_SUCCESS;
    ULONG                       ulPacketLength      = 0;
    ULONG                       ulResponseLength    = 0;
    PICE_GENERIC_PACKET         pPacket             = NULL;
    LARGE_INTEGER               liTimeout           = { 0 };
    BYTE                        pResponseBuffer[sizeof(FILTER_REPLY_HEADER) + sizeof(ICE_GENERIC_PACKET) + sizeof(ICE_FS_SCAN_RESULT_PACKET)] = { 0 };
    PFILTER_REPLY_HEADER        pReplyHeader        = NULL;
    PICE_GENERIC_PACKET         pResponsePacket     = NULL;
    PICE_FS_SCAN_RESULT_PACKET  pResponseVerdict    = NULL;
    PICE_FS_SCAN_REQUEST_PACKET pScanRequest        = NULL;

    PAGED_CODE();

    __try
    {
        *PUlNewOpFlags = UlPreOpFlags;
        if (NULL == gPIceComPorts || NULL == gPIceComPorts->PClientAppCtrlPort || 0 == gPIceComPorts->LNrOfConnections)
        {
            __leave;
        }

        ulPacketLength = IceGetFSPreCreateScanRequestSize(PUSFilePath, PUSProcessPath);
        pPacket = (PICE_GENERIC_PACKET) ExAllocatePoolWithTag(NonPagedPool, ulPacketLength, TAG_ICFS);
        if (pPacket == NULL)
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            __leave;
        }

        pPacket->DwPacketLength = ulPacketLength;
        pPacket->DwRequestType = ICE_FILTER_REQUEST_SCAN_FS;
        pScanRequest = (PICE_FS_SCAN_REQUEST_PACKET) (pPacket + 1);

        IceBuildPreCreateFSScanRequest(HProcessPid, PUSProcessPath, PUSFilePath, UlPreOpFlags, pScanRequest);

        liTimeout.QuadPart = (-10000LL * 500); //-10000LL * 5000

        pReplyHeader = (PFILTER_REPLY_HEADER) pResponseBuffer;
        pResponsePacket = (PICE_GENERIC_PACKET) (pReplyHeader + 1);
        ulResponseLength = sizeof(pResponseBuffer);

        ntStatus = FltSendMessage(gPData->PFilter, &gPIceComPorts->PClientFSScanPort, pPacket, pPacket->DwPacketLength,
            pResponsePacket, &ulResponseLength, &liTimeout);
        if (!NT_SUCCESS(ntStatus) || ntStatus == STATUS_TIMEOUT)
        {
            LogErrorNt(ntStatus, "FltSendMessage, ulResponseLength: %d", ulResponseLength);
            __leave;
        }

        if (ulResponseLength != (sizeof(pResponseBuffer) - sizeof(FILTER_REPLY_HEADER)) || pResponsePacket->DwPacketLength != ulResponseLength)
        {
            ntStatus = STATUS_INVALID_BUFFER_SIZE;
            LogErrorNt(ntStatus, "(ulResponseLength != sizeof(pResponseBuffer) || pResponsePacket->DwPacketLength != sizeof(pResponseBuffer))");
            LogError("ulResponseLength: %d, sizeof(pResponseBuffer): %d, pResponsePacket->DwPacketLength: %d", ulResponseLength, sizeof(pResponseBuffer), pResponsePacket->DwPacketLength);
            __leave;
        }

        pResponseVerdict = (PICE_FS_SCAN_RESULT_PACKET) (pResponsePacket + 1);
        *PUlNewOpFlags = pResponseVerdict->DwIceFsNewFlags;
    }
    __finally
    {
        if (NULL != pPacket)
        {
            ExFreePoolWithTag(pPacket, TAG_ICFS);
            pPacket = NULL;
        }
    }

    return ntStatus;
}
