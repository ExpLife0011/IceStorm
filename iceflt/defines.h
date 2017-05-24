#ifndef __DEFINES_H__
#define __DEFINES_H__

#include <fltKernel.h>

#ifndef GUID_ECP_CSV_DOWN_LEVEL_OPEN
#define GUID_ECP_CSV_DOWN_LEVEL_OPEN_notdefined
DEFINE_GUID(GUID_ECP_CSV_DOWN_LEVEL_OPEN, 0x4248be44, 0x647f, 0x488f, 0x8b, 0xe5, 0xa0, 0x8a, 0xaf, 0x70, 0xf0, 0x28);
#endif

#ifndef GUID_ECP_CSV_QUERY_FILE_REVISION
#define GUID_ECP_CSV_QUERY_FILE_REVISION_notdefined
DEFINE_GUID(GUID_ECP_CSV_QUERY_FILE_REVISION, 0x44aec90b, 0xde65, 0x4d46, 0x8f, 0xbf, 0x76, 0x3f, 0x9d, 0x97, 0xb, 0x1d);
#endif



typedef struct _CSV_QUERY_FILE_REVISION_ECP_CONTEXT
{

    //
    // NTFS File Id
    //
    LONGLONG FileId;

    //
    // FileRevision[0] increases each time the CSV MDS
    // stack is rebuilt and CSVFLT loses its state.
    //
    // FileRevision[1] increases each time CSV MDS
    // stack purges the cached revision # for the
    // file.
    //
    // FileRevision[2] increases each time CSV MDS
    // observes that file sizes might have
    // changed or the file might have been written
    // to. It also is incremented when one of the
    // nodes performs the first direct IO on a stream
    // associated with this file after opening this stream.
    //
    // If any of the numbers are 0 the caller should
    // assume that the file was modified
    //
    LONGLONG FileRevision[3];


} CSV_QUERY_FILE_REVISION_ECP_CONTEXT, *PCSV_QUERY_FILE_REVISION_ECP_CONTEXT;


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