#include <time.h>
#include "..\Contrib\SQLite\Source\sqlite3.h"
#include "debug2.h"
#include "db_sqlite.h"
#include "db_statements_sql.h"
#include "db_statements.h"

#define DATABASE_NAME   "ice.db"

BOOLEAN gBDBInit = FALSE;

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
    _Inout_     IC_APPCTRL_RULE                *PRule
)
{
    PRule->DwRuleId = (DWORD) sqlite3_column_int64(PStatement, 0);
    PRule->MatcherProcessPath = (DWORD) sqlite3_column_int64(PStatement, 1);
    PRule->PProcessPath = CreateCopyOfWString(sqlite3_column_type(PStatement, 2) == SQLITE_NULL ? NULL : (PWCHAR) sqlite3_column_text16(PStatement, 2));
    PRule->DwPid = (DWORD) sqlite3_column_int64(PStatement, 3);
    PRule->MatcherParentPath = (DWORD) sqlite3_column_int64(PStatement, 4);
    PRule->PParentPath = CreateCopyOfWString(sqlite3_column_type(PStatement, 5) == SQLITE_NULL ? NULL : (PWCHAR) sqlite3_column_text16(PStatement, 5));
    PRule->DwParentPid = (DWORD) sqlite3_column_int64(PStatement, 6);
    PRule->Verdict = (DWORD) sqlite3_column_int64(PStatement, 6);
    PRule->DwAddTime = (DWORD) sqlite3_column_int64(PStatement, 7);
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
    
    if (gBDBInit) return ERROR_SUCCESS;

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
        
        gBDBInit = TRUE;
    }
    __finally
    {
        if (dwStatus != ERROR_SUCCESS)
        {
            gBDBInit = TRUE;
            DBUninit();
            gBDBInit = FALSE;
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

    if (!gBDBInit) return ERROR_SUCCESS;

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

    gBDBInit = FALSE;
    return SQLITE_OK;
}

_Use_decl_anno_impl_
DWORD
DbGetAppCtrlVerdict(
    IC_APPCTRL_RULE                    *PRule,
    ICE_SCAN_VERDICT                   *PVerdict
)
{
    DWORD           dwStatus        = SQLITE_OK;
    DWORD           dwStepResult    = 0;
    DWORD           dwIndex         = 1;
    DWORD           dwRuleId        = 0;
    DWORD           dwVerdict       = 0;
    sqlite3_stmt   *pStatement      = NULL;

    __try
    {
        if (SQLITE_OK != (dwStatus = PrepareStmt(SQL_STM_SEARCH_APPCTRL_RULE(), &pStatement))) __leave;

        if (SQLITE_OK != (dwStatus = BindWTextOrNULL(pStatement, dwIndex++, PRule->PProcessPath))) __leave;
        if (SQLITE_OK != (dwStatus = BindWTextOrNULL(pStatement, dwIndex++, PRule->PProcessPath))) __leave;
        if (SQLITE_OK != (dwStatus = BindIntOrNULLIfVal(pStatement, dwIndex++, PRule->DwPid, 0))) __leave;
        if (SQLITE_OK != (dwStatus = BindWTextOrNULL(pStatement, dwIndex++, PRule->PParentPath))) __leave;
        if (SQLITE_OK != (dwStatus = BindWTextOrNULL(pStatement, dwIndex++, PRule->PParentPath))) __leave;
        if (SQLITE_OK != (dwStatus = BindIntOrNULLIfVal(pStatement, dwIndex++, PRule->DwParentPid, 0))) __leave;

        dwStepResult = Step(pStatement);
        if (dwStepResult != SQLITE_DONE && dwStepResult != SQLITE_ROW)
        {
            dwStatus = dwStepResult;
            __leave;
        }

        if (dwStepResult == SQLITE_DONE)
        {
            LogInfo(L"No rule found.");
            *PVerdict = IceScanVerdict_Allow;
            __leave;
        }

        dwRuleId = (DWORD) sqlite3_column_int64(pStatement, 0);
        dwVerdict = (DWORD) sqlite3_column_int64(pStatement, 1);
        LogInfo(L"Found %s rule, id: %d.", dwVerdict ? L"DENY" : L"ALLOW", dwRuleId);

        *PVerdict = dwVerdict;
    }
    __finally
    {
        if (NULL != pStatement)
        {
            Reset(pStatement);
            FinalizeStmt(pStatement);
            pStatement = NULL;
        }
    }

    return dwStatus;
}

_Use_decl_anno_impl_
DWORD
DbAddAppCtrlRule(
    IC_APPCTRL_RULE                    *PRule,
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
        if (SQLITE_OK != (dwStatus = PrepareStmt(SQL_STM_INSERT_APPCTRL_RULE(), &pStatement))) __leave;
        
        if (SQLITE_OK != (dwStatus = BindNULL(pStatement, dwIndex++))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PRule->MatcherProcessPath))) __leave;
        if (SQLITE_OK != (dwStatus = BindWTextOrNULL(pStatement, dwIndex++, PRule->PProcessPath))) __leave;
        if (SQLITE_OK != (dwStatus = BindIntOrNULLIfVal(pStatement, dwIndex++, PRule->DwPid, 0))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PRule->MatcherParentPath))) __leave;
        if (SQLITE_OK != (dwStatus = BindWTextOrNULL(pStatement, dwIndex++, PRule->PParentPath))) __leave;
        if (SQLITE_OK != (dwStatus = BindIntOrNULLIfVal(pStatement, dwIndex++, PRule->DwParentPid, 0))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PRule->Verdict))) __leave;
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

        LogInfo(L"%s rule with id: %d was inserted with success", PRule->Verdict ? L"Deny" : L"Allow", dwRuleId);
    }
    __finally
    {
        if (NULL != pStatement)
        {
            Reset(pStatement);
            FinalizeStmt(pStatement);
            pStatement = NULL;
        }
    }

    return dwStatus;
}

_Use_decl_anno_impl_
DWORD
DbDeleteAppCtrlRule(
    DWORD                               DwRuleId
)
{
    DWORD           dwStatus        = SQLITE_OK;
    DWORD           dwStepResult    = 0;
    sqlite3_stmt   *pStatement      = NULL;

    __try
    {
        if (SQLITE_OK != (dwStatus = PrepareStmt(SQL_STM_DELETE_APPCTRL_RULE(), &pStatement))) __leave;
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

        LogInfo(L"Rule with id %d was deleted with success", DwRuleId);
    }
    __finally
    {
        if (NULL != pStatement)
        {
            Reset(pStatement);
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
    IC_APPCTRL_RULE                    *PRule
)
{
    DWORD           dwStatus        = SQLITE_OK;
    DWORD           dwStepResult    = 0;
    DWORD           dwIndex         = 1;
    sqlite3_stmt   *pStatement      = NULL;

    __try
    {
        if (SQLITE_OK != (dwStatus = PrepareStmt(SQL_STM_UPDATE_APPCTRL_RULE(), &pStatement))) __leave;

        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PRule->MatcherProcessPath))) __leave;
        if (SQLITE_OK != (dwStatus = BindWTextOrNULL(pStatement, dwIndex++, PRule->PProcessPath))) __leave;
        if (SQLITE_OK != (dwStatus = BindIntOrNULLIfVal(pStatement, dwIndex++, PRule->DwPid, 0))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PRule->MatcherParentPath))) __leave;
        if (SQLITE_OK != (dwStatus = BindWTextOrNULL(pStatement, dwIndex++, PRule->PParentPath))) __leave;
        if (SQLITE_OK != (dwStatus = BindIntOrNULLIfVal(pStatement, dwIndex++, PRule->DwParentPid, 0))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PRule->Verdict))) __leave;
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

        LogInfo(L"Rule with id: %d was Updated with success", DwRuleId);
    }
    __finally
    {
        if (NULL != pStatement)
        {
            Reset(pStatement);
            FinalizeStmt(pStatement);
            pStatement = NULL;
        }
    }

    return dwStatus;
}

_Use_decl_anno_impl_
DWORD
DbGetAppCtrlRules(
    PIC_APPCTRL_RULE                       *PPRules,
    DWORD                                  *PDwLength
)
{
    DWORD               dwStatus            = SQLITE_OK;
    DWORD               dwStepResult        = 0;
    DWORD               dwIndex             = 0;
    DWORD               dwNrOfRules         = 0;
    PIC_APPCTRL_RULE    pRules              = NULL;
    sqlite3_stmt       *pStatement          = NULL;
    
    __try
    {
        dwNrOfRules = GetNumberOfRows(SQL_STM_GET_APPCTRL_RULES_COUNT());
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

        if (SQLITE_OK != (dwStatus = PrepareStmt(SQL_STM_GET_APPCTRL_RULES(), &pStatement))) __leave;
        
        dwStepResult = Step(pStatement);
        if (SQLITE_ROW != dwStepResult && SQLITE_DONE != dwStepResult)
        {
            dwStatus = dwStepResult;
            __leave;
        }

        while (SQLITE_ROW == dwStepResult && dwIndex < dwNrOfRules)
        {
            CreateAppCtrlRuleRow(pStatement, pRules + dwIndex);
            dwStepResult = Step(pStatement);
            dwIndex++;
        }
        
        *PPRules = pRules;
        *PDwLength = dwNrOfRules;
        LogInfo(L"Retrived %d rows", dwNrOfRules);
    }
    __finally
    {
        if (NULL != pStatement)
        {
            Reset(pStatement);
            FinalizeStmt(pStatement);
            pStatement = NULL;
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
        if (NULL != PRules[dwIdx].PParentPath)
        {
            free(PRules[dwIdx].PParentPath);
            PRules[dwIdx].PParentPath = NULL;
        }

        if (NULL != PRules[dwIdx].PParentPath)
        {
            free(PRules[dwIdx].PParentPath);
            PRules[dwIdx].PParentPath = NULL;
        }
    }
    
    free(PRules);
    PRules = NULL;
}


/************************************************************************/
/* FS SCAN                                                              */
/************************************************************************/

_Use_decl_anno_impl_
DWORD
DbGetFSScanDeniedFlags(
    IC_FS_RULE                         *PRule,
    ULONG                              *PUlDeniedFlags
)
{
    ULONG flags = 0;

    if (wcswcs(PRule->PFilePath, L"test.txt") && wcswcs(PRule->PProcessPath, L"FileTest.exe"))
    {
        flags = ICE_FS_FLAG_DELETE | ICE_FS_FLAG_WRITE;
    }
    else if (wcswcs(PRule->PFilePath, L"test.txt") && wcswcs(PRule->PProcessPath, L"TOTALCMD.EXE"))
    {
        flags = (ULONG) -1;
    }

    if (flags)
    {
        LogInfo(L"denied flags: %d", flags);
    }
    *PUlDeniedFlags = flags;

    return 0;
}
