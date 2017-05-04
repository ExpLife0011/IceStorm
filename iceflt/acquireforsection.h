#ifndef __ACQUIREFORSECTION_H__
#define __ACQUIREFORSECTION_H__

#include <fltKernel.h>

NTSTATUS
IceRegisterLoadImageCallback(
    VOID
);

VOID
IceCleanupLoadImage(
    VOID
);

FLT_PREOP_CALLBACK_STATUS
IcePreAcquireForSection(
    _Inout_ PFLT_CALLBACK_DATA    Data,
    _In_    PCFLT_RELATED_OBJECTS FltObjects,
    _Out_   PVOID                 *CompletionContext
);

#endif // !__ACQUIREFORSECTION_H__
