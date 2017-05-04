#include "os_version.h"

_Use_decl_anno_impl_
BOOLEAN
IceVerifyWindowsVersion(
    RTL_OSVERSIONINFOEXW                    *PVersionInfo, 
    ULONGLONG                               UllConditionMask
)
{
    return NT_SUCCESS(RtlVerifyVersionInfo(PVersionInfo, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, UllConditionMask));
}

_Use_decl_anno_impl_
BOOLEAN 
IsWindows7OrLater(
    VOID
)
{
    RTL_OSVERSIONINFOEXW VersionInfo = { 0 };
    ULONGLONG ConditionMask = 0;

    VersionInfo.dwOSVersionInfoSize = sizeof(VersionInfo);
    VersionInfo.dwMajorVersion = 6;
    VersionInfo.dwMinorVersion = 1;

    ConditionMask = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
    ConditionMask = VerSetConditionMask(ConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);

    return IceVerifyWindowsVersion(&VersionInfo, ConditionMask);
}

_Use_decl_anno_impl_
BOOLEAN 
IsWindows8OrLater(
    VOID
)
{
    RTL_OSVERSIONINFOEXW VersionInfo = { 0 };
    ULONGLONG ConditionMask = 0;

    VersionInfo.dwOSVersionInfoSize = sizeof(VersionInfo);
    VersionInfo.dwMajorVersion = 6;
    VersionInfo.dwMinorVersion = 2;

    ConditionMask = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
    ConditionMask = VerSetConditionMask(ConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);

    return IceVerifyWindowsVersion(&VersionInfo, ConditionMask);
}

_Use_decl_anno_impl_
BOOLEAN 
IsWindows81OrLater(
    VOID
)
{
    RTL_OSVERSIONINFOEXW VersionInfo = { 0 };
    ULONGLONG ConditionMask = 0;
    
    VersionInfo.dwOSVersionInfoSize = sizeof(VersionInfo);
    VersionInfo.dwMajorVersion = 6;
    VersionInfo.dwMinorVersion = 3;

    ConditionMask = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);
    ConditionMask = VerSetConditionMask(ConditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);

    return IceVerifyWindowsVersion(&VersionInfo, ConditionMask);
}

_Use_decl_anno_impl_
BOOLEAN 
IsWindows10OrLater(
    VOID
)
{
    RTL_OSVERSIONINFOEXW VersionInfo = { 0 };
    ULONGLONG ConditionMask = 0;
    
    VersionInfo.dwOSVersionInfoSize = sizeof(VersionInfo);
    VersionInfo.dwMajorVersion = 10;

    ConditionMask = VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL);

    return IceVerifyWindowsVersion(&VersionInfo, ConditionMask);
}
