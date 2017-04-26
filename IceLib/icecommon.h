#ifndef __ICECOMMON_H__
#define __ICECOMMON_H__


#include "../iceflt/ice_user_common.h"

#define ICEFLTUM_DLL                        L"icefltum.dll"

typedef struct _IC_APPCTRL_RULE
{
    DWORD       DwRuleId;
    DWORD       DwVerdict;
    PWCHAR      PFilePath;
    DWORD       DwPid;
    DWORD       DwAddTime;

} IC_APPCTRL_RULE, *PIC_APPCTRL_RULE;

#endif // !__ICECOMMON_H__
