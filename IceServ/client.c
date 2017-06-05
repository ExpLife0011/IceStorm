#define WIN32_LEAN_AND_MEAN

#include "debug2.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "client.h"

#undef WIN32_LEAN_AND_MEAN


SOCKET gServerSocket = INVALID_SOCKET;

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
                LogWarningNt(WSAGetLastError(), L"socket(%S)", ptr->ai_canonname);
                continue;
            }

            dwResult = connect(gServerSocket, ptr->ai_addr, (int) ptr->ai_addrlen);
            if (dwResult == SOCKET_ERROR)
            {
                LogWarningNt(WSAGetLastError(), L"connect(%S)", ptr->ai_canonname);
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
        closesocket(gServerSocket);
        gServerSocket = INVALID_SOCKET;
    }

    if (0 != WSACleanup())
    {
        LogWarningWin(WSAGetLastError(), L"WSACleanup");
    }
}