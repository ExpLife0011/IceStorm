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
    ICE_SCAN_VERDICT verdict = IcScanVerdict_Allow;

    if (ERROR_SUCCESS != DbGetAppCtrlVerdict(PRule, &verdict))
    {
        verdict = IcScanVerdict_Allow;
    }
    PResultPack->NtScanResult = verdict;

    return ERROR_SUCCESS;
}

_Use_decl_anno_impl_
DWORD
AddAppCtrlRule(
    PWCHAR                                  PProcessPath,
    DWORD                                   DwPid,
    PWCHAR                                  PParentPath,
    DWORD                                   DwParentPid,
    ICE_SCAN_VERDICT                        Verdict,
    DWORD                                  *PDwRuleId
)
{
    DWORD               dwStatus    = ERROR_SUCCESS;
    IC_APPCTRL_RULE     rule        = { 0 };

    rule.PProcessPath = PProcessPath;
    rule.DwPid = DwPid;
    rule.PParentPath = PParentPath;
    rule.DwParentPid = DwParentPid;
    rule.Verdict = Verdict;


    dwStatus = DbAddAppCtrlRule(&rule, PDwRuleId);
    if (ERROR_SUCCESS != dwStatus)
    {
        LogErrorWin(dwStatus, L"DbAddAppCtrlRule(%s, %d)", PProcessPath, DwPid);
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
    PWCHAR                                  PProcessPath,
    DWORD                                   DwPid,
    PWCHAR                                  PParentPath,
    DWORD                                   DwParentPid,
    ICE_SCAN_VERDICT                        Verdict
)
{
    DWORD               dwStatus    = ERROR_SUCCESS;
    IC_APPCTRL_RULE     rule        = { 0 };

    rule.PProcessPath = PProcessPath;
    rule.DwPid = DwPid;
    rule.PParentPath = PParentPath;
    rule.DwParentPid = DwParentPid;
    rule.Verdict = Verdict;


    dwStatus = DbUpdateAppCtrlRule(DwRuleId, &rule);
    if (ERROR_SUCCESS != dwStatus)
    {
        LogErrorWin(dwStatus, L"DbUpdateAppCtrlRule(%d, %s, %d, TRUE)", DwRuleId, PProcessPath, DwPid);
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
