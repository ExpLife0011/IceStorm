#include "driver_io.h"
#include "debug2.h"
#include "icecommon.h"
#include <fltUser.h>
#include "global_data.h"

IC_COMMUNICATION_PORTS                      gIcComPort      = { 0 };
HANDLE                                      gHEventStop     = NULL;

_Use_decl_anno_impl_
DWORD
InitConnectionToIceFlt(
    VOID
)
{
    HRESULT hrResult = S_OK;

    __try
    {
        RtlSecureZeroMemory(&gIcComPort, sizeof(gIcComPort));

        hrResult = FilterConnectCommunicationPort(ICE_SCAN_CONTROL_PORT, 0, NULL, 0, NULL, &gIcComPort.HScanCtrlPort);
        if (S_OK != hrResult)
        {
            LogError(L"FilterConnectCommunicationPort(%s) failed with hresult: 0x08X", ICE_SCAN_CONTROL_PORT, hrResult);
            __leave;
        }

        hrResult = FilterConnectCommunicationPort(ICE_APPCTRL_PORT, 0, NULL, 0, NULL, &gIcComPort.HAppCtrlPort);
        if (S_OK != hrResult)
        {
            LogError(L"FilterConnectCommunicationPort(%s) failed with hresult: 0x08X", ICE_APPCTRL_PORT, hrResult);
            __leave;
        }
    }
    __finally
    {
        if (S_OK != hrResult)
        {
            UninitConnectionToIceFlt();
        }
    }

    return HRESULT_TO_WIN32ERROR(hrResult);
}

_Use_decl_anno_impl_
DWORD
UninitConnectionToIceFlt(
    VOID
)
{
    HRESULT hr = S_OK;
    HRESULT hrToRet = S_OK;

    if (NULL != gIcComPort.HScanCtrlPort)
    {
        hr = FilterClose(gIcComPort.HScanCtrlPort);
        if (S_OK != hr)
        {
            hrToRet = hr;
            LogWarning(L"FilterClose(HScanCtrlPort) failed, hresult: 0x%X", hr);
        }
        else
        {
            gIcComPort.HScanCtrlPort = NULL;
        }
    }

    if (NULL != gIcComPort.HAppCtrlPort)
    {
        hr = FilterClose(gIcComPort.HAppCtrlPort);
        if (S_OK != hr)
        {
            hrToRet = hr;
            LogWarning(L"FilterClose(HAppCtrlPort) failed, hresult: 0x%X", hr);
        }
        else
        {
            gIcComPort.HAppCtrlPort = NULL;
        }
    }

    return HRESULT_TO_WIN32ERROR(hrToRet);
}


_Use_decl_anno_impl_
DWORD
SendSetOption(
    DWORD                                   DwOption,
    DWORD                                   DwValue
)
{
    HRESULT                                 hr;
    DWORD                                   dwResult;
    DWORD                                   dwBytesReturned;
    BYTE                                    pRequestBuffer[sizeof(ICE_GENERIC_PACKET) + sizeof(UINT64)] = { 0 };
    PICE_GENERIC_PACKET                     pRequestHeader;
    ICE_GENERIC_PACKET                      result = { 0 };

    hr                                      = S_OK;
    dwResult                                = ERROR_SUCCESS;
    pRequestHeader                          = NULL;
    dwBytesReturned                         = 0;


    if (0 == DwOption)
    {
        LogError(L"(0 == DwOption)");
        return ERROR_INVALID_PARAMETER;
    }

    pRequestHeader = (PICE_GENERIC_PACKET) pRequestBuffer;
    *((UINT64*) (pRequestHeader + 1)) = DwValue;

    pRequestHeader->DwRequestType = DwOption;
    pRequestHeader->DwPacketLength = sizeof(pRequestBuffer);

    hr = FilterSendMessage(
        gIcComPort.HScanCtrlPort,
        pRequestHeader,
        pRequestHeader->DwPacketLength,
        &result,
        sizeof(ICE_GENERIC_PACKET),
        &dwBytesReturned
    );

    dwResult = HRESULT_TO_WIN32ERROR(hr);
    if (STATUS_SUCCESS != dwResult)
    {
        LogErrorWin(dwResult, L"FilterSendMessage for SetOption %d %d", DwOption, DwValue);
    }

    return dwResult;
}


_Use_decl_anno_impl_
DWORD
ReplyScanMessage(
    HANDLE                                  HPort,
    PBYTE                                   PReadBuffer,
    PICE_APP_CTRL_SCAN_RESULT_PACKET        PResultPack
)
{
    PFILTER_MESSAGE_HEADER  pMsgHeader      = (PFILTER_MESSAGE_HEADER) PReadBuffer;
    BYTE                    pResultBuffer[sizeof(FILTER_REPLY_HEADER) + sizeof(ICE_GENERIC_PACKET) + sizeof(ICE_APP_CTRL_SCAN_RESULT_PACKET)] = { 0 };
    DWORD                   dwReplySize     = sizeof(pResultBuffer);
    PFILTER_REPLY_HEADER    pReplyHeader    = (PFILTER_REPLY_HEADER) pResultBuffer;
    PICE_GENERIC_PACKET     pReplyPacket    = (PICE_GENERIC_PACKET) (pReplyHeader + 1);
    PICE_APP_CTRL_SCAN_RESULT_PACKET    pReplyResult = (PICE_APP_CTRL_SCAN_RESULT_PACKET) (pReplyPacket + 1);
    HRESULT                 hrResult        = S_OK;

    pReplyHeader->MessageId         = pMsgHeader->MessageId;
    pReplyHeader->Status            = STATUS_SUCCESS;

    pReplyPacket->DwPacketLength    = dwReplySize - sizeof(FILTER_REPLY_HEADER);
    pReplyPacket->DwRequestType     = ICE_FILTER_REPLY_SCAN_REQUEST_PROCESS;

    RtlCopyMemory(pReplyResult, PResultPack, sizeof(ICE_APP_CTRL_SCAN_RESULT_PACKET));
    LogInfo(L"dwReplySize: %d", dwReplySize);
    hrResult = FilterReplyMessage(HPort, pReplyHeader, dwReplySize);
    if (S_OK != hrResult)
    {
        LogErrorWin(HRESULT_TO_WIN32ERROR(hrResult), L"FilterReplyMessage");
    }

    return HRESULT_TO_WIN32ERROR(hrResult);
}

_Use_decl_anno_impl_
DWORD
ReadScanMessage(
    PBYTE                                   PBuffer,
    DWORD                                  *PDwWritten,
    HANDLE                                  HPort,
    HANDLE                                  HStopEvent
)
{
    HANDLE      pWaitHandles[2] = { 0 };
    DWORD       dwWaitStatus    = STATUS_SUCCESS;
    DWORD       dwRetVal        = STATUS_SUCCESS;
    OVERLAPPED  ov              = { 0 };

    __try
    {
        ov.hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (NULL == ov.hEvent)
        {
            dwRetVal = GetLastError();
            LogErrorWin(dwRetVal, L"CreateEventW ov.hEvent");
            __leave;
        }

        dwRetVal = HRESULT_TO_WIN32ERROR(FilterGetMessage(HPort, (PFILTER_MESSAGE_HEADER) PBuffer, MESSAGE_BUFFER_SIZE, &ov));
        if ((dwRetVal != STATUS_SUCCESS) && (dwRetVal != ERROR_IO_PENDING))
        {
            LogErrorWin(dwRetVal, L"FilterGetMessage");
            __leave;
        }

        if (dwRetVal == STATUS_SUCCESS)
        {
            __leave;
        }

        pWaitHandles[0] = HStopEvent;
        pWaitHandles[1] = ov.hEvent;

        dwWaitStatus = WaitForMultipleObjects(2, pWaitHandles, FALSE, INFINITE);
        if (dwWaitStatus == WAIT_OBJECT_0)
        {
            dwRetVal = ERROR_CANCELLED;
            CancelIo(HPort);
            __leave;
        }

        if (dwWaitStatus != (WAIT_OBJECT_0 + 1))
        {
            dwRetVal = GetLastError();
            LogErrorWin(dwRetVal, L"WaitForMultipleObjects");
            __leave;
        }

        if (!GetOverlappedResult(HPort, &ov, PDwWritten, FALSE))
        {
            dwRetVal = GetLastError();
            LogErrorWin(dwRetVal, L"GetOverlappedResult");
            __leave;
        }

        dwRetVal = STATUS_SUCCESS;
    }
    __finally
    {
        if (ov.hEvent != NULL)
        {
            CloseHandle(ov.hEvent);
        }
        ZeroMemory(&ov, sizeof(ov));
    }

    return dwRetVal;
}
