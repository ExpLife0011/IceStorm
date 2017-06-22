#ifndef __ICE_USER_COMMON_H__
#define __ICE_USER_COMMON_H__


#ifdef KERNEL_MODE
#include <ntddk.h>
#else

#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS

#endif // KERNEL_MODE

#define ICE_APPCTRL_PORT                    L"\\IceAppCtrlPort"
#define ICE_SCAN_CONTROL_PORT               L"\\IceScanControlPort"
#define ICE_SCAN_FS_PORT                    L"\\IceScanFSPort"

typedef enum _ICE_FS_CREATE_FLAGS
{
    ICE_FS_FLAG_CREATE  = 0x1,
    ICE_FS_FLAG_OPEN    = 0x2,
    ICE_FS_FLAG_WRITE   = 0x4,
    ICE_FS_FLAG_READ    = 0x8,
    ICE_FS_FLAG_DELETE  = 0x10

} ICE_FS_CREATE_FLAGS, *PICE_FS_CREATE_FLAGS;

#pragma pack(push)      
#pragma pack(8)      

typedef struct _ICE_GENERIC_PACKET
{
    //UCHAR		Minor; 
    //UCHAR		Major;
    DWORD                                   DwPacketLength;
    DWORD                                   DwRequestType;
} ICE_GENERIC_PACKET, *PICE_GENERIC_PACKET;

// appctrl
typedef struct _ICE_APP_CTRL_SCAN_REQUEST_PACKET
{
    DWORD                                   DwPid;
    DWORD                                   DwParentPid;
    DWORD                                   DwProcessPathSize;
    DWORD                                   DwParentPathSize;
    WCHAR                                   PStrings[1];

} ICE_APP_CTRL_SCAN_REQUEST_PACKET, *PICE_APP_CTRL_SCAN_REQUEST_PACKET;

typedef struct _ICE_APP_CTRL_SCAN_RESULT_PACKET
{
    NTSTATUS                                NtScanResult;
} ICE_APP_CTRL_SCAN_RESULT_PACKET, *PICE_APP_CTRL_SCAN_RESULT_PACKET;

// fs scan
typedef struct _ICE_FS_SCAN_REQUEST_PACKET
{
    DWORD                                   DwIceFsFlags;
    
    DWORD                                   DwPid;

    DWORD                                   DwProcessPathSize;
    DWORD                                   DwFilePathSize;

    WCHAR                                   PStrings[1];

} ICE_FS_SCAN_REQUEST_PACKET, *PICE_FS_SCAN_REQUEST_PACKET;

typedef struct _ICE_FS_SCAN_RESULT_PACKET
{
    DWORD                                   DwIceFsNewFlags;
} ICE_FS_SCAN_RESULT_PACKET, *PICE_FS_SCAN_RESULT_PACKET;

#pragma pack(pop)


typedef struct _ICEFLT_VERSION
{
    DWORD                                   DwMinor;
    DWORD                                   DwMajor;
} ICEFLT_VERSION, *PICEFLT_VERSION;


typedef enum _ICE_FILTER_COMMAND
{
    ICE_FILTER_GET_VERSION                  = 1,
    ICE_FILTER_ALLOW_UNLOAD,

    ICE_FILTER_ENABLE_APPCTRL_SCAN,
    ICE_FILTER_ENABLE_FS_SCAN,

    __ICE_FILTER_MAXIMUM_COMMAND__
} ICE_FILTER_COMMAND;


typedef enum _ICE_FILTER_REQUEST_TYPE
{
    ICE_FILTER_REQUEST_SCAN_PROCESS = 1,
    ICE_FILTER_REQUEST_SCAN_FS,

    ICE_FILTER_REPLY_SCAN_REQUEST_PROCESS = 10,
    ICE_FILTER_REPLY_SCAN_REQUEST_FS
    //__ICE_FILTER_MAXIMUM_COMMAND__
} ICE_FILTER_REQUEST_TYPE;


typedef enum _ICE_SCAN_VERDICT
{
    IceScanVerdict_Allow = 0, // ERROR_SUCCESS
    IceScanVerdict_Deny = 5 // ERROR_ACCESS_DENIED
} ICE_SCAN_VERDICT, *PICE_SCAN_VERDICT;



#endif // !__ICE_USER_COMMON_H__
