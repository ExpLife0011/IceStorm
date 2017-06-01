#include "appctrl_rules.h"
#include "debug2.h"
#include "db_sqlite.h"
#include "icecommon.h"

DWORD
InitAppCtrlRules(
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
            UninitAppCtrlRules();
        }
    }
    
    return dwResult;
}

DWORD
UninitAppCtrlRules(
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
GetAppCtrlScanResult(
    IC_APPCTRL_RULE                        *PRule,
    ICE_APP_CTRL_SCAN_RESULT_PACKET        *PResultPack
)
{
    ICE_SCAN_VERDICT verdict = IceScanVerdict_Allow;

    if (ERROR_SUCCESS != DbGetAppCtrlVerdict(PRule, &verdict))
    {
        verdict = IceScanVerdict_Allow;
    }
    PResultPack->NtScanResult = verdict;

    return ERROR_SUCCESS;
}

VOID
CreateStringToAddInDB(
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
AddAppCtrlRule(
    IC_STRING_MATCHER                       MatcherProcessPath,
    PWCHAR                                  PProcessPath,
    DWORD                                   DwPid,
    IC_STRING_MATCHER                       MatcherParentPath,
    PWCHAR                                  PParentPath,
    DWORD                                   DwParentPid,
    ICE_SCAN_VERDICT                        Verdict,
    DWORD                                  *PDwRuleId
)
{
    DWORD               dwStatus    = ERROR_SUCCESS;
    DWORD               dwLen       = 0;
    PWCHAR              pAuxPath    = NULL;
    PWCHAR              pAuxPPath   = NULL;
    IC_APPCTRL_RULE     rule        = { 0 };

    __try
    {
        if (NULL != PProcessPath)
        {
            dwLen = (DWORD) (wcslen(PProcessPath) + 1);
            pAuxPath = (PWCHAR) malloc(dwLen * sizeof(WCHAR));
            if (NULL == pAuxPath)
            {
                dwStatus = ERROR_NOT_ENOUGH_MEMORY;
                __leave;
            }
            
            CreateStringToAddInDB(PProcessPath, pAuxPath, dwLen, MatcherProcessPath);
        }

        if (NULL != PParentPath)
        {
            dwLen = (DWORD) (wcslen(PParentPath) + 1);
            pAuxPPath = (PWCHAR) malloc(dwLen * sizeof(WCHAR));
            if (NULL == pAuxPPath)
            {
                dwStatus = ERROR_NOT_ENOUGH_MEMORY;
                __leave;
            }

            CreateStringToAddInDB(PParentPath, pAuxPPath, dwLen, MatcherParentPath);
        }

        rule.MatcherProcessPath = MatcherProcessPath;
        rule.PProcessPath       = pAuxPath;
        rule.DwPid              = DwPid;
        rule.MatcherParentPath  = MatcherParentPath;
        rule.PParentPath        = pAuxPPath;
        rule.DwParentPid        = DwParentPid;
        rule.Verdict            = Verdict;

        dwStatus = DbAddAppCtrlRule(&rule, PDwRuleId);
        if (ERROR_SUCCESS != dwStatus)
        {
            LogErrorWin(dwStatus, L"DbAddAppCtrlRule(%s, %d)", PProcessPath, DwPid);
        }
    }
    __finally
    {
        if (NULL != pAuxPath)
        {
            free(pAuxPath);
            pAuxPath = NULL;
        }

        if (NULL != pAuxPPath)
        {
            free(pAuxPPath);
            pAuxPPath = NULL;
        }
    }
   
    return dwStatus;
}

_Use_decl_anno_impl_
DWORD
DeleteAppCtrlRule(
    DWORD                                   DwRuleId
)
{
    DWORD dwStatus = ERROR_SUCCESS;

    dwStatus = DbDeleteAppCtrlRule(DwRuleId);
    if (ERROR_SUCCESS != dwStatus)
    {
        LogErrorWin(dwStatus, L"DbDeleteAppCtrlRule(%d)", DwRuleId);
    }

    return dwStatus;
}

_Use_decl_anno_impl_
DWORD
UpdateAppCtrlRule(
    DWORD                                   DwRuleId,
    IC_STRING_MATCHER                       MatcherProcessPath,
    PWCHAR                                  PProcessPath,
    DWORD                                   DwPid,
    IC_STRING_MATCHER                       MatcherParentPath,
    PWCHAR                                  PParentPath,
    DWORD                                   DwParentPid,
    ICE_SCAN_VERDICT                        Verdict
)
{
    DWORD               dwStatus    = ERROR_SUCCESS;
    DWORD               dwLen       = 0;
    PWCHAR              pAuxPath    = NULL;
    PWCHAR              pAuxPPath   = NULL;
    IC_APPCTRL_RULE     rule        = { 0 };

    __try
    {
        if (NULL != PProcessPath)
        {
            dwLen = (DWORD) (wcslen(PProcessPath) + 1);
            pAuxPath = (PWCHAR) malloc(dwLen * sizeof(WCHAR));
            if (NULL == pAuxPath)
            {
                dwStatus = ERROR_NOT_ENOUGH_MEMORY;
                __leave;
            }

            CreateStringToAddInDB(PProcessPath, pAuxPath, dwLen, MatcherProcessPath);
        }

        if (NULL != PParentPath)
        {
            dwLen = (DWORD) (wcslen(PParentPath) + 1);
            pAuxPPath = (PWCHAR) malloc(dwLen * sizeof(WCHAR));
            if (NULL == pAuxPPath)
            {
                dwStatus = ERROR_NOT_ENOUGH_MEMORY;
                __leave;
            }

            CreateStringToAddInDB(PParentPath, pAuxPPath, dwLen, MatcherParentPath);
        }

        rule.MatcherProcessPath = MatcherProcessPath;
        rule.PProcessPath       = pAuxPath;
        rule.DwPid              = DwPid;
        rule.MatcherParentPath  = MatcherParentPath;
        rule.PParentPath        = pAuxPPath;
        rule.DwParentPid        = DwParentPid;
        rule.Verdict            = Verdict;

        dwStatus = DbUpdateAppCtrlRule(DwRuleId, &rule);
        if (ERROR_SUCCESS != dwStatus)
        {
            LogErrorWin(dwStatus, L"DbUpdateAppCtrlRule(%d, %s, %d, TRUE)", DwRuleId, PProcessPath, DwPid);
        }
    }
    __finally
    {
        if (NULL != pAuxPath)
        {
            free(pAuxPath);
            pAuxPath = NULL;
        }

        if (NULL != pAuxPPath)
        {
            free(pAuxPPath);
            pAuxPPath = NULL;
        }
    }

    return dwStatus;
}

_Use_decl_anno_impl_
DWORD
GetAppCtrlRules(
    PIC_APPCTRL_RULE                       *PPRules,
    DWORD                                  *PDwLength
)
{
    return DbGetAppCtrlRules(PPRules, PDwLength);
}

_Use_decl_anno_impl_
VOID
FreeAppCtrlRulesList(
    PIC_APPCTRL_RULE                        PRules,
    DWORD                                   DwLength
)
{
    DbFreeAppCtrlRulesList(PRules, DwLength);
}
