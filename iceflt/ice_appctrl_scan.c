#include "ice_app_ctrl_scan.h"
#include "ice_user_common.h"
#include "global_data.h"

PICE_GENERIC_PACKET
IceAllocRequest(
    _In_    ULONG                           UlSize,
    _In_    ULONG                           Tag
)
{
    PICE_GENERIC_PACKET pPack = NULL;

    pPack = (PICE_GENERIC_PACKET) ExAllocatePoolWithTag(PagedPool, UlSize, Tag);

    return pPack;
}

NTSTATUS
IceBuildScanRequest(
    _In_    PEPROCESS                           PProcess,
    _In_    HANDLE                              HProcessId,
    _In_    PPS_CREATE_NOTIFY_INFO              PCreateInfo,
    _Inout_ PICE_APP_CTRL_SCAN_REQUEST_PACKET   PScanRequest

)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    if (NULL == PProcess || NULL == HProcessId || NULL == PCreateInfo || NULL == PScanRequest)
    {
        LogErrorNt(STATUS_INVALID_PARAMETER, "(NULL == PProcess || NULL == HProcessId || NULL == PCreateInfo || NULL == PScanRequest)");
        return STATUS_INVALID_PARAMETER;
    }

    
    RtlCopyMemory(PScanRequest->PProcessPath, PCreateInfo->ImageFileName->Buffer, PCreateInfo->ImageFileName->Length);
    PScanRequest->PProcessPath[PCreateInfo->ImageFileName->Length / sizeof(WCHAR)] = 0;
    
    PScanRequest->DwProcessPathSize = PCreateInfo->ImageFileName->Length;
    PScanRequest->DwPid             = (DWORD) (SIZE_T) HProcessId;

    return ntStatus;
}

_Use_decl_anno_impl_
NTSTATUS
IceAppCtrlScanProcess(
    PEPROCESS                               PProcess,
    HANDLE                                  HProcessId,
    PPS_CREATE_NOTIFY_INFO                  PCreateInfo,
    PNTSTATUS                               PNtScanResult
)
{
    NTSTATUS                                ntStatus            = STATUS_SUCCESS;
    NTSTATUS                                ntScanResult        = STATUS_SUCCESS;
    ULONG                                   ulPacketLength      = 0;
    ULONG                                   ulResponseLength    = 0;
    PICE_GENERIC_PACKET                     pPacket             = NULL;
    LARGE_INTEGER                           liTimeout           = { 0 };
    BYTE                                    pResponseBuffer[sizeof(FILTER_REPLY_HEADER) + sizeof(ICE_GENERIC_PACKET) + sizeof(ICE_APP_CTRL_SCAN_RESULT_PACKET)] = { 0 };
    PFILTER_REPLY_HEADER                    pReplyHeader        = NULL;
    PICE_GENERIC_PACKET                     pResponsePacket     = NULL;
    PICE_APP_CTRL_SCAN_RESULT_PACKET        pResponseVerdict    = NULL;
    PICE_APP_CTRL_SCAN_REQUEST_PACKET       pScanRequest        = NULL;

    if (NULL == PProcess || NULL == HProcessId || NULL == PCreateInfo || NULL == PNtScanResult)
    {
        LogErrorNt(STATUS_INVALID_PARAMETER, "(NULL == PProcess || NULL == HProcessId || NULL == PCreateInfo || NULL == PNtScanResult)");
        return STATUS_INVALID_PARAMETER;
    }

    __try
    {
        if (NULL == gPIceComPorts || NULL == gPIceComPorts->PClientAppCtrlPort || 0 == gPIceComPorts->LNrOfConnections)
        {
            LogInfo("No client connected");
            __leave;
        }

        // construiesc pachetul pentru scanare
        ulPacketLength = sizeof(ICE_GENERIC_PACKET) + sizeof(ICE_APP_CTRL_SCAN_REQUEST_PACKET) + PCreateInfo->ImageFileName->Length + sizeof(WCHAR);
        pPacket = IceAllocRequest(ulPacketLength, TAG_ICSP);
        if (NULL == pPacket)
        {
            ntScanResult = STATUS_INSUFFICIENT_RESOURCES;
            __leave;
        }

        LogInfo("ulPacketLength: %d", ulPacketLength);
        pPacket->DwPacketLength = ulPacketLength;
        pPacket->DwRequestType = ICE_FILTER_REQUEST_SCAN_PROCESS;
        pScanRequest = (PICE_APP_CTRL_SCAN_REQUEST_PACKET) (pPacket + 1);

        ntStatus = IceBuildScanRequest(PProcess, HProcessId, PCreateInfo, pScanRequest);
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "IceBuildScanRequest");
            __leave;
        }
        
        // trimit requestul
        liTimeout.QuadPart = (-10000LL * 500); //-10000LL * 5000


        pReplyHeader = (PFILTER_REPLY_HEADER) pResponseBuffer;
        pResponsePacket = (PICE_GENERIC_PACKET) (pReplyHeader + 1);
        ulResponseLength = sizeof(pResponseBuffer);// -sizeof(FILTER_REPLY_HEADER);

        LogInfo("ulResponseLength: %d", ulResponseLength);
        ntStatus = FltSendMessage(gPData->PFilter, &gPIceComPorts->PClientAppCtrlPort, pPacket, pPacket->DwPacketLength, 
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

        pResponseVerdict = (PICE_APP_CTRL_SCAN_RESULT_PACKET) (pResponsePacket + 1);
        LogInfo("Recived status: %d", pResponseVerdict->NtScanResult);
        ntScanResult = pResponseVerdict->NtScanResult;
        // hbflt hbscan.c 358
        // hbfltum - driver_io.cpp ~210
        //gPIceComPorts->PClientAppCtrlPort;

    }
    __finally
    {
        if (NULL != pPacket)
        {
            ExFreePoolWithTag(pPacket, TAG_ICSP);
            pPacket = NULL;
        }
    }

    if (NT_SUCCESS(ntStatus))
    {
        *PNtScanResult = ntScanResult;
    }

    return ntStatus;
}
