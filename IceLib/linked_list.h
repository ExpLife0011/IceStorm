#pragma once
#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS

VOID
InitializeListHead(
    _Out_       PLIST_ENTRY                 ListHead
);

BOOLEAN
IsListEmpty(
    _In_        const LIST_ENTRY           *ListHead
);

BOOLEAN
RemoveEntryList(
    _In_        PLIST_ENTRY                 Entry
);

PLIST_ENTRY
RemoveHeadList(
    _Inout_     PLIST_ENTRY                 ListHead
);

PLIST_ENTRY
RemoveTailList(
    _Inout_     PLIST_ENTRY                 ListHead
);

VOID
InsertTailList(
    _Inout_     PLIST_ENTRY                 ListHead,
    _Inout_ __drv_aliasesMem PLIST_ENTRY    Entry
);

VOID
InsertHeadList(
    _Inout_ PLIST_ENTRY ListHead,
    _Inout_ __drv_aliasesMem PLIST_ENTRY Entry
);

VOID
AppendTailList(
    _Inout_ PLIST_ENTRY ListHead,
    _Inout_ PLIST_ENTRY ListToAppend
);

//#define CONTAINING_RECORD(address, type, field) \
//    ((type *)( \
//    (PCHAR)(address) - \
//    (ULONG_PTR)(&((type *)0)->field)))

//#define FIELD_OFFSET(type, field)    ((LONG)(LONG_PTR)&(((type *)0)->field))

