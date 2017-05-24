#ifndef __FS_HELPERS_H__
#define __FS_HELPERS_H__

#include <fltKernel.h>

BOOLEAN
IceIsCsvDlEcpPresent(
    _In_ PFLT_CALLBACK_DATA PData
);

BOOLEAN
IceIsPrefetchEcpPresent(
    _In_ PFLT_CALLBACK_DATA PData
);
//
//NTSTATUS
//IcePreCreateCsvfs(
//    _Inout_     PFLT_CALLBACK_DATA      PData,
//    _In_        PCFLT_RELATED_OBJECTS   PFltObjects
//);
//
//NTSTATUS
//IceAddCsvRevisionECP(
//    _Inout_     PFLT_CALLBACK_DATA      PData
//);

NTSTATUS
IceGetFileEncrypted(
    _In_   PFLT_INSTANCE    PInstance,
    _In_   PFILE_OBJECT     PFileObject,
    _Out_  PBOOLEAN         PBEncrypted
);

BOOLEAN
IceIsStreamAlternate(
    _Inout_ PFLT_CALLBACK_DATA PData
);

#endif // !__FS_HELPERS_H__
