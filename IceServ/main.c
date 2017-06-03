#include "debug2.h"
#include "service.h"


#include <stdio.h>
#include <conio.h>

#define CMD_HELP        L"help"
#define CMD_INSTALL     L"install"
#define CMD_DEBUG       L"debug"
#define CMD_UNINSTALL   L"uninstall"
#define CMD_START       L"start"
#define CMD_STOP        L"stop"

VOID
PrintHelp(
    VOID
)
{
    wprintf(L"\n\n----- HELP -----\n");
    wprintf(L"%s - this screen\n", CMD_HELP);
    wprintf(L"%s - start in debug mode\n", CMD_DEBUG);
    wprintf(L"%s - install service\n", CMD_INSTALL);
    wprintf(L"%s - uninstall service\n", CMD_UNINSTALL);
    wprintf(L"\n\n");
}

VOID
InitService(
    VOID
)
{
#pragma warning(push)
#pragma warning( disable : 4204)
    SERVICE_TABLE_ENTRY DispatchTable[] = {
        { gServiceName, (LPSERVICE_MAIN_FUNCTION) SrvMain },
        { NULL, NULL }
    };
#pragma warning( pop )
    LogInfo(L"Starting the service...");

    StartServiceCtrlDispatcherW(DispatchTable);
}

BOOL 
DebugCtrlHandler(
    DWORD DwCtrlType
)
{
    BOOLEAN bStop = TRUE;

    switch (DwCtrlType)
    {
        // Handle the CTRL-C signal. 
        case CTRL_C_EVENT:
        {
            wprintf(L"Ctrl-C event\n\n");
            break;
        }

        // CTRL-CLOSE: confirm that the user wants to exit. 
        case CTRL_CLOSE_EVENT:
        {
            wprintf(L"Ctrl-Close event\n\n");
            break;
        }

        // Pass other signals to the next handler. 
        case CTRL_BREAK_EVENT:
        {
            bStop = FALSE;
            wprintf(L"Ctrl-Break event\n\n");
            break;
        }

        case CTRL_LOGOFF_EVENT:
        {
            wprintf(L"Ctrl-Logoff event\n\n");
            break;
        }

        case CTRL_SHUTDOWN_EVENT:
        {
            wprintf(L"Ctrl-Shutdown event\n\n");
            break;
        }

        default:
        {
            bStop = FALSE;
            break;
        }
    }

    if (bStop)
    {
        SrvStop();
    }

    return bStop;
}
DWORD
wmain(
    _In_        DWORD       DwArgc,
    _In_        PWCHAR     *PPArgv
)
{
    DWORD dwRetVal = ERROR_SUCCESS;

    do
    {
        if (DwArgc <= 1)
        {
            InitService();
            break;
        }
        
        if (!_wcsicmp(CMD_HELP, PPArgv[1]))
        {
            PrintHelp();
            break;
        }

        if (!_wcsicmp(CMD_INSTALL, PPArgv[1]))
        {
            LogInfo(L"Starting install...");
            dwRetVal = SrvInstall();
            break;
        }

        if (!_wcsicmp(CMD_UNINSTALL, PPArgv[1]))
        {
            LogInfo(L"Starting uninstall...");
            dwRetVal = SrvUninstall();
            break;
        }

        if (!_wcsicmp(CMD_DEBUG, PPArgv[1]))
        {
            LogInfo(L"Starting debug mode...");

            if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE) DebugCtrlHandler, TRUE))
            {
                dwRetVal = GetLastError();
                LogErrorWin(dwRetVal, L"SetConsoleCtrlHandler(DebugCtrlHandler)");
            }

            dwRetVal = SrvInit();
            if (dwRetVal != ERROR_SUCCESS)
            {
                LogErrorWin(dwRetVal, L"Init");
                break;
            }

            SrvRun();
            SrvDone();
            break;
        }

        dwRetVal = ERROR_INVALID_PARAMETER;
        LogErrorWin(dwRetVal, L"Unknown command: \"%s\"", PPArgv[1]);
        break;
        

    } while (0);


    LogInfo(L"Operation returned: %d", dwRetVal);
    return dwRetVal;
}
