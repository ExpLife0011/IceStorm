@echo off

pushd %~dp0
fltmc unload iceflt
rundll32.exe setupapi,InstallHinfSection DefaultUninstall 132 .\iceflt.inf
popd
