#include "driver_io.h"
#include "debug2.h"
#include "icecommon.h"
#include <fltUser.h>
#include "global_data.h"

IC_COMMUNICATION_PORTS                      gIcComPort = { 0 };
HANDLE                                      gHEventStop = NULL;
HANDLE                                      gHThreadAppCtrl = NULL;

_Success_(STATUS_SUCCESS == return)
DWORD
IcReplyScanMessage(
    _In_    HANDLE                          HPort,
    _In_    PBYTE                           PReadBuffer,
    _In_    PICE_APP_CTRL_SCAN_RESULT_PACKET    PResultPack
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

_Success_(STATUS_SUCCESS == return)
DWORD
IcReadScanMessage(
    _In_    PBYTE                           PBuffer,
    _Inout_ DWORD                          *PDwWritten,
    _In_    HANDLE                          HPort,
    _In_    HANDLE                          HStopEvent
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