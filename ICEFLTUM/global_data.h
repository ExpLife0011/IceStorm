#pragma once


#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>

#include "icecommon.h"

#define WAIT_TIME_FOR_THREADS   ((DWORD) 5000)
#define MESSAGE_BUFFER_SIZE     ((DWORD) ((2 * 65536) + 1024))

#define APPCTRL_SCAN_NR_OF_THREADS  ((DWORD) 4)
#define FS_SCAN_NR_OF_THREADS       ((DWORD) 6)

typedef struct _IC_COMMUNICATION_PORTS
{
    HANDLE                                  HScanCtrlPort;
    HANDLE                                  HAppCtrlPort;
    HANDLE                                  HFSScanPort;
} IC_COMMUNICATION_PORTS, *PIC_COMMUNICATION_PORTS;

extern IC_COMMUNICATION_PORTS                      gIcComPort;
extern HANDLE                                      gHThreadAppCtrlListener;
