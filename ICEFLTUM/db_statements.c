#include "db_statements.h"
#include "debug2.h"

sqlite3 *gPDB = NULL;

DWORD
GetLastRowId(
    VOID
)
{
    return (DWORD) sqlite3_last_insert_rowid(gPDB);
}

_Success_(return == SQLITE_OK)
DWORD 
Execute(
    _In_z_ PCHAR    PCommand
)
{
    DWORD dwResult = SQLITE_OK;
    PCHAR pErrMsg = NULL;

    if (NULL == gPDB)
    {
        return SQLITE_ERROR;
    }

    dwResult = sqlite3_exec(gPDB, PCommand, NULL, NULL, &pErrMsg);
    if (dwResult != SQLITE_OK)
    {
        LogErrorWin(dwResult, L"sqlite3_exec failedd (Error: %S) SQL: \n\"%S\"", pErrMsg, PCommand);
        sqlite3_free(pErrMsg);
    }

    return dwResult;
}

_Success_(return == SQLITE_OK)
DWORD
PrepareStmt(
    _In_z_      PCHAR           PSql,
    _Inout_     sqlite3_stmt  **PPStmt
)
{
    DWORD dwResult = SQLITE_OK;

    if (NULL == gPDB)
    {
        return SQLITE_ERROR;
    }

    dwResult = sqlite3_prepare_v2(gPDB, PSql, -1, PPStmt, NULL);
    if (dwResult != SQLITE_OK)
    {
        LogErrorWin(dwResult, L"sqlite3_prepare_v2 failed (Error: %S) SQL: \n\"%S\"", sqlite3_errmsg(gPDB), PSql);
    }

    return dwResult;
}

_Success_(return == SQLITE_OK)
DWORD
FinalizeStmt(
    _Inout_     sqlite3_stmt   *PStmt
)
{
    DWORD dwResult = SQLITE_OK;

    if (NULL == gPDB)
    {
        return SQLITE_ERROR;
    }

    dwResult = sqlite3_finalize(PStmt);
    if (dwResult != SQLITE_OK)
    {
        LogWarningWin(dwResult, L"sqlite3_finalize (Error: %S)", sqlite3_errmsg(gPDB));
    }

    return dwResult;
}

_Success_(return == SQLITE_DONE || return == SQLITE_ROW)
DWORD
Step(
    _In_        sqlite3_stmt   *PStmt
)
{
    DWORD dwResult  = SQLITE_OK;
    PCHAR pText     = NULL;

    if (NULL == gPDB)
    {
        return SQLITE_ERROR;
    }

    dwResult = sqlite3_step(PStmt);
    if (dwResult != SQLITE_DONE && dwResult != SQLITE_ROW)
    {
        pText = sqlite3_expanded_sql(PStmt);
        LogWarningWin(dwResult, L"sqlite3_step: %d (Error: %S), STATEMENT:\n%S", dwResult, sqlite3_errmsg(gPDB), pText);
        sqlite3_free(pText);
    }

    return dwResult;
}

_Success_(return == SQLITE_OK)
DWORD
Reset(
    _In_        sqlite3_stmt   *PStmt
)
{
    DWORD dwResult = SQLITE_OK;
    PCHAR pText = NULL;

    if (NULL == gPDB)
    {
        return SQLITE_ERROR;
    }

    dwResult = sqlite3_reset(PStmt);
    if (dwResult != SQLITE_OK)
    {
        pText = sqlite3_expanded_sql(PStmt);
        LogWarningWin(dwResult, L"sqlite3_reset: %d (Error: %S), STATEMENT:\n%S", dwResult, sqlite3_errmsg(gPDB), pText);
        sqlite3_free(pText);
    }

    return dwResult;
}

_Success_(return == SQLITE_OK)
DWORD
BindText(
    _Inout_     sqlite3_stmt   *PStmt, 
    _In_        DWORD           DwIndex, 
    _In_z_      PCHAR           PValue,
    _In_        DWORD           DwSize
)
{
    DWORD dwResult = SQLITE_OK;

    if (NULL == gPDB)
    {
        return SQLITE_ERROR;
    }

    dwResult = sqlite3_bind_text(PStmt, DwIndex, PValue, DwSize, SQLITE_STATIC);
    if (dwResult != SQLITE_OK)
    {
        LogErrorWin(dwResult, L"sqlite3_bind_text(idx: %d, val: %S, size: %d) (Error: %S)", DwIndex, PValue, DwSize, sqlite3_errmsg(gPDB));
    }

    return dwResult;
}

_Success_(return == SQLITE_OK)
DWORD
BindWText(
    _Inout_     sqlite3_stmt   *PStmt,
    _In_        DWORD           DwIndex,
    _In_z_      PWCHAR          PValue,
    _In_        DWORD           DwSize
)
{
    DWORD dwResult = SQLITE_OK;

    if (NULL == gPDB)
    {
        return SQLITE_ERROR;
    }

    dwResult = sqlite3_bind_text16(PStmt, DwIndex, PValue, DwSize, SQLITE_STATIC);
    if (dwResult != SQLITE_OK)
    {
        LogErrorWin(dwResult, L"sqlite3_bind_text16(idx: %d, val: %s, size: %d) (Error: %S)", DwIndex, PValue, DwSize, sqlite3_errmsg(gPDB));
    }

    return dwResult;
}

_Success_(return == SQLITE_OK)
DWORD
BindInt(
    _Inout_     sqlite3_stmt   *PStmt,
    _In_        DWORD           DwIndex,
    _In_        UINT64          QwValue
)
{
    DWORD dwResult = SQLITE_OK;

    if (NULL == gPDB)
    {
        return SQLITE_ERROR;
    }

    dwResult = sqlite3_bind_int64(PStmt, DwIndex, QwValue);
    if (dwResult != SQLITE_OK)
    {
        LogErrorWin(dwResult, L"sqlite3_bind_int64(idx: %d, val: %I64d) (Error: %S)", DwIndex, QwValue, sqlite3_errmsg(gPDB));
    }

    return dwResult;
}

_Success_(return == SQLITE_OK)
DWORD
BindNULL(
    _Inout_     sqlite3_stmt   *PStmt,
    _In_        DWORD           DwIndex
)
{
    DWORD dwResult = SQLITE_OK;

    if (NULL == gPDB)
    {
        return SQLITE_ERROR;
    }

    dwResult = sqlite3_bind_null(PStmt, DwIndex);
    if (dwResult != SQLITE_OK)
    {
        LogErrorWin(dwResult, L"sqlite3_bind_null(idx: %d) (Error: %S)", DwIndex, sqlite3_errmsg(gPDB));
    }

    return dwResult;
}

_Success_(return == SQLITE_OK)
DWORD
BindWTextOrNULL(
    _Inout_     sqlite3_stmt   *PStmt,
    _In_        DWORD           DwIndex,
    _In_z_      PWCHAR          PValue
)
{
    if (PValue != NULL)
    {
        return BindWText(PStmt, DwIndex, PValue, wcslen(PValue) * sizeof(WCHAR));
    }
    else
    {
        return BindNULL(PStmt, DwIndex);
    }
}

_Success_(return == SQLITE_OK)
DWORD
BindTextOrNULL(
    _Inout_     sqlite3_stmt   *PStmt,
    _In_        DWORD           DwIndex,
    _In_z_      PCHAR           PValue
)
{
    if (PValue != NULL)
    {
        return BindText(PStmt, DwIndex, PValue, strlen(PValue));
    }
    else
    {
        return BindNULL(PStmt, DwIndex);
    }
}
