#ifndef __PROCESS_LIST_H__
#define __PROCESS_LIST_H__

#include <intsafe.h>
#include <ntdef.h>
#include <ntstatus.h>


NTSTATUS
IceProLstInitialize(
    _In_        WORD                        WMaximumCacheSize
);

VOID
IceProLstUninitialize(
    VOID
);

VOID
IceProLstClearAll(
    VOID
);

NTSTATUS
IceProLstAddProcess(
    _In_z_ PWCHAR                           PPath
);

#endif // !__PROCESS_LIST_H__
