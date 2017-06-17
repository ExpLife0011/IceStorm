#include "fs_scan.h"
#include "driver_io.h"
#include "debug2.h"
#include "linked_list.h"
#include "icecommon.h"
#include "fs_rules.h"
#include <fltUser.h>

HANDLE              gHThreadFSListener          = NULL;
PHANDLE             gPHThreadFSScanners         = NULL;
PLIST_ENTRY         gPLEFSScanRequests          = NULL;
HANDLE              gHEventFSScanReqAdded       = NULL;
HANDLE              gHEventFSStop               = NULL;
CRITICAL_SECTION    gCSFSScanList               = { 0 };
BOOLEAN             gBFSScanStarted             = FALSE;

typedef struct _FS_SCAN_REQ_NODE
{
    PBYTE       PMessage;
    LIST_ENTRY  Link;

} FS_SCAN_REQ_NODE, *PFS_SCAN_REQ_NODE;


_Success_(return == ERROR_SUCCESS)
DWORD
ScanFSPackage(
    _In_    ULONG                           UlOriginalFlags,
    _In_    IC_FS_RULE                     *PRule,
    _Inout_ ICE_FS_SCAN_RESULT_PACKET      *PResultPack
)
{
    GetFSScanResult(UlOriginalFlags, PRule, PResultPack);

    return ERROR_SUCCESS;
}


VOID
FreeFSScanRule(
    _Inout_     IC_FS_RULE                *PRule
)
{
    if (NULL != PRule->PProcessPath)
    {
        free(PRule->PProcessPath);
        PRule->PProcessPath = NULL;
    }

    if (NULL != PRule->PFilePath)
    {
        free(PRule->PFilePath);
        PRule->PFilePath = NULL;
    }
}

VOID
ConvertPathToDOSPath(
    _In_z_  PWCHAR      *Path
)
{
    DWORD       idxVol          = 0;
    BOOLEAN     bFound          = FALSE;
    DWORD       dwPrefixLen     = 0;
    PWCHAR      pDosVolume      = NULL;
    DWORD       dwDosVolumeLen  = 0;
    DWORD       dwSize          = 0;
    DWORD       dwLen           = 0;
    PWCHAR      pAxuPath        = NULL;
    
    for (idxVol = 0; idxVol < gDwNrOfVolumes; idxVol++)
    {
        if (0 == memcmp(*Path, gVolumes[idxVol].PDevice, gVolumes[idxVol].LenDevice * sizeof(WCHAR)))
        {
            dwPrefixLen = gVolumes[idxVol].LenDevice;
            pDosVolume = gVolumes[idxVol].PPath;
            dwDosVolumeLen = gVolumes[idxVol].LenPath;
            bFound = TRUE;
            break;
        }

        if (0 == memcmp(*Path, gVolumes[idxVol].PVolume, gVolumes[idxVol].LenVolume * sizeof(WCHAR)))
        {
            dwPrefixLen = gVolumes[idxVol].LenVolume;
            pDosVolume = gVolumes[idxVol].PPath;
            dwDosVolumeLen = gVolumes[idxVol].LenPath;
            bFound = TRUE;
            break;
        }
    }

    if (!bFound) return;

    dwLen = (DWORD) wcslen(*Path);
    dwSize = (DWORD) ((dwLen + 3) * sizeof(WCHAR));
    if (NULL == (pAxuPath = malloc(dwSize))) return;
    RtlSecureZeroMemory(pAxuPath, dwSize);

    memcpy_s(pAxuPath, dwSize, pDosVolume, dwDosVolumeLen * sizeof(WCHAR));
    memcpy_s(
        ((PBYTE) pAxuPath) + (dwDosVolumeLen * sizeof(WCHAR)), 
        dwSize - (dwDosVolumeLen * sizeof(WCHAR)), 
        ((PBYTE) *Path) + (dwPrefixLen * sizeof(WCHAR)), 
        (dwLen - dwPrefixLen)* sizeof(WCHAR)
    );

    free(*Path);
    *Path = pAxuPath;
}

VOID
BuildFSScanRuleFromScanRequest(
    _In_    ICE_FS_SCAN_REQUEST_PACKET     *PScanRequest,
    _Out_   IC_FS_RULE                     *PRule
)
{
    DWORD dwSize = 0;

    PRule->DwPid = PScanRequest->DwPid;
    
    dwSize = PScanRequest->DwProcessPathSize + sizeof(WCHAR);
    PRule->PProcessPath = (PWCHAR) malloc(dwSize);
    if (NULL == PRule->PProcessPath) return;
    RtlSecureZeroMemory(PRule->PProcessPath, dwSize);
    RtlCopyMemory(PRule->PProcessPath, PScanRequest->PStrings, PScanRequest->DwProcessPathSize);

    ConvertPathToDOSPath(&PRule->PProcessPath);

    dwSize = PScanRequest->DwFilePathSize + sizeof(WCHAR);
    PRule->PFilePath = (PWCHAR) malloc(dwSize);
    if (NULL == PRule->PFilePath) return;
    RtlSecureZeroMemory(PRule->PFilePath, dwSize);
    RtlCopyMemory(
        PRule->PFilePath,
        ((PBYTE) PScanRequest->PStrings) + PScanRequest->DwProcessPathSize,
        PScanRequest->DwFilePathSize
    );
}

DWORD
WINAPI
ThreadFSScanner(
    _In_    LPVOID                          PParams
)
{
    DWORD                               dwStatus        = ERROR_SUCCESS;
    DWORD                               dwThreadId      = GetThreadId(GetCurrentThread());
    HANDLE                              pEvents[2]      = { 0 };
    IC_FS_RULE                          rule            = { 0 };
    PFS_SCAN_REQ_NODE                   pReqNode        = NULL;
    ICE_FS_SCAN_RESULT_PACKET           resultPack      = { 0 };
    PFILTER_MESSAGE_HEADER              pMsgHeader      = NULL;
    PICE_GENERIC_PACKET                 pPacket         = NULL;
    PICE_FS_SCAN_REQUEST_PACKET         pScanRequest    = NULL;

    UNREFERENCED_PARAMETER(PParams);
    UNREFERENCED_PARAMETER(dwThreadId);

    pEvents[0] = gHEventFSStop;
    pEvents[1] = gHEventFSScanReqAdded;

    LogInfo(L"ThreadFSScanner %d started", dwThreadId);

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

        EnterCriticalSection(&gCSFSScanList);
        pReqNode = CONTAINING_RECORD(RemoveHeadList(gPLEFSScanRequests), FS_SCAN_REQ_NODE, Link);
        LeaveCriticalSection(&gCSFSScanList);

        pMsgHeader = (PFILTER_MESSAGE_HEADER) pReqNode->PMessage;
        pPacket = (PICE_GENERIC_PACKET) (pMsgHeader + 1);
        pScanRequest = (PICE_FS_SCAN_REQUEST_PACKET) (pPacket + 1);

        RtlSecureZeroMemory(&rule, sizeof(rule));
        BuildFSScanRuleFromScanRequest(pScanRequest, &rule);

        resultPack.DwIceFsNewFlags = pScanRequest->DwIceFsFlags;
        dwStatus = ScanFSPackage(pScanRequest->DwIceFsFlags, &rule, &resultPack);
        if (dwStatus != ERROR_SUCCESS)
        {
            LogErrorWin(dwStatus, L"ScanFSPackage failed. Will send allow result.");
        }

        ReplyFSScanMessage(gIcComPort.HFSScanPort, pReqNode->PMessage, &resultPack);

        free(pReqNode->PMessage);
        pReqNode->PMessage = NULL;

        free(pReqNode);
        pReqNode = NULL;

        FreeFSScanRule(&rule);
    }

    LogInfo(L"ThreadFSScanner %d DONE", dwThreadId);
    return 0;
}

DWORD
WINAPI
ThreadFSScanListener(
    _In_    LPVOID                          PParams
)
{
    PBYTE                   pMessage        = NULL;
    DWORD                   dwStatus        = ERROR_SUCCESS;
    DWORD                   dwWritten       = 0;
    DWORD                   dwErrCount      = 0;
    PFS_SCAN_REQ_NODE       pScanReqNode    = NULL;

    UNREFERENCED_PARAMETER(PParams);


    __try
    {
        while (TRUE)
        {
            if (WaitForSingleObject(gHEventFSStop, 1) == STATUS_SUCCESS) break;

            pMessage = (PBYTE) malloc(MESSAGE_BUFFER_SIZE);
            if (NULL == pMessage)
            {
                LogErrorWin(ERROR_NOT_ENOUGH_MEMORY, L"memalloc");
                __leave;
            }
            ZeroMemory(pMessage, MESSAGE_BUFFER_SIZE);

            dwStatus = ReadScanMessage(pMessage, &dwWritten, gIcComPort.HFSScanPort, gHEventFSStop);
            if (dwStatus == ERROR_CANCELLED)
            {
                break;
            }

            if (dwStatus != STATUS_SUCCESS)
            {
                LogErrorWin(dwStatus, L"ReadFSScanMessage");

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

            pScanReqNode = (PFS_SCAN_REQ_NODE) malloc(sizeof(FS_SCAN_REQ_NODE));
            if (NULL == pScanReqNode)
            {
                LogErrorWin(ERROR_NOT_ENOUGH_MEMORY, L"malloc pScanReqNode");

                free(pMessage);
                pMessage = NULL;

                continue;
            }

            pScanReqNode->PMessage = pMessage;

            EnterCriticalSection(&gCSFSScanList);
            InsertTailList(gPLEFSScanRequests, &pScanReqNode->Link);
            LeaveCriticalSection(&gCSFSScanList);

            if (!SetEvent(gHEventFSScanReqAdded))
            {
                LogErrorWin(GetLastError(), L"SetEvent(gHEventFSScanReqAdded)");
                continue;
            }
        }
    }
    __finally
    {

    }

    LogInfo(L"ThreadFSScanListener DONE!");
    return 0;

}

_Use_decl_anno_impl_
DWORD
StartFSScan(
    VOID
)
{
    DWORD dwResult  = ERROR_SUCCESS;
    DWORD dwIndex   = 0;

    if (gBFSScanStarted)
    {
        dwResult = ERROR_ALREADY_INITIALIZED;
        LogErrorWin(dwResult, L"StartFSScan already called");
        return dwResult;
    }

    __try
    {
        dwResult = InitFSScanRules();
        if (ERROR_SUCCESS != dwResult)
        {
            LogErrorWin(dwResult, L"InitFSScanRules");
            __leave;
        }

        gHEventFSStop = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (NULL == gHEventFSStop)
        {
            dwResult = GetLastError();
            LogErrorWin(dwResult, L"CreateEventW");
            __leave;
        }

        gHEventFSScanReqAdded = CreateEventW(NULL, FALSE, FALSE, NULL);
        if (NULL == gHEventFSScanReqAdded)
        {
            dwResult = GetLastError();
            LogErrorWin(dwResult, L"CreateEventW");
            __leave;
        }

        InitializeCriticalSection(&gCSFSScanList);

        gPLEFSScanRequests = (PLIST_ENTRY) malloc(sizeof(LIST_ENTRY));
        if (NULL == gPLEFSScanRequests)
        {
            dwResult = ERROR_NOT_ENOUGH_MEMORY;
            LogErrorWin(dwResult, L"malloc gPLEFSScanRequests");
            __leave;
        }
        InitializeListHead(gPLEFSScanRequests);


        // scanners
        gPHThreadFSScanners = (PHANDLE) malloc(sizeof(HANDLE) * FS_SCAN_NR_OF_THREADS);
        if (NULL == gPHThreadFSScanners)
        {
            dwResult = ERROR_NOT_ENOUGH_MEMORY;
            LogErrorWin(dwResult, L"malloc gPHThreadFSScanners");
            __leave;
        }
        RtlSecureZeroMemory(gPHThreadFSScanners, sizeof(HANDLE) * FS_SCAN_NR_OF_THREADS);

        for (dwIndex = 0; dwIndex < FS_SCAN_NR_OF_THREADS; dwIndex++)
        {
            gPHThreadFSScanners[dwIndex] = CreateThread(NULL, 0, ThreadFSScanner, NULL, 0, NULL);
            if (NULL == gPHThreadFSScanners[dwIndex])
            {
                dwResult = GetLastError();
                LogErrorWin(dwResult, L"CreateThread %d ThreadFSScanner", dwIndex);
                __leave;
            }
        }

        gHThreadFSListener = CreateThread(NULL, 0, ThreadFSScanListener, NULL, 0, NULL);
        if (NULL == gHThreadFSListener)
        {
            dwResult = GetLastError();
            LogErrorWin(dwResult, L"CreateThread ThreadFSScanListener");
            __leave;
        }

        dwResult = SendSetOption(ICE_FILTER_ENABLE_FS_SCAN, 1);
        if (ERROR_SUCCESS != dwResult)
        {
            LogErrorWin(dwResult, L"IcSendSetOption(ICE_FILTER_ENABLE_FS_SCAN, 1)");
            __leave;
        }

        gBFSScanStarted = TRUE;
    }
    __finally
    {
        if (STATUS_SUCCESS != dwResult)
        {
            gBFSScanStarted = TRUE;
            StopFSScan(TRUE);
            gBFSScanStarted = FALSE;
        }
    }

    return dwResult;
}

_Use_decl_anno_impl_
DWORD
StopFSScan(
    BOOLEAN                                 BPersistent
)
{
    DWORD                   dwResult    = ERROR_SUCCESS;
    DWORD                   dwIndex     = 0;
    PFS_SCAN_REQ_NODE       pReqNode    = NULL;

    if (!gBFSScanStarted)
    {
        dwResult = ERROR_INIT_STATUS_NEEDED;
        LogErrorWin(dwResult, L"StopFSScan was never called or StopFSScan wa already called");
        return dwResult;
    }

    dwResult = SendSetOption(ICE_FILTER_ENABLE_FS_SCAN, 0);
    if (ERROR_SUCCESS != dwResult)
    {
        LogWarningWin(dwResult, L"SendSetOption(ICE_FILTER_ENABLE_FS_SCAN, 0)");
    }

    if (NULL != gHEventFSStop)
    {
        SetEvent(gHEventFSStop);
    }


    if (NULL != gHThreadFSListener)
    {
        dwResult = WaitForSingleObject(gHThreadFSListener, WAIT_TIME_FOR_THREADS);
        if (WAIT_OBJECT_0 != dwResult)
        {
            LogWarningWin(dwResult, L"WaitForSingleObject(gHThreadFSListener)");
        }

        CloseHandle(gHThreadFSListener);
        gHThreadFSListener = NULL;
    }

    if (NULL != gPHThreadFSScanners)
    {
        for (dwIndex = 0; dwIndex < FS_SCAN_NR_OF_THREADS; dwIndex++)
        {
            if (NULL != gPHThreadFSScanners[dwIndex])
            {
                dwResult = WaitForSingleObject(gPHThreadFSScanners[dwIndex], WAIT_TIME_FOR_THREADS);
                if (WAIT_OBJECT_0 != dwResult)
                {
                    LogWarningWin(dwResult, L"WaitForSingleObject(gPHThreadFSScanners[%d])", dwIndex);
                }

                CloseHandle(gPHThreadFSScanners[dwIndex]);
                gPHThreadFSScanners[dwIndex] = NULL;
            }
        }

        free(gPHThreadFSScanners);
        gPHThreadFSScanners = NULL;
    }

    if (NULL != gPLEFSScanRequests)
    {
        // ar trebui parcursa si lista si dealocata memoria in caz ca a ramas ceva in ea

        while (!IsListEmpty(gPLEFSScanRequests))
        {
            pReqNode = CONTAINING_RECORD(RemoveHeadList(gPLEFSScanRequests), FS_SCAN_REQ_NODE, Link);
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

        free(gPLEFSScanRequests);
        gPLEFSScanRequests = NULL;
    }
    DeleteCriticalSection(&gCSFSScanList);

    if (NULL != gHEventFSStop)
    {
        CloseHandle(gHEventFSStop);
        gHEventFSStop = NULL;
    }

    if (NULL != gHEventFSScanReqAdded)
    {
        CloseHandle(gHEventFSScanReqAdded);
        gHEventFSScanReqAdded = NULL;
    }

    dwResult = UninitFSScanRules(BPersistent);
    if (ERROR_SUCCESS != dwResult)
    {
        LogWarningWin(dwResult, L"UninitFSRules");
    }

    gBFSScanStarted = FALSE;
    return dwResult;
}

_Use_decl_anno_impl_
DWORD
GetFSscanStatus(
    BOOLEAN                                *PBEnabled
)
{
    *PBEnabled = gBFSScanStarted;
    return ERROR_SUCCESS;
}
