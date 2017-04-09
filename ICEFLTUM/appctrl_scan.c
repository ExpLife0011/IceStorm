#include "appctrl_scan.h"
#include "driver_io.h"
#include "debug2.h"
#include "driver_io.h"
#include <fltUser.h>

HANDLE gHThreadAppCtrlListener = NULL;

DWORD 
WINAPI 
ThreadAppCtrlScan(
    _In_    LPVOID                          PParams
)
{
    PBYTE   pMessage        = NULL;
    DWORD   dwStatus        = ERROR_SUCCESS;
    DWORD   dwWritten       = 0;
    DWORD   dwErrCount      = 0;
    ICE_APP_CTRL_SCAN_RESULT_PACKET resultPack = { 0 };

    UNREFERENCED_PARAMETER(PParams);

    __try
    {
        pMessage = (PBYTE) malloc(MESSAGE_BUFFER_SIZE);
        if (NULL == pMessage)
        {
            LogErrorWin(ERROR_NOT_ENOUGH_MEMORY, L"memalloc");
            __leave;
        }
        ZeroMemory(pMessage, MESSAGE_BUFFER_SIZE);

        while (TRUE)
        {
            dwStatus = ReadScanMessage(pMessage, &dwWritten, gIcComPort.HAppCtrlPort, gHEventStop);
            if (dwStatus == ERROR_CANCELLED)
            {
                break;
            }

            if (dwStatus != STATUS_SUCCESS)
            {
                LogErrorNt(dwStatus, L"ReadScanMessage");

                dwErrCount++;
                if (dwErrCount == 10)
                {
                    dwErrCount = 0;
                    Sleep(10);
                }
                continue;
            }
            dwErrCount = 0;

            PFILTER_MESSAGE_HEADER pMsgHeader       = (PFILTER_MESSAGE_HEADER) pMessage;
            PICE_GENERIC_PACKET pPacket             = (PICE_GENERIC_PACKET) (pMsgHeader + 1);
            PICE_APP_CTRL_SCAN_REQUEST_PACKET pReq  = (PICE_APP_CTRL_SCAN_REQUEST_PACKET) (pPacket + 1);

            LogInfo(L"Process with pid: %d started. MsgID: %I64d, RepLen: %d, PackLen: %d, ReqType: %d, PPath: %s", 
                pReq->DwPid, pMsgHeader->MessageId, pMsgHeader->ReplyLength, pPacket->DwPacketLength, pPacket->DwRequestType, pReq->PProcessPath);

            // scan
            resultPack.NtScanResult = ERROR_SUCCESS;
            if (wcswcs(pReq->PProcessPath, L"firefox"))
            {
                resultPack.NtScanResult = ERROR_ACCESS_DENIED;
            }

            // trimite raspunnsul
            ReplyScanMessage(gIcComPort.HAppCtrlPort, pMessage, &resultPack);
        }
    }
    __finally
    {
        if (NULL != pMessage)
        {
            free(pMessage);
            pMessage = NULL;
        }
    }

    return 0;
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
StartAppCtrlScan(
    VOID
)
{
    DWORD dwResult  = ERROR_SUCCESS;

    __try
    {
        gHEventStop = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (NULL == gHEventStop)
        {
            dwResult = GetLastError();
            LogErrorNt(dwResult, L"CreateEventW");
            __leave;
        }

        gHThreadAppCtrlListener = CreateThread(NULL, 0, ThreadAppCtrlScan, NULL, 0, NULL);
        if (NULL == gHThreadAppCtrlListener)
        {
            dwResult = GetLastError();
            LogErrorNt(dwResult, L"CreateThread ThreadAppCtrlScan");
            __leave;
        }

        dwResult = SendSetOption(ICE_FILTER_ENABLE_APPCTRL_SCAN, 1);
        if (ERROR_SUCCESS != dwResult)
        {
            LogErrorWin(dwResult, L"IcSendSetOption(ICE_FILTER_ENABLE_APPCTRL_SCAN, 1)");
            __leave;
        }
    }
    __finally
    {
        if (STATUS_SUCCESS != dwResult)
        {
            StopAppCtrlScan();
        }
    }

    return dwResult;
}

_Use_decl_anno_impl_
DWORD
StopAppCtrlScan(
    VOID
)
{
    DWORD dwResult = ERROR_SUCCESS;

    if (NULL != gHEventStop)
    {
        SetEvent(gHEventStop);
    }

    if (NULL != gHThreadAppCtrlListener)
    {
        dwResult = WaitForSingleObject(gHThreadAppCtrlListener, WAIT_TIME_FOR_THREADS);
        if (WAIT_OBJECT_0 != dwResult)
        {
            LogWarningNt(dwResult, L"WaitForSingleObject(gHThreadAppCtrl)");
        }
    }

    if (NULL != gHEventStop)
    {
        CloseHandle(gHEventStop);
        gHEventStop = NULL;
    }

    dwResult = SendSetOption(ICE_FILTER_ENABLE_APPCTRL_SCAN, 0);
    if (ERROR_SUCCESS != dwResult)
    {
        LogWarningNt(dwResult, L"SendSetOption(ICE_FILTER_ENABLE_APPCTRL_SCAN, 0)");
    }

    return dwResult;
}
