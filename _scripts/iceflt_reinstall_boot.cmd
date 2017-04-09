@echo off
fltmc unload hbflt
pushd %~dp0
rundll32.exe setupapi, InstallHinfSection DefaultUninstall 132 .\iceflt.inf
popd

pushd %~dp0
rundll32.exe setupapi, InstallHinfSection BootInstall 132 .\iceflt.inf
popd
