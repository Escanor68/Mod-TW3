@echo off
setlocal enabledelayedexpansion

REM Create logs directory
if not exist "logs" mkdir "logs"

REM Set log file with timestamp
for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "YY=%dt:~2,2%" & set "YYYY=%dt:~0,4%" & set "MM=%dt:~4,2%" & set "DD=%dt:~6,2%"
set "HH=%dt:~8,2%" & set "Min=%dt:~10,2%" & set "Sec=%dt:~12,2%"
set "timestamp=%YYYY%-%MM%-%DD%_%HH%-%Min%-%Sec%"

set "LOG_FILE=logs\installation_%timestamp%.log"
set "REPORT_FILE=logs\installation_report_%timestamp%.txt"

echo ======================================== > "%LOG_FILE%"
echo Witcher3-MP Installation Script >> "%LOG_FILE%"
echo Started: %date% %time% >> "%LOG_FILE%"
echo ======================================== >> "%LOG_FILE%"
echo. >> "%LOG_FILE%"

echo ========================================
echo Witcher3-MP Installation Script
echo ========================================
echo.

echo This script will prepare Witcher3-MP for game testing...
echo Log file: %LOG_FILE%
echo Report file: %REPORT_FILE%
echo.

REM Check if we're in the right directory
echo [%time%] Checking project directory structure... >> "%LOG_FILE%"
if not exist "mod\scripts\game\witcher3_mp" (
    echo [%time%] ERROR: mod\scripts\game\witcher3_mp directory not found >> "%LOG_FILE%"
    echo ❌ Error: Please run this script from the Witcher3-MP-Clean directory
    echo [%time%] Installation failed - wrong directory >> "%LOG_FILE%"
    pause
    exit /b 1
)

echo [%time%] SUCCESS: Found Witcher3-MP project directory >> "%LOG_FILE%"
echo ✅ Found Witcher3-MP project directory
echo.

REM Create mod directory structure
echo [%time%] Creating mod directory structure... >> "%LOG_FILE%"
echo Creating mod directory structure...

set "MOD_DIR=..\..\modWitcher3MP"
set "FILES_COPIED=0"
set "ERRORS=0"

if not exist "%MOD_DIR%" (
    echo [%time%] Creating main mod directory: %MOD_DIR% >> "%LOG_FILE%"
    mkdir "%MOD_DIR%"
    if !ERRORLEVEL! neq 0 (
        echo [%time%] ERROR: Failed to create main mod directory >> "%LOG_FILE%"
        set /a ERRORS+=1
    )
)

if not exist "%MOD_DIR%\scripts" (
    echo [%time%] Creating scripts directory >> "%LOG_FILE%"
    mkdir "%MOD_DIR%\scripts"
    if !ERRORLEVEL! neq 0 (
        echo [%time%] ERROR: Failed to create scripts directory >> "%LOG_FILE%"
        set /a ERRORS+=1
    )
)

if not exist "%MOD_DIR%\scripts\game" (
    echo [%time%] Creating game directory >> "%LOG_FILE%"
    mkdir "%MOD_DIR%\scripts\game"
    if !ERRORLEVEL! neq 0 (
        echo [%time%] ERROR: Failed to create game directory >> "%LOG_FILE%"
        set /a ERRORS+=1
    )
)

if not exist "%MOD_DIR%\scripts\game\witcher3_mp" (
    echo [%time%] Creating witcher3_mp directory >> "%LOG_FILE%"
    mkdir "%MOD_DIR%\scripts\game\witcher3_mp"
    if !ERRORLEVEL! neq 0 (
        echo [%time%] ERROR: Failed to create witcher3_mp directory >> "%LOG_FILE%"
        set /a ERRORS+=1
    )
)

if not exist "%MOD_DIR%\content" (
    echo [%time%] Creating content directory >> "%LOG_FILE%"
    mkdir "%MOD_DIR%\content"
    if !ERRORLEVEL! neq 0 (
        echo [%time%] ERROR: Failed to create content directory >> "%LOG_FILE%"
        set /a ERRORS+=1
    )
)

if not exist "%MOD_DIR%\config" (
    echo [%time%] Creating config directory >> "%LOG_FILE%"
    mkdir "%MOD_DIR%\config"
    if !ERRORLEVEL! neq 0 (
        echo [%time%] ERROR: Failed to create config directory >> "%LOG_FILE%"
        set /a ERRORS+=1
    )
)

if %ERRORS% equ 0 (
    echo [%time%] SUCCESS: Mod directory structure created >> "%LOG_FILE%"
    echo ✅ Mod directory structure created
) else (
    echo [%time%] WARNING: %ERRORS% errors occurred during directory creation >> "%LOG_FILE%"
    echo ⚠️ Mod directory structure created with %ERRORS% errors
)
echo.

REM Copy WitcherScript files
echo [%time%] Copying WitcherScript files... >> "%LOG_FILE%"
echo Copying WitcherScript files...

REM Count source files
set "WS_COUNT=0"
for %%f in ("mod\scripts\game\witcher3_mp\*.ws") do set /a WS_COUNT+=1
echo [%time%] Found %WS_COUNT% WitcherScript files to copy >> "%LOG_FILE%"

xcopy "mod\scripts\game\witcher3_mp\*.ws" "%MOD_DIR%\scripts\game\witcher3_mp\" /Y /Q
if !ERRORLEVEL! neq 0 (
    echo [%time%] ERROR: Failed to copy WitcherScript files >> "%LOG_FILE%"
    echo ❌ Error copying WitcherScript files
    set /a ERRORS+=1
) else (
    echo [%time%] SUCCESS: WitcherScript files copied >> "%LOG_FILE%"
    echo ✅ WitcherScript files copied
    set /a FILES_COPIED+=%WS_COUNT%
)

REM List copied files
echo [%time%] Copied WitcherScript files: >> "%LOG_FILE%"
for %%f in ("%MOD_DIR%\scripts\game\witcher3_mp\*.ws") do (
    echo [%time%]   - %%~nxf >> "%LOG_FILE%"
)
echo.

REM Copy configuration files
echo Copying configuration files...
xcopy "config\*.json" "..\..\modWitcher3MP\config\" /Y /Q
if %ERRORLEVEL% neq 0 (
    echo ❌ Error copying configuration files
    pause
    exit /b 1
)

echo ✅ Configuration files copied
echo.

REM Create modInfo.ws
echo Creating modInfo.ws...
echo // Witcher3-MP Mod Info > "..\..\modWitcher3MP\modInfo.ws"
echo // Generated by installation script >> "..\..\modWitcher3MP\modInfo.ws"
echo. >> "..\..\modWitcher3MP\modInfo.ws"
echo modWitcher3MP >> "..\..\modWitcher3MP\modInfo.ws"
echo { >> "..\..\modWitcher3MP\modInfo.ws"
echo     version = "1.0.0"; >> "..\..\modWitcher3MP\modInfo.ws"
echo     name = "Witcher3-MP Multiplayer Mod"; >> "..\..\modWitcher3MP\modInfo.ws"
echo     description = "Multiplayer combat system for The Witcher 3 Next-Gen"; >> "..\..\modWitcher3MP\modInfo.ws"
echo     author = "Witcher3-MP Team"; >> "..\..\modWitcher3MP\modInfo.ws"
echo } >> "..\..\modWitcher3MP\modInfo.ws"

echo ✅ modInfo.ws created
echo.

REM Create installation instructions
echo Creating installation instructions...
echo # Witcher3-MP Installation Instructions > "..\..\modWitcher3MP\INSTALLATION.txt"
echo. >> "..\..\modWitcher3MP\INSTALLATION.txt"
echo 1. Copy the modWitcher3MP folder to your The Witcher 3 mods directory >> "..\..\modWitcher3MP\INSTALLATION.txt"
echo 2. Ensure you have the base mod "Fix sounds of sword hits when adding effects" installed >> "..\..\modWitcher3MP\INSTALLATION.txt"
echo 3. Start The Witcher 3 and verify the mod loads without errors >> "..\..\modWitcher3MP\INSTALLATION.txt"
echo 4. Check the game logs for any error messages >> "..\..\modWitcher3MP\INSTALLATION.txt"
echo 5. Test the combat system in-game >> "..\..\modWitcher3MP\INSTALLATION.txt"
echo. >> "..\..\modWitcher3MP\INSTALLATION.txt"
echo For support, check the documentation in the docs folder. >> "..\..\modWitcher3MP\INSTALLATION.txt"

echo ✅ Installation instructions created
echo.

REM Copy documentation
echo Copying documentation...
if not exist "..\..\modWitcher3MP\docs" mkdir "..\..\modWitcher3MP\docs"
xcopy "docs\*.md" "..\..\modWitcher3MP\docs\" /Y /Q

echo ✅ Documentation copied
echo.

REM Generate final report
echo [%time%] Generating installation report... >> "%LOG_FILE%"
echo Generating installation report...

echo ======================================== > "%REPORT_FILE%"
echo Witcher3-MP Installation Report >> "%REPORT_FILE%"
echo Generated: %date% %time% >> "%REPORT_FILE%"
echo ======================================== >> "%REPORT_FILE%"
echo. >> "%REPORT_FILE%"

echo Installation Summary: >> "%REPORT_FILE%"
echo - Files copied: %FILES_COPIED% >> "%REPORT_FILE%"
echo - Errors encountered: %ERRORS% >> "%REPORT_FILE%"
echo - Mod directory: %MOD_DIR% >> "%REPORT_FILE%"
echo. >> "%REPORT_FILE%"

echo Directory Structure: >> "%REPORT_FILE%"
echo %MOD_DIR%\ >> "%REPORT_FILE%"
echo ├── scripts\game\witcher3_mp\ >> "%REPORT_FILE%"
echo │   ├── MP_CombatSystemEnhanced.ws >> "%REPORT_FILE%"
echo │   ├── MP_CombatHelpers.ws >> "%REPORT_FILE%"
echo │   ├── MP_BridgeSupport.ws >> "%REPORT_FILE%"
echo │   └── ... (other .ws files) >> "%REPORT_FILE%"
echo ├── config\ >> "%REPORT_FILE%"
echo │   └── mp_config.json >> "%REPORT_FILE%"
echo ├── docs\ >> "%REPORT_FILE%"
echo │   └── ... (documentation files) >> "%REPORT_FILE%"
echo └── modInfo.ws >> "%REPORT_FILE%"
echo. >> "%REPORT_FILE%"

echo Next Steps: >> "%REPORT_FILE%"
echo 1. Copy %MOD_DIR% to your The Witcher 3 mods directory >> "%REPORT_FILE%"
echo 2. Install the base mod "Fix sounds of sword hits when adding effects" >> "%REPORT_FILE%"
echo 3. Start The Witcher 3 and verify the mod loads without errors >> "%REPORT_FILE%"
echo 4. Check the game logs for any error messages >> "%REPORT_FILE%"
echo 5. Test the combat system in-game >> "%REPORT_FILE%"
echo. >> "%REPORT_FILE%"

echo Troubleshooting: >> "%REPORT_FILE%"
echo - If mod doesn't load: Check file paths and permissions >> "%REPORT_FILE%"
echo - If errors occur: Check game logs and mod compatibility >> "%REPORT_FILE%"
echo - If performance issues: Adjust settings in mp_config.json >> "%REPORT_FILE%"
echo. >> "%REPORT_FILE%"

echo For support: >> "%REPORT_FILE%"
echo - Check documentation in modWitcher3MP\docs\ >> "%REPORT_FILE%"
echo - Review installation logs in logs\ >> "%REPORT_FILE%"
echo - Verify mod dependencies are installed >> "%REPORT_FILE%"

echo [%time%] Installation report generated: %REPORT_FILE% >> "%LOG_FILE%"
echo [%time%] Installation completed with %ERRORS% errors >> "%LOG_FILE%"

echo ========================================
echo Installation Complete!
echo ========================================
echo.
echo The mod has been prepared for game testing.
echo.
echo Installation Summary:
echo - Files copied: %FILES_COPIED%
echo - Errors encountered: %ERRORS%
echo - Mod directory: %MOD_DIR%
echo.
echo Logs and reports:
echo - Installation log: %LOG_FILE%
echo - Installation report: %REPORT_FILE%
echo.
echo Next steps:
echo 1. Copy the modWitcher3MP folder to your The Witcher 3 mods directory
echo 2. Install the base mod "Fix sounds of sword hits when adding effects"
echo 3. Start The Witcher 3 and test the combat system
echo.
echo For detailed instructions, see: modWitcher3MP\INSTALLATION.txt
echo For documentation, see: modWitcher3MP\docs\
echo.
echo Happy testing! 🎮
echo.
pause
