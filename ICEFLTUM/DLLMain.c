#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include "debug2.h"

BOOL APIENTRY
DllMain(
    _In_    HINSTANCE                       HInstance,
    _In_    DWORD                           DwReason,
    _In_    LPVOID                          PReserved
)
{
    UNREFERENCED_PARAMETER(HInstance);
    UNREFERENCED_PARAMETER(PReserved);

    switch (DwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            break;
        }

        case DLL_PROCESS_DETACH:
        {
            break;
        }

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        {
            break;
        }

        default:
        {
            break;
        }
    }

    return TRUE;
}