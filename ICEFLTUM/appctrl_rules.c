#include "appctrl_rules.h"
#include "debug2.h"
#include "db_sqlite.h"

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
    PICE_APP_CTRL_SCAN_REQUEST_PACKET       PScanRequest,
    PICE_APP_CTRL_SCAN_RESULT_PACKET        PResultPack
)
{

    BOOLEAN     bBlockProcess = FALSE;
    BOOLEAN     bHasDenyRule = FALSE;
    BOOLEAN     bHasAllowRule = FALSE;
    DWORD       dwDenyTimestamp = 0;
    DWORD       dwAllowTimestamp = 0;


    do
    {
        if (ERROR_SUCCESS != DbContainsAppCtrlDenyRule(PScanRequest, &bHasDenyRule, &dwDenyTimestamp) || !bHasDenyRule)
        {
            break;
        }
        bBlockProcess = TRUE;

        if (ERROR_SUCCESS != DbContainsAppCtrlAllowRule(PScanRequest, &bHasAllowRule, &dwAllowTimestamp) || !bHasAllowRule)
        {
            break;
        }

        if (dwDenyTimestamp < dwAllowTimestamp)
        {
            bBlockProcess = FALSE;
        }

    } while (0);

    PResultPack->NtScanResult = bBlockProcess ? STATUS_ACCESS_DENIED : STATUS_SUCCESS;

    return ERROR_SUCCESS;
}

_Success_(ERROR_SUCCESS == return)
DWORD
AddAppCtrlDenyRule(
    _In_z_      PWCHAR                      PFilePath,
    _In_        DWORD                       DwPid,
    _Inout_opt_ DWORD                      *PDwRuleId
)
{
    DWORD dwStatus = ERROR_SUCCESS;

    dwStatus = DbAddAppCtrlRule(PFilePath, DwPid, TRUE, PDwRuleId);
    if (ERROR_SUCCESS != dwStatus)
    {
        LogErrorWin(dwStatus, L"DbAddAppCtrlRule(%s, %d, TRUE)", PFilePath, DwPid);
    }

    return dwStatus;
}

_Success_(ERROR_SUCCESS == return)
DWORD
AddAppCtrlAllowRule(
    _In_z_      PWCHAR                      PFilePath,
    _In_        DWORD                       DwPid,
    _Inout_opt_ DWORD                      *PDwRuleId
)
{
    DWORD dwStatus = ERROR_SUCCESS;

    dwStatus = DbAddAppCtrlRule(PFilePath, DwPid, FALSE, PDwRuleId);
    if (ERROR_SUCCESS != dwStatus)
    {
        LogErrorWin(dwStatus, L"DbAddAppCtrlRule(%s, %d, TRUE)", PFilePath, DwPid);
    }

    return dwStatus;
}
