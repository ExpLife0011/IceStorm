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



    }
    __finally
    {
        StopClient();
    }

    return ERROR_SUCCESS;
}
