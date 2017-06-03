#ifndef __ICE_SERVICE_H__
#define __ICE_SERVICE_H__

#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS

extern PWCHAR gServiceName;

_Success_(return == ERROR_SUCCESS)
DWORD
SrvInstall(
    VOID
);

_Success_(return == ERROR_SUCCESS)
DWORD
SrvUninstall(
    VOID
);

VOID
SrvMain(
    _In_    DWORD           DwArgc,
    _In_    LPWSTR         *PPArgv
);

_Success_(return == ERROR_SUCCESS)
DWORD
SrvInit(
    VOID
);

_Success_(return == ERROR_SUCCESS)
DWORD
SrvRun(
    VOID
);

_Success_(return == ERROR_SUCCESS)
DWORD
SrvStop(
    VOID
);

_Success_(return == ERROR_SUCCESS)
DWORD
SrvDone(
    VOID
);

#endif // !__SERVICE_H__
