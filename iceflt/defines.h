#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <fltKernel.h>

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryInformationProcess(
    __in HANDLE ProcessHandle,
    __in PROCESSINFOCLASS ProcessInformationClass,
    __out_bcount_opt(ProcessInformationLength) PVOID ProcessInformation,
    __in ULONG ProcessInformationLength,
    __out_opt PULONG ReturnLength
);

#endif // ~__DEFINES_H__