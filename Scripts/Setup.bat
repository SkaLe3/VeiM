@echo off
setlocal
pushd "%~dp0.."

rem Register the engine installation...
echo Registering the engine installation...
if not exist .\Engine\Binaries\Win64\Shipping\VeiMManagerTool-Win64-Shipping.exe goto no_vmmt
.\Engine\Binaries\Win64\Shipping\VeiMManagerTool-Win64-Shipping.exe /register
echo Engine installation registered!
goto :end
:no_vmmt
echo VeiMManagerTool-Win64-Shipping.exe doesn't exist!
rem Done!
goto :end

:error
pause

:end
popd

pause