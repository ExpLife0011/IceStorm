#include "debug2.h"
#include "manager.h"
#include "client.h"
#include "global_data.h"
#include "client_helpers.h"
#include "import_icefltum.h"

#define CONFIG_FILE_NAME L"icestorm.ipv4"

BOOLEAN
SleepOrExit(
    _In_    DWORD           DwMilliseconds
)
{
    LogInfo(L"Will wait for %d milliseconds", DwMilliseconds);

    if (WAIT_OBJECT_0 == WaitForSingleObject(gHStopEvent, DwMilliseconds))
    {
        LogInfo(L"Stop event was signaled.");
        return TRUE;
    }
    
    LogInfo(L"Slept for %d milliseconds", DwMilliseconds);
    return FALSE;
}

VOID
LoadServerInfo(
    _Inout_     PCHAR           PIpAddr,
    _Inout_     PCHAR           PPort
)
{
    DWORD       dwStatus                    = ERROR_SUCCESS;
    DWORD       i                           = 0;
    DWORD       dwRead                      = 0;
    WCHAR       pConfigPath[MAX_PATH]       = { 0 };
    HANDLE      hFile                       = ERROR_SUCCESS;
    CHAR        pBuffer[MAX_PATH]           = { 0 };

    if (0 == GetModuleFileNameW(NULL, pConfigPath, MAX_PATH))
    {
        dwStatus = GetLastError();
        LogWarningWin(GetLastError(), L"GetModuleFileNameA");
        wcscpy_s(pConfigPath, MAX_PATH, CONFIG_FILE_NAME);
    }
    else
    {
        for (i = (DWORD) wcslen(pConfigPath); (i > 0) && (pConfigPath[i] != L'\\'); i--);
        pConfigPath[i] = 0;
        swprintf_s(pConfigPath, MAX_PATH, L"%s\\%s", pConfigPath, CONFIG_FILE_NAME);
    }

    __try
    {
        hFile = CreateFile(pConfigPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (INVALID_HANDLE_VALUE == hFile)
        {
            dwStatus = GetLastError();
            LogErrorWin(dwStatus, L"CreateFile(%s)", pConfigPath);
            __leave;
        }

        if (!ReadFile(hFile, pBuffer, MAX_PATH, &dwRead, NULL))
        {
            dwStatus = GetLastError();
            LogErrorWin(dwStatus, L"ReadFile");
            __leave;
        }

        LogInfo(L">>>>>>>>>>>>>>>>>>>>>>>> %S", pBuffer);
        for (i = 0; i < dwRead && pBuffer[i] && pBuffer[i] != ':'; i++)
        {
            PIpAddr[i] = pBuffer[i];
        }
        strcpy_s(PPort, MAX_PATH, pBuffer + i + 1);

        LogInfo(L">>>>>>>>>> IP: %S", PIpAddr);
        LogInfo(L">>>>>>>>>> Port: %S", PPort);
    }
    __finally
    {
        if (INVALID_HANDLE_VALUE != hFile)
        {
            CloseHandle(hFile);
            hFile = INVALID_HANDLE_VALUE;
        }

        if (ERROR_SUCCESS != dwStatus)
        {
            sprintf_s(PIpAddr, MAX_PATH, "192.168.194.1");
            sprintf_s(PPort, MAX_PATH, "12345");
        }
    }
}

VOID
LoadMachineInfo(
    _Inout_     IC_MACHINE_INFO     *PMachineInfo
)
{
    DWORD           dwSize      = 0;
    SYSTEM_INFO     systemInfo  = { 0 };
    OSVERSIONINFOEX ov          = { 0 };

    dwSize = MAX_COMPUTERNAME_LENGTH + 2;
    if (!GetComputerName(PMachineInfo->PMachineName, &dwSize))
    {
        LogWarningWin(GetLastError(), L"GetComputerName");
    }


    GetSystemInfo(&systemInfo);

    PMachineInfo->DwNrOfProcessors = systemInfo.dwNumberOfProcessors;

    if (systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) wcscpy_s(PMachineInfo->PArchitecture, MAX_PROCESSOR_NAME, L"x64");
    else if (systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL) wcscpy_s(PMachineInfo->PArchitecture, MAX_PROCESSOR_NAME, L"x86");
    else if (systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM) wcscpy_s(PMachineInfo->PArchitecture, MAX_PROCESSOR_NAME, L"ARM");
    else if (systemInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64) wcscpy_s(PMachineInfo->PArchitecture, MAX_PROCESSOR_NAME, L"Intel Italium-based");
    

    ov.dwOSVersionInfoSize = sizeof(ov);
#pragma warning(suppress: 4996)
    if (!GetVersionEx((LPOSVERSIONINFOW) &ov))
    {
        LogWarningWin(GetLastError(), L"GetVersionEx");
    }
    else
    {
        if (ov.dwMajorVersion == 10 && ov.dwMinorVersion == 0 && ov.wProductType == VER_NT_WORKSTATION)
            wcscpy_s(PMachineInfo->POS, MAX_OS_NAME, L"Windows 10");
        else if (ov.dwMajorVersion == 10 && ov.dwMinorVersion == 0 && ov.wProductType != VER_NT_WORKSTATION)
            wcscpy_s(PMachineInfo->POS, MAX_OS_NAME, L"Windows Server 2016");
        
        else if (ov.dwMajorVersion == 6 && ov.dwMinorVersion == 3 && ov.wProductType == VER_NT_WORKSTATION)
            wcscpy_s(PMachineInfo->POS, MAX_OS_NAME, L"Windows 8.1");
        else if (ov.dwMajorVersion == 6 && ov.dwMinorVersion == 3 && ov.wProductType != VER_NT_WORKSTATION)
            wcscpy_s(PMachineInfo->POS, MAX_OS_NAME, L"Windows Server 2012 R2");

        else if (ov.dwMajorVersion == 6 && ov.dwMinorVersion == 2 && ov.wProductType == VER_NT_WORKSTATION)
            wcscpy_s(PMachineInfo->POS, MAX_OS_NAME, L"Windows 8");
        else if (ov.dwMajorVersion == 6 && ov.dwMinorVersion == 2 && ov.wProductType != VER_NT_WORKSTATION)
            wcscpy_s(PMachineInfo->POS, MAX_OS_NAME, L"Windows Server 2012");

        else if (ov.dwMajorVersion == 6 && ov.dwMinorVersion == 1 && ov.wProductType == VER_NT_WORKSTATION)
            wcscpy_s(PMachineInfo->POS, MAX_OS_NAME, L"Windows 7");
        else if (ov.dwMajorVersion == 6 && ov.dwMinorVersion == 1 && ov.wProductType != VER_NT_WORKSTATION)
            wcscpy_s(PMachineInfo->POS, MAX_OS_NAME, L"Windows Server 2008 R2");

        else
            wcscpy_s(PMachineInfo->POS, MAX_OS_NAME, L"Unkwnown");
    }

    //swprintf_s(PMachineInfo->POS, MAX_OS_NAME, L"%s (%d)", PMachineInfo->POS, ov.dwBuildNumber);
    swprintf_s(PMachineInfo->POS, MAX_OS_NAME, L"%s (%d)", L"Windows 10", ov.dwBuildNumber);
}

VOID
SendAppCtrlStatus(
    VOID
)
{
    BOOLEAN bEnabled    = FALSE;
    DWORD   dwStatus    = ERROR_SUCCESS;

    dwStatus = IcGetAppCtrlStatus(&bEnabled);
    if (ERROR_SUCCESS != dwStatus)
    {
        LogErrorWin(dwStatus, L"IcGetAppCtrlStatus");
    }

    if (ERROR_SUCCESS != ClSendDWORD(dwStatus == ERROR_SUCCESS ? IcServerCommandResult_Success : IcServerCommandResult_Error)) return;

    if (dwStatus != ERROR_SUCCESS) return;
    
    ClSendDWORD(bEnabled ? 1 : 0);
}

VOID
SendSetAppCtrlStatus(
    VOID
)
{
    DWORD dwStatus = ERROR_SUCCESS;
    DWORD dwEnable = 0;

    if (ERROR_SUCCESS != ClRecvDWORD(&dwEnable)) return;

    dwStatus = (dwEnable == 1) ? IcStartAppCtrlScan() : IcStopAppCtrlScan(TRUE);
    if (dwStatus != ERROR_SUCCESS)
    {
        LogErrorWin(dwStatus, L"%s", (dwEnable == 1) ? L"IcStartAppCtrlScan" : L"IcStopAppCtrlScan");
    }

    ClSendDWORD(dwStatus == ERROR_SUCCESS ? IcServerCommandResult_Success : IcServerCommandResult_Error);
}

VOID
SendFSScanStatus(
    VOID
)
{
    BOOLEAN bEnabled = FALSE;
    DWORD   dwStatus = ERROR_SUCCESS;

    dwStatus = IcGetFSscanStatus(&bEnabled);
    if (ERROR_SUCCESS != dwStatus)
    {
        LogErrorWin(dwStatus, L"IcGetFSscanStatus");
    }

    if (ERROR_SUCCESS != ClSendDWORD(dwStatus == ERROR_SUCCESS ? IcServerCommandResult_Success : IcServerCommandResult_Error)) return;

    if (dwStatus != ERROR_SUCCESS) return;

    ClSendDWORD(bEnabled ? 1 : 0);
}

VOID
SendSetFSScanStatus(
    VOID
)
{
    DWORD dwStatus = ERROR_SUCCESS;
    DWORD dwEnable = 0;

    if (ERROR_SUCCESS != ClRecvDWORD(&dwEnable)) return;

    dwStatus = (dwEnable == 1) ? IcStartFSScan() : IcStopFSScan(TRUE);
    if (dwStatus != ERROR_SUCCESS)
    {
        LogErrorWin(dwStatus, L"%s", (dwEnable == 1) ? L"IcStartFSScan" : L"IcStopFSScan");
    }

    ClSendDWORD(dwStatus == ERROR_SUCCESS ? IcServerCommandResult_Success : IcServerCommandResult_Error);
}

VOID
SendAppCtrlEvents(
    VOID
)
{
    PIC_APPCTRL_EVENT   pEvents     = NULL;
    DWORD               dwLength    = 0;
    DWORD               dwStatus    = ERROR_SUCCESS;
    DWORD               dwFirstId   = 0;
    DWORD               idx         = 0;

    if (ERROR_SUCCESS != ClRecvDWORD(&dwFirstId)) return;

    dwStatus = IcGetAppCtrlEvents(&pEvents, &dwLength, dwFirstId);
    if (ERROR_SUCCESS != dwStatus && ERROR_NOT_FOUND != dwStatus)
    {
        LogErrorWin(dwStatus, L"IcGetAppCtrlEvents");
        ClSendDWORD(IcServerCommandResult_Error);
        return;
    }

    __try
    {
        if (ERROR_SUCCESS != ClSendDWORD(IcServerCommandResult_Success)) __leave;

        if (ERROR_SUCCESS != ClSendDWORD(dwLength)) __leave;

        for (idx = 0; idx < dwLength; idx++)
        {
            if (ERROR_SUCCESS != ClSendDWORD(pEvents[idx].DwEventId)) __leave;
            if (ERROR_SUCCESS != ClSendString(pEvents[idx].PProcessPath)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pEvents[idx].DwPid)) __leave;
            if (ERROR_SUCCESS != ClSendString(pEvents[idx].PParentPath)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pEvents[idx].DwParentPid)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pEvents[idx].Verdict)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pEvents[idx].DwMatchedRuleId)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pEvents[idx].DwEventTime)) __leave;
        }
    }
    __finally
    {
        IcFreeAppCtrlEventsList(pEvents, dwLength);
        pEvents = NULL;
    }
}

VOID
SendAddAppCtrlRule(
    VOID
)
{
    DWORD               dwStatus                    = ERROR_SUCCESS;
    IC_STRING_MATCHER   procMatcher                 = IcStringMatcher_Equal;
    PWCHAR              pProcPath                   = NULL;
    DWORD               dwPID                       = 0;
    IC_STRING_MATCHER   parentProcMatcher           = IcStringMatcher_Equal;
    PWCHAR              pParentProcPath             = NULL;
    DWORD               dwParentPID                 = 0;
    ICE_SCAN_VERDICT    verdict                     = IceScanVerdict_Allow;
    DWORD               dwRuleId                    = 0;

    __try
    {
        if (ERROR_SUCCESS != ClRecvDWORD((PDWORD) &procMatcher)) __leave;
        if (ERROR_SUCCESS != ClRecvString(&pProcPath)) __leave;
        if (ERROR_SUCCESS != ClRecvDWORD(&dwPID)) __leave;
        if (ERROR_SUCCESS != ClRecvDWORD((PDWORD) &parentProcMatcher)) __leave;
        if (ERROR_SUCCESS != ClRecvString(&pParentProcPath)) __leave;
        if (ERROR_SUCCESS != ClRecvDWORD(&dwParentPID)) __leave;
        if (ERROR_SUCCESS != ClRecvDWORD((PDWORD) &verdict)) __leave;

        dwStatus = IcAddAppCtrlRule(procMatcher, pProcPath, dwPID, parentProcMatcher, pParentProcPath, dwParentPID, verdict, &dwRuleId);
        if (ERROR_SUCCESS != dwStatus)
        {
            LogErrorWin(dwStatus, L"IcAddAppCtrlRule");
            ClSendDWORD(IcServerCommandResult_Error);
            __leave;
        }

        if (ERROR_SUCCESS != ClSendDWORD(IcServerCommandResult_Success)) __leave;

        if (ERROR_SUCCESS != ClSendDWORD(dwRuleId)) __leave;
    }
    __finally
    {
        if (NULL != pProcPath)
        {
            free(pProcPath);
            pProcPath = NULL;
        }

        if (NULL != pParentProcPath)
        {
            free(pParentProcPath);
            pParentProcPath = NULL;
        }
    }
}

VOID
SendAppCtrlRules(
    VOID
)
{
    PIC_APPCTRL_RULE    pRules      = NULL;
    DWORD               dwLength    = 0;
    DWORD               dwStatus    = ERROR_SUCCESS;
    DWORD               idx         = 0;

    dwStatus = IcGetAppCtrlRules(&pRules, &dwLength);
    if (ERROR_SUCCESS != dwStatus && ERROR_NOT_FOUND != dwStatus)
    {
        LogErrorWin(dwStatus, L"IcGetAppCtrlRules");
        ClSendDWORD(IcServerCommandResult_Error);
        return;
    }

    __try 
    {
        if (ERROR_SUCCESS != ClSendDWORD(IcServerCommandResult_Success)) __leave;

        if (ERROR_SUCCESS != ClSendDWORD(dwLength)) __leave;

        for (idx = 0; idx < dwLength; idx++)
        {
            if (ERROR_SUCCESS != ClSendDWORD(pRules[idx].DwRuleId)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pRules[idx].MatcherProcessPath)) __leave;
            if (ERROR_SUCCESS != ClSendString(pRules[idx].PProcessPath)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pRules[idx].DwPid)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pRules[idx].MatcherParentPath)) __leave;
            if (ERROR_SUCCESS != ClSendString(pRules[idx].PParentPath)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pRules[idx].DwParentPid)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pRules[idx].Verdict)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pRules[idx].DwAddTime)) __leave;
        }
    }
    __finally
    {
        IcFreeAppCtrlRulesList(pRules, dwLength);
        pRules = NULL;
    }
}

VOID
SendDeleteAppCtrlRule(
    VOID
)
{
    DWORD dwRuleId = 0;
    DWORD dwStatus = ERROR_SUCCESS;

    if (ERROR_SUCCESS != ClRecvDWORD((PDWORD) &dwRuleId)) return;

    dwStatus = IcDeleteAppCtrlRule(dwRuleId);
    if (ERROR_SUCCESS != dwStatus)
    {
        LogErrorWin(dwStatus, L"IcDeleteAppCtrlRule(%d)", dwRuleId);
    }
    
    ClSendDWORD(dwStatus == ERROR_SUCCESS ? IcServerCommandResult_Success : IcServerCommandResult_Error);
}

VOID
SendUpdateAppCtrlRule(
    VOID
)
{
    DWORD               dwStatus                    = ERROR_SUCCESS;
    IC_STRING_MATCHER   procMatcher                 = IcStringMatcher_Equal;
    PWCHAR              pProcPath                   = NULL;
    DWORD               dwPID                       = 0;
    IC_STRING_MATCHER   parentProcMatcher           = IcStringMatcher_Equal;
    PWCHAR              pParentProcPath             = NULL;
    DWORD               dwParentPID                 = 0;
    ICE_SCAN_VERDICT    verdict                     = IceScanVerdict_Allow;
    DWORD               dwRuleId                    = 0;

    __try
    {
        if (ERROR_SUCCESS != ClRecvDWORD(&dwRuleId)) __leave;

        if (ERROR_SUCCESS != ClRecvDWORD((PDWORD) &procMatcher)) __leave;
        if (ERROR_SUCCESS != ClRecvString(&pProcPath)) __leave;
        if (ERROR_SUCCESS != ClRecvDWORD(&dwPID)) __leave;
        if (ERROR_SUCCESS != ClRecvDWORD((PDWORD) &parentProcMatcher)) __leave;
        if (ERROR_SUCCESS != ClRecvString(&pParentProcPath)) __leave;
        if (ERROR_SUCCESS != ClRecvDWORD(&dwParentPID)) __leave;
        if (ERROR_SUCCESS != ClRecvDWORD((PDWORD) &verdict)) __leave;

        dwStatus = IcUpdateAppCtrlRule(dwRuleId, procMatcher, pProcPath, dwPID, parentProcMatcher, pParentProcPath, dwParentPID, verdict);
        if (ERROR_SUCCESS != dwStatus)
        {
            LogErrorWin(dwStatus, L"IcAddAppCtrlRule");
            ClSendDWORD(IcServerCommandResult_Error);
            __leave;
        }

        if (ERROR_SUCCESS != ClSendDWORD(IcServerCommandResult_Success)) __leave;
    }
    __finally
    {
        if (NULL != pProcPath)
        {
            free(pProcPath);
            pProcPath = NULL;
        }

        if (NULL != pParentProcPath)
        {
            free(pParentProcPath);
            pParentProcPath = NULL;
        }
    }
}

VOID
SendSetOptionCmd(
    VOID
)
{
    DWORD dwStatus  = ERROR_SUCCESS;
    DWORD dwOption  = 0;
    DWORD dwValue   = 0;

    if (ERROR_SUCCESS != ClRecvDWORD(&dwOption)) return;
    if (ERROR_SUCCESS != ClRecvDWORD(&dwValue)) return;

    dwStatus = IcSendSetOption(dwOption, dwValue);
    if (ERROR_SUCCESS != dwStatus)
    {
        LogErrorWin(dwStatus, L"IcSendSetOption(%d, %d)", dwOption, dwValue);
    }

    if (ERROR_SUCCESS != ClSendDWORD(dwStatus)) return;

    ClSendDWORD(dwStatus == ERROR_SUCCESS ? IcServerCommandResult_Success : IcServerCommandResult_Error);
}

VOID
SendAddFSScanRule(
    VOID
)
{
    DWORD               dwStatus                    = ERROR_SUCCESS;
    IC_STRING_MATCHER   procMatcher                 = IcStringMatcher_Equal;
    PWCHAR              pProcPath                   = NULL;
    DWORD               dwPID                       = 0;
    IC_STRING_MATCHER   fileProcMatcher           = IcStringMatcher_Equal;
    PWCHAR              pFilePath                   = NULL;
    DWORD               dwDeniedOperations          = 0;
    DWORD               dwRuleId                    = 0;

    __try
    {
        if (ERROR_SUCCESS != ClRecvDWORD((PDWORD) &procMatcher)) __leave;
        if (ERROR_SUCCESS != ClRecvString(&pProcPath)) __leave;
        if (ERROR_SUCCESS != ClRecvDWORD(&dwPID)) __leave;
        if (ERROR_SUCCESS != ClRecvDWORD((PDWORD) &fileProcMatcher)) __leave;
        if (ERROR_SUCCESS != ClRecvString(&pFilePath)) __leave;
        if (ERROR_SUCCESS != ClRecvDWORD((PDWORD) &dwDeniedOperations)) __leave;

        dwStatus = IcAddFSScanRule(procMatcher, pProcPath, dwPID, fileProcMatcher, pFilePath, dwDeniedOperations, &dwRuleId);
        if (ERROR_SUCCESS != dwStatus)
        {
            LogErrorWin(dwStatus, L"IcAddFSScanRule");
            ClSendDWORD(IcServerCommandResult_Error);
            __leave;
        }

        if (ERROR_SUCCESS != ClSendDWORD(IcServerCommandResult_Success)) __leave;

        if (ERROR_SUCCESS != ClSendDWORD(dwRuleId)) __leave;
    }
    __finally
    {
        if (NULL != pProcPath)
        {
            free(pProcPath);
            pProcPath = NULL;
        }

        if (NULL != pFilePath)
        {
            free(pFilePath);
            pFilePath = NULL;
        }
    }
}

VOID
SendFSScanRules(
    VOID
)
{
    PIC_FS_RULE         pRules      = NULL;
    DWORD               dwLength    = 0;
    DWORD               dwStatus    = ERROR_SUCCESS;
    DWORD               idx         = 0;

    dwStatus = IcGetFSScanRules(&pRules, &dwLength);
    if (ERROR_SUCCESS != dwStatus && ERROR_NOT_FOUND != dwStatus)
    {
        LogErrorWin(dwStatus, L"IcGetFSScanRules");
        ClSendDWORD(IcServerCommandResult_Error);
        return;
    }

    __try 
    {
        if (ERROR_SUCCESS != ClSendDWORD(IcServerCommandResult_Success)) __leave;

        if (ERROR_SUCCESS != ClSendDWORD(dwLength)) __leave;

        for (idx = 0; idx < dwLength; idx++)
        {
            if (ERROR_SUCCESS != ClSendDWORD(pRules[idx].DwRuleId)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pRules[idx].MatcherProcessPath)) __leave;
            if (ERROR_SUCCESS != ClSendString(pRules[idx].PProcessPath)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pRules[idx].DwPid)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pRules[idx].MatcherFilePath)) __leave;
            if (ERROR_SUCCESS != ClSendString(pRules[idx].PFilePath)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pRules[idx].UlDeniedOperations)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pRules[idx].DwAddTime)) __leave;
        }
    }
    __finally
    {
        IcFreeFSScanList(pRules, dwLength);
        pRules = NULL;
    }
}

VOID
SendFSScanEvents(
    VOID
)
{
    PIC_FS_EVENT        pEvents     = NULL;
    DWORD               dwLength    = 0;
    DWORD               dwStatus    = ERROR_SUCCESS;
    DWORD               dwFirstId   = 0;
    DWORD               idx         = 0;

    if (ERROR_SUCCESS != ClRecvDWORD(&dwFirstId)) return;

    dwStatus = IcGetFSEvents(&pEvents, &dwLength, dwFirstId);
    if (ERROR_SUCCESS != dwStatus && ERROR_NOT_FOUND != dwStatus)
    {
        LogErrorWin(dwStatus, L"IcGetFSEvents");
        ClSendDWORD(IcServerCommandResult_Error);
        return;
    }

    __try
    {
        if (ERROR_SUCCESS != ClSendDWORD(IcServerCommandResult_Success)) __leave;

        if (ERROR_SUCCESS != ClSendDWORD(dwLength)) __leave;

        for (idx = 0; idx < dwLength; idx++)
        {
            if (ERROR_SUCCESS != ClSendDWORD(pEvents[idx].DwEventId)) __leave;
            if (ERROR_SUCCESS != ClSendString(pEvents[idx].PProcessPath)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pEvents[idx].DwPid)) __leave;
            if (ERROR_SUCCESS != ClSendString(pEvents[idx].PFilePath)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pEvents[idx].UlRequiredOperations)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pEvents[idx].UlDeniedOperations)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pEvents[idx].UlRemainingOperations)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pEvents[idx].DwMatchedRuleId)) __leave;
            if (ERROR_SUCCESS != ClSendDWORD(pEvents[idx].DwEventTime)) __leave;
        }
    }
    __finally
    {
        IcFreeFSEventsList(pEvents, dwLength);
        pEvents = NULL;
    }
}

VOID
SendDeleteFSScanRule(
    VOID
)
{
    DWORD dwRuleId = 0;
    DWORD dwStatus = ERROR_SUCCESS;

    if (ERROR_SUCCESS != ClRecvDWORD((PDWORD) &dwRuleId)) return;

    dwStatus = IcDeleteFSScanRule(dwRuleId);
    if (ERROR_SUCCESS != dwStatus)
    {
        LogErrorWin(dwStatus, L"IcDeleteFSScanRule(%d)", dwRuleId);
    }

    ClSendDWORD(dwStatus == ERROR_SUCCESS ? IcServerCommandResult_Success : IcServerCommandResult_Error);
}

VOID
SendUpdateFSScanRule(
    VOID
)
{
    DWORD               dwStatus                    = ERROR_SUCCESS;
    IC_STRING_MATCHER   procMatcher                 = IcStringMatcher_Equal;
    PWCHAR              pProcPath                   = NULL;
    DWORD               dwPID                       = 0;
    IC_STRING_MATCHER   fileProcMatcher           = IcStringMatcher_Equal;
    PWCHAR              pFilePath                   = NULL;
    DWORD               dwDeniedOperations          = 0;
    DWORD               dwRuleId                    = 0;

    __try
    {
        if (ERROR_SUCCESS != ClRecvDWORD(&dwRuleId)) __leave;

        if (ERROR_SUCCESS != ClRecvDWORD((PDWORD) &procMatcher)) __leave;
        if (ERROR_SUCCESS != ClRecvString(&pProcPath)) __leave;
        if (ERROR_SUCCESS != ClRecvDWORD(&dwPID)) __leave;
        if (ERROR_SUCCESS != ClRecvDWORD((PDWORD) &fileProcMatcher)) __leave;
        if (ERROR_SUCCESS != ClRecvString(&pFilePath)) __leave;
        if (ERROR_SUCCESS != ClRecvDWORD((PDWORD) &dwDeniedOperations)) __leave;

        dwStatus = IcUpdateFSScanRule(dwRuleId, procMatcher, pProcPath, dwPID, fileProcMatcher, pFilePath, dwDeniedOperations);
        if (ERROR_SUCCESS != dwStatus)
        {
            LogErrorWin(dwStatus, L"IcUpdateFSScanRule");
            ClSendDWORD(IcServerCommandResult_Error);
            __leave;
        }

        if (ERROR_SUCCESS != ClSendDWORD(IcServerCommandResult_Success)) __leave;
    }
    __finally
    {
        if (NULL != pProcPath)
        {
            free(pProcPath);
            pProcPath = NULL;
        }

        if (NULL != pFilePath)
        {
            free(pFilePath);
            pFilePath = NULL;
        }
    }
}

VOID
ExecuteCommand(
    _In_    IC_SERVER_COMMAND       DwCommand
)
{
    switch (DwCommand)
    {
        case IcServerCommand_Ping:
            ClSendDWORD(IcServerCommandResult_Success);
            break;

        case IcServerCommand_GetAppCtrlStatus:
            SendAppCtrlStatus();
            break;

        case IcServerCommand_SetAppCtrlStatus:
            SendSetAppCtrlStatus();
            break;

        case IcServerCommand_GetFSScanStatus:
            SendFSScanStatus();
            break;

        case IcServerCommand_SetFSScanStatus:
            SendSetFSScanStatus();
            break;

        case IcServerCommand_GetAppCtrlEvents:
            SendAppCtrlEvents();
            break;

        case IcServerCommand_AddAppCtrlRule:
            SendAddAppCtrlRule();
            break;

        case IcServerCommand_GetAppCtrlRules:
            SendAppCtrlRules();
            break;

        case IcServerCommand_DeleteAppCtrlRule:
            SendDeleteAppCtrlRule();
            break;

        case IcServerCommand_UpdateAppCtrlRule:
            SendUpdateAppCtrlRule();
            break;

        case IcServerCommand_SetOption:
            SendSetOptionCmd();
            break;

        case IcServerCommand_AddFSScanRule:
            SendAddFSScanRule();
            break;

        case IcServerCommand_GetFSScanRules:
            SendFSScanRules();
            break;

        case IcServerCommand_GetFSScanEvents:
            SendFSScanEvents();
            break;

        case IcServerCommand_DeleteFSScanRule:
            SendDeleteFSScanRule();
            break;

        case IcServerCommand_UpdateFSScanRule:
            SendUpdateFSScanRule();
            break;

        default:
            LogInfo(L"Unknown command: %d", DwCommand);
            ClSendDWORD(IcServerCommandResult_Error);
            break;
    }
}

VOID
WaitForCommands(
    VOID
)
{
    IC_SERVER_COMMAND dwCommand = 0;

    LogInfo(L"Starting to wait for commands...");
    while (TRUE)
    {
        if (SleepOrExit(1)) break;
        
        dwCommand = GetCommandFromServer();

        if (SleepOrExit(1)) break;

        if (dwCommand == IcServerCommandResult_Error)
        {
            LogWarning(L"Server does not respond");
            break;
        }

        ExecuteCommand(dwCommand);
     
        if (SleepOrExit(1)) break;
    }

    LogInfo(L"WaitForCommands DONE");
}

_Use_decl_anno_impl_
DWORD
WINAPI
ManagerThread(
    PVOID                       PParam
)
{
    CHAR            pIpAddr[MAX_PATH]       = { 0 };
    CHAR            pPort[MAX_PATH]         = { 0 };
    IC_MACHINE_INFO machineInfo             = { 0 };

    UNREFERENCED_PARAMETER(PParam);

    __try
    {
        while (TRUE)
        {
            LoadMachineInfo(&machineInfo);

            LogInfo(L"machineInfo.PMachineName: %s", machineInfo.PMachineName);
            LogInfo(L"machineInfo.POS: %s", machineInfo.POS);
            LogInfo(L"machineInfo.PArchitecture: %s", machineInfo.PArchitecture);
            LogInfo(L"machineInfo.DwNrOfProcessors: %d", machineInfo.DwNrOfProcessors);

            LoadServerInfo(pIpAddr, pPort);
            LogInfo(L"Server address: %S:%S", pIpAddr, pPort);

            while (!StartClient(pIpAddr, pPort))
            {
                if (SleepOrExit(5 * 1000)) __leave;
            }
            LogInfo(L"We are now connected to the server...");

            SendMachineInfo(&machineInfo);
            LogInfo(L"Machine data sent");
            if (SleepOrExit(5)) __leave;

            WaitForCommands();

            StopClient();

            if (SleepOrExit(5 * 1000)) __leave;
        }
    }
    __finally
    {
        StopClient();
    }

    return ERROR_SUCCESS;
}
