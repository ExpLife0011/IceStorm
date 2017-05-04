#ifndef __OS_VERSION__
#define __OS_VERSION__

#include <wdm.h>
#include <ntddk.h>

_Success_(FALSE != return)
BOOLEAN
IceVerifyWindowsVersion(
    _In_        RTL_OSVERSIONINFOEXW       *PVersionInfo, 
    _In_        ULONGLONG                   UllConditionMask
);

_Success_(FALSE != return)
BOOLEAN
IsWindows7OrLater(
    VOID
);

_Success_(FALSE != return)
BOOLEAN
IsWindows8OrLater(
    VOID
);

_Success_(FALSE != return)
BOOLEAN
IsWindows81OrLater(
    VOID
);

_Success_(FALSE != return)
BOOLEAN
IsWindows10OrLater(
    VOID
);

#endif // !__OS_VERSION__
