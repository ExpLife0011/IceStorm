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


#pragma pack(push)      
#pragma pack(8)      

typedef struct _ICE_GENERIC_PACKET
{
    //UCHAR		Minor; 
    //UCHAR		Major;
    DWORD                                   DwPacketLength;
    DWORD                                   DwRequestType;
} ICE_GENERIC_PACKET, *PICE_GENERIC_PACKET;

typedef struct _ICE_APP_CTRL_SCAN_REQUEST_PACKET
{
    DWORD                                   DwPid;
    DWORD                                   DwProcessPathSize;
    WCHAR                                   PProcessPath[1];
} ICE_APP_CTRL_SCAN_REQUEST_PACKET, *PICE_APP_CTRL_SCAN_REQUEST_PACKET;

typedef struct _ICE_APP_CTRL_SCAN_RESULT_PACKET
{
    NTSTATUS                                NtScanResult;
} ICE_APP_CTRL_SCAN_RESULT_PACKET, *PICE_APP_CTRL_SCAN_RESULT_PACKET;

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

    ICE_FILTER_CLEAR_APPCTRL_CACHE,
    ICE_FILTER_ENABLE_APPCTRL_SCAN,
    __ICE_FILTER_MAXIMUM_COMMAND__
} ICE_FILTER_COMMAND;


typedef enum _ICE_FILTER_REQUEST_TYPE
{
    ICE_FILTER_REQUEST_SCAN_PROCESS = 1,

    ICE_FILTER_REPLY_SCAN_REQUEST_PROCESS = 10
    //__ICE_FILTER_MAXIMUM_COMMAND__
} ICE_FILTER_REQUEST_TYPE;



#endif // !__ICE_USER_COMMON_H__
