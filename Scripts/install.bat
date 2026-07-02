@echo off
echo ========================================
echo RainbowSixESP - Driver Installer
echo ========================================
echo.
echo Running as Administrator required!
echo.

:: Check for admin
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Not running as Administrator!
    echo Please right-click and select "Run as administrator"
    pause
    exit /b 1
)

echo [1/3] Copying driver files...
copy Deploy\driver.sys C:\Windows\System32\drivers\
if errorlevel 1 goto error

echo [2/3] Creating service...
sc create RainbowSixESP type= kernel binPath= C:\Windows\System32\drivers\driver.sys
if errorlevel 1 goto error

echo [3/3] Starting service...
sc start RainbowSixESP
if errorlevel 1 goto error

echo.
echo ========================================
echo DRIVER INSTALLED SUCCESSFULLY!
echo ========================================
echo.
pause
exit /b 0

:error
echo.
echo ========================================
echo INSTALLATION FAILED!
echo ========================================
echo.
pause
exit /b 1
