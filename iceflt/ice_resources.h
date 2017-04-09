#ifndef __ICE_RESOURCES__
#define __ICE_RESOURCES__

#include <fltKernel.h>

FORCEINLINE
VOID
IceAcquireResourceExclusive(
    _In_        PERESOURCE                  Resource
)
{
    KeEnterCriticalRegion();
    (VOID) ExAcquireResourceExclusiveLite(Resource, TRUE);
}

FORCEINLINE
VOID
HbAcquireResourceShared(
    _In_        PERESOURCE                  Resource
)
{
    KeEnterCriticalRegion();
    (VOID) ExAcquireResourceSharedLite(Resource, TRUE);
}

FORCEINLINE
VOID
IceReleaseResource(
    _In_        PERESOURCE                  Resource
)
{
    ExReleaseResourceLite(Resource);
    KeLeaveCriticalRegion();
}

#endif // !__ICE_RESOURCES__
