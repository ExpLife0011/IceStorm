#include "debug2.h"
#include "import_icefltum.h"
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#define MAX_LENGTH_CMD                      4096

#define CMD_HELP                            L"help"
#define CMD_SETOPTION                       L"SetOption"
#define CMD_ENABLE_APPCTRL                  L"enable appctrl"
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
