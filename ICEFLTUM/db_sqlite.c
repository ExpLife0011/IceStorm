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

_Success_(ERROR_SUCCESS == return)
DWORD
CreateSettingsDefaultRow(
    VOID
)
{
    DWORD               dwStatus    = SQLITE_OK;
    sqlite3_stmt       *pStatement  = NULL;
    DWORD               dwRows      = 0;

    dwRows = GetNumberOfRows(SQL_STM_GET_SETTINGS_COUNT());
    if (dwRows != 0)
    {
        return ERROR_SUCCESS;
    }

    __try
    {
        if (SQLITE_OK != (dwStatus = PrepareStmt(SQL_STM_INSERT_SETTINGS_DEFAULT_ROW(), &pStatement))) __leave;

        dwStatus = Step(pStatement);
        if (SQLITE_DONE != dwStatus)
        {
            LogErrorWin(dwStatus, L"Step for SQL_STM_INSERT_SETTINGS_DEFAULT_ROW");
            __leave;
        }

        dwStatus = ERROR_SUCCESS;
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

_Success_(return != NULL)
PWCHAR
CreateCopyOfWString(
    _In_    PWCHAR                              PString,
    _In_    DWORD                               Matcher
)
{
    PWCHAR  pNewStr = NULL;
    DWORD   dwSize  = 0;
    DWORD   idx     = 0;

    if (NULL == PString) return NULL;
    
    dwSize = (DWORD) ((wcslen(PString) + 1) * sizeof(WCHAR));
    while (NULL == (pNewStr = malloc(dwSize)))
    {
        LogInfo(L"WHAAAAAAAT???");
        Sleep(10);
    }
    memcpy(pNewStr, PString, dwSize);

    if (Matcher == IcStringMatcher_Wildmat)
    {
        for (idx = 0; idx < dwSize / sizeof(WCHAR); idx++)
        {
            if (pNewStr[idx] == L'%') pNewStr[idx] = L'*';
            else if (pNewStr[idx] == L'_') pNewStr[idx] = L'?';
        }
    }

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
    PRule->PProcessPath = CreateCopyOfWString(sqlite3_column_type(PStatement, 2) == SQLITE_NULL ? NULL : (PWCHAR) sqlite3_column_text16(PStatement, 2), PRule->MatcherProcessPath);
    PRule->DwPid = (DWORD) sqlite3_column_int64(PStatement, 3);
    PRule->MatcherParentPath = (DWORD) sqlite3_column_int64(PStatement, 4);
    PRule->PParentPath = CreateCopyOfWString(sqlite3_column_type(PStatement, 5) == SQLITE_NULL ? NULL : (PWCHAR) sqlite3_column_text16(PStatement, 5), PRule->MatcherParentPath);
    PRule->DwParentPid = (DWORD) sqlite3_column_int64(PStatement, 6);
    PRule->Verdict = (DWORD) sqlite3_column_int64(PStatement, 7);
    PRule->DwAddTime = (DWORD) sqlite3_column_int64(PStatement, 8);
}

VOID
CreateFSScanRuleRow(
    _Inout_     sqlite3_stmt                   *PStatement,
    _Inout_     IC_FS_RULE                     *PRule
)
{
    PRule->DwRuleId = (DWORD) sqlite3_column_int64(PStatement, 0);
    PRule->MatcherProcessPath = (DWORD) sqlite3_column_int64(PStatement, 1);
    PRule->PProcessPath = CreateCopyOfWString(sqlite3_column_type(PStatement, 2) == SQLITE_NULL ? NULL : (PWCHAR) sqlite3_column_text16(PStatement, 2), PRule->MatcherProcessPath);
    PRule->DwPid = (DWORD) sqlite3_column_int64(PStatement, 3);
    PRule->MatcherFilePath = (DWORD) sqlite3_column_int64(PStatement, 4);
    PRule->PFilePath = CreateCopyOfWString(sqlite3_column_type(PStatement, 5) == SQLITE_NULL ? NULL : (PWCHAR) sqlite3_column_text16(PStatement, 5), PRule->MatcherFilePath);
    PRule->UlDeniedOperations = (DWORD) sqlite3_column_int64(PStatement, 6);
    PRule->DwAddTime = (DWORD) sqlite3_column_int64(PStatement, 7);
}

VOID
CreateFSEventRow(
    _Inout_     sqlite3_stmt                   *PStatement,
    _Inout_     IC_FS_EVENT                    *PEvent
)
{
    PEvent->DwEventId = (DWORD) sqlite3_column_int64(PStatement, 0);
    PEvent->PProcessPath = CreateCopyOfWString(sqlite3_column_type(PStatement, 1) == SQLITE_NULL ? NULL : (PWCHAR) sqlite3_column_text16(PStatement, 1), 100);
    PEvent->DwPid = (DWORD) sqlite3_column_int64(PStatement, 2);
    PEvent->PFilePath = CreateCopyOfWString(sqlite3_column_type(PStatement, 3) == SQLITE_NULL ? NULL : (PWCHAR) sqlite3_column_text16(PStatement, 3), 100);
    PEvent->UlRequiredOperations = (DWORD) sqlite3_column_int64(PStatement, 4);
    PEvent->UlDeniedOperations = (DWORD) sqlite3_column_int64(PStatement, 5);
    PEvent->UlRemainingOperations = (DWORD) sqlite3_column_int64(PStatement, 6);
    PEvent->DwMatchedRuleId = (DWORD) sqlite3_column_int64(PStatement, 7);
    PEvent->DwEventTime = (DWORD) sqlite3_column_int64(PStatement, 8);
}

VOID
CreateAppCtrlEventRow(
    _Inout_     sqlite3_stmt                   *PStatement,
    _Inout_     IC_APPCTRL_EVENT               *PEvent
)
{
    PEvent->DwEventId = (DWORD) sqlite3_column_int64(PStatement, 0);
    PEvent->PProcessPath = CreateCopyOfWString(sqlite3_column_type(PStatement, 1) == SQLITE_NULL ? NULL : (PWCHAR) sqlite3_column_text16(PStatement, 1), 100);
    PEvent->DwPid = (DWORD) sqlite3_column_int64(PStatement, 2);
    PEvent->PParentPath = CreateCopyOfWString(sqlite3_column_type(PStatement, 3) == SQLITE_NULL ? NULL : (PWCHAR) sqlite3_column_text16(PStatement, 3), 100);
    PEvent->DwParentPid = (DWORD) sqlite3_column_int64(PStatement, 4);
    PEvent->Verdict = (DWORD) sqlite3_column_int64(PStatement, 5);
    PEvent->DwMatchedRuleId = (DWORD) sqlite3_column_int64(PStatement, 6);
    PEvent->DwEventTime = (DWORD) sqlite3_column_int64(PStatement, 7);
}

DWORD
GetNumberOfRowsWithLimit(
    _In_z_      PCHAR                           PStmtSql,
    _In_        DWORD                           DwFirstId
)
{
    DWORD           dwNrOfRows      = 0;
    sqlite3_stmt   *pStatement      = NULL;

    if (SQLITE_OK != PrepareStmt(PStmtSql, &pStatement)) return dwNrOfRows;

    do 
    {
        if (SQLITE_OK != BindInt(pStatement, 1, DwFirstId)) break;

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
    DWORD   dwStatus            = SQLITE_OK;
    CHAR    pDBPath[MAX_PATH]   = { 0 };
    DWORD   i                   = 0;

    if (gBDBInit) return ERROR_SUCCESS;

    __try
    {

        if (0 == GetModuleFileNameA(NULL, pDBPath, MAX_PATH))
        {
            dwStatus = GetLastError();
            LogWarningWin(GetLastError(), L"GetModuleFileNameA");
            strcpy_s(pDBPath, MAX_PATH, DATABASE_NAME);
        }
        else
        {
            for (i = (DWORD) strlen(pDBPath); (i > 0) && (pDBPath[i] != L'\\'); i--);
            pDBPath[i] = 0;
            sprintf_s(pDBPath, MAX_PATH, "%s\\%s", pDBPath, DATABASE_NAME);
        }

        dwStatus = sqlite3_initialize();
        if (SQLITE_OK != dwStatus)
        {
            LogErrorWin(dwStatus, L"sqlite3_initialize");
            __leave;
        }

        dwStatus = sqlite3_open_v2(pDBPath, &gPDB, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);
        if (SQLITE_OK != dwStatus)
        {
            LogErrorWin(dwStatus, L"sqlite3_open_v2: %S", gPDB ? sqlite3_errmsg(gPDB) : "Probably not enough memory");
            __leave;
        }

        if (SQLITE_OK != (dwStatus = InitPragma())) __leave;

        if (SQLITE_OK != (dwStatus = CreateTables())) __leave;
        
        if (SQLITE_OK != (dwStatus = CreateSettingsDefaultRow())) __leave;

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

/************************************************************************/
/* AppCtrl                                                              */
/************************************************************************/

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
            dwRuleId = 0;
            dwVerdict = IceScanVerdict_Allow;
            __leave;
        }

        dwRuleId = (DWORD) sqlite3_column_int64(pStatement, 0);
        dwVerdict = (DWORD) sqlite3_column_int64(pStatement, 1);
        LogInfo(L"Found %s rule, id: %d.", dwVerdict ? L"DENY" : L"ALLOW", dwRuleId);
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
    
    PRule->DwRuleId = dwRuleId;
    *PVerdict = dwVerdict;

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

        if (NULL != PRules[dwIdx].PProcessPath)
        {
            free(PRules[dwIdx].PProcessPath);
            PRules[dwIdx].PProcessPath = NULL;
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
    DWORD           dwStatus        = SQLITE_OK;
    DWORD           dwStepResult    = 0;
    DWORD           dwIndex         = 1;
    DWORD           dwRuleId        = 0;
    DWORD           dwDeniedFlags   = 0;
    sqlite3_stmt   *pStatement      = NULL;

    __try
    {
        if (SQLITE_OK != (dwStatus = PrepareStmt(SQL_STM_SEARCH_FSSCAN_RULE(), &pStatement))) __leave;

        if (SQLITE_OK != (dwStatus = BindWTextOrNULL(pStatement, dwIndex++, PRule->PProcessPath))) __leave;
        if (SQLITE_OK != (dwStatus = BindWTextOrNULL(pStatement, dwIndex++, PRule->PProcessPath))) __leave;
        if (SQLITE_OK != (dwStatus = BindIntOrNULLIfVal(pStatement, dwIndex++, PRule->DwPid, 0))) __leave;
        if (SQLITE_OK != (dwStatus = BindWTextOrNULL(pStatement, dwIndex++, PRule->PFilePath))) __leave;
        if (SQLITE_OK != (dwStatus = BindWTextOrNULL(pStatement, dwIndex++, PRule->PFilePath))) __leave;
        dwStepResult = Step(pStatement);
        
        if (dwStepResult != SQLITE_DONE && dwStepResult != SQLITE_ROW)
        {
            dwStatus = dwStepResult;
            __leave;
        }

        if (dwStepResult == SQLITE_DONE)
        {
            *PUlDeniedFlags = 0;
            __leave;
        }

        dwRuleId = (DWORD) sqlite3_column_int64(pStatement, 0);
        dwDeniedFlags = (DWORD) sqlite3_column_int64(pStatement, 1);
        LogInfo(L"Found with id: %d, flags: %d.", dwRuleId, dwDeniedFlags);
        
        PRule->DwRuleId = dwRuleId;
        *PUlDeniedFlags = dwDeniedFlags;
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
DbAddFSScanRule(
    IC_FS_RULE                         *PRule,
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
        if (SQLITE_OK != (dwStatus = PrepareStmt(SQL_STM_INSERT_FSSCAN_RULE(), &pStatement))) __leave;

        if (SQLITE_OK != (dwStatus = BindNULL(pStatement, dwIndex++))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PRule->MatcherProcessPath))) __leave;
        if (SQLITE_OK != (dwStatus = BindWTextOrNULL(pStatement, dwIndex++, PRule->PProcessPath))) __leave;
        if (SQLITE_OK != (dwStatus = BindIntOrNULLIfVal(pStatement, dwIndex++, PRule->DwPid, 0))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PRule->MatcherFilePath))) __leave;
        if (SQLITE_OK != (dwStatus = BindWTextOrNULL(pStatement, dwIndex++, PRule->PFilePath))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PRule->UlDeniedOperations))) __leave;
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

        LogInfo(L"FS Rule: %d with id: %d was inserted with success", PRule->UlDeniedOperations, dwRuleId);
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
DbDeleteFSScanRule(
    DWORD                               DwRuleId
)
{
    DWORD           dwStatus        = SQLITE_OK;
    DWORD           dwStepResult    = 0;
    sqlite3_stmt   *pStatement      = NULL;

    __try
    {
        if (SQLITE_OK != (dwStatus = PrepareStmt(SQL_STM_DELETE_FSSCAN_RULE(), &pStatement))) __leave;
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
DbUpdateFSScanRule(
    DWORD                               DwRuleId,
    IC_FS_RULE                         *PRule
)
{
    DWORD           dwStatus        = SQLITE_OK;
    DWORD           dwStepResult    = 0;
    DWORD           dwIndex         = 1;
    sqlite3_stmt   *pStatement      = NULL;

    __try
    {
        if (SQLITE_OK != (dwStatus = PrepareStmt(SQL_STM_UPDATE_FSSCAN_RULE(), &pStatement))) __leave;

        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PRule->MatcherProcessPath))) __leave;
        if (SQLITE_OK != (dwStatus = BindWTextOrNULL(pStatement, dwIndex++, PRule->PProcessPath))) __leave;
        if (SQLITE_OK != (dwStatus = BindIntOrNULLIfVal(pStatement, dwIndex++, PRule->DwPid, 0))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PRule->MatcherFilePath))) __leave;
        if (SQLITE_OK != (dwStatus = BindWTextOrNULL(pStatement, dwIndex++, PRule->PFilePath))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PRule->UlDeniedOperations))) __leave;
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
DbGetFSScanRules(
    PIC_FS_RULE                            *PPRules,
    DWORD                                  *PDwLength
)
{
    DWORD               dwStatus            = SQLITE_OK;
    DWORD               dwStepResult        = 0;
    DWORD               dwIndex             = 0;
    DWORD               dwNrOfRules         = 0;
    PIC_FS_RULE         pRules              = NULL;
    sqlite3_stmt       *pStatement          = NULL;

    __try
    {
        dwNrOfRules = GetNumberOfRows(SQL_STM_GET_FSSCAN_RULES_COUNT());
        if (dwNrOfRules == 0)
        {
            dwStatus = ERROR_NOT_FOUND;
            LogWarningWin(dwStatus, L"No rules were found in database");
            __leave;
        }

        pRules = (PIC_FS_RULE) malloc(dwNrOfRules * sizeof(IC_FS_RULE));
        if (NULL == pRules)
        {
            dwStatus = ERROR_NOT_ENOUGH_MEMORY;
            LogErrorWin(dwStatus, L"malloc(%d)", dwNrOfRules * sizeof(IC_FS_RULE));
            __leave;
        }
        RtlSecureZeroMemory(pRules, dwNrOfRules * sizeof(IC_FS_RULE));

        if (SQLITE_OK != (dwStatus = PrepareStmt(SQL_STM_GET_FSSCAN_RULES(), &pStatement))) __leave;

        dwStepResult = Step(pStatement);
        if (SQLITE_ROW != dwStepResult && SQLITE_DONE != dwStepResult)
        {
            dwStatus = dwStepResult;
            __leave;
        }

        while (SQLITE_ROW == dwStepResult && dwIndex < dwNrOfRules)
        {
            CreateFSScanRuleRow(pStatement, pRules + dwIndex);
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
            DbFreeFSScanRulesList(pRules, dwNrOfRules);
        }
    }

    return dwStatus;
}

_Use_decl_anno_impl_
VOID
DbFreeFSScanRulesList(
    PIC_FS_RULE                             PRules,
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

        if (NULL != PRules[dwIdx].PProcessPath)
        {
            free(PRules[dwIdx].PProcessPath);
            PRules[dwIdx].PProcessPath = NULL;
        }
    }

    free(PRules);
    PRules = NULL;
}

/************************************************************************/
/* Events                                                               */
/************************************************************************/

_Use_decl_anno_impl_
DWORD
DbAddFSEvent(
    PIC_FS_EVENT                    PEvent
)
{
    DWORD           dwStatus        = SQLITE_OK;
    DWORD           dwStepResult    = 0;
    DWORD           dwIndex         = 1;
    DWORD           dwEventId       = 0;
    sqlite3_stmt   *pStatement      = NULL;

    __try
    {
        if (SQLITE_OK != (dwStatus = PrepareStmt(SQL_STM_INSERT_FS_EVENT(), &pStatement))) __leave;

        if (SQLITE_OK != (dwStatus = BindNULL(pStatement, dwIndex++))) __leave;
        if (SQLITE_OK != (dwStatus = BindWText(pStatement, dwIndex++, PEvent->PProcessPath, (DWORD) (wcslen(PEvent->PProcessPath) * sizeof(WCHAR))))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PEvent->DwPid))) __leave;
        if (SQLITE_OK != (dwStatus = BindWText(pStatement, dwIndex++, PEvent->PFilePath, (DWORD) (wcslen(PEvent->PFilePath) * sizeof(WCHAR))))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PEvent->UlRequiredOperations))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PEvent->UlDeniedOperations))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PEvent->UlRemainingOperations))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PEvent->DwMatchedRuleId))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, _time64(NULL)))) __leave;

        dwStepResult = Step(pStatement);
        if (dwStepResult != SQLITE_DONE)
        {
            dwStatus = dwStepResult;
            __leave;
        }

        dwEventId = GetLastRowId();
        PEvent->DwEventId = dwEventId;
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
DbAddAppEvent(
    PIC_APPCTRL_EVENT               PEvent
)
{
    DWORD           dwStatus        = SQLITE_OK;
    DWORD           dwStepResult    = 0;
    DWORD           dwIndex         = 1;
    DWORD           dwEventId       = 0;
    sqlite3_stmt   *pStatement      = NULL;

    __try
    {
        if (SQLITE_OK != (dwStatus = PrepareStmt(SQL_STM_INSERT_APPCTRL_EVENT(), &pStatement))) __leave;

        if (SQLITE_OK != (dwStatus = BindNULL(pStatement, dwIndex++))) __leave;
        if (SQLITE_OK != (dwStatus = BindWText(pStatement, dwIndex++, PEvent->PProcessPath, (DWORD) (wcslen(PEvent->PProcessPath) * sizeof(WCHAR))))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PEvent->DwPid))) __leave;
        if (SQLITE_OK != (dwStatus = BindWText(pStatement, dwIndex++, PEvent->PParentPath, (DWORD) (wcslen(PEvent->PParentPath) * sizeof(WCHAR))))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PEvent->DwParentPid))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PEvent->Verdict))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, PEvent->DwMatchedRuleId))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, dwIndex++, _time64(NULL)))) __leave;

        dwStepResult = Step(pStatement);
        if (dwStepResult != SQLITE_DONE)
        {
            dwStatus = dwStepResult;
            __leave;
        }

        dwEventId = GetLastRowId();
        PEvent->DwEventId = dwEventId;
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
DbGetFSEvents(
    PIC_FS_EVENT                           *PPEvents,
    DWORD                                  *PDwLength,
    DWORD                                   DwFirstId
)
{
    DWORD               dwStatus            = SQLITE_OK;
    DWORD               dwStepResult        = 0;
    DWORD               dwIndex             = 0;
    DWORD               dwNrOfEvents        = 0;
    PIC_FS_EVENT        pEvents             = NULL;
    sqlite3_stmt       *pStatement          = NULL;

    __try
    {
        dwNrOfEvents = GetNumberOfRowsWithLimit(SQL_STM_GET_FS_EVENTS_COUNT(), DwFirstId);
        if (dwNrOfEvents == 0)
        {
            dwStatus = ERROR_NOT_FOUND;
            LogWarningWin(dwStatus, L"No events were found in database");
            __leave;
        }

        if (dwNrOfEvents > 500) dwNrOfEvents = 500;

        pEvents = (PIC_FS_EVENT) malloc(dwNrOfEvents * sizeof(IC_FS_EVENT));
        if (NULL == pEvents)
        {
            dwStatus = ERROR_NOT_ENOUGH_MEMORY;
            LogErrorWin(dwStatus, L"malloc(%d)", dwNrOfEvents * sizeof(IC_FS_EVENT));
            __leave;
        }
        RtlSecureZeroMemory(pEvents, dwNrOfEvents * sizeof(IC_FS_EVENT));

        if (SQLITE_OK != (dwStatus = PrepareStmt(SQL_STM_GET_FS_EVENTS(), &pStatement))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, 1, DwFirstId))) __leave;

        dwStepResult = Step(pStatement);
        if (SQLITE_ROW != dwStepResult && SQLITE_DONE != dwStepResult)
        {
            dwStatus = dwStepResult;
            __leave;
        }

        while (SQLITE_ROW == dwStepResult && dwIndex < dwNrOfEvents)
        {
            CreateFSEventRow(pStatement, pEvents + dwIndex);
            dwStepResult = Step(pStatement);
            dwIndex++;
        }

        *PPEvents = pEvents;
        *PDwLength = dwNrOfEvents;
        LogInfo(L"Retrived %d rows", dwNrOfEvents);
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
            DbFreeFSEventsList(pEvents, dwNrOfEvents);
        }
    }

    return dwStatus;
}

_Use_decl_anno_impl_
VOID
DbFreeFSEventsList(
    PIC_FS_EVENT                            PEvents,
    DWORD                                   DwLength
)
{
    DWORD dwIdx = 0;

    if (NULL == PEvents) return;

    for (dwIdx = 0; dwIdx < DwLength; dwIdx++)
    {
        if (NULL != PEvents[dwIdx].PFilePath)
        {
            free(PEvents[dwIdx].PFilePath);
            PEvents[dwIdx].PFilePath = NULL;
        }

        if (NULL != PEvents[dwIdx].PProcessPath)
        {
            free(PEvents[dwIdx].PProcessPath);
            PEvents[dwIdx].PProcessPath = NULL;
        }
    }

    free(PEvents);
    PEvents = NULL;
}

_Use_decl_anno_impl_
DWORD
DbGetAppCtrlEvents(
    PIC_APPCTRL_EVENT                      *PPEvents,
    DWORD                                  *PDwLength,
    DWORD                                   DwFirstId
)
{
    DWORD               dwStatus            = SQLITE_OK;
    DWORD               dwStepResult        = 0;
    DWORD               dwIndex             = 0;
    DWORD               dwNrOfEvents        = 0;
    PIC_APPCTRL_EVENT   pEvents             = NULL;
    sqlite3_stmt       *pStatement          = NULL;

    __try
    {
        dwNrOfEvents = GetNumberOfRowsWithLimit(SQL_STM_GET_APPCTRL_EVENTS_COUNT(), DwFirstId);
        if (dwNrOfEvents == 0)
        {
            dwStatus = ERROR_NOT_FOUND;
            LogWarningWin(dwStatus, L"No events were found in database");
            __leave;
        }

        if (dwNrOfEvents > 500) dwNrOfEvents = 500;

        pEvents = (PIC_APPCTRL_EVENT) malloc(dwNrOfEvents * sizeof(IC_APPCTRL_EVENT));
        if (NULL == pEvents)
        {
            dwStatus = ERROR_NOT_ENOUGH_MEMORY;
            LogErrorWin(dwStatus, L"malloc(%d)", dwNrOfEvents * sizeof(IC_APPCTRL_EVENT));
            __leave;
        }
        RtlSecureZeroMemory(pEvents, dwNrOfEvents * sizeof(IC_APPCTRL_EVENT));

        if (SQLITE_OK != (dwStatus = PrepareStmt(SQL_STM_GET_APPCTRL_EVENTS(), &pStatement))) __leave;
        if (SQLITE_OK != (dwStatus = BindInt(pStatement, 1, DwFirstId))) __leave;

        dwStepResult = Step(pStatement);
        if (SQLITE_ROW != dwStepResult && SQLITE_DONE != dwStepResult)
        {
            dwStatus = dwStepResult;
            __leave;
        }

        while (SQLITE_ROW == dwStepResult && dwIndex < dwNrOfEvents)
        {
            CreateAppCtrlEventRow(pStatement, pEvents + dwIndex);
            dwStepResult = Step(pStatement);
            dwIndex++;
        }

        *PPEvents = pEvents;
        *PDwLength = dwNrOfEvents;
        LogInfo(L"Retrived %d rows", dwNrOfEvents);
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
            DbFreeAppCtrlEventsList(pEvents, dwNrOfEvents);
        }
    }

    return dwStatus;
}

_Use_decl_anno_impl_
VOID
DbFreeAppCtrlEventsList(
    PIC_APPCTRL_EVENT                       PEvents,
    DWORD                                   DwLength
)
{
    DWORD dwIdx = 0;

    if (NULL == PEvents) return;

    for (dwIdx = 0; dwIdx < DwLength; dwIdx++)
    {
        if (NULL != PEvents[dwIdx].PParentPath)
        {
            free(PEvents[dwIdx].PParentPath);
            PEvents[dwIdx].PParentPath = NULL;
        }

        if (NULL != PEvents[dwIdx].PProcessPath)
        {
            free(PEvents[dwIdx].PProcessPath);
            PEvents[dwIdx].PProcessPath = NULL;
        }
    }

    free(PEvents);
    PEvents = NULL;
}

_Use_decl_anno_impl_
DWORD
DBSetAppCtrlScanStatus(
    DWORD                       DwStatus
)
{
    DWORD           dwError     = ERROR_SUCCESS;
    sqlite3_stmt   *pStatement  = NULL;

    __try
    {
        if (SQLITE_OK != (dwError = PrepareStmt(SQL_STM_UPDATE_SETTINGS_APPCTRL(), &pStatement))) __leave;
        if (SQLITE_OK != (dwError = BindInt(pStatement, 1, DwStatus))) __leave;
        
        if (SQLITE_DONE != (dwError = Step(pStatement))) __leave;

        dwError = ERROR_SUCCESS;
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

    return dwError;
}

_Use_decl_anno_impl_
DWORD
DBSetFSScanStatus(
    DWORD                       DwStatus
)
{
    DWORD           dwError     = ERROR_SUCCESS;
    sqlite3_stmt   *pStatement  = NULL;

    __try
    {
        if (SQLITE_OK != (dwError = PrepareStmt(SQL_STM_UPDATE_SETTINGS_FSSCAN(), &pStatement))) __leave;
        if (SQLITE_OK != (dwError = BindInt(pStatement, 1, DwStatus))) __leave;

        if (SQLITE_DONE != (dwError = Step(pStatement))) __leave;

        dwError = ERROR_SUCCESS;
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

    return dwError;
}

DWORD
DbGetScanStatus(
    DWORD           *PDwAppCtrlStatus,
    DWORD           *PDwFSScanStatus
)
{
    DWORD               dwStatus            = SQLITE_OK;
    DWORD               dwStepResult        = 0;
    DWORD               dwIndex             = 0;
    sqlite3_stmt       *pStatement          = NULL;

    __try
    {
        if (SQLITE_OK != (dwStatus = PrepareStmt(SQL_STM_GET_SETTINGS(), &pStatement))) __leave;
        
        dwStepResult = Step(pStatement);
        if (SQLITE_ROW != dwStepResult && SQLITE_DONE != dwStepResult)
        {
            dwStatus = dwStepResult;
            __leave;
        }

        while (SQLITE_ROW == dwStepResult)
        {
            *PDwAppCtrlStatus = (DWORD) sqlite3_column_int64(pStatement, 0);
            *PDwFSScanStatus = (DWORD) sqlite3_column_int64(pStatement, 1);

            dwStepResult = Step(pStatement);
            dwIndex++;
        }
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