@echo off
echo ========================================
echo RainbowSixESP - Build Script
echo ========================================
echo.

echo [1/4] Building Shared Library...
cd ..\Shared
msbuild Shared.vcxproj /p:Configuration=Release /p:Platform=x64
if errorlevel 1 goto error
echo.

echo [2/4] Building Driver...
cd ..\Driver
msbuild Driver.vcxproj /p:Configuration=Release /p:Platform=x64
if errorlevel 1 goto error
echo.

echo [3/4] Building Overlay...
cd ..\Overlay
msbuild Overlay.vcxproj /p:Configuration=Release /p:Platform=x64
if errorlevel 1 goto error
echo.

echo [4/4] Copying files to Deploy...
cd ..
mkdir Deploy 2>nul
copy Driver\x64\Release\driver.sys Deploy\
copy Overlay\x64\Release\Overlay.exe Deploy\
copy config.json Deploy\ 2>nul
echo.

echo ========================================
echo BUILD SUCCESSFUL!
echo ========================================
echo.
echo Files located in Deploy folder
echo.
pause
exit /b 0

:error
echo.
echo ========================================
echo BUILD FAILED!
echo ========================================
pause
exit /b 1
