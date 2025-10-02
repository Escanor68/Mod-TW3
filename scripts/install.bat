@echo off
echo ========================================
echo    Witcher3-MP Installation Script
echo ========================================
echo.

REM Check if running as administrator
net session >nul 2>&1
if %errorLevel% == 0 (
    echo Running as administrator... OK
) else (
    echo WARNING: Not running as administrator
    echo Some operations may fail without admin privileges
    echo.
)

REM Check for The Witcher 3 installation
echo Checking for The Witcher 3 installation...
set "TW3_PATH="
if exist "C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3" (
    set "TW3_PATH=C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3"
    echo Found TW3 in Steam (x86): %TW3_PATH%
) else if exist "C:\Program Files\Steam\steamapps\common\The Witcher 3" (
    set "TW3_PATH=C:\Program Files\Steam\steamapps\common\The Witcher 3"
    echo Found TW3 in Steam: %TW3_PATH%
) else if exist "C:\GOG Games\The Witcher 3 Wild Hunt" (
    set "TW3_PATH=C:\GOG Games\The Witcher 3 Wild Hunt"
    echo Found TW3 in GOG: %TW3_PATH%
) else (
    echo ERROR: The Witcher 3 not found!
    echo Please install The Witcher 3: Wild Hunt Next-Gen first
    pause
    exit /b 1
)

REM Check for mods directory
if not exist "%TW3_PATH%\mods" (
    echo Creating mods directory...
    mkdir "%TW3_PATH%\mods"
)

REM Create mod directory
set "MOD_PATH=%TW3_PATH%\mods\modWitcher3MP"
if exist "%MOD_PATH%" (
    echo Removing existing mod installation...
    rmdir /s /q "%MOD_PATH%"
)

echo Creating mod directory...
mkdir "%MOD_PATH%"

REM Copy mod files
echo Copying mod files...
xcopy /E /I /Y "mod\*" "%MOD_PATH%\"

REM Copy configuration
if not exist "%MOD_PATH%\config" (
    mkdir "%MOD_PATH%\config"
)
copy /Y "config\*.json" "%MOD_PATH%\config\"

echo.
echo ========================================
echo    Installation Complete!
echo ========================================
echo.
echo Mod installed to: %MOD_PATH%
echo.
echo Next steps:
echo 1. Start The Witcher 3: Wild Hunt
echo 2. Check that the mod loads without errors
echo 3. Configure settings in: %MOD_PATH%\config\mp_config.json
echo.
echo For troubleshooting, run: scripts\DIAGNOSE_PROBLEM.bat
echo.
pause
