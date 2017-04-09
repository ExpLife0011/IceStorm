@REM
@REM Runs the DefaultInstall section of iceflt.inf
@REM

@echo off

pushd %~dp0
rundll32.exe setupapi,InstallHinfSection DefaultInstall 132 .\iceflt.inf
popd

