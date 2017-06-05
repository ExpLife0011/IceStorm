#include "debug2.h"
#include "manager.h"
#include "client.h"
#include "global_data.h"

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
    CHAR            pIpAddr[MAX_PATH]       = { 0 };
    CHAR            pPort[MAX_PATH]         = { 0 };

    UNREFERENCED_PARAMETER(PParam);

    

    __try
    {
        LoadServerInfo(pIpAddr, pPort);
        LogInfo(L"Server address: %S:%S", pIpAddr, pPort);

        while (!StartClient(pIpAddr, pPort))
        {
            if (SleepOrExit(5 * 1000)) __leave;
        }




        LogInfo(L"We are now connected to the server...");
    }
    __finally
    {
        StopClient();
    }

    return ERROR_SUCCESS;
}
