@REM
@REM Runs the DefaultInstall section of hbflt.inf
@REM

@echo off

pushd %~dp0

rundll32.exe setupapi,InstallHinfSection DefaultInstall 132 .\iceflt.inf

sc start iceflt

popd


