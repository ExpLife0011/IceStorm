#pragma once

#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS

typedef struct _MANAGER_PARAM
{
    HANDLE         *PHStopEvent;

} MANAGER_PARAM, *PMANAGER_PARAM;

DWORD 
WINAPI
ManagerThread(
    _In_    PVOID       PParam
);
