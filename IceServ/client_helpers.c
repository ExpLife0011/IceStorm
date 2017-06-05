#define WIN32_LEAN_AND_MEAN

#include "debug2.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "client_helpers.h"
#include "global_data.h"

#undef WIN32_LEAN_AND_MEAN

_Success_(return == ERROR_SUCCESS)
DWORD
ClSendAuxBuffer(
    _In_        PBYTE           PBuffer,
    _In_        DWORD           DwBufferSize
)
{
    INT32       iResult         = 0;
    INT32       iMessageLen     = 0;
    PCHAR       pAuxBuffer      = 0;
    
    iMessageLen = DwBufferSize;
    pAuxBuffer = (PCHAR) PBuffer;

    while (TRUE)
    {
        iResult = send(gServerSocket, pAuxBuffer, iMessageLen, 0);
        if (iResult == SOCKET_ERROR)
        {
            DWORD dwResult = WSAGetLastError();
            LogErrorWin(dwResult, L"send");
            return dwResult;
        }
        LogInfo(L"Sent: %d / %d bytes", iResult, iMessageLen);
        if (iResult == iMessageLen) break;

        iMessageLen -= iResult;
        pAuxBuffer += iResult;
    }

    return ERROR_SUCCESS;
}

_Use_decl_anno_impl_
DWORD
ClSendMessage(
    PBYTE           PBuffer,
    DWORD           DwBufferSize
)
{
    DWORD dwResult = ERROR_SUCCESS;

    if (ERROR_SUCCESS != (dwResult = ClSendAuxBuffer((PBYTE) &DwBufferSize, (DWORD) sizeof(DWORD)))) return dwResult;
    LogInfo(L"Buffer: %S", (PCHAR) PBuffer);
    if (ERROR_SUCCESS != (dwResult = ClSendAuxBuffer(PBuffer, DwBufferSize))) return dwResult;

    return ERROR_SUCCESS;
}


_Success_(return == ERROR_SUCCESS)
DWORD
ClRecvAuxBuffer(
    _Inout_bytecap_(DwBufferSize)   PBYTE           PBuffer,
    _In_                            DWORD           DwBufferSize,
    _In_                            DWORD           DwFlags
)
{
    INT32       iResult     = 0;
    INT32       iMessageLen = 0;
    INT32       iError      = 0;
    PCHAR       pAuxBuffer  = 0;
    DWORD       dwResult    = ERROR_SUCCESS;

    iMessageLen = DwBufferSize;
    pAuxBuffer = (PCHAR) PBuffer;

    while (TRUE)
    {
        iResult = recv(gServerSocket, pAuxBuffer, iMessageLen, DwFlags);
        if (iResult == SOCKET_ERROR)
        {
            iError = WSAGetLastError();
            if (WSAEWOULDBLOCK == iError)
            {
                if (WAIT_OBJECT_0 == WaitForSingleObject(gHStopEvent, 50))
                {
                    dwResult = ERROR_CANCELLED;
                    break;
                }
                continue;
            }
            else
            {
                dwResult = iError;
                LogErrorWin(dwResult, L"recv");
                break;
            }
        }

        LogInfo(L"Received: %d / %d bytes", iResult, iMessageLen);

        if (iResult == iMessageLen) break;

        iMessageLen -= iResult;
        pAuxBuffer += iResult;
    }

    return dwResult;
}

_Use_decl_anno_impl_
DWORD
ClRecvMessage(
    PBYTE           PBuffer,
    DWORD           DwBufferSize
)
{
    PBuffer,
        DwBufferSize;
    return 0;
}

_Use_decl_anno_impl_
DWORD
ClGetNextMessageSize(
    VOID
)
{
    //DWORD dwMessageSize = 0;

    //recv(gServerSocket, &dwMessageSize, sizeof(DWORD), MSG_PEEK);
    return 0;
}

_Use_decl_anno_impl_
DWORD
ClRecvMessageWithoutSize(
    PBYTE           PBuffer,
    DWORD           DwBufferSize
)
{
    return ClRecvAuxBuffer(PBuffer, DwBufferSize, 0);
}

_Use_decl_anno_impl_
DWORD
ClRecvDWORD(
    DWORD           *PDwValue
)
{
    return ClRecvMessageWithoutSize((PBYTE) PDwValue, sizeof(DWORD));
}
