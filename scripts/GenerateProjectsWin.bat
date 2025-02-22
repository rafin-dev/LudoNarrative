@echo off
pushd ..\
call premake5\bin\premake5.exe vs2022
popd
PAUSE