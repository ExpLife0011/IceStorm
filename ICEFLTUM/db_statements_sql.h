#pragma once

#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS

__inline
PCHAR
SQL_STM_CREATE_TABLES(
    VOID
)
{
    return
        // AppCtrl Rules
        "CREATE TABLE IF NOT EXISTS [appctrl_deny_rules] ("
        "  [id] INTEGER PRIMARY KEY AUTOINCREMENT, "
        "  [file_path] TEXT, "
        "  [pid] INT, "
        "  [add_time] INTEGER "
        ");"
        ""
        ""
        "CREATE TABLE IF NOT EXISTS [appctrl_allow_rules] ("
        "  [id] INTEGER PRIMARY KEY AUTOINCREMENT, "
        "  [file_path] TEXT, "
        "  [pid] INT, "
        "  [add_time] INTEGER "
        ");"
        "";
}

__inline
PCHAR
SQL_STM_SEARCH_APPCTRL_DENY_RULE(
    VOID
)
{
    return 
        "SELECT "
        "  [add_time] "
        "FROM "
        "  [appctrl_deny_rules] "
        "WHERE "
        "  (([pid] IS NOT NULL) AND ([pid] = ?)) OR "
        "  (([file_path] IS NOT NULL) AND ([file_path] = ?)) "
        "ORDER BY"
        "  [add_time] DESC;"
        "";
}

__inline
PCHAR
SQL_STM_SEARCH_APPCTRL_ALLOW_RULE(
    VOID
)
{
    return
        "SELECT "
        "  [add_time] "
        "FROM "
        "  [appctrl_allow_rules] "
        "WHERE "
        "  (([pid] IS NOT NULL) AND ([pid] = ?)) OR "
        "  (([file_path] IS NOT NULL) AND ([file_path] = ?)) "
        "ORDER BY"
        "  [add_time] DESC;"
        "";
}

__inline
PCHAR
SQL_STM_INSERT_APPCTRL_DENY_RULE(
    VOID
)
{
    return
        "INSERT INTO [appctrl_deny_rules] "
        "   ([id], [file_path], [pid], [add_time]) "
        "VALUES "
        "   (?, ?, ?, ?);"
        "";
}

__inline
PCHAR
SQL_STM_INSERT_APPCTRL_ALLOW_RULE(
    VOID
)
{
    return
        "INSERT INTO [appctrl_allow_rules] "
        "   ([id], [file_path], [pid], [add_time]) "
        "VALUES "
        "   (?, ?, ?, ?);"
        "";
}

__inline
PCHAR
SQL_STM_DELETE_APPCTRL_DENY_RULE(
    VOID
)
{
    return
        "DELETE FROM "
        "  [appctrl_deny_rules] "
        "WHERE "
        "  [id] = ?"
        "";
}

__inline
PCHAR
SQL_STM_DELETE_APPCTRL_ALLOW_RULE(
    VOID
)
{
    return
        "DELETE FROM "
        "  [appctrl_allow_rules] "
        "WHERE "
        "  [id] = ?"
        "";
}

__inline
PCHAR
SQL_STM_UPDATE_APPCTRL_DENY_RULE(
    VOID
)
{
    return
        "UPDATE [appctrl_deny_rules] "
        "SET "
        "  [file_path] = ?, "
        "  [pid] = ? "
        "WHERE "
        "  [id] = ?"
        "";
}

__inline
PCHAR
SQL_STM_UPDATE_APPCTRL_ALLOW_RULE(
    VOID
)
{
    return
        "UPDATE [appctrl_allow_rules] "
        "SET "
        "  [file_path] = ?, "
        "  [pid] = ? "
        "WHERE "
        "  [id] = ?"
        "";
}

__inline
PCHAR
SQL_STM_GET_APPCTRL_DENY_RULES(
    VOID
)
{
    return "SELECT * FROM [appctrl_deny_rules];";
}

__inline
PCHAR
SQL_STM_GET_APPCTRL_ALLOW_RULES(
    VOID
)
{
    return "SELECT * FROM [appctrl_allow_rules];";
}

__inline
PCHAR
SQL_STM_GET_APPCTRL_DENY_RULES_COUNT(
    VOID
)
{
    return "SELECT COUNT(*) FROM [appctrl_deny_rules];";
}

__inline
PCHAR
SQL_STM_GET_APPCTRL_ALLOW_RULES_COUNT(
    VOID
)
{
    return "SELECT COUNT(*) FROM [appctrl_allow_rules];";
}
