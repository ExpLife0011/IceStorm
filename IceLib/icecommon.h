#ifndef __ICECOMMON_H__
#define __ICECOMMON_H__


#include "../iceflt/ice_user_common.h"

#define ICEFLTUM_DLL                        L"icefltum.dll"

typedef enum _IC_STRING_MATCHER
{
    IcStringMatcher_Equal = 0,
    IcStringMatcher_Wildmat = 1
} IC_STRING_MATCHER, *PIC_STRING_MATCHER;

typedef struct _IC_APPCTRL_RULE
{
    DWORD               DwRuleId;
    ICE_SCAN_VERDICT    Verdict;

    IC_STRING_MATCHER   MatcherProcessPath;
    PWCHAR              PProcessPath;
    DWORD               DwPid;

    IC_STRING_MATCHER   MatcherParentPath;
    PWCHAR              PParentPath;
    DWORD               DwParentPid;

    DWORD               DwAddTime;

} IC_APPCTRL_RULE, *PIC_APPCTRL_RULE;

typedef struct _IC_FS_RULE
{
    DWORD               DwRuleId;
    ULONG               UlDeniedOperations;

    IC_STRING_MATCHER   MatcherProcessPath;
    PWCHAR              PProcessPath;
    DWORD               DwPid;

    IC_STRING_MATCHER   MatcherFilePath;
    PWCHAR              PFilePath;
    
    DWORD               DwAddTime;

} IC_FS_RULE, *PIC_FS_RULE;

typedef struct _IC_APPCTRL_EVENT
{
    DWORD               DwEventId;

    PWCHAR              PProcessPath;
    DWORD               DwPid;
    PWCHAR              PParentPath;
    DWORD               DwParentPid;

    ICE_SCAN_VERDICT    Verdict;
    DWORD               DwMatchedRuleId;

    DWORD               DwEventTime;

} IC_APPCTRL_EVENT, *PIC_APPCTRL_EVENT;

typedef struct _IC_FS_EVENT
{
    DWORD               DwEventId;

    PWCHAR              PProcessPath;
    DWORD               DwPid;
    PWCHAR              PFilePath;

    ULONG               UlRequiredOperations;
    ULONG               UlDeniedOperations;
    ULONG               UlRemainingOperations;

    DWORD               DwMatchedRuleId;
    
    DWORD               DwEventTime;

} IC_FS_EVENT, *PIC_FS_EVENT;

#endif // !__ICECOMMON_H__
