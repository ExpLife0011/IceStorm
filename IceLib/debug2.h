#pragma once

#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>

#define NT_SUCCESS(Status)                  (((NTSTATUS)(Status)) >= 0)

#include <stdio.h>

#ifndef NOTHING
#define NOTHING
#endif // !NOTHING


DWORD
HRESULT_TO_WIN32ERROR(
    HRESULT                                 HRes
);


#ifdef _DEBUG

#pragma warning(disable: 6271) // Extra argument passed to 'IceDebugLog':  _Param_(2) is not used by the format string.

_Success_(NULL != return)
PCHAR
IcNtStatusToString(
    _In_        NTSTATUS                    NtStatus
);

VOID
IcDebugPrint(
    _In_z_      PWCHAR                      PFormat,
    ...
);

#define __FILENAME__ (((NULL != strchr(__FILE__, '\\')) && (NULL != strchr(strrchr(__FILE__, '\\') - 12, '\\'))) ? (strchr(strrchr(__FILE__, '\\') - 12, '\\') + 1) : __FILE__)

#define IC_LOG_FORMAT                                                   L"IUM %-11s %-25S %4d -- "

#define IcDebugLog(PLevel, PFormat, ...)                                IcDebugPrint(IC_LOG_FORMAT, PLevel, __FILENAME__, __LINE__),     \
                                                                        IcDebugPrint(PFormat L"\n", __VA_ARGS__)

#define IcDebugLogWin(WinError, PLevel, PFormat, ...)                   IcDebugPrint(IC_LOG_FORMAT, PLevel, __FILENAME__, __LINE__),     \
                                                                        IcDebugPrint(PFormat, __VA_ARGS__),               \
                                                                        IcDebugPrint(L"  {Error: 0x%08X <--> %s}\n", WinError, L"??")

#define IcDebugLogNt(NtStatus, PLevel, PFormat, ...)                    IcDebugPrint(IC_LOG_FORMAT, PLevel, __FILENAME__, __LINE__),     \
                                                                        IcDebugPrint(PFormat, __VA_ARGS__),               \
                                                                        IcDebugPrint(L"  {Error: 0x%08X <--> %S}\n", NtStatus, IcNtStatusToString(NtStatus))

#else

#define IcDebugLog(PLevel, PFormat, ...)                                {NOTHING;}
#define IcDebugLogWin(WinError, PLevel, PFormat, ...)                   {NOTHING;}
#define IcDebugLogNt(NtStatus, PLevel, PFormat, ...)                    {NOTHING;}

#endif // !DBG



#define LogInfo(PFormat, ...)                   IcDebugLog(L"[INFO]", PFormat, __VA_ARGS__+0)

#define LogWarning(PFormat, ...)                IcDebugLog(L"[WARNING]", PFormat, __VA_ARGS__+0)

#define LogError(PFormat, ...)                  IcDebugLog(L"[ERROR]", PFormat, __VA_ARGS__+0)

#define LogErrorWin(WinError, PFormat, ...)     IcDebugLogWin(WinError, L"[ERROR]", PFormat, __VA_ARGS__+0)

#define LogWarningWin(WinError, PFormat, ...)   IcDebugLogWin(WinError, L"[WARNING]", PFormat, __VA_ARGS__+0)

#define LogErrorNt(NtStatus, PFormat, ...)      IcDebugLogNt(NtStatus, L"[ERROR]", PFormat, __VA_ARGS__+0)

#define LogWarningNt(NtStatus, PFormat, ...)    IcDebugLogNt(NtStatus, L"[WARNING]", PFormat, __VA_ARGS__+0)


