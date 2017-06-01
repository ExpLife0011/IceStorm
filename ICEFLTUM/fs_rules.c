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

VOID
CreateFSStringToAddInDB(
    _In_z_      PWCHAR                      PPath,
    _Inout_z_   PWCHAR                      PResultPath,
    _In_        DWORD                       DwLen,
    _In_        IC_STRING_MATCHER           MatcherPath
)
{
    DWORD dwIdx = 0;

    if (MatcherPath == IcStringMatcher_Wildmat)
    {
        for (dwIdx = 0; dwIdx < DwLen; dwIdx++)
        {
            if (PPath[dwIdx] == L'*') PResultPath[dwIdx] = L'%';
            else if (PPath[dwIdx] == L'?') PResultPath[dwIdx] = L'_';
            else PResultPath[dwIdx] = PPath[dwIdx];
        }
    }
    else
    {
        memcpy(PResultPath, PPath, DwLen * sizeof(WCHAR));
    }
}

//_Use_decl_anno_impl_
//DWORD
//AddFSScanRule(
//    IC_STRING_MATCHER               MatcherProcessPath,
//    PWCHAR                          PProcessPath,
//    DWORD                           DwPid,
//    IC_STRING_MATCHER               MatcherFilePath,
//    PWCHAR                          PFilePath,
//    DWORD                           DwDeniedFlags,
//    DWORD                          *PDwRuleId
//)
//{
//
//}
