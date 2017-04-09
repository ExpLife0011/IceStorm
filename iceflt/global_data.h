#ifndef __GLOBAL_DATA_H__
#define __GLOBAL_DATA_H__


#include <initguid.h>
#include <fltKernel.h>
#include <dontuse.h>
#include <suppress.h>
#include <intsafe.h>
#include "debug.h"


#define IS_ANY_FLAG_ON(var, flag)           ((BOOLEAN) (( ((UINT) var) & ((UINT) flag)) != 0))        // verifica daca e setat cel putin un flag
#define IS_FLAG_ON(var, flag)               ((BOOLEAN) (( ((UINT) var) & ((UINT) flag)) == flag))     // verifica daca sunt setate toate flagurile


// os verison to use for gPData->DwOsVersion
typedef enum _ICE_OS_Version
{
    OS_Windows7OrLater      = 1,

    OS_Windows8OrLater      = 2,
    OS_Windows81OrLater     = 4,

    OS_Windows10OrLater     = 8

} ICE_OS_Version;


typedef struct _ICE_SETTINGS
{
    BYTE                                    BtEnableAppCtrlScan;
    ULONG                                   UlAllowUnload;
    ULONG                                   UlMaximumProcessCache;
} ICE_SETTINGS, *PICE_SETTINGS;


typedef struct _ICE_GLOBAL_DATA
{
    PFLT_FILTER                             PFilter;
    PDRIVER_OBJECT                          PDriverObject;
    PDEVICE_OBJECT                          PDeviceObject;
    
    DWORD                                   DwSystemProcessId;          // System Process ID
    
    volatile LONG                           LnInit;                     // Number of times the driver loaded

    PWSTR                                   PConfigRegistryPath;        // Path to the registry key assignated to IceFlt

    BOOLEAN                                 BProcessCallbackSet;        // Indicates if the CreateProcess callback is set 
    BOOLEAN                                 BUnloading;                 // Indicates if the driver is unloading

    DWORD                                   DwOSVersion;                // Flags indicating Windows Version
    
    DWORD                                   DwServiceProcessId;         // PID of the connected client process
    PEPROCESS                               PEServiceProcess;           // Ptr to EPROCESS structure of  the connected client process

    ICE_SETTINGS                            IceSettings;                // Setting from registry

    ERESOURCE                               EMainResource;              //

} ICE_GLOBAL_DATA, *PICE_GLOBAL_DATA;


extern PICE_GLOBAL_DATA  gPData;


_Success_(NT_SUCCESS(return))
NTSTATUS
IceInitGlobalData(
    _In_        PDRIVER_OBJECT              PDriverObject,
    _In_        PUNICODE_STRING             PUsRegistryPath
);

VOID
IceCleanupGlobalData(
    VOID
    );

_Success_(NT_SUCCESS(return))
NTSTATUS
IceLoadSettings(
    VOID
);

#endif
