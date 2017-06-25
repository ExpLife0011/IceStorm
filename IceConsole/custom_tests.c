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
    wprintf(L"alldir <dir_path> -- try all operations on <dir_path>\n");
    wprintf(L"ransomware <dir_path> -- encrypt recursive <dir_path>\n");
    wprintf(L"spyware <dir_path> <dest_path> -- copy recursive from <dir_path> to <dest_path>\n");
}


VOID
RunAll(
    _In_z_      PWCHAR          PFilePath
)
{
    HANDLE      hFile           = INVALID_HANDLE_VALUE;
    DWORD       dwLastError     = ERROR_SUCCESS;
    DWORD       dwBytes         = 0;
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


VOID
IceEncryptBuffer(
    _Inout_     PBYTE           PBuffer,
    _In_        DWORD           DwBufferSize,
    _In_        BYTE            Key
)
{
    DWORD idx = 0;

    for (idx = 0; idx < DwBufferSize; idx++)
    {
        PBuffer[idx] ^= Key;
    }
}


VOID
IceEncryptFile(
    _In_z_      PWCHAR          PFilePath
)
{
    HANDLE          hFile               = INVALID_HANDLE_VALUE;
    DWORD           dwLastError         = ERROR_SUCCESS;
    DWORD           dwBytes             = 0;
    PBYTE           pBuffer             = NULL;
    DWORD           dwBufferSize        = 0;
    LARGE_INTEGER   liFileSize          = { 0 };

    hFile = CreateFileW(PFilePath, GENERIC_ALL, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        dwLastError = GetLastError();
        wprintf(L"CreateFile(%s) FAILED, error %d\n", PFilePath, dwLastError);
        return;
    }
    
    if (!GetFileSizeEx(hFile, &liFileSize))
    {
        dwBufferSize = 51200;
    }
    else
    {
        dwBufferSize = (DWORD) liFileSize.LowPart;
    }

    pBuffer = (PBYTE) malloc(dwBufferSize);
    if (NULL == pBuffer)
    {
        wprintf_s(L"malloc FAILED\n");
        return;
    }
    ZeroMemory(pBuffer, dwBufferSize);

    if (!ReadFile(hFile, pBuffer, dwBufferSize, &dwBytes, NULL))
    {
        dwLastError = GetLastError();
        wprintf(L"ReadFile(%s) FAILED, error %d\n", PFilePath, dwLastError);
    }
    else
    {
        wprintf(L"ReadFile(%s) had SUCCESS\n", PFilePath);
        if (dwBufferSize > dwBytes) dwBufferSize = dwBytes;
    }
    
    if (dwBytes)
    {
        IceEncryptBuffer(pBuffer, dwBufferSize, 197);


        if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, 0, FILE_BEGIN))
        {
            dwLastError = GetLastError();
            wprintf(L"SetFilePointer(%s) FAILED, error %d\n", PFilePath, dwLastError);
        }

        if (!WriteFile(hFile, pBuffer, dwBufferSize, &dwBytes, NULL))
        {
            dwLastError = GetLastError();
            wprintf(L"WriteFile(%s) FAILED, error %d\n", PFilePath, dwLastError);
        }
        else
        {
            wprintf(L"WriteFile(%s) had SUCCESS\n", PFilePath);
            
            if (!SetEndOfFile(hFile))
            {
                dwLastError = GetLastError();
                wprintf(L"SetEndOfFile(%s) FAILED, error %d\n", PFilePath, dwLastError);
            }
        }

    }

    free(pBuffer);
    pBuffer = NULL;
    
    CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;
}

VOID
RunRansomware(
    _In_z_      PWCHAR          PDirPath
)
{
    HANDLE              hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW    findData = { 0 };
    DWORD               dwError = ERROR_SUCCESS;
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
        if (!_wcsicmp(findData.cFileName, L".") || !_wcsicmp(findData.cFileName, L"..")) continue;

        swprintf_s(pFilePath, MAX_PATH, L"%s\\%s", PDirPath, findData.cFileName);

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            RunRansomware(pFilePath);
            continue;
        }

        wprintf(L"\n");
        IceEncryptFile(pFilePath);

    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);
    hFind = INVALID_HANDLE_VALUE;
}

VOID
RunSpyware(
    _In_z_      PWCHAR          PSrcDirPath,
    _In_z_      PWCHAR          PDstDirPath
)
{
    HANDLE              hFind                   = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW    findData                = { 0 };
    DWORD               dwError                 = ERROR_SUCCESS;
    WCHAR               pSrcFilePath[MAX_PATH]  = { 0 };
    WCHAR               pDstFilePath[MAX_PATH]  = { 0 };
    WCHAR               pFindPath[MAX_PATH]     = { 0 };

    swprintf_s(pFindPath, MAX_PATH, L"%s\\*", PSrcDirPath);

    if (!CreateDirectoryW(PDstDirPath, NULL) && ERROR_ALREADY_EXISTS != GetLastError())
    {
        dwError = GetLastError();
        wprintf(L"CreateDirectoryW(%s) FAILED, error: %d", PDstDirPath, dwError);
    }

    hFind = FindFirstFile(pFindPath, &findData);
    if (INVALID_HANDLE_VALUE == hFind)
    {
        dwError = GetLastError();
        wprintf(L"FindFirstFile(%s) FAILED, error: %d\n", PSrcDirPath, dwError);
        return;
    }

    do
    {
        if (!_wcsicmp(findData.cFileName, L".") || !_wcsicmp(findData.cFileName, L"..")) continue;

        swprintf_s(pSrcFilePath, MAX_PATH, L"%s\\%s", PSrcDirPath, findData.cFileName);
        swprintf_s(pDstFilePath, MAX_PATH, L"%s\\%s", PDstDirPath, findData.cFileName);

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            RunSpyware(pSrcFilePath, pDstFilePath);
            continue;
        }

        if (!CopyFileW(pSrcFilePath, pDstFilePath, FALSE))
        {
            dwError = GetLastError();
            wprintf(L"CopyFileW(%s -> %s) FAILED, error: %d\n", pSrcFilePath, pDstFilePath, dwError);
        }
        else
        {
            wprintf(L"CopyFileW(%s -> %s) had SUCCESS\n", pSrcFilePath, pDstFilePath);
        }
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

    if (!_wcsicmp(pTest, L"ransomware"))
    {
        if (DwArgc < 3)
        {
            PrintTestsHelp();
            return;
        }

        RunRansomware(PPArgv[2]);
    }

    if (!_wcsicmp(pTest, L"spyware"))
    {
        if (DwArgc < 4)
        {
            PrintTestsHelp();
            return;
        }

        RunSpyware(PPArgv[2], PPArgv[3]);
    }
}
