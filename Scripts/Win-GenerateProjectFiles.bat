@echo off

echo Building project files
@echo on

cd ..\Engine\
call .\Programs\premake\premake5.exe vs2022

@echo off
PAUSE