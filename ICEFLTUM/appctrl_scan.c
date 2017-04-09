#include "appctrl_scan.h"
#include "driver_io.h"
#include "debug2.h"
#include "driver_io.h"
#include <fltUser.h>
#include "linked_list.h"

HANDLE              gHThreadAppCtrlListener     = NULL;
PHANDLE             gPHThreadAppCtrlScanners    = NULL;
PLIST_ENTRY         gPLEAppCtrlScanRequests     = NULL;
HANDLE              gHEventScanReqAdded         = NULL;
CRITICAL_SECTION    gCSAppCtrlList              = { 0 };
BOOLEAN             gBAppCtrlStarted            = FALSE;

typedef struct _APPCTRL_SCAN_REQ_NODE
{
    PBYTE       PMessage;
    LIST_ENTRY  Link;

} APPCTRL_SCAN_REQ_NODE, *PAPPCTRL_SCAN_REQ_NODE;

DWORD
WINAPI 
ThreadAppCtrlListener(
    _In_    LPVOID                          PParams
)
{
    PBYTE                   pMessage        = NULL;
    DWORD                   dwStatus        = ERROR_SUCCESS;
    DWORD                   dwWritten       = 0;
    DWORD                   dwErrCount      = 0;
    PAPPCTRL_SCAN_REQ_NODE  pScanReqNode    = NULL;
    
    UNREFERENCED_PARAMETER(PParams);

    __try
    {
        while (TRUE)
        {
            if (WaitForSingleObject(gHEventStop, 1) == STATUS_SUCCESS) break;

            pMessage = (PBYTE) malloc(MESSAGE_BUFFER_SIZE);
            if (NULL == pMessage)
            {
                LogErrorWin(ERROR_NOT_ENOUGH_MEMORY, L"memalloc");
                __leave;
            }
            ZeroMemory(pMessage, MESSAGE_BUFFER_SIZE);

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

                free(pMessage);
                pMessage = NULL;

                continue;
            }
            dwErrCount = 0;

            pScanReqNode = (PAPPCTRL_SCAN_REQ_NODE) malloc(sizeof(APPCTRL_SCAN_REQ_NODE));
            if (NULL == pScanReqNode)
            {
                LogErrorNt(ERROR_NOT_ENOUGH_MEMORY, L"malloc pScanReqNode");

                free(pMessage);
                pMessage = NULL;
                
                continue;
            }

            pScanReqNode->PMessage = pMessage;

            EnterCriticalSection(&gCSAppCtrlList);
            InsertTailList(gPLEAppCtrlScanRequests, &pScanReqNode->Link);
            LeaveCriticalSection(&gCSAppCtrlList);

            if (!SetEvent(gHEventScanReqAdded))
            {
                LogErrorNt(GetLastError(), L"SetEvent(gHEventScanReqAdded)");
                continue;
            }
        }
    }
    __finally
    {

    }

    LogInfo(L"ThreadAppCtrlListener DONE!");
    return 0;
}


DWORD
WINAPI
ThreadAppCtrlScanner(
    _In_    LPVOID                          PParams
)
{
    DWORD                           dwStatus    = ERROR_SUCCESS;
    DWORD                           dwThreadId  = GetThreadId(GetCurrentThread());
    HANDLE                          pEvents[2]  = { 0 };
    PAPPCTRL_SCAN_REQ_NODE          pReqNode    = NULL;
    ICE_APP_CTRL_SCAN_RESULT_PACKET resultPack  = { 0 };

    UNREFERENCED_PARAMETER(PParams);
    
    pEvents[0] = gHEventStop;
    pEvents[1] = gHEventScanReqAdded;
    
    LogInfo(L"ThreadAppCtrlScanner %d started", dwThreadId);

    while (TRUE)
    {
        dwStatus = WaitForMultipleObjects(2, pEvents, FALSE, INFINITE);
        if (dwStatus == WAIT_OBJECT_0)
        {
            break;
        }

        if (dwStatus != (WAIT_OBJECT_0 + 1))
        {
            LogErrorNt(GetLastError(), L"WaitForMultipleObjects");
            break;
        }

        EnterCriticalSection(&gCSAppCtrlList);
        pReqNode = CONTAINING_RECORD(RemoveHeadList(gPLEAppCtrlScanRequests), APPCTRL_SCAN_REQ_NODE, Link);
        LeaveCriticalSection(&gCSAppCtrlList);
        

        PFILTER_MESSAGE_HEADER pMsgHeader = (PFILTER_MESSAGE_HEADER) pReqNode->PMessage;
        PICE_GENERIC_PACKET pPacket = (PICE_GENERIC_PACKET) (pMsgHeader + 1);
        PICE_APP_CTRL_SCAN_REQUEST_PACKET pReq = (PICE_APP_CTRL_SCAN_REQUEST_PACKET) (pPacket + 1);

        LogInfo(L"TID %d: Process with pid: %d started. MsgID: %I64d, RepLen: %d, PackLen: %d, ReqType: %d, PPath: %s",
            dwThreadId, pReq->DwPid, pMsgHeader->MessageId, pMsgHeader->ReplyLength, pPacket->DwPacketLength, pPacket->DwRequestType, pReq->PProcessPath);

        // scan
        resultPack.NtScanResult = ERROR_SUCCESS;
        if (wcswcs(pReq->PProcessPath, L"firefox"))
        {
            resultPack.NtScanResult = ERROR_ACCESS_DENIED;
        }

        // trimite raspunnsul
        ReplyScanMessage(gIcComPort.HAppCtrlPort, pReqNode->PMessage, &resultPack);

        free(pReqNode->PMessage);
        pReqNode->PMessage = NULL;

        free(pReqNode);
        pReqNode = NULL;
    }

    
    LogInfo(L"ThreadAppCtrlScanner %d DONE", dwThreadId);
    return 0;
}


_Use_decl_anno_impl_
DWORD
StartAppCtrlScan(
    VOID
)
{
    DWORD dwResult  = ERROR_SUCCESS;
    DWORD dwIndex   = 0;

    if (gBAppCtrlStarted)
    {
        dwResult = ERROR_ALREADY_INITIALIZED;
        LogErrorNt(dwResult, L"StartAppCtrlScan already called");
        return dwResult;
    }

    __try
    {
        // stop event
        gHEventStop = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (NULL == gHEventStop)
        {
            dwResult = GetLastError();
            LogErrorNt(dwResult, L"CreateEventW");
            __leave;
        }

        // scan req added event
        gHEventScanReqAdded = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (NULL == gHEventScanReqAdded)
        {
            dwResult = GetLastError();
            LogErrorNt(dwResult, L"CreateEvent");
            __leave;
        }

        // list mutex
        InitializeCriticalSection(&gCSAppCtrlList);

        // init scan request list
        gPLEAppCtrlScanRequests = (PLIST_ENTRY) malloc(sizeof(LIST_ENTRY));
        if (NULL == gPLEAppCtrlScanRequests)
        {
            dwResult = ERROR_NOT_ENOUGH_MEMORY;
            LogErrorNt(dwResult, L"malloc gPLEAppCtrlScanRequests");
            __leave;
        }
        InitializeListHead(gPLEAppCtrlScanRequests);

        // scanners
        gPHThreadAppCtrlScanners = (PHANDLE) malloc(sizeof(HANDLE) * APPCTRL_SCAN_NR_OF_THREADS);
        if (NULL == gPHThreadAppCtrlScanners)
        {
            dwResult = ERROR_NOT_ENOUGH_MEMORY;
            LogErrorNt(dwResult, L"malloc gPHThreadAppCtrlScanners");
            __leave;
        }
        RtlSecureZeroMemory(gPHThreadAppCtrlScanners, sizeof(HANDLE) * APPCTRL_SCAN_NR_OF_THREADS);

        for (dwIndex = 0; dwIndex < APPCTRL_SCAN_NR_OF_THREADS; dwIndex++)
        {
            gPHThreadAppCtrlScanners[dwIndex] = CreateThread(NULL, 0, ThreadAppCtrlScanner, NULL, 0, NULL);
            if (NULL == gPHThreadAppCtrlScanners[dwIndex])
            {
                dwResult = GetLastError();
                LogErrorNt(dwResult, L"CreateThread %d ThreadAppCtrlScanner", dwIndex);
                __leave;
            }
        }

        // listener
        gHThreadAppCtrlListener = CreateThread(NULL, 0, ThreadAppCtrlListener, NULL, 0, NULL);
        if (NULL == gHThreadAppCtrlListener)
        {
            dwResult = GetLastError();
            LogErrorNt(dwResult, L"CreateThread ThreadAppCtrlScan");
            __leave;
        }

        // start appctrl in KM
        dwResult = SendSetOption(ICE_FILTER_ENABLE_APPCTRL_SCAN, 1);
        if (ERROR_SUCCESS != dwResult)
        {
            LogErrorWin(dwResult, L"IcSendSetOption(ICE_FILTER_ENABLE_APPCTRL_SCAN, 1)");
            __leave;
        }

        gBAppCtrlStarted = TRUE;
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
    DWORD                   dwResult    = ERROR_SUCCESS;
    DWORD                   dwIndex     = 0;
    PAPPCTRL_SCAN_REQ_NODE  pReqNode    = NULL;

    if (!gBAppCtrlStarted)
    {
        dwResult = ERROR_INIT_STATUS_NEEDED;
        LogErrorNt(dwResult, L"StartAppCtrlScan was never called or StopAppCtrlScan wa already called");
        return dwResult;
    }

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

        CloseHandle(gHThreadAppCtrlListener);
        gHThreadAppCtrlListener = NULL;
    }

    if (NULL != gPHThreadAppCtrlScanners)
    {
        for (dwIndex = 0; dwIndex < APPCTRL_SCAN_NR_OF_THREADS; dwIndex++)
        {
            if (NULL != gPHThreadAppCtrlScanners[dwIndex])
            {
                dwResult = WaitForSingleObject(gPHThreadAppCtrlScanners[dwIndex], WAIT_TIME_FOR_THREADS);
                if (WAIT_OBJECT_0 != dwResult)
                {
                    LogWarningNt(dwResult, L"WaitForSingleObject(gPHThreadAppCtrlScanners[%d])", dwIndex);
                }

                CloseHandle(gPHThreadAppCtrlScanners[dwIndex]);
                gPHThreadAppCtrlScanners[dwIndex] = NULL;
            }
        }

        free(gPHThreadAppCtrlScanners);
        gPHThreadAppCtrlScanners = NULL;
    }

    if (NULL != gPLEAppCtrlScanRequests)
    {
        // ar trebui parcursa si lista si dealocata memoria in caz ca a ramas ceva in ea

        while (!IsListEmpty(gPLEAppCtrlScanRequests))
        {
            pReqNode = CONTAINING_RECORD(RemoveHeadList(gPLEAppCtrlScanRequests), APPCTRL_SCAN_REQ_NODE, Link);
            if (NULL != pReqNode)
            {
                if (NULL != pReqNode->PMessage)
                {
                    free(pReqNode->PMessage);
                    pReqNode->PMessage = NULL;
                }

                free(pReqNode);
                pReqNode = NULL;
            }
        }

        free(gPLEAppCtrlScanRequests);
        gPLEAppCtrlScanRequests = NULL;
    }
    DeleteCriticalSection(&gCSAppCtrlList);

    if (NULL != gHEventStop)
    {
        CloseHandle(gHEventStop);
        gHEventStop = NULL;
    }

    if (NULL != gHEventScanReqAdded)
    {
        CloseHandle(gHEventScanReqAdded);
        gHEventScanReqAdded = NULL;
    }

    dwResult = SendSetOption(ICE_FILTER_ENABLE_APPCTRL_SCAN, 0);
    if (ERROR_SUCCESS != dwResult)
    {
        LogWarningNt(dwResult, L"SendSetOption(ICE_FILTER_ENABLE_APPCTRL_SCAN, 0)");
    }

    gBAppCtrlStarted = FALSE;
    return dwResult;
}
