@echo off
setlocal
pushd "%~dp0.."

rem Register the engine installation...
echo Registering the engine installation...
if not exist .\Engine\Binaries\Win64\Shipping\VeiMManagerTool-Win64-Shipping.exe goto no_vmmt
.\Engine\Binaries\Win64\Shipping\VeiMManagerTool-Win64-Shipping.exe /register
:no_vmmt

rem Done!
goto :end

:error
pause

:end
popd

pause