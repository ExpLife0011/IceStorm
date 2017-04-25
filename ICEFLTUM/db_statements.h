#pragma once

#include "..\Contrib\SQLite\Source\sqlite3.h"

#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS

DWORD
GetLastRowId(
    VOID
);

_Success_(return == SQLITE_OK)
DWORD
Execute(
    _In_z_ PCHAR    PCommand
);

_Success_(return == SQLITE_OK)
DWORD
PrepareStmt(
    _In_z_      PCHAR           PSql,
    _Inout_     sqlite3_stmt  **PPStmt
);

_Success_(return == SQLITE_OK)
DWORD
FinalizeStmt(
    _Inout_     sqlite3_stmt   *PStmt
);

_Success_(return == SQLITE_DONE || return == SQLITE_ROW)
DWORD
Step(
    _In_        sqlite3_stmt   *PStmt
);

_Success_(return == SQLITE_OK)
DWORD
Reset(
    _In_        sqlite3_stmt   *PStmt
);

_Success_(return == SQLITE_OK)
DWORD
BindText(
    _Inout_     sqlite3_stmt   *PStmt,
    _In_        DWORD           DwIndex,
    _In_z_      PCHAR           PValue,
    _In_        DWORD           DwSize
);

_Success_(return == SQLITE_OK)
DWORD
BindWText(
    _Inout_     sqlite3_stmt   *PStmt,
    _In_        DWORD           DwIndex,
    _In_z_      PWCHAR          PValue,
    _In_        DWORD           DwSize
);

_Success_(return == SQLITE_OK)
DWORD
BindInt(
    _Inout_     sqlite3_stmt   *PStmt,
    _In_        DWORD           DwIndex,
    _In_        UINT64          QwValue
);

_Success_(return == SQLITE_OK)
DWORD
BindNULL(
    _Inout_     sqlite3_stmt   *PStmt,
    _In_        DWORD           DwIndex
);

_Success_(return == SQLITE_OK)
DWORD
BindWTextOrNULL(
    _Inout_     sqlite3_stmt   *PStmt,
    _In_        DWORD           DwIndex,
    _In_z_      PWCHAR          PValue
);

_Success_(return == SQLITE_OK)
DWORD
BindTextOrNULL(
    _Inout_     sqlite3_stmt   *PStmt,
    _In_        DWORD           DwIndex,
    _In_z_      PCHAR           PValue
);
