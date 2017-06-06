#define WIN32_LEAN_AND_MEAN

#include "debug2.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "client.h"
#include "client_helpers.h"
#include "global_data.h"

#undef WIN32_LEAN_AND_MEAN


#define HANDSHAKE       "168494987!#())-=+_IceStorm153_))((--85*"
#define HANDSHAKE_SIZE  ((DWORD) 40)

SOCKET gServerSocket        = INVALID_SOCKET;

BOOLEAN
SendHandshakePackage(
    VOID
)
{
    BOOLEAN     bResult             = FALSE;
    DWORD       dwResult            = ERROR_SUCCESS;
    DWORD       dwHandShakeResult   = 0;

    __try
    {
        dwResult = ClSendMessage((PBYTE) HANDSHAKE, HANDSHAKE_SIZE);
        if (ERROR_SUCCESS != dwResult)
        {
            LogErrorWin(dwResult, L"Failed to send HANDSHAKE packege");
            __leave;
        }

        dwResult = ClRecvDWORD(&dwHandShakeResult);
        if (ERROR_SUCCESS != dwResult)
        {
            LogErrorWin(dwResult, L"ClRecvMessageWithoutSize");
            __leave;
        }

        if (dwHandShakeResult == 0)
        {
            LogError(L"Handshake failed");
            __leave;
        }

        BYTE x[200];
        dwResult = ClRecvMessage(x, 200);
        if (0 != dwResult)
        {
            LogError(L"xxxxxxxxxxxxxxxxxxxxxxx: %d", dwResult);
            __leave;
        }

        LogInfo(L"Handshake had success");
        bResult = TRUE;
    }
    __finally
    {

    }

    return bResult;
}

_Use_decl_anno_impl_
BOOLEAN
StartClient(
    PCHAR       PIpAddr,
    PCHAR       PPort
)
{
    WSADATA             wsaData         = { 0 };
    struct addrinfo    *pResult         = NULL;
    struct addrinfo    *ptr             = NULL;
    struct addrinfo     hints           = { 0 };
    DWORD               dwResult        = ERROR_SUCCESS;
    BOOLEAN             bResult         = FALSE;
    ULONG               ulMode          = 1;

    LogInfo(L"Trying to connect to sever...");

    __try
    {
        dwResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (dwResult != ERROR_SUCCESS)
        {
            LogErrorWin(dwResult, L"WSAStartup");
            __leave;
        }

        hints.ai_family     = AF_UNSPEC;
        hints.ai_socktype   = SOCK_STREAM;
        hints.ai_protocol   = IPPROTO_TCP;

        dwResult = getaddrinfo(PIpAddr, PPort, &hints, &pResult);
        if (dwResult != 0 || NULL == pResult)
        {
            LogErrorWin(dwResult, L"getaddrinfo(%S, %S)", PIpAddr, PPort);
            __leave;
        }

        for (ptr = pResult; ptr != NULL; ptr = ptr->ai_next)
        {

            gServerSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
            if (gServerSocket == INVALID_SOCKET)
            {
                LogWarningWin(WSAGetLastError(), L"socket(%S)", ptr->ai_canonname);
                continue;
            }

            dwResult = connect(gServerSocket, ptr->ai_addr, (int) ptr->ai_addrlen);
            if (dwResult == SOCKET_ERROR)
            {
                LogWarningWin(WSAGetLastError(), L"connect(%S)", ptr->ai_canonname);
                closesocket(gServerSocket);
                gServerSocket = INVALID_SOCKET;
                continue;
            }

            break;
        }

        if (gServerSocket == INVALID_SOCKET)
        {
            LogError(L"Failed to connect to server.");
            __leave;
        }

        dwResult = ioctlsocket(gServerSocket, FIONBIO, &ulMode);
        if (dwResult != NO_ERROR)
        {
            LogErrorWin(dwResult, L"ioctlsocket(FIONBIO, 1)");
            __leave;
        }

        if (!SendHandshakePackage())
        {
            LogError(L"SendHandshakePackage");
            __leave;
        }

        LogInfo(L"Connected to server with success.");
        bResult = TRUE;
    }
    __finally
    {
        if (NULL != pResult)
        {
            freeaddrinfo(pResult);
            pResult = NULL;
        }

        if (!bResult)
        {
            StopClient();
        }
    }

    return bResult;
}

VOID
StopClient(
    VOID
)
{
    LogInfo(L"Stopping client...");

    if (INVALID_SOCKET != gServerSocket)
    {
        shutdown(gServerSocket, SD_BOTH);

        closesocket(gServerSocket);
        gServerSocket = INVALID_SOCKET;
    }

    if (0 != WSACleanup())
    {
        LogWarningWin(WSAGetLastError(), L"WSACleanup");
    }
}