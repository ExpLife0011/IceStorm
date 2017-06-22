#include "service.h"
#include "debug2.h"
#include "import_icefltum.h"
#include "manager.h"
#include "global_data.h"

#define MANAGER_THREAD_WAIT     (30 * 1000)

typedef struct _SERVICE_DATA
{
    //Service                *PSrv;
    SERVICE_STATUS_HANDLE   HStatus;
    SERVICE_STATUS          ServiceStatus;
    DWORD                   DwCheckPoint;
} SERVICE_DATA, *PSERVICE_DATA;

PWCHAR  gServiceName    = L"IceServ";

HANDLE  gHStopEvent     = NULL;
BOOLEAN gBInitCalled    = FALSE;
HANDLE  gHManagerThread = NULL;

_Use_decl_anno_impl_
DWORD
SrvInit(
    VOID
)
{
    DWORD           dwRetVal    = ERROR_SUCCESS;
    
    __try
    {
        gHStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (NULL == gHStopEvent)
        {
            dwRetVal = GetLastError();
            LogErrorWin(dwRetVal, L"CreateEvent");
            __leave;
        }

        dwRetVal = IcImportIcefltUmAPI();
        if (ERROR_SUCCESS != dwRetVal)
        {
            LogErrorNt(dwRetVal, L"IcImportIcefltUmAPI");
            __leave;
        }
        LogInfo(L"Functions imported");

        dwRetVal = IcInitConnectionToIceFlt();
        if (ERROR_SUCCESS != dwRetVal)
        {
            LogErrorWin(dwRetVal, L"IcInitConnectionToIceFlt");
            __leave;
        }
        LogInfo(L"Connection to iceflt was initialized");

        gHManagerThread = CreateThread(NULL, 0, ManagerThread, NULL, 0, NULL);
        if (NULL == gHManagerThread)
        {
            dwRetVal = GetLastError();
            LogErrorWin(dwRetVal, L"CreateThread");
            __leave;
        }
        LogInfo(L"Manager Thread created");

        LogInfo(L"Init is OK");
        gBInitCalled = TRUE;
    } 
    __finally
    {

    }

    return dwRetVal;
}

_Use_decl_anno_impl_
DWORD
SrvDone(
    VOID
)
{
    DWORD dwRetVal = ERROR_SUCCESS;

    if (!gBInitCalled) return ERROR_SUCCESS;
    
    __try
    {
        SrvStop();

        if (NULL != gHManagerThread)
        {
            WaitForSingleObject(gHManagerThread, MANAGER_THREAD_WAIT);

            CloseHandle(gHManagerThread);
            gHManagerThread = NULL;
        }

        if (NULL != gHStopEvent)
        {
            CloseHandle(gHStopEvent);
            gHStopEvent = NULL;
        }

        (VOID) IcStopAppCtrlScan(FALSE);

        (VOID) IcStopFSScan(FALSE);

        dwRetVal = IcUninitConnectionToIceFlt();
        if (ERROR_SUCCESS != dwRetVal)
        {
            LogErrorWin(dwRetVal, L"IcUninitConnectionToIceFlt");
            __leave;
        }

        IcFreeIcefltUmAPI();
    } 
    __finally
    {

    }

    gBInitCalled = FALSE;
    return dwRetVal;
}
_Use_decl_anno_impl_
DWORD
SrvStop(
    VOID
)
{
    if (NULL == gHStopEvent) return ERROR_INVALID_PARAMETER;
    
    if (!SetEvent(gHStopEvent))
    {
        LogErrorWin(GetLastError(), L"SetEvent(gHStopEvent)");
        return GetLastError();
    }
    Sleep(200);
    LogInfo(L"Stop event signaled.");

    return ERROR_SUCCESS;
}

_Use_decl_anno_impl_
DWORD
SrvRun(
    VOID
)
{
    WaitForSingleObject(gHStopEvent, INFINITE);
    Sleep(100);
    return ERROR_SUCCESS;
}

_Use_decl_anno_impl_
DWORD
SrvInstall(
    VOID
)
{
    SC_HANDLE   hSCManager      = NULL;
    SC_HANDLE   hService        = NULL;
    WCHAR       pPath[MAX_PATH] = { 0 };
    DWORD       dwRetVal        = ERROR_SUCCESS;

    __try
    {
        if (!GetModuleFileName(NULL, pPath, MAX_PATH))
        {
            dwRetVal = GetLastError();
            LogErrorWin(dwRetVal, L"GetModuleFileName");
            __leave;
        }

        hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (NULL == hSCManager)
        {
            dwRetVal = GetLastError();
            LogErrorWin(dwRetVal, L"OpenSCManager");
            __leave;
        }

        hService = CreateService(
            hSCManager,
            gServiceName, 
            gServiceName, 
            SERVICE_ALL_ACCESS,       
            SERVICE_WIN32_OWN_PROCESS,
            SERVICE_AUTO_START,       
            SERVICE_ERROR_NORMAL,     
            pPath,                     
            NULL,                     
            NULL,                     
            NULL,                     
            NULL,                     
            NULL                      
        );
        if (NULL == hService)
        {
            dwRetVal = GetLastError();
            LogErrorWin(dwRetVal, L"CreateService");
            __leave;
        }
    }
    __finally
    {
        if (NULL != hSCManager)
        {
            CloseServiceHandle(hSCManager);
            hSCManager = NULL;
        }

        if (NULL != hService)
        {
            CloseServiceHandle(hService);
            hService = NULL;
        }
    }

    return dwRetVal;
}

_Use_decl_anno_impl_
DWORD
SrvUninstall(
    VOID
)
{
    SC_HANDLE   hSCManager = NULL;
    SC_HANDLE   hService = NULL;
    DWORD       dwRetVal = 0;

    __try
    {
        hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (NULL == hSCManager)
        {
            dwRetVal = GetLastError();
            LogErrorWin(dwRetVal, L"OpenSCManager");
            __leave;
        }
        
        hService = OpenService(hSCManager, gServiceName, DELETE);
        if (NULL == hService)
        {
            dwRetVal = GetLastError();
            LogErrorWin(dwRetVal, L"OpenService");
            __leave;
        }
        
        if (!DeleteService(hService))
        {
            dwRetVal = GetLastError();
            LogErrorWin(dwRetVal, L"DeleteService");
            __leave;
        }
    } 
    __finally
    {
        if (NULL != hService)
        {
            CloseServiceHandle(hService);
            hService = NULL;
        }

        if (NULL != hSCManager)
        {
            CloseServiceHandle(hSCManager);
            hSCManager = NULL;
        }
    }

    return dwRetVal;
}

VOID
ReportSvcStatus(
    _In_    PSERVICE_DATA       PServiceData,
    _In_    DWORD               DwCurrentState,
    _In_    DWORD               DwWin32ExitCode,
    _In_    DWORD               DwWaitHint
)
{
    PServiceData->ServiceStatus.dwCurrentState = DwCurrentState;
    PServiceData->ServiceStatus.dwWin32ExitCode = DwWin32ExitCode;
    PServiceData->ServiceStatus.dwWaitHint = DwWaitHint;

    if (DwCurrentState == SERVICE_START_PENDING)
    {
        PServiceData->ServiceStatus.dwControlsAccepted = 0;
    }
    else
    {
        PServiceData->ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_SESSIONCHANGE | SERVICE_ACCEPT_POWEREVENT | SERVICE_ACCEPT_STOP;
    }

    if ((DwCurrentState == SERVICE_RUNNING) || (DwCurrentState == SERVICE_STOPPED))
    {
        PServiceData->DwCheckPoint = 0;
    }
    else
    {
        PServiceData->DwCheckPoint++;
    }

    if (!SetServiceStatus(PServiceData->HStatus, &PServiceData->ServiceStatus))
    {
        LogErrorWin(GetLastError(), L"SetServiceStatus...");
    }
}

DWORD 
WINAPI 
SrvCtrlHandler(
    _In_    DWORD      DwCtrl, 
    _In_    DWORD      DwEventType, 
    _In_    LPVOID     PEventData, 
    _In_    LPVOID     PContext
)
{
    PSERVICE_DATA pServiceData = (PSERVICE_DATA) PContext;

    UNREFERENCED_PARAMETER(DwEventType);
    UNREFERENCED_PARAMETER(PEventData);
    
    switch (DwCtrl)
    {
        case SERVICE_CONTROL_STOP:
        case SERVICE_CONTROL_SHUTDOWN:
        {
            ReportSvcStatus(pServiceData, SERVICE_STOP_PENDING, NO_ERROR, 0);
            SrvStop();
        }
        break;
    }

    return 0;
}

_Use_decl_anno_impl_
VOID
SrvMain(
    DWORD               DwArgc,
    LPWSTR             *PPArgv
)
{
    SERVICE_DATA    sd          = { 0 };
    DWORD           dwStatus    = ERROR_SUCCESS;

    UNREFERENCED_PARAMETER(DwArgc);
    UNREFERENCED_PARAMETER(PPArgv);


    do
    {
        sd.HStatus = RegisterServiceCtrlHandlerEx(gServiceName, SrvCtrlHandler, &sd);
        if (NULL == sd.HStatus)
        {
            LogErrorWin(GetLastError(), L"RegisterServiceCtrlHandlerEx");
            break;
        }

        sd.ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
        sd.ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
        sd.ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_SESSIONCHANGE | SERVICE_ACCEPT_POWEREVENT;
        sd.ServiceStatus.dwWin32ExitCode = 0;
        sd.ServiceStatus.dwServiceSpecificExitCode = 0;
        sd.ServiceStatus.dwCheckPoint = 0;
        sd.ServiceStatus.dwWaitHint = 0;

        if (!SetServiceStatus(sd.HStatus, &sd.ServiceStatus))
        {
            LogErrorWin(GetLastError(), L"SetServiceStatus");
            break;
        }

        dwStatus = SrvInit();
        if (ERROR_SUCCESS != dwStatus)
        {
            LogErrorWin(dwStatus, L"Init");
            break;
        }
        ReportSvcStatus(&sd, SERVICE_RUNNING, NO_ERROR, 0);

        SrvRun();

    } while (0);
    
    SrvDone();

    LogInfo(L"Bye Bye");
    ReportSvcStatus(&sd, SERVICE_STOPPED, NO_ERROR, 0);
}
