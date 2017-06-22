#include "registrationdata.h"
#include "fs_routines.h"
#include "acquireforsection.h"

#ifdef ALLOC_DATA_PRAGMA
    #pragma data_seg("INIT")
    #pragma const_seg("INIT")
#endif

// Windows 7
CONST FLT_CONTEXT_REGISTRATION ContextRegistration_Win7[] = {
    {FLT_CONTEXT_END}
};

// Windows8 or later
CONST FLT_CONTEXT_REGISTRATION ContextRegistration_Win8[] = {
    {FLT_CONTEXT_END}
};

CONST FLT_OPERATION_REGISTRATION Callbacks[] = 
{
    {IRP_MJ_CREATE,                 0, IcePreCreate,            IcePostCreate},
    {IRP_MJ_READ,                   0, IcePreRead,              IcePostRead},
    {IRP_MJ_WRITE,                  0, IcePreWrite,             IcePostWrite},
    {IRP_MJ_CLEANUP,                0, IcePreCleanup,           IcePostCleanup},
    {IRP_MJ_SET_INFORMATION,        0, IcePreSetInfo, 	        IcePostSetInfo},
    {IRP_MJ_FILE_SYSTEM_CONTROL,    0, IcePreFileSystemControl, IcePostFileSystemControl},
    {IRP_MJ_VOLUME_MOUNT,           0, IcePreMountVolume,       NULL},
    {IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION, 0, IcePreAcquireForSection, NULL},
    {IRP_MJ_OPERATION_END}
};


CONST FLT_REGISTRATION_LONGHORN FilterRegistration_WIN7 = {
    sizeof(FLT_REGISTRATION_LONGHORN),
    FLT_REGISTRATION_VERSION_0202,
    FLTFL_REGISTRATION_DO_NOT_SUPPORT_SERVICE_STOP,
    ContextRegistration_Win7,
    Callbacks,
    IceUnload,
    NULL, // InstanceSetup,
    NULL, // InstanceQueryTeardown,
    NULL, // InstanceTeardownStart,
    NULL, // InstanceTeardownComplete,
    NULL,
    NULL,
    NULL,
    NULL, // TxfCallback,
    NULL
};

CONST FLT_REGISTRATION_LONGHORN FilterRegistration_WIN8 = {
    sizeof(FLT_REGISTRATION_LONGHORN),
    FLT_REGISTRATION_VERSION_0202,
    FLTFL_REGISTRATION_DO_NOT_SUPPORT_SERVICE_STOP,
    ContextRegistration_Win8,
    Callbacks,
    IceUnload,
    NULL, // InstanceSetup,
    NULL, // InstanceQueryTeardown,
    NULL, // InstanceTeardownStart,
    NULL, // InstanceTeardownComplete,
    NULL,
    NULL,
    NULL,
    NULL, // TxfCallback,
    NULL
};

#ifdef ALLOC_DATA_PRAGMA
#pragma data_seg()
#pragma const_seg()
#endif
