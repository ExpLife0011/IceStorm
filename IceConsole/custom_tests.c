#include "custom_tests.h"
#include <stdio.h>

#define READ_BUFFER_LENGTH       30

#define WRITE_BUFFER            "aaaa"
#define WRITE_BUFFER_LENGTH     sizeof(WRITE_BUFFER)

VOID
PrintTestsHelp(
    VOID
)
{
    wprintf(L"---- HELP ----\n");
    wprintf(L"IceConsole.exe <test>\n\n");
    wprintf(L"all <file_path> -- try all operations on <file_path>\n");
}


VOID
RunAll(
    _In_z_      PWCHAR          PFilePath
)
{
    HANDLE      hFile = INVALID_HANDLE_VALUE;
    DWORD       dwLastError = ERROR_SUCCESS;
    DWORD       dwBytes = 0;
    BYTE        pBuffer[READ_BUFFER_LENGTH + 1] = { 0 };


    hFile = CreateFileW(
        PFilePath,
        GENERIC_ALL,
        FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (hFile == INVALID_HANDLE_VALUE)
    {
        dwLastError = GetLastError();
        wprintf(L"CreateFile(%s) FAILED, error %d\n", PFilePath, dwLastError);
        return;
    }
    else
    {
        wprintf(L"CreateFile(%s) had SUCCESS\n", PFilePath);
    }

    if (!WriteFile(hFile, WRITE_BUFFER, WRITE_BUFFER_LENGTH, &dwBytes, NULL))
    {
        dwLastError = GetLastError();
        wprintf(L"WriteFile(%s) FAILED, error %d\n", PFilePath, dwLastError);
    }
    else
    {
        wprintf(L"WriteFile(%s) had SUCCESS\n", PFilePath);
    }

    if (!ReadFile(hFile, pBuffer, READ_BUFFER_LENGTH, &dwBytes, NULL))
    {
        dwLastError = GetLastError();
        wprintf(L"ReadFile(%s) FAILED, error %d\n", PFilePath, dwLastError);
    }
    else
    {
        wprintf(L"ReadFile(%s) had SUCCESS\n", PFilePath);
    }
    
    CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;

    if (!DeleteFile(PFilePath))
    {
        dwLastError = GetLastError();
        wprintf(L"DeleteFile(%s) FAILED, error %d\n", PFilePath, dwLastError);
    }
    else
    {
        wprintf(L"DeleteFile(%s) had SUCCESS\n", PFilePath);    
    }
}

VOID
RunAllDir(
    _In_z_      PWCHAR          PDirPath
)
{
    HANDLE              hFind               = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW    findData            = { 0 };
    DWORD               dwError             = ERROR_SUCCESS;
    WCHAR               pFilePath[MAX_PATH] = { 0 };
    WCHAR               pFindPath[MAX_PATH] = { 0 };

    swprintf_s(pFindPath, MAX_PATH, L"%s\\*", PDirPath);

    hFind = FindFirstFile(pFindPath, &findData);
    if (INVALID_HANDLE_VALUE == hFind)
    {
        dwError = GetLastError();
        wprintf(L"FindFirstFile(%s) FAILED, error: %d\n", PDirPath, dwError);
        return;
    }

    do 
    {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;

        swprintf_s(pFilePath, MAX_PATH, L"%s\\%s", PDirPath, findData.cFileName);
        wprintf(L"\n");
        RunAll(pFilePath);

    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);
    hFind = INVALID_HANDLE_VALUE;
}

_Use_decl_anno_impl_
VOID
RunCustomTestOperation(
    DWORD                       DwArgc,
    PWCHAR                     *PPArgv
)
{
    PWCHAR          pTest       = PPArgv[1];

    if (!_wcsicmp(pTest, L"all"))
    {
        if (DwArgc < 3)
        {
            PrintTestsHelp();
            return;
        }

        RunAll(PPArgv[2]);
    }


    if (!_wcsicmp(pTest, L"alldir"))
    {
        if (DwArgc < 3)
        {
            PrintTestsHelp();
            return;
        }

        RunAllDir(PPArgv[2]);
    }
}
