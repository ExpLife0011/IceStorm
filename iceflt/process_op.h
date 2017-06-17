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

NTSTATUS
IceGetUMFilePath(
    _In_    PFLT_CALLBACK_DATA                  PData,
    _Inout_ PUNICODE_STRING                    *PPUSUmFilePath
);

NTSTATUS
IceCreateUMPath(
    _In_    PUNICODE_STRING                     KMName,
    _In_    PUNICODE_STRING                     VolumeKMName,
    _In_    PUNICODE_STRING                     VolumeDosName,
    _Out_   PUNICODE_STRING                    *DosName
);

NTSTATUS
IceGetUMProcessPath(
    _In_    UNICODE_STRING             *PKMProcessPath,
    _Out_   PUNICODE_STRING            *PPUMProcessPath
);

#endif // !__PROCESS_OP_H__
