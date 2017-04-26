#include <time.h>
#include "..\Contrib\SQLite\Source\sqlite3.h"
#include "debug2.h"
#include "db_sqlite.h"
#include "db_statements_sql.h"
#include "db_statements.h"

#define DATABASE_NAME   "ice.db"


_Success_(return == SQLITE_OK)
DWORD
InitPragma(
    VOID
)
{
    DWORD dwStatus = SQLITE_OK;

    do 
    {
        if (SQLITE_OK != (dwStatus = Execute("PRAGMA encoding=\"UTF-16\";"))) break;
        if (SQLITE_OK != (dwStatus = Execute("PRAGMA page_size=16384;"))) break;
        if (SQLITE_OK != (dwStatus = sqlite3_extended_result_codes(gPDB, 1)))
        {
            LogErrorWin(dwStatus, L"sqlite3_extended_result_codes");
            break;
        }
        if (SQLITE_OK != (dwStatus = Execute("PRAGMA journal_mode=WAL;"))) break;
        if (SQLITE_OK != (dwStatus = Execute("PRAGMA wal_autocheckpoint=2000;"))) break;
        if (SQLITE_OK != (dwStatus = Execute("PRAGMA synchronous=NORMAL;"))) break;
        if (SQLITE_OK != (dwStatus = Execute("PRAGMA locking_mode=EXCLUSIVE;"))) break;
        if (SQLITE_OK != (dwStatus = Execute("PRAGMA foreign_keys=ON;"))) break;

    } while (0);
    
    return dwStatus;
}

_Success_(return == SQLITE_OK)
DWORD
CreateTables(
    VOID
)
{
    DWORD dwStatus = SQLITE_OK;

    do 
    {
        if (SQLITE_OK != (dwStatus = Execute(SQL_STM_CREATE_TABLES()))) break;
    } while (0);

    return dwStatus;
}

_Success_(return == SQLITE_OK)
DWORD
SearchAppCtrlRule(
    _In_        PICE_APP_CTRL_SCAN_REQUEST_PACKET   PScanReq,
    _In_z_      BOOLEAN     BDenyTable,
    _Inout_     BOOLEAN    *PBHasRule,
    _Inout_     DWORD      *PDwRuleTimestamp
)
{
    DWORD           dwStatus        = SQLITE_OK;
    DWORD           dwStepResult    = 0;
    sqlite3_stmt   *pStatement      = NULL;

    __try
    {
        if (SQLITE_OK != (dwStatus = PrepareStmt(BDenyTable ? SQL_STM_SEARCH_APPCTRL_DENY_RULE() : SQL_STM_SEARCH_APPCTRL_ALLOW_RULE(), &pStatement))) __leave;
        
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, 1, PScanReq->DwPid))) __leave;

        if (PScanReq->PProcessPath != NULL)
        {
            if (SQLITE_OK != (dwStatus = BindWText(pStatement, 2, PScanReq->PProcessPath, PScanReq->DwProcessPathSize))) __leave;
        }
        else
        {
            if (SQLITE_OK != (dwStatus = BindNULL(pStatement, 2))) __leave;
        }

        dwStepResult = Step(pStatement);
        if (dwStepResult != SQLITE_DONE && dwStepResult != SQLITE_ROW)
        {
            dwStatus = dwStepResult;
            __leave;
        }
        
        if (dwStepResult == SQLITE_DONE)
        {
            LogInfo(L"No %s rule found.", BDenyTable ? L"deny" : L"allow");
            *PBHasRule = FALSE;
            *PDwRuleTimestamp = 0;
            __leave;
        }

        *PBHasRule = TRUE;
        *PDwRuleTimestamp = (DWORD) sqlite3_column_int64(pStatement, 0);
        LogInfo(L"Found %s rule with timestamp: %d.", BDenyTable ? L"deny" : L"allow", *PDwRuleTimestamp);
    }
    __finally
    {
        Reset(pStatement);

        if (NULL != pStatement)
        {
            FinalizeStmt(pStatement);
            pStatement = NULL;
        }
    }

    return dwStatus;
}

_Success_(return != NULL)
PWCHAR
CreateCopyOfWString(
    _In_    PWCHAR                              PString
)
{
    PWCHAR  pNewStr = NULL;
    DWORD   dwSize  = 0;
    
    if (NULL == PString) return NULL;
    
    dwSize = (DWORD) ((wcslen(PString) + 1) * sizeof(WCHAR));
    while (NULL == (pNewStr = malloc(dwSize)))
    {
        LogInfo(L"WHAAAAAAAT???");
        Sleep(10);
    }
    memcpy(pNewStr, PString, dwSize);

    return pNewStr;
}

VOID
CreateAppCtrlRuleRow(
    _Inout_     sqlite3_stmt                   *PStatement,
    _Inout_     IC_APPCTRL_RULE                *PRule,
    _In_        DWORD                           DwVerdict                       
)
{
    PRule->DwRuleId = (DWORD) sqlite3_column_int64(PStatement, 0);
    PRule->DwVerdict = DwVerdict;
    PRule->PFilePath = CreateCopyOfWString(sqlite3_column_type(PStatement, 1) == SQLITE_NULL ? NULL : (PWCHAR) sqlite3_column_text16(PStatement, 1));
    PRule->DwPid = (DWORD) sqlite3_column_int64(PStatement, 2);
    PRule->DwAddTime = (DWORD) sqlite3_column_int64(PStatement, 3);
}

DWORD
GetNumberOfRows(
    _In_z_      PCHAR                           PStmtSql
)
{
    DWORD           dwNrOfRows      = 0;
    sqlite3_stmt   *pStatement      = NULL;

    if (SQLITE_OK != PrepareStmt(PStmtSql, &pStatement)) return dwNrOfRows;
    
    do 
    {
        if (SQLITE_ROW != Step(pStatement)) break;

        dwNrOfRows = (DWORD) sqlite3_column_int64(pStatement, 0);
        PCHAR pTxt = sqlite3_expanded_sql(pStatement);
        LogInfo(L"%S returned %d rows", pTxt, dwNrOfRows);
        sqlite3_free(pTxt);
    } while (0);

    if (NULL != pStatement)
    {
        Reset(pStatement);
        FinalizeStmt(pStatement);
        pStatement = NULL;
    }
    
    return dwNrOfRows;
}

/************************************************************************/
/* PUBLIC METHODS                                                       */
/************************************************************************/
_Use_decl_anno_impl_
SQL_ERROR
DBInit(
    VOID
)
{
    DWORD dwStatus = SQLITE_OK;
    
    DBUninit();

    __try
    {
        dwStatus = sqlite3_initialize();
        if (SQLITE_OK != dwStatus)
        {
            LogErrorWin(dwStatus, L"sqlite3_initialize");
            __leave;
        }

        dwStatus = sqlite3_open_v2(DATABASE_NAME, &gPDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);
        if (SQLITE_OK != dwStatus)
        {
            LogErrorWin(dwStatus, L"sqlite3_open_v2: %S", gPDB ? sqlite3_errmsg(gPDB) : "Probably not enough memory");
            __leave;
        }

        if (SQLITE_OK != (dwStatus = InitPragma())) __leave;

        if (SQLITE_OK != (dwStatus = CreateTables())) __leave;
        
    }
    __finally
    {
        if (dwStatus != ERROR_SUCCESS)
        {
            DBUninit();
        }
    }

    return dwStatus;
}

_Use_decl_anno_impl_
SQL_ERROR
DBUninit(
    VOID
)
{
    DWORD dwResult = ERROR_SUCCESS;

    if (NULL != gPDB)
    {
        dwResult = sqlite3_close_v2(gPDB);
        if (SQLITE_OK != dwResult)
        {
            LogWarningWin(dwResult, L"sqlite3_close_v2");
        }
        gPDB = NULL;
    }

    sqlite3_shutdown();

    return SQLITE_OK;
}

_Use_decl_anno_impl_
DWORD
DbContainsAppCtrlDenyRule(
    PICE_APP_CTRL_SCAN_REQUEST_PACKET   PScanRequest,
    BOOLEAN                            *PBHasDenyRule,
    DWORD                              *PDwDenyTimestamp
)
{
    return SearchAppCtrlRule(PScanRequest, TRUE, PBHasDenyRule, PDwDenyTimestamp);
}

_Use_decl_anno_impl_
DWORD
DbContainsAppCtrlAllowRule(
    PICE_APP_CTRL_SCAN_REQUEST_PACKET   PScanRequest,
    BOOLEAN                            *PBHasAllowRule,
    DWORD                              *PDwAllowTimestamp
)
{
    return SearchAppCtrlRule(PScanRequest, FALSE, PBHasAllowRule, PDwAllowTimestamp);
}

_Use_decl_anno_impl_
DWORD
DbAddAppCtrlRule(
    PWCHAR                              PFilePath,
    DWORD                               DwPid,
    BOOLEAN                             BIsDenyRule,
    DWORD                              *PDwRuleId
)
{
    DWORD           dwStatus        = SQLITE_OK;
    DWORD           dwStepResult    = 0;
    DWORD           dwIndex         = 1;
    DWORD           dwRuleId        = 0;
    sqlite3_stmt   *pStatement      = NULL;

    __try
    {
        if (SQLITE_OK != (dwStatus = PrepareStmt(BIsDenyRule ? SQL_STM_INSERT_APPCTRL_DENY_RULE() : SQL_STM_INSERT_APPCTRL_ALLOW_RULE(), &pStatement))) __leave;
        
        if (SQLITE_OK != (dwStatus = BindNULL(pStatement, dwIndex++))) __leave;
        
        if (SQLITE_OK != (dwStatus = BindWTextOrNULL(pStatement, dwIndex++, PFilePath))) __leave;
        
        if (0 != DwPid)
        {
            if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, DwPid))) __leave;
        }
        else
        {
            if (SQLITE_OK != (dwStatus = BindNULL(pStatement, dwIndex++))) __leave;
        }

        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, _time64(NULL)))) __leave;


        dwStepResult = Step(pStatement);
        if (dwStepResult != SQLITE_DONE)
        {
            dwStatus = dwStepResult;
            __leave;
        }

        dwRuleId = GetLastRowId();
        if (NULL != PDwRuleId)
        {
            *PDwRuleId = dwRuleId;
        }

        LogInfo(L"%s rule with id: %d was inserted with success", BIsDenyRule ? L"Deny" : L"Allow", dwRuleId);
    }
    __finally
    {
        Reset(pStatement);

        if (NULL != pStatement)
        {
            FinalizeStmt(pStatement);
            pStatement = NULL;
        }
    }

    return dwStatus;
}

_Use_decl_anno_impl_
DWORD
DbDeleteAppCtrlRule(
    DWORD                               DwRuleId,
    BOOLEAN                             BIsDenyRule
)
{
    DWORD           dwStatus        = SQLITE_OK;
    DWORD           dwStepResult    = 0;
    sqlite3_stmt   *pStatement      = NULL;

    __try
    {
        if (SQLITE_OK != (dwStatus = PrepareStmt(BIsDenyRule ? SQL_STM_DELETE_APPCTRL_DENY_RULE() : SQL_STM_DELETE_APPCTRL_ALLOW_RULE(), &pStatement))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, 1, DwRuleId))) __leave;
        dwStepResult = Step(pStatement);
        if (dwStepResult != SQLITE_DONE)
        {
            dwStatus = dwStepResult;
            __leave;
        }

        if (Changes() == 0)
        {
            dwStatus = ERROR_NOT_FOUND;
            LogErrorWin(dwStatus, L"No rows were affected by the delete (RowID: %d)", DwRuleId);
            __leave;
        }

        LogInfo(L"%s rule with id %d was deleted with success", BIsDenyRule ? L"Deny" : L"Allow", DwRuleId);
    }
    __finally
    {
        Reset(pStatement);

        if (NULL != pStatement)
        {
            FinalizeStmt(pStatement);
            pStatement = NULL;
        }
    }
    
    return dwStatus;
}

_Use_decl_anno_impl_
DWORD
DbUpdateAppCtrlRule(
    DWORD                               DwRuleId,
    PWCHAR                              PFilePath,
    DWORD                               DwPid,
    BOOLEAN                             BIsDenyRule
)
{
    DWORD           dwStatus        = SQLITE_OK;
    DWORD           dwStepResult    = 0;
    DWORD           dwIndex         = 1;
    sqlite3_stmt   *pStatement      = NULL;

    __try
    {
        if (SQLITE_OK != (dwStatus = PrepareStmt(BIsDenyRule ? SQL_STM_UPDATE_APPCTRL_DENY_RULE() : SQL_STM_UPDATE_APPCTRL_ALLOW_RULE(), &pStatement))) __leave;

        if (SQLITE_OK != (dwStatus = BindWTextOrNULL(pStatement, dwIndex++, PFilePath))) __leave;

        if (0 != DwPid)
        {
            if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, DwPid))) __leave;
        }
        else
        {
            if (SQLITE_OK != (dwStatus = BindNULL(pStatement, dwIndex++))) __leave;
        }

        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, DwRuleId))) __leave;


        dwStepResult = Step(pStatement);
        if (dwStepResult != SQLITE_DONE)
        {
            dwStatus = dwStepResult;
            __leave;
        }
        
        if (Changes() == 0)
        {
            dwStatus = ERROR_NOT_FOUND;
            LogErrorWin(dwStatus, L"No rows were affected by the update (RowID: %d)", DwRuleId);
            __leave;
        }

        LogInfo(L"%s rule with id: %d was Updated with success", BIsDenyRule ? L"Deny" : L"Allow", DwRuleId);
    }
    __finally
    {
        Reset(pStatement);

        if (NULL != pStatement)
        {
            FinalizeStmt(pStatement);
            pStatement = NULL;
        }
    }

    return dwStatus;
}

_Use_decl_anno_impl_
DWORD
DbGetAppCtrlRules(
    BOOLEAN                                 BGetAllowRules,
    BOOLEAN                                 BGetDenyRules,
    PIC_APPCTRL_RULE                       *PPRules,
    DWORD                                  *PDwLength
)
{
    DWORD               dwStatus            = SQLITE_OK;
    DWORD               dwStepResult        = 0;
    DWORD               dwIndex             = 0;
    DWORD               dwNrOfAllowRules    = 0;
    DWORD               dwNrOfDenyRules     = 0;
    DWORD               dwNrOfRules         = 0;
    PIC_APPCTRL_RULE    pRules              = NULL;
    sqlite3_stmt       *pGetAllowStatement  = NULL;
    sqlite3_stmt       *pGetDenyStatement   = NULL;

    __try
    {
        if (BGetAllowRules)
        {
            dwNrOfAllowRules = GetNumberOfRows(SQL_STM_GET_APPCTRL_ALLOW_RULES_COUNT());
            if (dwNrOfAllowRules != 0)
            {
                if (SQLITE_OK != (dwStatus = PrepareStmt(SQL_STM_GET_APPCTRL_ALLOW_RULES(), &pGetAllowStatement))) __leave;
            }
        }

        if (BGetDenyRules)
        {
            dwNrOfDenyRules = GetNumberOfRows(SQL_STM_GET_APPCTRL_DENY_RULES_COUNT());
            if (dwNrOfDenyRules != 0)
            {
                if (SQLITE_OK != (dwStatus = PrepareStmt(SQL_STM_GET_APPCTRL_DENY_RULES(), &pGetDenyStatement))) __leave;
            }
        }

        dwNrOfRules = dwNrOfAllowRules + dwNrOfDenyRules;
        if (dwNrOfRules == 0)
        {
            dwStatus = ERROR_NOT_FOUND;
            LogWarningWin(dwStatus, L"No rules were found in database");
            __leave;
        }

        pRules = (PIC_APPCTRL_RULE) malloc(dwNrOfRules * sizeof(IC_APPCTRL_RULE));
        if (NULL == pRules)
        {
            dwStatus = ERROR_NOT_ENOUGH_MEMORY;
            LogErrorWin(dwStatus, L"malloc(%d)", dwNrOfRules * sizeof(IC_APPCTRL_RULE));
            __leave;
        }
        RtlSecureZeroMemory(pRules, dwNrOfRules * sizeof(IC_APPCTRL_RULE));

        if (0 != dwNrOfAllowRules)
        {
            dwStepResult = Step(pGetAllowStatement);
            if (SQLITE_ROW != dwStepResult)
            {
                dwStatus = dwStepResult;
                LogErrorWin(dwStepResult, L"Step(pGetAllowStatement)");
                __leave;
            }

            while (SQLITE_ROW == dwStepResult && dwIndex < dwNrOfAllowRules)
            {
                CreateAppCtrlRuleRow(pGetAllowStatement, pRules + dwIndex, ERROR_SUCCESS);
                dwStepResult = Step(pGetAllowStatement);
                dwIndex++;
            }
        }

        if (0 != dwNrOfDenyRules)
        {
            dwStepResult = Step(pGetDenyStatement);
            if (SQLITE_ROW != dwStepResult)
            {
                dwStatus = dwStepResult;
                LogErrorWin(dwStepResult, L"Step(pGetDenyStatement)");
                __leave;
            }

            while (SQLITE_ROW == dwStepResult && dwIndex < dwNrOfRules)
            {
                CreateAppCtrlRuleRow(pGetDenyStatement, pRules + dwIndex, ERROR_ACCESS_DENIED);
                dwStepResult = Step(pGetDenyStatement);
                dwIndex++;
            }
        }

        *PPRules = pRules;
        *PDwLength = dwNrOfRules;
        LogInfo(L"Retrived %d rows (%d allow, %d deny)", dwNrOfRules, dwNrOfAllowRules, dwNrOfDenyRules);
    }
    __finally
    {
        if (NULL != pGetAllowStatement)
        {
            Reset(pGetAllowStatement);
            FinalizeStmt(pGetAllowStatement);
            pGetAllowStatement = NULL;
        }

        if (NULL != pGetDenyStatement)
        {
            Reset(pGetDenyStatement);
            FinalizeStmt(pGetDenyStatement);
            pGetDenyStatement = NULL;
        }

        if (ERROR_SUCCESS != dwStatus)
        {
            DbFreeAppCtrlRulesList(pRules, dwNrOfRules);
        }
    }

    return dwStatus;
}

_Use_decl_anno_impl_
VOID
DbFreeAppCtrlRulesList(
    PIC_APPCTRL_RULE                        PRules,
    DWORD                                   DwLength
)
{
    DWORD dwIdx = 0;

    if (NULL == PRules) return;

    for (dwIdx = 0; dwIdx < DwLength; dwIdx++)
    {
        if (NULL != PRules[dwIdx].PFilePath)
        {
            free(PRules[dwIdx].PFilePath);
            PRules[dwIdx].PFilePath = NULL;
        }
    }
    
    free(PRules);
    PRules = NULL;
}
