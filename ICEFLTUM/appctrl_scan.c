#include "appctrl_scan.h"
#include "driver_io.h"
#include "debug2.h"
#include <fltUser.h>
#include "linked_list.h"
#include "appctrl_rules.h"

HANDLE              gHThreadAppCtrlListener     = NULL;
PHANDLE             gPHThreadAppCtrlScanners    = NULL;
PLIST_ENTRY         gPLEAppCtrlScanRequests     = NULL;
HANDLE              gHEventAppCtrlScanReqAdded  = NULL;
CRITICAL_SECTION    gCSAppCtrlList              = { 0 };
BOOLEAN             gBAppCtrlStarted            = FALSE;

typedef struct _APPCTRL_SCAN_REQ_NODE
{
    PBYTE       PMessage;
    LIST_ENTRY  Link;

} APPCTRL_SCAN_REQ_NODE, *PAPPCTRL_SCAN_REQ_NODE;

_Success_(return == ERROR_SUCCESS)
DWORD
ScanAppCtrlPackage(
    _In_    IC_APPCTRL_RULE                        *PRule,
    _Inout_ ICE_APP_CTRL_SCAN_RESULT_PACKET        *PResultPack
)
{
    GetAppCtrlScanResult(PRule, PResultPack);

    return ERROR_SUCCESS;
}

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
                LogErrorWin(dwStatus, L"ReadScanMessage");

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
                LogErrorWin(ERROR_NOT_ENOUGH_MEMORY, L"malloc pScanReqNode");

                free(pMessage);
                pMessage = NULL;
                
                continue;
            }

            pScanReqNode->PMessage = pMessage;

            EnterCriticalSection(&gCSAppCtrlList);
            InsertTailList(gPLEAppCtrlScanRequests, &pScanReqNode->Link);
            LeaveCriticalSection(&gCSAppCtrlList);

            if (!SetEvent(gHEventAppCtrlScanReqAdded))
            {
                LogErrorWin(GetLastError(), L"SetEvent(gHEventScanReqAdded)");
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

VOID
FreeRule(
    _Inout_     IC_APPCTRL_RULE                *PRule
)
{
    if (NULL != PRule->PProcessPath)
    {
        free(PRule->PProcessPath);
        PRule->PProcessPath = NULL;
    }

    if (NULL != PRule->PParentPath)
    {
        free(PRule->PParentPath);
        PRule->PParentPath = NULL;
    }
}

VOID
BuildRuleFromScanRequest(
    _In_    ICE_APP_CTRL_SCAN_REQUEST_PACKET   *PScanRequest,
    _Out_   IC_APPCTRL_RULE                    *PRule
)
{
    DWORD dwSize = 0;

    PRule->DwPid = PScanRequest->DwPid;
    PRule->DwParentPid = PScanRequest->DwParentPid;

    dwSize = PScanRequest->DwProcessPathSize + sizeof(WCHAR);
    PRule->PProcessPath = (PWCHAR) malloc(dwSize);
    if (NULL == PRule->PProcessPath) return;
    RtlSecureZeroMemory(PRule->PProcessPath, dwSize);
    RtlCopyMemory(PRule->PProcessPath, PScanRequest->PStrings, PScanRequest->DwProcessPathSize);

    dwSize = PScanRequest->DwParentPathSize + sizeof(WCHAR);
    PRule->PParentPath = (PWCHAR) malloc(dwSize);
    if (NULL == PRule->PParentPath) return;
    RtlSecureZeroMemory(PRule->PParentPath, dwSize);
    RtlCopyMemory(
        PRule->PParentPath, 
        ((PBYTE) PScanRequest->PStrings) + PScanRequest->DwProcessPathSize, 
        PScanRequest->DwParentPathSize
    );
}

DWORD
WINAPI
ThreadAppCtrlScanner(
    _In_    LPVOID                          PParams
)
{
    DWORD                               dwStatus        = ERROR_SUCCESS;
    DWORD                               dwThreadId      = GetThreadId(GetCurrentThread());
    HANDLE                              pEvents[2]      = { 0 };
    IC_APPCTRL_RULE                     rule            = { 0 };
    PAPPCTRL_SCAN_REQ_NODE              pReqNode        = NULL;
    ICE_APP_CTRL_SCAN_RESULT_PACKET     resultPack      = { 0 };
    PFILTER_MESSAGE_HEADER              pMsgHeader      = NULL;
    PICE_GENERIC_PACKET                 pPacket         = NULL;
    PICE_APP_CTRL_SCAN_REQUEST_PACKET   pScanRequest    = NULL;

    UNREFERENCED_PARAMETER(PParams);
    UNREFERENCED_PARAMETER(dwThreadId);
    
    pEvents[0] = gHEventStop;
    pEvents[1] = gHEventAppCtrlScanReqAdded;
    
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
            LogErrorWin(GetLastError(), L"WaitForMultipleObjects");
            break;
        }

        EnterCriticalSection(&gCSAppCtrlList);
        pReqNode = CONTAINING_RECORD(RemoveHeadList(gPLEAppCtrlScanRequests), APPCTRL_SCAN_REQ_NODE, Link);
        LeaveCriticalSection(&gCSAppCtrlList);
        
        pMsgHeader = (PFILTER_MESSAGE_HEADER) pReqNode->PMessage;
        pPacket = (PICE_GENERIC_PACKET) (pMsgHeader + 1);
        pScanRequest = (PICE_APP_CTRL_SCAN_REQUEST_PACKET) (pPacket + 1);

        BuildRuleFromScanRequest(pScanRequest, &rule);

        LogInfo(L"TID %d: Process with pid: %d started. MsgID: %I64d, RepLen: %d, PackLen: %d, ReqType: %d, PPath: %s",
            dwThreadId, rule.DwPid, pMsgHeader->MessageId, pMsgHeader->ReplyLength, pPacket->DwPacketLength, pPacket->DwRequestType, rule.PProcessPath);

        // scan
        resultPack.NtScanResult = ERROR_SUCCESS;
        dwStatus = ScanAppCtrlPackage(&rule, &resultPack);
        if (dwStatus != ERROR_SUCCESS)
        {
            LogErrorWin(dwStatus, L"ScanAppCtrlPackage failed. Will send allow result.");
        }

        // trimite raspunnsul
        ReplyScanMessage(gIcComPort.HAppCtrlPort, pReqNode->PMessage, &resultPack);

        free(pReqNode->PMessage);
        pReqNode->PMessage = NULL;

        free(pReqNode);
        pReqNode = NULL;

        FreeRule(&rule);
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
        LogErrorWin(dwResult, L"StartAppCtrlScan already called");
        return dwResult;
    }

    __try
    {
        dwResult = InitAppCtrlRules();
        if (ERROR_SUCCESS != dwResult)
        {
            LogErrorWin(dwResult, L"InitAppCtrlRules");
            __leave;
        }

        // stop event
        gHEventStop = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (NULL == gHEventStop)
        {
            dwResult = GetLastError();
            LogErrorWin(dwResult, L"CreateEventW");
            __leave;
        }

        // scan req added event
        gHEventAppCtrlScanReqAdded = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (NULL == gHEventAppCtrlScanReqAdded)
        {
            dwResult = GetLastError();
            LogErrorWin(dwResult, L"CreateEvent");
            __leave;
        }

        // list mutex
        InitializeCriticalSection(&gCSAppCtrlList);

        // init scan request list
        gPLEAppCtrlScanRequests = (PLIST_ENTRY) malloc(sizeof(LIST_ENTRY));
        if (NULL == gPLEAppCtrlScanRequests)
        {
            dwResult = ERROR_NOT_ENOUGH_MEMORY;
            LogErrorWin(dwResult, L"malloc gPLEAppCtrlScanRequests");
            __leave;
        }
        InitializeListHead(gPLEAppCtrlScanRequests);

        // scanners
        gPHThreadAppCtrlScanners = (PHANDLE) malloc(sizeof(HANDLE) * APPCTRL_SCAN_NR_OF_THREADS);
        if (NULL == gPHThreadAppCtrlScanners)
        {
            dwResult = ERROR_NOT_ENOUGH_MEMORY;
            LogErrorWin(dwResult, L"malloc gPHThreadAppCtrlScanners");
            __leave;
        }
        RtlSecureZeroMemory(gPHThreadAppCtrlScanners, sizeof(HANDLE) * APPCTRL_SCAN_NR_OF_THREADS);

        for (dwIndex = 0; dwIndex < APPCTRL_SCAN_NR_OF_THREADS; dwIndex++)
        {
            gPHThreadAppCtrlScanners[dwIndex] = CreateThread(NULL, 0, ThreadAppCtrlScanner, NULL, 0, NULL);
            if (NULL == gPHThreadAppCtrlScanners[dwIndex])
            {
                dwResult = GetLastError();
                LogErrorWin(dwResult, L"CreateThread %d ThreadAppCtrlScanner", dwIndex);
                __leave;
            }
        }

        // listener
        gHThreadAppCtrlListener = CreateThread(NULL, 0, ThreadAppCtrlListener, NULL, 0, NULL);
        if (NULL == gHThreadAppCtrlListener)
        {
            dwResult = GetLastError();
            LogErrorWin(dwResult, L"CreateThread ThreadAppCtrlScan");
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
            gBAppCtrlStarted = TRUE;
            StopAppCtrlScan();
            gBAppCtrlStarted = FALSE;
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
        LogErrorWin(dwResult, L"StartAppCtrlScan was never called or StopAppCtrlScan wa already called");
        return dwResult;
    }

    dwResult = SendSetOption(ICE_FILTER_ENABLE_APPCTRL_SCAN, 0);
    if (ERROR_SUCCESS != dwResult)
    {
        LogWarningWin(dwResult, L"SendSetOption(ICE_FILTER_ENABLE_APPCTRL_SCAN, 0)");
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
            LogWarningWin(dwResult, L"WaitForSingleObject(gHThreadAppCtrl)");
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
                    LogWarningWin(dwResult, L"WaitForSingleObject(gPHThreadAppCtrlScanners[%d])", dwIndex);
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

    if (NULL != gHEventAppCtrlScanReqAdded)
    {
        CloseHandle(gHEventAppCtrlScanReqAdded);
        gHEventAppCtrlScanReqAdded = NULL;
    }

    dwResult = UninitAppCtrlRules();
    if (ERROR_SUCCESS != dwResult)
    {
        LogWarningWin(dwResult, L"UninitAppCtrlRules");
    }

    gBAppCtrlStarted = FALSE;
    return dwResult;
}
