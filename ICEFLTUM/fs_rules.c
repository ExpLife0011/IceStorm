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

_Use_decl_anno_impl_
DWORD
AddFSScanRule(
    IC_STRING_MATCHER               MatcherProcessPath,
    PWCHAR                          PProcessPath,
    DWORD                           DwPid,
    IC_STRING_MATCHER               MatcherFilePath,
    PWCHAR                          PFilePath,
    ULONG                           UlDeniedOperations,
    DWORD                          *PDwRuleId
)
{
    DWORD               dwStatus        = ERROR_SUCCESS;
    DWORD               dwLen           = 0;
    PWCHAR              pAuxFilePath    = NULL;
    PWCHAR              pAuxProcPath    = NULL;
    IC_FS_RULE          rule            = { 0 };

    __try
    {
        if (PProcessPath != NULL)
        {
            dwLen = (DWORD) (wcslen(PProcessPath) + 1);
            pAuxProcPath = (PWCHAR) malloc(dwLen * sizeof(WCHAR));
            if (NULL == pAuxProcPath)
            {
                dwStatus = ERROR_NOT_ENOUGH_MEMORY;
                __leave;
            }

            CreateFSStringToAddInDB(PProcessPath, pAuxProcPath, dwLen, MatcherProcessPath);
        }

        if (PFilePath != NULL)
        {
            dwLen = (DWORD) (wcslen(PFilePath) + 1);
            pAuxFilePath = (PWCHAR) malloc(dwLen * sizeof(WCHAR));
            if (NULL == pAuxFilePath)
            {
                dwStatus = ERROR_NOT_ENOUGH_MEMORY;
                __leave;
            }

            CreateFSStringToAddInDB(PFilePath, pAuxFilePath, dwLen, MatcherFilePath);
        }

        rule.MatcherProcessPath = MatcherProcessPath;
        rule.PProcessPath = pAuxProcPath;
        rule.DwPid = DwPid;
        rule.MatcherFilePath = MatcherFilePath;
        rule.PFilePath = pAuxFilePath;
        rule.UlDeniedOperations = UlDeniedOperations;

        dwStatus = DbAddFSScanRule(&rule, PDwRuleId);
        if (ERROR_SUCCESS != dwStatus)
        {
            LogErrorWin(dwStatus, L"DbAddFSScanRule(%s, %d)", PProcessPath, DwPid);
        }
    }
    __finally
    {
        if (NULL != pAuxFilePath)
        {
            free(pAuxFilePath);
            pAuxFilePath = NULL;
        }

        if (NULL != pAuxProcPath)
        {
            free(pAuxProcPath);
            pAuxProcPath = NULL;
        }
    }

    return dwStatus;
}

_Use_decl_anno_impl_
DWORD
DeleteFSScanRule(
    DWORD                                   DwRuleId
)
{
    DWORD dwStatus = ERROR_SUCCESS;

    dwStatus = DbDeleteFSScanRule(DwRuleId);
    if (ERROR_SUCCESS != dwStatus)
    {
        LogErrorWin(dwStatus, L"DbDeleteFSScanRule(%d)", DwRuleId);
    }

    return dwStatus;
}

_Use_decl_anno_impl_
DWORD
UpdateFSScanRule(
    DWORD                           DwRuleId,
    IC_STRING_MATCHER               MatcherProcessPath,
    PWCHAR                          PProcessPath,
    DWORD                           DwPid,
    IC_STRING_MATCHER               MatcherFilePath,
    PWCHAR                          PFilePath,
    ULONG                           UlDeniedOperations
)
{
    DWORD               dwStatus = ERROR_SUCCESS;
    DWORD               dwLen = 0;
    PWCHAR              pAuxFilePath = NULL;
    PWCHAR              pAuxProcPath = NULL;
    IC_FS_RULE          rule = { 0 };

    __try
    {
        if (PProcessPath != NULL)
        {
            dwLen = (DWORD) (wcslen(PProcessPath) + 1);
            pAuxProcPath = (PWCHAR) malloc(dwLen * sizeof(WCHAR));
            if (NULL == pAuxProcPath)
            {
                dwStatus = ERROR_NOT_ENOUGH_MEMORY;
                __leave;
            }

            CreateFSStringToAddInDB(PProcessPath, pAuxProcPath, dwLen, MatcherProcessPath);
        }

        if (PFilePath != NULL)
        {
            dwLen = (DWORD) (wcslen(PFilePath) + 1);
            pAuxFilePath = (PWCHAR) malloc(dwLen * sizeof(WCHAR));
            if (NULL == pAuxFilePath)
            {
                dwStatus = ERROR_NOT_ENOUGH_MEMORY;
                __leave;
            }

            CreateFSStringToAddInDB(PFilePath, pAuxFilePath, dwLen, MatcherFilePath);
        }

        rule.MatcherProcessPath = MatcherProcessPath;
        rule.PProcessPath = pAuxProcPath;
        rule.DwPid = DwPid;
        rule.MatcherFilePath = MatcherFilePath;
        rule.PFilePath = pAuxFilePath;
        rule.UlDeniedOperations = UlDeniedOperations;

        dwStatus = DbUpdateFSScanRule(DwRuleId, &rule);
        if (ERROR_SUCCESS != dwStatus)
        {
            LogErrorWin(dwStatus, L"DbUpdateFSScanRule(%d)", DwRuleId);
        }
    }
    __finally
    {
        if (NULL != pAuxFilePath)
        {
            free(pAuxFilePath);
            pAuxFilePath = NULL;
        }

        if (NULL != pAuxProcPath)
        {
            free(pAuxProcPath);
            pAuxProcPath = NULL;
        }
    }

    return dwStatus;
}

_Use_decl_anno_impl_
DWORD
GetFSScanRules(
    PIC_FS_RULE                            *PPRules,
    DWORD                                  *PDwLength
)
{
    return DbGetFSScanRules(PPRules, PDwLength);
}

_Use_decl_anno_impl_
VOID
FreeAppFSScanList(
    PIC_FS_RULE                             PRules,
    DWORD                                   DwLength
)
{
    DbFreeFSScanRulesList(PRules, DwLength);
}
