#include "linked_list.h"


_Use_decl_anno_impl_
VOID
InitializeListHead(
    PLIST_ENTRY                             ListHead
    )
{
    ListHead->Flink = ListHead->Blink = ListHead;
}

_Use_decl_anno_impl_
BOOLEAN
IsListEmpty(
    const LIST_ENTRY                       *ListHead
    )
{
    return (BOOLEAN) (ListHead->Flink == ListHead);
}

_Use_decl_anno_impl_
BOOLEAN
RemoveEntryList(
    PLIST_ENTRY                             Entry
    )
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Flink;

    Flink = Entry->Flink;
    Blink = Entry->Blink;
    Blink->Flink = Flink;
    Flink->Blink = Blink;
    return (BOOLEAN) (Flink == Blink);
}

_Use_decl_anno_impl_
PLIST_ENTRY
RemoveHeadList(
    PLIST_ENTRY                             ListHead
    )
{
    PLIST_ENTRY Flink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Flink;
    Flink = Entry->Flink;
    ListHead->Flink = Flink;
    Flink->Blink = ListHead;
    return Entry;
}

_Use_decl_anno_impl_
PLIST_ENTRY
RemoveTailList(
    PLIST_ENTRY                             ListHead
    )
{
    PLIST_ENTRY Blink;
    PLIST_ENTRY Entry;

    Entry = ListHead->Blink;
    Blink = Entry->Blink;
    ListHead->Blink = Blink;
    Blink->Flink = ListHead;
    return Entry;
}

_Use_decl_anno_impl_
VOID
InsertTailList(
    PLIST_ENTRY                             ListHead,
    PLIST_ENTRY                             Entry
    )
{
    PLIST_ENTRY Blink;

    Blink = ListHead->Blink;
    Entry->Flink = ListHead;
    Entry->Blink = Blink;
    Blink->Flink = Entry;
    ListHead->Blink = Entry;
}

_Use_decl_anno_impl_
VOID
InsertHeadList(
    PLIST_ENTRY                             ListHead,
    PLIST_ENTRY                             Entry
)
{
    PLIST_ENTRY Flink;

    Flink = ListHead->Flink;
    Entry->Flink = Flink;
    Entry->Blink = ListHead;
    Flink->Blink = Entry;
    ListHead->Flink = Entry;
    return;
}

_Use_decl_anno_impl_
VOID
AppendTailList(
    PLIST_ENTRY                             ListHead,
    PLIST_ENTRY                             ListToAppend
)
{
    PLIST_ENTRY ListEnd = ListHead->Blink;

    ListHead->Blink->Flink = ListToAppend;
    ListHead->Blink = ListToAppend->Blink;
    ListToAppend->Blink->Flink = ListHead;
    ListToAppend->Blink = ListEnd;
    return;
}
