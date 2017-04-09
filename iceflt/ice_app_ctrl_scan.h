#ifndef __ICE_APP_CTRL_SCAN_H__
#define __ICE_APP_CTRL_SCAN_H__

#include "communication.h"


_Success_(NT_SUCCESS(return))
NTSTATUS
IceAppCtrlScanProcess(
    _In_        PEPROCESS                   PProcess,
    _In_        HANDLE                      HProcessId,
    _In_        PPS_CREATE_NOTIFY_INFO      PCreateInfo,
    _Out_       PNTSTATUS                   PNtScanResult
);

#endif // !__ICE_APP_CTRL_SCAN_H__
