@echo off
setlocal enabledelayedexpansion

echo ========================================
echo Witcher3-MP Installation Script (DEBUG)
echo ========================================
echo.

REM Create logs directory
if not exist "logs" (
    echo Creating logs directory...
    mkdir "logs"
    if !ERRORLEVEL! neq 0 (
        echo ERROR: Failed to create logs directory
        pause
        exit /b 1
    )
)

REM Set log file with timestamp
for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value 2^>nul') do set "dt=%%a"
if "%dt%"=="" (
    echo WARNING: Could not get timestamp, using default
    set "timestamp=debug"
) else (
    set "YY=%dt:~2,2%" & set "YYYY=%dt:~0,4%" & set "MM=%dt:~4,2%" & set "DD=%dt:~6,2%"
    set "HH=%dt:~8,2%" & set "Min=%dt:~10,2%" & set "Sec=%dt:~12,2%"
    set "timestamp=%YYYY%-%MM%-%DD%_%HH%-%Min%-%Sec%"
)

set "LOG_FILE=logs\installation_debug_%timestamp%.log"
set "REPORT_FILE=logs\installation_report_debug_%timestamp%.txt"

echo Log file: %LOG_FILE%
echo Report file: %REPORT_FILE%
echo.

REM Initialize log file
echo ======================================== > "%LOG_FILE%"
echo Witcher3-MP Installation Script (DEBUG) >> "%LOG_FILE%"
echo Started: %date% %time% >> "%LOG_FILE%"
echo ======================================== >> "%LOG_FILE%"
echo. >> "%LOG_FILE%"

echo [%time%] Starting installation process... >> "%LOG_FILE%"
echo Starting installation process...

REM Check if we're in the right directory
echo [%time%] Checking project directory structure... >> "%LOG_FILE%"
echo Checking project directory structure...

if not exist "mod" (
    echo [%time%] ERROR: mod directory not found >> "%LOG_FILE%"
    echo ERROR: mod directory not found
    echo Current directory: %CD%
    echo Please run this script from the Witcher3-MP-Clean directory
    pause
    exit /b 1
)

if not exist "mod\scripts" (
    echo [%time%] ERROR: mod\scripts directory not found >> "%LOG_FILE%"
    echo ERROR: mod\scripts directory not found
    pause
    exit /b 1
)

if not exist "mod\scripts\game" (
    echo [%time%] ERROR: mod\scripts\game directory not found >> "%LOG_FILE%"
    echo ERROR: mod\scripts\game directory not found
    pause
    exit /b 1
)

if not exist "mod\scripts\game\witcher3_mp" (
    echo [%time%] ERROR: mod\scripts\game\witcher3_mp directory not found >> "%LOG_FILE%"
    echo ERROR: mod\scripts\game\witcher3_mp directory not found
    pause
    exit /b 1
)

echo [%time%] SUCCESS: Project directory structure verified >> "%LOG_FILE%"
echo SUCCESS: Project directory structure verified
echo.

REM Check for WitcherScript files
echo [%time%] Checking for WitcherScript files... >> "%LOG_FILE%"
echo Checking for WitcherScript files...

set "WS_FILES=0"
for %%f in ("mod\scripts\game\witcher3_mp\*.ws") do set /a WS_FILES+=1

echo [%time%] Found %WS_FILES% WitcherScript files >> "%LOG_FILE%"
echo Found %WS_FILES% WitcherScript files

if %WS_FILES% equ 0 (
    echo [%time%] ERROR: No WitcherScript files found >> "%LOG_FILE%"
    echo ERROR: No WitcherScript files found
    echo Please ensure the mod files are in the correct location
    pause
    exit /b 1
)

REM List WitcherScript files
echo [%time%] WitcherScript files found: >> "%LOG_FILE%"
echo WitcherScript files found:
for %%f in ("mod\scripts\game\witcher3_mp\*.ws") do (
    echo [%time%]   - %%~nxf >> "%LOG_FILE%"
    echo   - %%~nxf
)
echo.

REM Create mod directory structure
echo [%time%] Creating mod directory structure... >> "%LOG_FILE%"
echo Creating mod directory structure...

set "MOD_DIR=..\..\modWitcher3MP"
set "FILES_COPIED=0"
set "ERRORS=0"

echo [%time%] Target mod directory: %MOD_DIR% >> "%LOG_FILE%"
echo Target mod directory: %MOD_DIR%

REM Create directories one by one with error checking
echo [%time%] Creating main mod directory... >> "%LOG_FILE%"
if not exist "%MOD_DIR%" (
    echo Creating main mod directory...
    mkdir "%MOD_DIR%"
    if !ERRORLEVEL! neq 0 (
        echo [%time%] ERROR: Failed to create main mod directory >> "%LOG_FILE%"
        echo ERROR: Failed to create main mod directory
        set /a ERRORS+=1
    ) else (
        echo [%time%] SUCCESS: Main mod directory created >> "%LOG_FILE%"
        echo SUCCESS: Main mod directory created
    )
) else (
    echo [%time%] INFO: Main mod directory already exists >> "%LOG_FILE%"
    echo INFO: Main mod directory already exists
)

echo [%time%] Creating scripts directory... >> "%LOG_FILE%"
if not exist "%MOD_DIR%\scripts" (
    echo Creating scripts directory...
    mkdir "%MOD_DIR%\scripts"
    if !ERRORLEVEL! neq 0 (
        echo [%time%] ERROR: Failed to create scripts directory >> "%LOG_FILE%"
        echo ERROR: Failed to create scripts directory
        set /a ERRORS+=1
    ) else (
        echo [%time%] SUCCESS: Scripts directory created >> "%LOG_FILE%"
        echo SUCCESS: Scripts directory created
    )
) else (
    echo [%time%] INFO: Scripts directory already exists >> "%LOG_FILE%"
    echo INFO: Scripts directory already exists
)

echo [%time%] Creating game directory... >> "%LOG_FILE%"
if not exist "%MOD_DIR%\scripts\game" (
    echo Creating game directory...
    mkdir "%MOD_DIR%\scripts\game"
    if !ERRORLEVEL! neq 0 (
        echo [%time%] ERROR: Failed to create game directory >> "%LOG_FILE%"
        echo ERROR: Failed to create game directory
        set /a ERRORS+=1
    ) else (
        echo [%time%] SUCCESS: Game directory created >> "%LOG_FILE%"
        echo SUCCESS: Game directory created
    )
) else (
    echo [%time%] INFO: Game directory already exists >> "%LOG_FILE%"
    echo INFO: Game directory already exists
)

echo [%time%] Creating witcher3_mp directory... >> "%LOG_FILE%"
if not exist "%MOD_DIR%\scripts\game\witcher3_mp" (
    echo Creating witcher3_mp directory...
    mkdir "%MOD_DIR%\scripts\game\witcher3_mp"
    if !ERRORLEVEL! neq 0 (
        echo [%time%] ERROR: Failed to create witcher3_mp directory >> "%LOG_FILE%"
        echo ERROR: Failed to create witcher3_mp directory
        set /a ERRORS+=1
    ) else (
        echo [%time%] SUCCESS: witcher3_mp directory created >> "%LOG_FILE%"
        echo SUCCESS: witcher3_mp directory created
    )
) else (
    echo [%time%] INFO: witcher3_mp directory already exists >> "%LOG_FILE%"
    echo INFO: witcher3_mp directory already exists
)

echo [%time%] Creating content directory... >> "%LOG_FILE%"
if not exist "%MOD_DIR%\content" (
    echo Creating content directory...
    mkdir "%MOD_DIR%\content"
    if !ERRORLEVEL! neq 0 (
        echo [%time%] ERROR: Failed to create content directory >> "%LOG_FILE%"
        echo ERROR: Failed to create content directory
        set /a ERRORS+=1
    ) else (
        echo [%time%] SUCCESS: Content directory created >> "%LOG_FILE%"
        echo SUCCESS: Content directory created
    )
) else (
    echo [%time%] INFO: Content directory already exists >> "%LOG_FILE%"
    echo INFO: Content directory already exists
)

echo [%time%] Creating config directory... >> "%LOG_FILE%"
if not exist "%MOD_DIR%\config" (
    echo Creating config directory...
    mkdir "%MOD_DIR%\config"
    if !ERRORLEVEL! neq 0 (
        echo [%time%] ERROR: Failed to create config directory >> "%LOG_FILE%"
        echo ERROR: Failed to create config directory
        set /a ERRORS+=1
    ) else (
        echo [%time%] SUCCESS: Config directory created >> "%LOG_FILE%"
        echo SUCCESS: Config directory created
    )
) else (
    echo [%time%] INFO: Config directory already exists >> "%LOG_FILE%"
    echo INFO: Config directory already exists
)

if %ERRORS% equ 0 (
    echo [%time%] SUCCESS: All directories created successfully >> "%LOG_FILE%"
    echo SUCCESS: All directories created successfully
) else (
    echo [%time%] WARNING: %ERRORS% errors occurred during directory creation >> "%LOG_FILE%"
    echo WARNING: %ERRORS% errors occurred during directory creation
)
echo.

REM Copy WitcherScript files
echo [%time%] Copying WitcherScript files... >> "%LOG_FILE%"
echo Copying WitcherScript files...

echo [%time%] Source: mod\scripts\game\witcher3_mp\*.ws >> "%LOG_FILE%"
echo [%time%] Destination: %MOD_DIR%\scripts\game\witcher3_mp\ >> "%LOG_FILE%"

xcopy "mod\scripts\game\witcher3_mp\*.ws" "%MOD_DIR%\scripts\game\witcher3_mp\" /Y /Q
if !ERRORLEVEL! neq 0 (
    echo [%time%] ERROR: Failed to copy WitcherScript files >> "%LOG_FILE%"
    echo ERROR: Failed to copy WitcherScript files
    echo Error code: !ERRORLEVEL!
    set /a ERRORS+=1
) else (
    echo [%time%] SUCCESS: WitcherScript files copied >> "%LOG_FILE%"
    echo SUCCESS: WitcherScript files copied
    set /a FILES_COPIED+=%WS_FILES%
)

REM Verify copied files
echo [%time%] Verifying copied files... >> "%LOG_FILE%"
echo Verifying copied files...

set "COPIED_COUNT=0"
for %%f in ("%MOD_DIR%\scripts\game\witcher3_mp\*.ws") do set /a COPIED_COUNT+=1

echo [%time%] Files copied: %COPIED_COUNT% >> "%LOG_FILE%"
echo Files copied: %COPIED_COUNT%

if %COPIED_COUNT% neq %WS_FILES% (
    echo [%time%] WARNING: File count mismatch - expected %WS_FILES%, got %COPIED_COUNT% >> "%LOG_FILE%"
    echo WARNING: File count mismatch - expected %WS_FILES%, got %COPIED_COUNT%
)

REM List copied files
echo [%time%] Copied WitcherScript files: >> "%LOG_FILE%"
echo Copied WitcherScript files:
for %%f in ("%MOD_DIR%\scripts\game\witcher3_mp\*.ws") do (
    echo [%time%]   - %%~nxf >> "%LOG_FILE%"
    echo   - %%~nxf
)
echo.

REM Copy configuration files
echo [%time%] Copying configuration files... >> "%LOG_FILE%"
echo Copying configuration files...

if exist "config" (
    if exist "config\*.json" (
        xcopy "config\*.json" "%MOD_DIR%\config\" /Y /Q
        if !ERRORLEVEL! neq 0 (
            echo [%time%] ERROR: Failed to copy configuration files >> "%LOG_FILE%"
            echo ERROR: Failed to copy configuration files
            set /a ERRORS+=1
        ) else (
            echo [%time%] SUCCESS: Configuration files copied >> "%LOG_FILE%"
            echo SUCCESS: Configuration files copied
        )
    ) else (
        echo [%time%] WARNING: No JSON configuration files found >> "%LOG_FILE%"
        echo WARNING: No JSON configuration files found
    )
) else (
    echo [%time%] WARNING: config directory not found >> "%LOG_FILE%"
    echo WARNING: config directory not found
)
echo.

REM Create modInfo.ws
echo [%time%] Creating modInfo.ws... >> "%LOG_FILE%"
echo Creating modInfo.ws...

echo // Witcher3-MP Mod Info > "%MOD_DIR%\modInfo.ws"
echo // Generated by installation script >> "%MOD_DIR%\modInfo.ws"
echo. >> "%MOD_DIR%\modInfo.ws"
echo modWitcher3MP >> "%MOD_DIR%\modInfo.ws"
echo { >> "%MOD_DIR%\modInfo.ws"
echo     version = "1.0.0"; >> "%MOD_DIR%\modInfo.ws"
echo     name = "Witcher3-MP Multiplayer Mod"; >> "%MOD_DIR%\modInfo.ws"
echo     description = "Multiplayer combat system for The Witcher 3 Next-Gen"; >> "%MOD_DIR%\modInfo.ws"
echo     author = "Witcher3-MP Team"; >> "%MOD_DIR%\modInfo.ws"
echo } >> "%MOD_DIR%\modInfo.ws"

echo [%time%] SUCCESS: modInfo.ws created >> "%LOG_FILE%"
echo SUCCESS: modInfo.ws created
echo.

REM Create installation instructions
echo [%time%] Creating installation instructions... >> "%LOG_FILE%"
echo Creating installation instructions...

echo # Witcher3-MP Installation Instructions > "%MOD_DIR%\INSTALLATION.txt"
echo. >> "%MOD_DIR%\INSTALLATION.txt"
echo 1. Copy the modWitcher3MP folder to your The Witcher 3 mods directory >> "%MOD_DIR%\INSTALLATION.txt"
echo 2. Ensure you have the base mod "Fix sounds of sword hits when adding effects" installed >> "%MOD_DIR%\INSTALLATION.txt"
echo 3. Start The Witcher 3 and verify the mod loads without errors >> "%MOD_DIR%\INSTALLATION.txt"
echo 4. Check the game logs for any error messages >> "%MOD_DIR%\INSTALLATION.txt"
echo 5. Test the combat system in-game >> "%MOD_DIR%\INSTALLATION.txt"
echo. >> "%MOD_DIR%\INSTALLATION.txt"
echo For support, check the documentation in the docs folder. >> "%MOD_DIR%\INSTALLATION.txt"

echo [%time%] SUCCESS: Installation instructions created >> "%LOG_FILE%"
echo SUCCESS: Installation instructions created
echo.

REM Copy documentation
echo [%time%] Copying documentation... >> "%LOG_FILE%"
echo Copying documentation...

if exist "docs" (
    if not exist "%MOD_DIR%\docs" mkdir "%MOD_DIR%\docs"
    xcopy "docs\*.md" "%MOD_DIR%\docs\" /Y /Q
    if !ERRORLEVEL! neq 0 (
        echo [%time%] WARNING: Failed to copy some documentation files >> "%LOG_FILE%"
        echo WARNING: Failed to copy some documentation files
    ) else (
        echo [%time%] SUCCESS: Documentation copied >> "%LOG_FILE%"
        echo SUCCESS: Documentation copied
    )
) else (
    echo [%time%] WARNING: docs directory not found >> "%LOG_FILE%"
    echo WARNING: docs directory not found
)
echo.

REM Generate final report
echo [%time%] Generating installation report... >> "%LOG_FILE%"
echo Generating installation report...

echo ======================================== > "%REPORT_FILE%"
echo Witcher3-MP Installation Report (DEBUG) >> "%REPORT_FILE%"
echo Generated: %date% %time% >> "%REPORT_FILE%"
echo ======================================== >> "%REPORT_FILE%"
echo. >> "%REPORT_FILE%"

echo Installation Summary: >> "%REPORT_FILE%"
echo - Files copied: %FILES_COPIED% >> "%REPORT_FILE%"
echo - Errors encountered: %ERRORS% >> "%REPORT_FILE%"
echo - Mod directory: %MOD_DIR% >> "%REPORT_FILE%"
echo - WitcherScript files: %WS_FILES% >> "%REPORT_FILE%"
echo - Files actually copied: %COPIED_COUNT% >> "%REPORT_FILE%"
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
echo - WitcherScript files: %WS_FILES%
echo - Files actually copied: %COPIED_COUNT%
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
echo Press any key to continue...
pause >nul
