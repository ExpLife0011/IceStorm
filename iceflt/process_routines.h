#ifndef __PROCESS_ROUTINES_H__
#define __PROCESS_ROUTINES_H__

#include <fltKernel.h>

_Success_(NT_SUCCESS(return))
NTSTATUS
IceRegisterProcessCallback(
    VOID
);

VOID
IceCleanupProcessCalback(
    VOID
);

#endif