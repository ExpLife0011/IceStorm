#include "db_statements.h"

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
