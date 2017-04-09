#include "registrationdata.h"

#ifdef ALLOC_DATA_PRAGMA
#pragma data_seg("INIT")
#pragma const_seg("INIT")
#endif

// Windows 7
CONST FLT_CONTEXT_REGISTRATION ContextRegistration_Win7[] = {
    //{FLT_INSTANCE_CONTEXT,       0, HbDeleteInstanceContext, sizeof(HB_INSTANCE_CONTEXT), 'icBH', NULL, NULL, NULL},
    //{FLT_STREAM_CONTEXT,         0, HbDeleteStreamContext,   sizeof(HB_STREAM_CONTEXT),   'csBH', NULL, NULL, NULL},
    //{FLT_STREAMHANDLE_CONTEXT,   0, HbDeleteHandleContext,   sizeof(HB_HANDLE_CONTEXT),   'chBH', NULL, NULL, NULL},
    //{FLT_TRANSACTION_CONTEXT,    0, HbTxfDeleteContext,      sizeof(HB_TXF_CONTEXT),      'ctBH', NULL, NULL, NULL},
    {FLT_CONTEXT_END}
};

// Windows8 or later
CONST FLT_CONTEXT_REGISTRATION ContextRegistration_Win8[] = {
    //{FLT_INSTANCE_CONTEXT,       0, HbDeleteInstanceContext, sizeof(HB_INSTANCE_CONTEXT), 'icBH', NULL, NULL, NULL},
    //{FLT_STREAM_CONTEXT,         0, HbDeleteStreamContext,   sizeof(HB_STREAM_CONTEXT),   'csBH', NULL, NULL, NULL},
    //{FLT_STREAMHANDLE_CONTEXT,   0, HbDeleteHandleContext,   sizeof(HB_HANDLE_CONTEXT),   'chBH', NULL, NULL, NULL},
    //{FLT_TRANSACTION_CONTEXT,    0, HbTxfDeleteContext,      sizeof(HB_TXF_CONTEXT),      'ctBH', NULL, NULL, NULL},
    //{FLT_SECTION_CONTEXT,		 0, HbDeleteSectionContext,  sizeof(HB_SECTION_CONTEXT),  'scBH', NULL, NULL, NULL},
    {FLT_CONTEXT_END}
};

CONST FLT_OPERATION_REGISTRATION Callbacks[] = 
{
    //{IRP_MJ_CREATE, 		0, PreCreate, 	PostCreate},
    //{IRP_MJ_CLEANUP, 		0, PreCleanup, 	PostCleanup},
    //{IRP_MJ_SET_INFORMATION, 	0, PreSetInfo, 	PostSetInfo},
    //{IRP_MJ_WRITE, 			0, PreWrite, 		PostWrite},
    //{IRP_MJ_FILE_SYSTEM_CONTROL, 	0, PreFsctl, 	PostFsctl},
    //{IRP_MJ_VOLUME_MOUNT, 		0, PreMountVolume, 	NULL},
    //{IRP_MJ_ACQUIRE_FOR_SECTION_SYNCHRONIZATION, 0, PreAcquireForSection, NULL},
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
