#ifndef __ICE_FS_SCAN_H__
#define __ICE_FS_SCAN_H__

#include <fltKernel.h>

NTSTATUS
IceScanFSPreCreate(
    _In_    HANDLE              HProcessPid,
    _In_    PUNICODE_STRING     PUSProcessPath,
    _In_    PUNICODE_STRING     PUSFilePath,
    _In_    ULONG               UlPreOpFlags,
    _In_    ULONG              *PUlNewOpFlags
);

#endif // !__ICE_FS_SCAN_H__
