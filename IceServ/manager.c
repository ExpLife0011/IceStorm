#include "debug2.h"
#include "manager.h"
#include "client.h"

HANDLE gHStop = NULL;

BOOLEAN
SleepOrExit(
    _In_    DWORD           DwMilliseconds
)
{
    LogInfo(L"Will wait for %d milliseconds", DwMilliseconds);

    if (WAIT_OBJECT_0 == WaitForSingleObject(gHStop, DwMilliseconds))
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
    sprintf_s(PIpAddr, MAX_PATH, "192.168.194.1");
    sprintf_s(PPort, MAX_PATH, "12345");
}

_Use_decl_anno_impl_
DWORD
WINAPI
ManagerThread(
    PVOID                       PParam
)
{
    PMANAGER_PARAM  pParam                  = (PMANAGER_PARAM) PParam;
    CHAR            pIpAddr[MAX_PATH]       = { 0 };
    CHAR            pPort[MAX_PATH]         = { 0 };

    gHStop = *pParam->PHStopEvent;

    __try
    {
        LoadServerInfo(pIpAddr, pPort);
        LogInfo(L"Server address: %S:%S", pIpAddr, pPort);

        while (!StartClient(pIpAddr, pPort))
        {
            if (SleepOrExit(5000)) __leave;
        }

        LogInfo(L"We are now connected to the server...");
    }
    __finally
    {
        StopClient();

        free(PParam);
        PParam = NULL;
    }

    return ERROR_SUCCESS;
}
