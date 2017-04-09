#ifndef __PROCESS_LIST_H__
#define __PROCESS_LIST_H__

#include <intsafe.h>
#include <ntdef.h>
#include <ntstatus.h>


_Success_(NT_SUCCESS(return))
NTSTATUS
IceProLstInitialize(
    _In_        WORD                        WMaximumCacheSize
);

_Success_(NT_SUCCESS(return))
VOID
IceProLstUninitialize(
    VOID
);

VOID
IceProLstClearAll(
    VOID
);

#endif // !__PROCESS_LIST_H__
