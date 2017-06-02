#include "debug2.h"
#include "import_icefltum.h"
#include <stdio.h>
#include <conio.h>
#include <windows.h>

#define MAX_LENGTH_CMD                      4096

#define CMD_HELP                            L"help"
#define CMD_SETOPTION                       L"SetOption"
#define CMD_ENABLE_APPCTRL                  L"enable appctrl"
#define CMD_ADD_APPCTRL_RULE                L"add appctrl"
#define CMD_DELETE_APPCTRL_RULE             L"delete appctrl"
#define CMD_UPDATE_APPCTRL_RULE             L"update appctrl"
#define CMD_GET_APPCTRL_RULES               L"get appctrl"
#define CMD_ENABLE_FS_SCAN                  L"enable fs scan"
#define CMD_ADD_FSSCAN_RULE                 L"add fs"
#define CMD_DELETE_FSSCAN_RULE              L"delete fs"
#define CMD_UPDATE_FSSCAN_RULE              L"update fs"
#define CMD_GET_FSSCAN_RULES                L"get fs"
#define CMD_EXIT                            L"exit"

VOID
PrintHelp(
    VOID
)
{
    printf("\nUSAGE:\n");
    printf(" - %S : this screen\n", CMD_HELP);
    printf(" - %S <optiion> <value>: setoption in driver\n", CMD_SETOPTION);
    printf(" - %S <value>: enable / disable (1 / 0) appctrl scan\n", CMD_ENABLE_APPCTRL);
    printf(" - %S : adds an appctrl rule\n", CMD_ADD_APPCTRL_RULE);
    printf(" - %S <rule_id>: deletes an appctrl rule\n", CMD_DELETE_APPCTRL_RULE);
    printf(" - %S : updates an appctrl rule\n", CMD_UPDATE_APPCTRL_RULE);
    printf(" - %S : get all aptctrl rules\n", CMD_GET_APPCTRL_RULES);
    printf(" - %S <value>: enable / disable (1 / 0) fs scan\n", CMD_ENABLE_FS_SCAN);
    printf(" - %S : exit console\n", CMD_EXIT);
}

VOID
SendOption(
    _In_z_      PWCHAR                      PCmd
)
{
    DWORD       dwOption    = 0;
    DWORD       dwValue     = 0;
    DWORD       dwIndex     = 0;
    HRESULT     hr          = S_OK;

    if (L'\n' == PCmd[0])
    {
        printf("Invalid command format!\n");
        PrintHelp();
        return;
    }

    if (L' ' != PCmd[0])
    {
        printf("Invalid command!\n");
        PrintHelp();
        return;
    }

    for (dwIndex = 1; (L' ' == PCmd[dwIndex]) && (L'\0' != PCmd[dwIndex]); dwIndex++);
    dwOption = _wtoi(PCmd + dwIndex);
    if (0 == dwOption)
    {
        printf("Invalid OPTION!\n");
        PrintHelp();
        return;
    }

    for (; (L' ' != PCmd[dwIndex]) && (L'\0' != PCmd[dwIndex]); dwIndex++);
    for (; (L' ' == PCmd[dwIndex]) && (L'\0' != PCmd[dwIndex]); dwIndex++);
    dwValue = _wtoi(PCmd + dwIndex);
    
    if (0 == dwValue && PCmd[dwIndex] != L'0')
    {
        printf("Invalid VALUE!\n");
        PrintHelp();
        return;
    }

    hr = IcSendSetOption(dwOption, dwValue);
    printf("Comand returned: %d\n", hr);
}

VOID
EnableAppCtrl(
    _In_z_      PWCHAR                      PCmd
)
{
    DWORD       dwValue     = 0;
    DWORD       dwIndex     = 0;
    DWORD       dwResult    = 0;

    if (L'\0' == PCmd[0] || L'\n' == PCmd[0])
    {
        printf("Invalid command format!\n");
        PrintHelp();
        return;
    }

    if (L' ' != PCmd[0])
    {
        printf("Invalid command!\n");
        PrintHelp();
        return;
    }

    for (dwIndex = 1; (L' ' == PCmd[dwIndex]) && (L'\0' != PCmd[dwIndex]); dwIndex++);
    dwValue = _wtoi(PCmd + dwIndex);
    if (
        (0 == dwValue && PCmd[dwIndex] != L'0') || 
        (0 != dwValue && 1 != dwValue)
        )
    {
        printf("Invalid VALUE!\n");
        PrintHelp();
        return;
    }

    if (1 == dwValue)
    {
        dwResult = IcStartAppCtrlScan();
        if (ERROR_SUCCESS != dwResult)
        {
            LogErrorWin(dwResult, L"IcStartAppCtrlScan");
        }
    }
    else
    {
        dwResult = IcStopAppCtrlScan();
        if (ERROR_SUCCESS != dwResult)
        {
            LogErrorWin(dwResult, L"IcStopAppCtrlScan");
        }
    }
}

VOID
AddAppCtrlRule(
    VOID
)
{
    DWORD   dwResult            = ERROR_SUCCESS;
    DWORD   dwRuleId            = 0;
    DWORD   dwPathMatch         = 0;
    WCHAR   pAuxCmd[MAX_PATH]   = { 0 };
    WCHAR   pPath[MAX_PATH]     = { 0 };
    DWORD   dwPid               = 0;
    DWORD   dwParPathMatch      = 0;
    WCHAR   pParPath[MAX_PATH]  = { 0 };
    DWORD   dwParPid            = 0;
    DWORD   dwVerdict           = 0;

    printf("    * Path Matcher: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwPathMatch = _wtoi(pAuxCmd);

    printf("    * Path: ");
    fgetws(pPath, MAX_PATH, stdin);
    pPath[wcslen(pPath) - 1] = 0;

    printf("    * PID: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwPid = _wtoi(pAuxCmd);

    printf("    * Parent Path Matcher: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwParPathMatch = _wtoi(pAuxCmd);

    printf("    * ParentPath: ");
    fgetws(pParPath, MAX_PATH, stdin);
    pParPath[wcslen(pParPath) - 1] = 0;

    printf("    * ParentPID: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwParPid = _wtoi(pAuxCmd);

    printf("    * Verdict: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwVerdict = _wtoi(pAuxCmd);


    dwResult = IcAddAppCtrlRule(
        dwPathMatch,
        !_wcsicmp(pPath, L"NULL") ? NULL : pPath, 
        dwPid, 
        dwParPathMatch,
        !_wcsicmp(pParPath, L"NULL") ? NULL : pParPath, 
        dwParPid, 
        dwVerdict, 
        &dwRuleId
    );
    printf("Command returned: %d\n", dwResult);
    printf(">> Rule ID: %d\n", dwRuleId);
}

VOID
DeleteAppCtrlRule(
    _In_z_      PWCHAR                      PCmd
)
{
    DWORD       dwRuleId    = 0;
    DWORD       dwIndex     = 0;
    DWORD       dwResult    = 0;

    if (L'\0' == PCmd[0] || L'\n' == PCmd[0])
    {
        printf("Invalid command format!\n");
        PrintHelp();
        return;
    }

    if (L' ' != PCmd[0])
    {
        printf("Invalid command!\n");
        PrintHelp();
        return;
    }

    for (dwIndex = 1; (L' ' == PCmd[dwIndex]) && (L'\0' != PCmd[dwIndex]); dwIndex++);
    dwRuleId = _wtoi(PCmd + dwIndex);
    if (
        (0 == dwRuleId && PCmd[dwIndex] != L'0')
        )
    {
        printf("Invalid RULE_ID!\n");
        PrintHelp();
        return;
    }

    dwResult = IcDeleteAppCtrlRule(dwRuleId);
    printf("Command returned: %d", dwResult);
}

VOID
UpdateAppCtrlRule(
    VOID
)
{
    DWORD   dwResult            = ERROR_SUCCESS;
    DWORD   dwRuleId            = 0;
    WCHAR   pAuxCmd[MAX_PATH]   = { 0 };
    DWORD   dwPathMatch         = 0;
    WCHAR   pPath[MAX_PATH]     = { 0 };
    DWORD   dwPid               = 0;
    DWORD   dwParPathMatch      = 0;
    WCHAR   pParPath[MAX_PATH]  = { 0 };
    DWORD   dwParPid            = 0;
    DWORD   dwVerdict           = 0;

    printf("    * RuleID: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwRuleId = _wtoi(pAuxCmd);

    printf("    * Path Matcher: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwPathMatch = _wtoi(pAuxCmd);

    printf("    * Path: ");
    fgetws(pPath, MAX_PATH, stdin);
    pPath[wcslen(pPath) - 1] = 0;

    printf("    * PID: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwPid = _wtoi(pAuxCmd);

    printf("    * Parent Path Matcher: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwParPathMatch = _wtoi(pAuxCmd);

    printf("    * ParentPath: ");
    fgetws(pParPath, MAX_PATH, stdin);
    pParPath[wcslen(pParPath) - 1] = 0;

    printf("    * ParentPID: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwParPid = _wtoi(pAuxCmd);

    printf("    * Verdict: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwVerdict = _wtoi(pAuxCmd);


    dwResult = IcUpdateAppCtrlRule(
        dwRuleId,
        dwPathMatch,
        !_wcsicmp(pPath, L"NULL") ? NULL : pPath, 
        dwPid, 
        dwParPathMatch,
        !_wcsicmp(pParPath, L"NULL") ? NULL : pParPath, 
        dwParPid, 
        dwVerdict
    );

    printf("Command returned: %d\n", dwResult);
}

VOID
GetAllAppCtrl(
    _In_z_      PWCHAR                      PCmd
)
{
    PIC_APPCTRL_RULE    pRules      = NULL;
    DWORD               dwLen       = 0;
    DWORD               dwResult    = ERROR_SUCCESS;

    if (L'\0' != PCmd[0] && L'\n' != PCmd[0])
    {
        printf("Invalid command!\n");
        PrintHelp();
        return;
    }

    dwResult = IcGetAppCtrlRules(&pRules, &dwLen);
    printf("Command returned: %d\n", dwResult);
    if (0 == dwResult)
    {
        printf("\n\n-------- Rules --------:\n");
        for (DWORD i = 0; i < dwLen; i++)
        {
            printf("Rule %02d, pM: %d, path: [%S], pid: %d, ppM: %d, parPath: [%S], parPid: %d, verdict: %s (%d), add: %d\n", 
                pRules[i].DwRuleId, 
                pRules[i].MatcherProcessPath,
                pRules[i].PProcessPath,
                pRules[i].DwPid, 
                pRules[i].MatcherParentPath,
                pRules[i].PParentPath,
                pRules[i].DwParentPid,
                pRules[i].Verdict ? "DENY" : "ALLOW", 
                pRules[i].Verdict,
                pRules[i].DwAddTime
            );
        }
    }

    IcFreeAppCtrlRulesList(pRules, dwLen);
}

VOID
EnableFSScan(
    _In_z_      PWCHAR                      PCmd
)
{
    DWORD       dwValue     = 0;
    DWORD       dwIndex     = 0;
    DWORD       dwResult    = 0;

    if (L'\0' == PCmd[0] || L'\n' == PCmd[0])
    {
        printf("Invalid command format!\n");
        PrintHelp();
        return;
    }

    if (L' ' != PCmd[0])
    {
        printf("Invalid command!\n");
        PrintHelp();
        return;
    }

    for (dwIndex = 1; (L' ' == PCmd[dwIndex]) && (L'\0' != PCmd[dwIndex]); dwIndex++);
    dwValue = _wtoi(PCmd + dwIndex);
    if (
        (0 == dwValue && PCmd[dwIndex] != L'0') || 
        (0 != dwValue && 1 != dwValue)
        )
    {
        printf("Invalid VALUE!\n");
        PrintHelp();
        return;
    }

    if (1 == dwValue)
    {
        dwResult = IcStartFSScan();
        if (ERROR_SUCCESS != dwResult)
        {
            LogErrorWin(dwResult, L"IcStartFSScan");
        }
    }
    else
    {
        dwResult = IcStopFSScan();
        if (ERROR_SUCCESS != dwResult)
        {
            LogErrorWin(dwResult, L"IcStopFSScan");
        }
    }
}

VOID
AddFSScanRule(
    VOID
)
{
    DWORD   dwResult            = ERROR_SUCCESS;
    DWORD   dwRuleId            = 0;
    DWORD   dwProcPathMatch     = 0;
    WCHAR   pAuxCmd[MAX_PATH]   = { 0 };
    WCHAR   pProcPath[MAX_PATH] = { 0 };
    DWORD   dwPid               = 0;
    DWORD   dwFilePathMatch     = 0;
    WCHAR   pFilePath[MAX_PATH] = { 0 };
    DWORD   dwDeniedFlags       = 0;

    printf("    * Path Matcher: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwProcPathMatch = _wtoi(pAuxCmd);

    printf("    * Path: ");
    fgetws(pProcPath, MAX_PATH, stdin);
    pProcPath[wcslen(pProcPath) - 1] = 0;

    printf("    * PID: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwPid = _wtoi(pAuxCmd);

    printf("    * File Path Matcher: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwFilePathMatch = _wtoi(pAuxCmd);

    printf("    * File Path: ");
    fgetws(pFilePath, MAX_PATH, stdin);
    pFilePath[wcslen(pFilePath) - 1] = 0;

    printf("    * Denied Flags: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwDeniedFlags = _wtoi(pAuxCmd);


    dwResult = IcAddFSScanRule(
        dwProcPathMatch,
        !_wcsicmp(pProcPath, L"NULL") ? NULL : pProcPath,
        dwPid,
        dwFilePathMatch,
        !_wcsicmp(pFilePath, L"NULL") ? NULL : pFilePath,
        dwDeniedFlags,
        &dwRuleId
    );

    printf("Command returned: %d\n", dwResult);
    printf(">> Rule ID: %d\n", dwRuleId);
}

VOID
DeleteFSScanRule(
    _In_z_      PWCHAR                      PCmd
)
{
    DWORD       dwRuleId    = 0;
    DWORD       dwIndex     = 0;
    DWORD       dwResult    = 0;

    if (L'\0' == PCmd[0] || L'\n' == PCmd[0])
    {
        printf("Invalid command format!\n");
        PrintHelp();
        return;
    }

    if (L' ' != PCmd[0])
    {
        printf("Invalid command!\n");
        PrintHelp();
        return;
    }

    for (dwIndex = 1; (L' ' == PCmd[dwIndex]) && (L'\0' != PCmd[dwIndex]); dwIndex++);
    dwRuleId = _wtoi(PCmd + dwIndex);
    if (
        (0 == dwRuleId && PCmd[dwIndex] != L'0')
        )
    {
        printf("Invalid RULE_ID!\n");
        PrintHelp();
        return;
    }

    dwResult = IcDeleteFSScanRule(dwRuleId);
    printf("Command returned: %d", dwResult);
}

VOID
UpdateFSScanRule(
    VOID
)
{
    DWORD   dwResult            = ERROR_SUCCESS;
    DWORD   dwRuleId            = 0;
    WCHAR   pAuxCmd[MAX_PATH]   = { 0 };
    DWORD   dwProcPathMatch     = 0;
    WCHAR   pProcPath[MAX_PATH] = { 0 };
    DWORD   dwPid               = 0;
    DWORD   dwFilePathMatch     = 0;
    WCHAR   pFilePath[MAX_PATH] = { 0 };
    DWORD   dwDeniedFlags       = 0;

    printf("    * RuleID: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwRuleId = _wtoi(pAuxCmd);

    printf("    * Process Path Matcher: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwProcPathMatch = _wtoi(pAuxCmd);

    printf("    * Process Path: ");
    fgetws(pProcPath, MAX_PATH, stdin);
    pProcPath[wcslen(pProcPath) - 1] = 0;

    printf("    * PID: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwPid = _wtoi(pAuxCmd);

    printf("    * File Path Matcher: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwFilePathMatch = _wtoi(pAuxCmd);

    printf("    * File Path: ");
    fgetws(pFilePath, MAX_PATH, stdin);
    pFilePath[wcslen(pFilePath) - 1] = 0;

    printf("    * Denied operations: ");
    fgetws(pAuxCmd, MAX_PATH, stdin);
    dwDeniedFlags = _wtoi(pAuxCmd);


    dwResult = IcUpdateFSScanRule(
        dwRuleId,
        dwProcPathMatch,
        !_wcsicmp(pProcPath, L"NULL") ? NULL : pProcPath, 
        dwPid, 
        dwFilePathMatch,
        !_wcsicmp(pFilePath, L"NULL") ? NULL : pFilePath, 
        dwDeniedFlags
    );

    printf("Command returned: %d\n", dwResult);
}

VOID
GetAllFSRules(
    VOID
)
{
    PIC_FS_RULE         pRules      = NULL;
    DWORD               dwLen       = 0;
    DWORD               dwResult    = ERROR_SUCCESS;

    dwResult = IcGetFSScanRules(&pRules, &dwLen);
    printf("Command returned: %d\n", dwResult);
    if (0 == dwResult)
    {
        printf("\n\n-------- Rules --------:\n");
        for (DWORD i = 0; i < dwLen; i++)
        {
            printf("Rule %02d, pM: %d, procPath: [%S], pid: %d, fM: %d, filePath: [%S], Dop: %d, add: %d\n", 
                pRules[i].DwRuleId, 
                pRules[i].MatcherProcessPath,
                pRules[i].PProcessPath,
                pRules[i].DwPid, 
                pRules[i].MatcherFilePath,
                pRules[i].PFilePath,
                pRules[i].UlDeniedOperations,
                pRules[i].DwAddTime
            );
        }
    }

    IcFreeAppFSScanList(pRules, dwLen);
}



DWORD
wmain(
    _In_        DWORD                       DwArgc,
    _In_        PWCHAR                     *PPArcv
)
{
    PWCHAR                                  pCmd;
    NTSTATUS                                ntStatus;
    DWORD                                   dwStatusReturned;

    pCmd                                    = NULL;
    ntStatus                                = STATUS_SUCCESS;
    dwStatusReturned                        = ERROR_SUCCESS;
    
    UNREFERENCED_PARAMETER(DwArgc);
    UNREFERENCED_PARAMETER(PPArcv);

    ntStatus = IcImportIcefltUmAPI();
    if (!NT_SUCCESS(ntStatus))
    {
        LogErrorNt(ntStatus, L"IcImportIcefltUmAPI");
        return ntStatus;
    }

    __try
    {
        dwStatusReturned = IcInitConnectionToIceFlt();
        if (ERROR_SUCCESS != dwStatusReturned)
        {
            ntStatus = NTSTATUS_FROM_WIN32(dwStatusReturned);
            LogErrorNt(dwStatusReturned, L"IcInitConnection");
            __leave;
        }

        pCmd = (PWCHAR) malloc(MAX_LENGTH_CMD * sizeof(WCHAR));
        if (NULL == pCmd)
        {
            ntStatus = NTSTATUS_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
            LogErrorNt(ntStatus, L"malloc");
            __leave;
        }

        printf("Type help for commands...\n");
        while (TRUE)
        {
            printf(">");
            fgetws(pCmd, MAX_LENGTH_CMD, stdin);

            if (0 == _wcsnicmp(pCmd, CMD_HELP, wcslen(CMD_HELP)))
            {
                PrintHelp();
            }
            else if (0 == _wcsnicmp(pCmd, CMD_SETOPTION, wcslen(CMD_SETOPTION)))
            {
                SendOption(pCmd + wcslen(CMD_SETOPTION));
            }
            else if (0 == _wcsnicmp(pCmd, CMD_ENABLE_APPCTRL, wcslen(CMD_ENABLE_APPCTRL)))
            {
                EnableAppCtrl(pCmd + wcslen(CMD_ENABLE_APPCTRL));
            }
            else if (0 == _wcsnicmp(pCmd, CMD_ADD_APPCTRL_RULE, wcslen(CMD_ADD_APPCTRL_RULE)))
            {
                AddAppCtrlRule();
            }
            else if (0 == _wcsnicmp(pCmd, CMD_DELETE_APPCTRL_RULE, wcslen(CMD_DELETE_APPCTRL_RULE)))
            {
                DeleteAppCtrlRule(pCmd + wcslen(CMD_DELETE_APPCTRL_RULE));
            }
            else if (0 == _wcsnicmp(pCmd, CMD_UPDATE_APPCTRL_RULE, wcslen(CMD_UPDATE_APPCTRL_RULE)))
            {
                UpdateAppCtrlRule();
            }
            else if (0 == _wcsnicmp(pCmd, CMD_GET_APPCTRL_RULES, wcslen(CMD_GET_APPCTRL_RULES)))
            {
                GetAllAppCtrl(pCmd + wcslen(CMD_GET_APPCTRL_RULES));
            }

            else if (0 == _wcsnicmp(pCmd, CMD_ADD_FSSCAN_RULE, wcslen(CMD_ADD_FSSCAN_RULE)))
            {
                AddFSScanRule();
            }
            else if (0 == _wcsnicmp(pCmd, CMD_DELETE_FSSCAN_RULE, wcslen(CMD_DELETE_FSSCAN_RULE)))
            {
                DeleteFSScanRule(pCmd + wcslen(CMD_DELETE_FSSCAN_RULE));
            }
            else if (0 == _wcsnicmp(pCmd, CMD_UPDATE_FSSCAN_RULE, wcslen(CMD_UPDATE_FSSCAN_RULE)))
            {
                UpdateFSScanRule();
            }
            else if (0 == _wcsnicmp(pCmd, CMD_GET_FSSCAN_RULES, wcslen(CMD_GET_FSSCAN_RULES)))
            {
                GetAllFSRules();
            }
            else if (0 == _wcsnicmp(pCmd, CMD_ENABLE_FS_SCAN, wcslen(CMD_ENABLE_FS_SCAN)))
            {
                EnableFSScan(pCmd + wcslen(CMD_ENABLE_FS_SCAN));
            }
            else if (0 == _wcsnicmp(pCmd, CMD_EXIT, wcslen(CMD_EXIT)))
            {        
                printf("gata, ies\n");
                break;
            }
            else
            {
                printf("Invalid command!\n");
                PrintHelp();
            }
        }
    }
    __finally
    {
        IcStopAppCtrlScan();
        
        IcStopFSScan();

        IcUninitConnectionToIceFlt();

        IcFreeIcefltUmAPI();

        if (NULL != pCmd)
        {
            free(pCmd);
            pCmd = NULL;
        }
    }

    return ntStatus; 
}
