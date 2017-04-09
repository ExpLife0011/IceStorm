#include "communication.h"
#include "global_data.h"
#include "ice_user_common.h"
#include "ice_resources.h"
#include "process_list.h"

PICE_COMMUNCATION_PORTS gPIceComPorts = NULL;

USHORT PShFunctionMinimumInputBufferLength[__ICE_FILTER_MAXIMUM_COMMAND__ + 1] = { 0 };
USHORT PShFunctionMinimumOutputBufferLength[__ICE_FILTER_MAXIMUM_COMMAND__ + 1] = { 0 };

_Success_(NT_SUCCESS(return))
NTSTATUS
IceCreateCommPort(
    _In_        PFLT_PORT*                  PPServerPort,
    _In_z_      PCWSTR                      PPortName,
    _In_        PVOID                       PSecurityDescriptor,
    _In_        PVOID                       PServerPortCookie,
    _In_        PFLT_CONNECT_NOTIFY         PConnectNotifyCallback,
    _In_        PFLT_DISCONNECT_NOTIFY      PDisconnectNotifyCallback,
    _In_        PFLT_MESSAGE_NOTIFY         PMessageNotifyCallback,
    _In_        LONG                        LMaxConnections
);

_Use_decl_anno_impl_
NTSTATUS
IceInitCommPorts(
    VOID
);


VOID
IceCleanupCommPorts(
    VOID
);

_Success_(NT_SUCCESS(return))
NTSTATUS
IceConnectCallback(
    _In_        PFLT_PORT                           PClientPort,
    _In_opt_    PVOID                               PServerPortCookie,
    _In_reads_bytes_opt_(UlSizeOfContext)   PVOID   PConnectionContext,
    _In_        ULONG                               UlSizeOfContext,
    _Outptr_result_maybenull_               PVOID  *PPConnectionPortCookie
);

VOID
IceDisconnectCallback(
    _In_        PVOID                       PConnectionCookie
);

_Success_(NT_SUCCESS(return))
NTSTATUS
IceMessageCallback(
    _In_opt_    PVOID                       PPortCookie,
    _In_reads_bytes_opt_(UlInputBufferLength) PVOID PInputBuffer,
    _In_        ULONG                       UlInputBufferLength,
    _Out_writes_bytes_to_opt_(UlOutputBufferLength, *PUlReturnOutputBufferLength) PVOID POutputBuffer,
    _In_        ULONG                       UlOutputBufferLength,
    _Out_       PULONG                      PUlReturnOutputBufferLength
);

VOID
IceSetOptionMinSize(
    VOID
);

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, IceCleanupCommPorts)
#pragma alloc_text(PAGE, IceConnectCallback)
#pragma alloc_text(PAGE, IceDisconnectCallback)
#pragma alloc_text(PAGE, IceMessageCallback)
#pragma alloc_text(PAGE, IceSetOptionMinSize)
//#pragma alloc_text(PAGE, HbIsThisProcessExcluded)
//#pragma alloc_text(PAGE, HbQueryDosName)
//#pragma alloc_text(PAGE, HbGetInstanceFromFileHandle)
//#pragma alloc_text(PAGE, HbQueryName)
//#pragma alloc_text(PAGE, HbGetContextFromFileHandle)
//#pragma alloc_text(PAGE, HbQueryFileScanVerdict)
//#pragma alloc_text(PAGE, HbInvalidateScanVerdictForFile)
//#pragma alloc_text(INIT, HbSetIOMinSize)
//#pragma alloc_text(INIT, HbCreateSD)
#pragma alloc_text(INIT, IceCreateCommPort)
#pragma alloc_text(INIT, IceInitCommPorts)
#endif


_Success_(NT_SUCCESS(return))
NTSTATUS 
IceConnectCallback(
    _In_        PFLT_PORT                           PClientPort,
    _In_opt_    PVOID                               PServerPortCookie,
    _In_reads_bytes_opt_(UlSizeOfContext)   PVOID   PConnectionContext,
    _In_        ULONG                               UlSizeOfContext,
    _Outptr_result_maybenull_               PVOID  *PPConnectionPortCookie
)
{
    NTSTATUS                                ntStatus;
    PEPROCESS                               pCurrentEProcess;
    DWORD                                   dwCurrentProcessId;

    ntStatus                                = STATUS_SUCCESS;
    pCurrentEProcess                        = NULL;
    dwCurrentProcessId                      = 0;

    UNREFERENCED_PARAMETER(UlSizeOfContext);
    UNREFERENCED_PARAMETER(PConnectionContext);

    PAGED_CODE();

    pCurrentEProcess    = PsGetCurrentProcess();
    dwCurrentProcessId  = (DWORD) (SIZE_T) PsGetCurrentProcessId();

    IceAcquireResourceExclusive(&gPData->EMainResource);
    __try
    {
        // verific daca e primul client conectat
        if ((NULL == gPData->PEServiceProcess) || (0 == gPData->DwServiceProcessId))
        {
            gPData->PEServiceProcess    = pCurrentEProcess;
            gPData->DwServiceProcessId  = dwCurrentProcessId;
            LogInfo("Client with pid: %d connected!", dwCurrentProcessId);
        }

        // daca e alt proces => PaPa
        if ((dwCurrentProcessId != gPData->DwServiceProcessId) && (pCurrentEProcess != gPData->PEServiceProcess))
        {
            ntStatus = STATUS_ACCESS_DENIED;
            LogErrorNt(ntStatus, "Client with pid: %d tried to connect, but there is another client with pid: %d already connected", dwCurrentProcessId, gPData->DwServiceProcessId);
            __leave;
        }
        
        // setez portul pentru callback-ul curent
        if (PServerPortCookie == &gPIceComPorts->PClientPCPort)
        {
            gPIceComPorts->PClientPCPort = PClientPort;
            *PPConnectionPortCookie = &gPIceComPorts->PClientPCPort;
        }
        else if (PServerPortCookie == &gPIceComPorts->PClientAppCtrlPort)
        {
            gPIceComPorts->PClientAppCtrlPort = PClientPort;
            *PPConnectionPortCookie = &gPIceComPorts->PClientAppCtrlPort;
        }
        else
        {
            ntStatus = STATUS_UNSUCCESSFUL;
            LogErrorNt(ntStatus, "What??? What port is this??? Where am I?? Who are you?");
            __leave;
        }

        // incrementez nr cunexiunilor
        InterlockedIncrement(&gPIceComPorts->LNrOfConnections);
    }
    __finally
    {

    }
    IceReleaseResource(&gPData->EMainResource);

    return ntStatus;
}

VOID 
IceDisconnectCallback(
    _In_        PVOID                       PConnectionCookie
)
{
    PAGED_CODE();

    if ((NULL == gPIceComPorts) || (NULL == gPData) || (NULL == gPData->PFilter))
    {
        LogErrorNt(STATUS_DEVICE_NOT_READY, "((NULL == gPIceComPorts) || (NULL == gPData) || (NULL == gPData->PFilter))");
        return;
    }

    FltCloseClientPort(gPData->PFilter, PConnectionCookie);


    IceAcquireResourceExclusive(&gPData->EMainResource);

    if (PConnectionCookie == &gPIceComPorts->PClientPCPort)
    {
        gPIceComPorts->PClientPCPort = NULL;
    }
    else if (PConnectionCookie == &gPIceComPorts->PClientAppCtrlPort)
    {
        gPIceComPorts->PClientAppCtrlPort = NULL;
    }

    if (0 == InterlockedDecrement(&gPIceComPorts->LNrOfConnections))
    {
        gPData->DwServiceProcessId = 0;
        gPData->PEServiceProcess   = NULL;

        gPIceComPorts->PClientPCPort = NULL;
        gPIceComPorts->PClientAppCtrlPort = NULL;
        LogInfo("All ports are closed");
    }

    IceReleaseResource(&gPData->EMainResource);
    
    LogInfo("Client DISCONNECTED");
}

_Success_(NT_SUCCESS(return))
NTSTATUS 
IceMessageCallback(
    _In_opt_    PVOID                       PPortCookie,
    _In_reads_bytes_opt_(UlInputBufferLength) PVOID PInputBuffer,
    _In_        ULONG                       UlInputBufferLength,
    _Out_writes_bytes_to_opt_(UlOutputBufferLength, *PUlReturnOutputBufferLength) PVOID POutputBuffer,
    _In_        ULONG                       UlOutputBufferLength,
    _Out_       PULONG                      PUlReturnOutputBufferLength
)
{
    NTSTATUS                                ntStatus;
    PICE_GENERIC_PACKET                     pInputBuffer;
    PICE_GENERIC_PACKET                     pOutputBuffer;
    DWORD                                   dwOption;
    UINT64                                  qwValue;

    ntStatus                                = STATUS_SUCCESS;
    pInputBuffer                            = NULL;
    pOutputBuffer                           = NULL;
    dwOption                                = 0;
    qwValue                                 = 0;

    PAGED_CODE();

    PPortCookie, PInputBuffer, UlInputBufferLength, POutputBuffer, UlOutputBufferLength, PUlReturnOutputBufferLength;

    LogInfo("IceMessageCallback");
    
    __try
    {
        if (NULL == PInputBuffer || NULL == POutputBuffer || NULL == PUlReturnOutputBufferLength)
        {
            ntStatus = STATUS_INVALID_PARAMETER;
            LogErrorNt(ntStatus, "(NULL == PInputBuffer || NULL == POutputBuffer)");
            __leave;
        }

        if (UlOutputBufferLength < sizeof(ICE_GENERIC_PACKET) || UlInputBufferLength < sizeof(ICE_GENERIC_PACKET))
        {
            ntStatus = STATUS_INVALID_BUFFER_SIZE;
            LogErrorNt(ntStatus, "UlOutputBufferLength < sizeof(ICE_GENERIC_PACKET) || UlInputBufferLength < sizeof(ICE_GENERIC_PACKET)");
            __leave;
        }

        pInputBuffer = ExAllocatePoolWithQuotaTag((POOL_QUOTA_FAIL_INSTEAD_OF_RAISE | PagedPool), UlInputBufferLength, TAG_ICGP);
        pOutputBuffer = ExAllocatePoolWithQuotaTag((POOL_QUOTA_FAIL_INSTEAD_OF_RAISE | PagedPool), UlOutputBufferLength, TAG_ICGP);
        if ((NULL == pInputBuffer) || (NULL == pOutputBuffer))
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            LogErrorNt(ntStatus, "ExAllocatePoolWithQuotaTag");
            __leave;
        }

        *PUlReturnOutputBufferLength = 0;

        __try
        {
            ProbeForRead(PInputBuffer, UlInputBufferLength, 1);
            ProbeForWrite(POutputBuffer, UlOutputBufferLength, 1);

            memcpy(pInputBuffer, PInputBuffer, UlInputBufferLength);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            ntStatus = GetExceptionCode();
        }

        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "Probe failed");
            __leave;
        }
        
        dwOption = pInputBuffer->DwRequestType;
        if (dwOption >= __ICE_FILTER_MAXIMUM_COMMAND__)
        {
            ntStatus = STATUS_INVALID_DEVICE_REQUEST;
            LogErrorNt(ntStatus, "Option %d is not supported. Maximum is: %d", dwOption, __ICE_FILTER_MAXIMUM_COMMAND__);
            __leave;
        }
        LogInfo("Client process (%d) sent OPTION: %d", gPData->DwServiceProcessId, dwOption);

        if (pInputBuffer->DwPacketLength != UlInputBufferLength)
        {
            ntStatus = STATUS_INVALID_BUFFER_SIZE;
            LogErrorNt(ntStatus, "(pInputBuffer->DwPacketLength != UlInputBufferLength)");
            __leave;
        }

        if (UlInputBufferLength < PShFunctionMinimumInputBufferLength[dwOption])
        {
            ntStatus = STATUS_INVALID_BUFFER_SIZE;
            LogErrorNt(ntStatus, "(UlInputBufferLength < PShFunctionMinimumInputBufferLength[dwOption]) (%d < %d)", UlInputBufferLength, PShFunctionMinimumInputBufferLength[dwOption]);
            __leave;
        }

        if (UlOutputBufferLength < PShFunctionMinimumOutputBufferLength[dwOption])
        {
            ntStatus = STATUS_BUFFER_TOO_SMALL;
            LogErrorNt(ntStatus, "(UlOutputBufferLength < PShFunctionMinimumOutputBufferLength[dwOption]) (%d < %d)", UlOutputBufferLength, PShFunctionMinimumOutputBufferLength[dwOption]);
            __leave;
        }
        
        RtlZeroMemory(pOutputBuffer, sizeof(ICE_GENERIC_PACKET));
        pOutputBuffer->DwPacketLength = PShFunctionMinimumOutputBufferLength[dwOption];

        switch (dwOption)
        {
            case ICE_FILTER_GET_VERSION:
            {
                PICEFLT_VERSION pVersion = (PICEFLT_VERSION) (pOutputBuffer + 1);
                pVersion->DwMajor = 1;
                pVersion->DwMinor = 2;
                break;
            }

            case ICE_FILTER_ALLOW_UNLOAD:
            {
                qwValue = *((UINT64 *) (pInputBuffer + 1));
                LogInfo("Value is: %I64d", qwValue);
                gPData->IceSettings.UlAllowUnload = (ULONG) qwValue;
                break;
            }

            case ICE_FILTER_CLEAR_APPCTRL_CACHE:
            {
                IceProLstClearAll();
                LogInfo("Process List Cache for Application Control was cleared.");
                break;
            }

            case ICE_FILTER_ENABLE_APPCTRL_SCAN:
            {
                qwValue = *((UINT64 *) (pInputBuffer + 1));
                gPData->IceSettings.BtEnableAppCtrlScan = (BYTE) qwValue;
                LogInfo("Aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
                LogInfo("EnableAppCtrlScan is now set on: %d, %I64d", gPData->IceSettings.BtEnableAppCtrlScan, qwValue);
                break;
            }

            default:
            {
                ntStatus = STATUS_INVALID_DEVICE_REQUEST;
                LogErrorNt(ntStatus, "Option %d cannot be resovled.", dwOption);
                break;
            }
        }

        if (!NT_SUCCESS(ntStatus))
        {
            __leave;
        }

        try
        {
            memcpy(POutputBuffer, pOutputBuffer, pOutputBuffer->DwPacketLength);
        }
        except(EXCEPTION_EXECUTE_HANDLER)
        {
            ntStatus = GetExceptionCode();
        }
        
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "memcpy of output buffer");
            __leave;
        }

        *PUlReturnOutputBufferLength = pOutputBuffer->DwPacketLength;
    }
    __finally
    {
        if (NULL != pInputBuffer)
        {
            ExFreePoolWithTag(pInputBuffer, TAG_ICGP);
            pInputBuffer = NULL;
        }

        if (NULL != pOutputBuffer)
        {
            ExFreePoolWithTag(pOutputBuffer, TAG_ICGP);
            pOutputBuffer = NULL;
        }
    }

    return ntStatus;
}

VOID
IceSetOptionMinSize(
    VOID
)
{
    USHORT usPacketSize = sizeof(ICE_GENERIC_PACKET);

    PShFunctionMinimumInputBufferLength[ICE_FILTER_GET_VERSION] = usPacketSize;
    PShFunctionMinimumOutputBufferLength[ICE_FILTER_GET_VERSION] = usPacketSize + sizeof(ICEFLT_VERSION);

    PShFunctionMinimumInputBufferLength[ICE_FILTER_ALLOW_UNLOAD] = usPacketSize + sizeof(UINT64);
    PShFunctionMinimumOutputBufferLength[ICE_FILTER_ALLOW_UNLOAD] = usPacketSize;

    PShFunctionMinimumInputBufferLength[ICE_FILTER_CLEAR_APPCTRL_CACHE] = usPacketSize;
    PShFunctionMinimumOutputBufferLength[ICE_FILTER_CLEAR_APPCTRL_CACHE] = usPacketSize;

    PShFunctionMinimumInputBufferLength[ICE_FILTER_ENABLE_APPCTRL_SCAN] = usPacketSize;
    PShFunctionMinimumOutputBufferLength[ICE_FILTER_ENABLE_APPCTRL_SCAN] = usPacketSize;
}

_Success_(NT_SUCCESS(return))
NTSTATUS 
IceCreateCommPort(
    _In_        PFLT_PORT*                  PPServerPort, 
    _In_z_      PCWSTR                      PPortName, 
    _In_        PVOID                       PSecurityDescriptor, 
    _In_        PVOID                       PServerPortCookie,
    _In_        PFLT_CONNECT_NOTIFY         PConnectNotifyCallback, 
    _In_        PFLT_DISCONNECT_NOTIFY      PDisconnectNotifyCallback,
    _In_        PFLT_MESSAGE_NOTIFY         PMessageNotifyCallback, 
    _In_        LONG                        LMaxConnections
)
{

    UNICODE_STRING uString = { 0 };
    OBJECT_ATTRIBUTES objectAttributes = { 0 };

    RtlInitUnicodeString(&uString, PPortName);
    InitializeObjectAttributes(&objectAttributes, &uString, (OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE), NULL, PSecurityDescriptor);

    return FltCreateCommunicationPort(
        gPData->PFilter, 
        PPServerPort, 
        &objectAttributes, 
        PServerPortCookie, 
        PConnectNotifyCallback,
        PDisconnectNotifyCallback, 
        PMessageNotifyCallback, 
        LMaxConnections
    );
}


_Use_decl_anno_impl_
VOID
IceCleanupCommPorts(
    VOID
)
{
    PAGED_CODE();

    if (NULL == gPIceComPorts)
    {
        return;
    }
    

    if (NULL != gPIceComPorts->PClientAppCtrlPort)
    {
        FltCloseClientPort(gPData->PFilter, &gPIceComPorts->PClientAppCtrlPort);
        gPIceComPorts->PClientAppCtrlPort = NULL;
    }

    if (NULL != gPIceComPorts->PClientPCPort)
    {
        FltCloseClientPort(gPData->PFilter, &gPIceComPorts->PClientPCPort);
        gPIceComPorts->PClientPCPort = NULL;
    }


    if (NULL != gPIceComPorts->PProtectionControlPort)
    {
        FltCloseCommunicationPort(gPIceComPorts->PProtectionControlPort);
        gPIceComPorts->PProtectionControlPort = NULL;
    }

    if (NULL != gPIceComPorts->PAppCtrlPort)
    {
        FltCloseCommunicationPort(gPIceComPorts->PAppCtrlPort);
        gPIceComPorts->PAppCtrlPort = NULL;
    }

    ExFreePoolWithTag(gPIceComPorts, TAG_ICCM);
    gPIceComPorts = NULL;

}

_Use_decl_anno_impl_
NTSTATUS 
IceInitCommPorts(
    VOID
)
{
    PSECURITY_DESCRIPTOR                    pSecurityDescriptor;
    NTSTATUS                                ntStatus;

    pSecurityDescriptor                     = NULL;
    ntStatus                                = STATUS_SUCCESS;

    __try
    {
        gPIceComPorts = ExAllocatePoolWithTag(NonPagedPoolNx, sizeof(ICE_COMMUNCATION_PORTS), TAG_ICCM);
        if (NULL == gPIceComPorts)
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            LogErrorNt(ntStatus, "ExAllocatePoolWithTag(NonPagedPoolNx, %d, TAG_ICCM failed", sizeof(ICE_COMMUNCATION_PORTS));
            __leave;
        }
        RtlZeroMemory(gPIceComPorts, sizeof(ICE_COMMUNCATION_PORTS));
        
        ntStatus = FltBuildDefaultSecurityDescriptor(&pSecurityDescriptor, FLT_PORT_ALL_ACCESS);
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "FltBuildDefaultSecurityDescriptor(FLT_PORT_ALL_ACCESS) failed");
            __leave;
        }

        IceSetOptionMinSize();
        
        ntStatus = IceCreateCommPort(
            &gPIceComPorts->PProtectionControlPort,
            ICE_SCAN_CONTROL_PORT, 
            pSecurityDescriptor, 
            &gPIceComPorts->PClientPCPort,
            IceConnectCallback, 
            IceDisconnectCallback, 
            IceMessageCallback, 
            1
        );
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "IceCreateCommPort for %S failed", ICE_SCAN_CONTROL_PORT);
            __leave;
        }
        LogInfo("IceCreateCommPort for %S created with success", ICE_SCAN_CONTROL_PORT);


        ntStatus = IceCreateCommPort(
            &gPIceComPorts->PAppCtrlPort,
            ICE_APPCTRL_PORT,
            pSecurityDescriptor,
            &gPIceComPorts->PClientAppCtrlPort,
            IceConnectCallback,
            IceDisconnectCallback,
            NULL,
            1
        );
        if (!NT_SUCCESS(ntStatus))
        {
            LogErrorNt(ntStatus, "IceCreateCommPort for %S failed", ICE_APPCTRL_PORT);
            __leave;
        }
        LogInfo("IceCreateCommPort for %S created with success", ICE_APPCTRL_PORT);
    }
    __finally
    {

    }
    
    if (NULL != pSecurityDescriptor)
    {
        FltFreeSecurityDescriptor(pSecurityDescriptor);
        pSecurityDescriptor = NULL;
    }
        
    if(!NT_SUCCESS(ntStatus))
    {
        IceCleanupCommPorts();
    }
    
    return ntStatus;
}
