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
        "CREATE TABLE IF NOT EXISTS [appctrl_rules] ("
        "  [id] INTEGER PRIMARY KEY AUTOINCREMENT, "
        "  [matcher_process_path] INT NOT NULL DEFAULT 0, "
        "  [process_path] TEXT, "
        "  [pid] INT, "
        "  [matcher_parent_path] INT NOT NULL DEFAULT 0, "
        "  [parent_path] TEXT, "
        "  [parent_pid] INT, "
        "  [verdict] INT, "
        "  [add_time] INTEGER "
        "); "
        ""
        ""
        // FsScan Rules
        "CREATE TABLE IF NOT EXISTS [fsscan_rules] ("
        "  [id] INTEGER PRIMARY KEY AUTOINCREMENT, "
        "  [matcher_process_path] INT NOT NULL DEFAULT 0, "
        "  [process_path] TEXT, "
        "  [pid] INT, "
        "  [matcher_file_path] INT NOT NULL DEFAULT 0, "
        "  [file_path] TEXT, "
        "  [denied_operations] INT, "
        "  [add_time] INTEGER "
        "); "
        ""
        ""
        // AppCtrl Events
        "CREATE TABLE IF NOT EXISTS [appctrl_events] ("
        "  [id] INTEGER PRIMARY KEY AUTOINCREMENT, "
        "  [process_path] TEXT NOT NULL, "
        "  [pid] INT NOT NULL, "
        "  [parent_path] TEXT NOT NULL, "
        "  [parent_pid] INT NOT NULL, "
        "  [verdict] INT NOT NULL, "
        "  [matched_rule_id] INT NOT NULL, "
        "  [event_time] INTEGER NOT NULL"
        "); "
        ""
        ""
        // FS Event
        "CREATE TABLE IF NOT EXISTS [fs_events] ("
        "  [id] INTEGER PRIMARY KEY AUTOINCREMENT, "
        "  [process_path] TEXT NOT NULL, "
        "  [pid] INT NOT NULL, "
        "  [file_path] TEXT NOT NULL, "
        "  [required_operations] INT NOT NULL, "
        "  [denied_operations] INT NOT NULL, "
        "  [remaining_operations] INT NOT NULL, "
        "  [matched_rule_id] INT NOT NULL, "
        "  [event_time] INTEGER NOT NULL "
        ""
        "); "
        ""
        ""
        // Settings
        "CREATE TABLE IF NOT EXISTS [ice_setting] ("
        "  [appctrl_status] INTEGER NOT NULL, "
        "  [fsscan_status] INTEGER NOT NULL "
        ""
        "); "
        ""
        ""
        "";
}

__inline
PCHAR
SQL_STM_SEARCH_APPCTRL_RULE(
    VOID
)
{
    return
        "SELECT "
        "  [id], [verdict] "
        "FROM "
        "  [appctrl_rules] "
        "WHERE "
        "  (([process_path] IS NULL) OR "
        "     ((([matcher_process_path] = 0) AND ([process_path] = ? COLLATE NOCASE)) OR "
        "     (([matcher_process_path] = 1) AND (? LIKE [process_path] COLLATE NOCASE)))) AND "
        "  (([pid] IS NULL) OR ([pid] = ?)) AND "
        "  (([parent_path] IS NULL) OR "
        "     ((([matcher_parent_path] = 0) AND ([parent_path] = ? COLLATE NOCASE)) OR "
        "     (([matcher_parent_path] = 1) AND (? LIKE [parent_path] COLLATE NOCASE)))) AND "
        "  (([parent_pid] IS NULL) OR ([parent_pid] = ?)) "
        "ORDER BY "
        "  [add_time] DESC "
        "LIMIT 1;"
        "";
}

__inline
PCHAR
SQL_STM_INSERT_APPCTRL_RULE(
    VOID
)
{
    return
        "INSERT INTO [appctrl_rules] "
        "   ([id], [matcher_process_path], [process_path], [pid], [matcher_parent_path], [parent_path], [parent_pid], [verdict], [add_time]) "
        "VALUES "
        "   (?, ?, ?, ?, ?, ?, ?, ?, ?);"
        "";
}

__inline
PCHAR
SQL_STM_DELETE_APPCTRL_RULE(
    VOID
)
{
    return
        "DELETE FROM "
        "  [appctrl_rules] "
        "WHERE "
        "  [id] = ?"
        "";
}

__inline
PCHAR
SQL_STM_UPDATE_APPCTRL_RULE(
    VOID
)
{
    return
        "UPDATE [appctrl_rules] "
        "SET "
        "  [matcher_process_path] = ?, "
        "  [process_path] = ?, "
        "  [pid] = ?, "
        "  [matcher_parent_path] = ?, "
        "  [parent_path] = ?, "
        "  [parent_pid] = ?, "
        "  [verdict] = ? "
        "WHERE "
        "  [id] = ?"
        "";
}

__inline
PCHAR
SQL_STM_GET_APPCTRL_RULES(
    VOID
)
{
    return "SELECT * FROM [appctrl_rules];";
}

__inline
PCHAR
SQL_STM_GET_APPCTRL_RULES_COUNT(
    VOID
)
{
    return "SELECT COUNT(*) FROM [appctrl_rules];";
}

__inline
PCHAR
SQL_STM_SEARCH_FSSCAN_RULE(
    VOID
)
{
    return
        "SELECT "
        "  [id], [denied_operations] "
        "FROM "
        "  [fsscan_rules] "
        "WHERE "
        "  (([process_path] IS NULL) OR "
        "     ((([matcher_process_path] = 0) AND ([process_path] = ? COLLATE NOCASE)) OR "
        "     (([matcher_process_path] = 1) AND (? LIKE [process_path] COLLATE NOCASE)))) AND "
        "  (([pid] IS NULL) OR ([pid] = ?)) AND "
        "  (([file_path] IS NULL) OR "
        "     ((([matcher_file_path] = 0) AND ([file_path] = ? COLLATE NOCASE)) OR "
        "     (([matcher_file_path] = 1) AND (? LIKE [file_path] COLLATE NOCASE)))) "
        "ORDER BY "
        "  [add_time] DESC "
        "LIMIT 1;"
        "";
}

__inline
PCHAR
SQL_STM_INSERT_FSSCAN_RULE(
    VOID
)
{
    return
        "INSERT INTO [fsscan_rules] "
        "   ([id], [matcher_process_path], [process_path], [pid], [matcher_file_path], [file_path], [denied_operations], [add_time]) "
        "VALUES "
        "   (?, ?, ?, ?, ?, ?, ?, ?);"
        "";
}

__inline
PCHAR
SQL_STM_DELETE_FSSCAN_RULE(
    VOID
)
{
    return
        "DELETE FROM "
        "  [fsscan_rules] "
        "WHERE "
        "  [id] = ?"
        "";
}

__inline
PCHAR
SQL_STM_UPDATE_FSSCAN_RULE(
    VOID
)
{
    return
        "UPDATE [fsscan_rules] "
        "SET "
        "  [matcher_process_path] = ?, "
        "  [process_path] = ?, "
        "  [pid] = ?, "
        "  [matcher_file_path] = ?, "
        "  [file_path] = ?, "
        "  [denied_operations] = ? "
        "WHERE "
        "  [id] = ?"
        "";
}

__inline
PCHAR
SQL_STM_GET_FSSCAN_RULES(
    VOID
)
{
    return "SELECT * FROM [fsscan_rules];";
}

__inline
PCHAR
SQL_STM_GET_FSSCAN_RULES_COUNT(
    VOID
)
{
    return "SELECT COUNT(*) FROM [fsscan_rules];";
}

__inline
PCHAR
SQL_STM_INSERT_APPCTRL_EVENT(
    VOID
)
{
    return
        "INSERT INTO [appctrl_events] "
        "   ([id], [process_path], [pid], [parent_path], [parent_pid], [verdict], [matched_rule_id], [event_time]) "
        "VALUES "
        "   (?, ?, ?, ?, ?, ?, ?, ?);"
        "";
}

__inline
PCHAR
SQL_STM_INSERT_FS_EVENT(
    VOID
)
{
    return
        "INSERT INTO [fs_events] "
        "   ([id], [process_path], [pid], [file_path], [required_operations], [denied_operations], [remaining_operations], [matched_rule_id], [event_time]) "
        "VALUES "
        "   (?, ?, ?, ?, ?, ?, ?, ?, ?);"
        "";
}

__inline
PCHAR
SQL_STM_GET_APPCTRL_EVENTS(
    VOID
)
{
    return "SELECT * FROM [appctrl_events] WHERE [id] > ? ORDER BY [id] DESC";

}

__inline
PCHAR
SQL_STM_GET_APPCTRL_EVENTS_COUNT(
    VOID
)
{
    return "SELECT COUNT(*) FROM [appctrl_events] WHERE [id] > ? ORDER BY [id] DESC";
}

__inline
PCHAR
SQL_STM_GET_FS_EVENTS(
    VOID
)
{
    return "SELECT * FROM [fs_events] WHERE [id] > ? ORDER BY [id] DESC";
}

__inline
PCHAR
SQL_STM_GET_FS_EVENTS_COUNT(
    VOID
)
{
    return "SELECT COUNT(*) FROM [fs_events] WHERE [id] > ? ORDER BY [id] DESC";
}

__inline
PCHAR
SQL_STM_GET_SETTINGS_COUNT(
    VOID
)
{
    return "SELECT COUNT(*) FROM [ice_setting]";
}

__inline
PCHAR
SQL_STM_INSERT_SETTINGS_DEFAULT_ROW(
    VOID
)
{
    return "INSERT INTO [ice_setting] ([appctrl_status], [fsscan_status]) VALUES (0, 0)";
}

__inline
PCHAR
SQL_STM_UPDATE_SETTINGS_APPCTRL(
    VOID
)
{
    return "UPDATE [ice_setting] SET [appctrl_status] = ?";
}

__inline
PCHAR
SQL_STM_UPDATE_SETTINGS_FSSCAN(
    VOID
)
{
    return "UPDATE [ice_setting] SET [fsscan_status] = ?";
}

__inline
PCHAR
SQL_STM_GET_SETTINGS(
    VOID
)
{
    return "SELECT * FROM [ice_setting]";
}
