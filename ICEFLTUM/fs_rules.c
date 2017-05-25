#include "fs_rules.h"
#include "db_sqlite.h"
#include "debug2.h"

DWORD
InitFSScanRules(
    VOID
)
{
    DWORD dwResult = ERROR_SUCCESS;

    __try
    {
        dwResult = DBInit();
        if (ERROR_SUCCESS != dwResult)
        {
            LogErrorWin(dwResult, L"DBInit");
            __leave;
        }
    }
    __finally
    {
        if (ERROR_SUCCESS != dwResult)
        {
            UninitFSScanRules();
        }
    }
    
    return dwResult;
}

DWORD
UninitFSScanRules(
    VOID
)
{
    DWORD dwResult = ERROR_SUCCESS;

    dwResult = DBUninit();
    if (ERROR_SUCCESS != dwResult)
    {
        LogWarningWin(dwResult, L"DBUninit");
    }

    return dwResult;
}

_Use_decl_anno_impl_
DWORD
GetFSScanResult(
    ULONG                           UlOriginalFlags,
    IC_FS_RULE                     *PRule,
    ICE_FS_SCAN_RESULT_PACKET      *PResultPack
)
{
    ULONG ulDeniedFlags = 0;
    ULONG ulNewFlags    = 0;

    if (ERROR_SUCCESS != DbGetFSScanDeniedFlags(PRule, &ulDeniedFlags))
    {
        ulNewFlags = UlOriginalFlags;
    }
    else
    {
        ulNewFlags = UlOriginalFlags & (~ulDeniedFlags);
    }
    
    PResultPack->DwIceFsNewFlags = ulNewFlags;

    return ERROR_SUCCESS;
}
