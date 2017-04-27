#ifndef __ICECOMMON_H__
#define __ICECOMMON_H__


#include "../iceflt/ice_user_common.h"

#define ICEFLTUM_DLL                        L"icefltum.dll"

typedef struct _IC_APPCTRL_RULE
{
    DWORD               DwRuleId;
    ICE_SCAN_VERDICT    Verdict;

    PWCHAR              PProcessPath;
    DWORD               DwPid;
    PWCHAR              PParentPath;
    DWORD               DwParentPid;

    DWORD               DwAddTime;

} IC_APPCTRL_RULE, *PIC_APPCTRL_RULE;

#endif // !__ICECOMMON_H__
