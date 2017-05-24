#ifndef __PROCESS_OP_H__
#define __PROCESS_OP_H__

#include <fltKernel.h>

NTSTATUS
IceGetProcessPathByHandle(
    _In_    HANDLE                              HProcess,
    _Out_   UNICODE_STRING                    **PPPath
);

NTSTATUS
IceGetProcessPathByPid(
    _In_    HANDLE                              HProcessId,
    _Out_   UNICODE_STRING                    **PPPath
);

#endif // !__PROCESS_OP_H__
