#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "tags.h"
#include <fltKernel.h>
#include <Ntstrsafe.h>
#include <suppress.h>

#if DBG

PUCHAR
IceNtStatusToString(
    _In_ NTSTATUS Status
);

VOID
DrvDumpMemory(
    _In_ PVOID PAddress,
    _In_ DWORD DwLength
);

#define ICE_LOG_FORMAT                                              "Ice %-11s %-20s Line: %4d :: "

#define IceDebugLog(PLevel, PFile, DwLine, PFormat, ...)            KdPrint((ICE_LOG_FORMAT, PLevel, PFile, DwLine)),    \
                                                                    KdPrint((PFormat "\n", __VA_ARGS__))

#define IceDebugLogNt(NtError, PLevel, PFile, DwLine, PFormat, ...) KdPrint((ICE_LOG_FORMAT, PLevel, PFile, DwLine)),    \
                                                                    KdPrint((PFormat "\n", __VA_ARGS__)),                                   \
                                                                    KdPrint(("{NtError: 0x%08X <--> %s}\n", NtError, IceNtStatusToString(NtError)))

#else

#define IceDebugLog(PLevel, PFile, DwLine, PFormat, ...)            {NOTHING;}
#define IceDebugLogNt(NtError, PLevel, PFile, DwLine, PFormat, ...) {NOTHING;}

#endif // !DBG


#pragma warning(push)
#pragma warning(suppress: 6271) // Extra argument passed to 'IceDebugLog':  _Param_(2) is not used by the format string.


#define LogInfo(PFormat, ...)                   IceDebugLog("[INFO]", __FILE__, __LINE__, PFormat, __VA_ARGS__+0)

#define LogWarning(PFormat, ...)                IceDebugLog("[WARNING]", __FILE__, __LINE__, PFormat, __VA_ARGS__+0)

#define LogError(PFormat, ...)                  IceDebugLog("[ERROR]", __FILE__, __LINE__, PFormat, __VA_ARGS__+0)

#define LogErrorNt(NtError, PFormat, ...)       IceDebugLogNt(NtError, "[ERROR]", __FILE__, __LINE__, PFormat, __VA_ARGS__+0)

#define LogWarningNt(NtError, PFormat, ...)     IceDebugLogNt(NtError, "[WARNING]", __FILE__, __LINE__, PFormat, __VA_ARGS__+0)

#pragma warning(pop)



#endif // !__DEBUG_H__

