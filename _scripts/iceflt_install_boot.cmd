@echo off
pushd %~dp0
rundll32.exe setupapi, InstallHinfSection BootInstall 132 .\iceflt.inf
popd