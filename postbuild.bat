@echo off

set workingDir=%1
set platform=%2
set config=%3

set srcPath1=%workingDir%\bin\%platform%\%config%\*.*
set srcPath2=%workingDir%\Contrib\SQLite\%platform%\*.dll
set dstPath=d:\Licenta\Shares\W10x86\temp_share

echo Will copy %srcPath1% to %dstPath%

xcopy  %srcPath1% %dstPath% /y /e /s
if %errorlevel% NEQ 0 (
    exit %errorlevel%
)

echo Will copy %srcPath2% to %dstPath%
xcopy  %srcPath2% %dstPath% /y /e /s
if %errorlevel% NEQ 0 (
    exit %errorlevel%
)

echo GATAAAA!!!!
exit 0