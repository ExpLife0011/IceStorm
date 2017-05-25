#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include "debug.h"
#include "global_data.h"


typedef struct _ICE_COMMUNCATION_PORTS
{
    // setoption ...
    PFLT_PORT                               PProtectionControlPort;
    PFLT_PORT                               PClientPCPort;

    // scan process for AppCtrl
    PFLT_PORT                               PAppCtrlPort;
    PFLT_PORT                               PClientAppCtrlPort;

    // scan FSScan
    PFLT_PORT                               PFSScanPort;
    PFLT_PORT                               PClientFSScanPort;

    volatile LONG                           LNrOfConnections;

} ICE_COMMUNCATION_PORTS, *PICE_COMMUNCATION_PORTS;

EXTERN_C PICE_COMMUNCATION_PORTS gPIceComPorts;


_Success_(NT_SUCCESS(return))
NTSTATUS
IceInitCommPorts(
    VOID
);

VOID
IceCleanupCommPorts(
    VOID
);

#endif // !__COMMUNICATION_H__
