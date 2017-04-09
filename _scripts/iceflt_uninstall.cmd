@REM
@REM Runs the DefaultUninstall section of hbflt
@REM

@echo off

pushd %~dp0
rundll32.exe setupapi,InstallHinfSection DefaultUninstall 132 .\iceflt.inf
popd

