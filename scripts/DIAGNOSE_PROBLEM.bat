@echo off
setlocal enabledelayedexpansion

echo ========================================
echo Witcher3-MP Problem Diagnosis
echo ========================================
echo.

REM Create logs directory
if not exist "logs" mkdir "logs"

REM Set log file with timestamp
for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value 2^>nul') do set "dt=%%a"
if "%dt%"=="" (
    set "timestamp=debug"
) else (
    set "YY=%dt:~2,2%" & set "YYYY=%dt:~0,4%" & set "MM=%dt:~4,2%" & set "DD=%dt:~6,2%"
    set "HH=%dt:~8,2%" & set "Min=%dt:~10,2%" & set "Sec=%dt:~12,2%"
    set "timestamp=%YYYY%-%MM%-%DD%_%HH%-%Min%-%Sec%"
)

set "LOG_FILE=logs\diagnosis_%timestamp%.log"

echo Log file: %LOG_FILE%
echo.

REM Initialize log file
echo ======================================== > "%LOG_FILE%"
echo Witcher3-MP Problem Diagnosis >> "%LOG_FILE%"
echo Started: %date% %time% >> "%LOG_FILE%"
echo ======================================== >> "%LOG_FILE%"
echo. >> "%LOG_FILE%"

echo [%time%] Starting diagnosis... >> "%LOG_FILE%"
echo Starting diagnosis...

REM Check current directory
echo [%time%] Current directory: %CD% >> "%LOG_FILE%"
echo Current directory: %CD%

REM Check if we're in the right directory
echo [%time%] Checking project structure... >> "%LOG_FILE%"
echo Checking project structure...

set "MISSING_DIRS=0"

if not exist "mod" (
    echo [%time%] ERROR: mod directory not found >> "%LOG_FILE%"
    echo ❌ ERROR: mod directory not found
    set /a MISSING_DIRS+=1
) else (
    echo [%time%] SUCCESS: mod directory found >> "%LOG_FILE%"
    echo ✅ mod directory found
)

if not exist "mod\scripts" (
    echo [%time%] ERROR: mod\scripts directory not found >> "%LOG_FILE%"
    echo ❌ ERROR: mod\scripts directory not found
    set /a MISSING_DIRS+=1
) else (
    echo [%time%] SUCCESS: mod\scripts directory found >> "%LOG_FILE%"
    echo ✅ mod\scripts directory found
)

if not exist "mod\scripts\game" (
    echo [%time%] ERROR: mod\scripts\game directory not found >> "%LOG_FILE%"
    echo ❌ ERROR: mod\scripts\game directory not found
    set /a MISSING_DIRS+=1
) else (
    echo [%time%] SUCCESS: mod\scripts\game directory found >> "%LOG_FILE%"
    echo ✅ mod\scripts\game directory found
)

if not exist "mod\scripts\game\witcher3_mp" (
    echo [%time%] ERROR: mod\scripts\game\witcher3_mp directory not found >> "%LOG_FILE%"
    echo ❌ ERROR: mod\scripts\game\witcher3_mp directory not found
    set /a MISSING_DIRS+=1
) else (
    echo [%time%] SUCCESS: mod\scripts\game\witcher3_mp directory found >> "%LOG_FILE%"
    echo ✅ mod\scripts\game\witcher3_mp directory found
)

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
    echo ❌ ERROR: No WitcherScript files found
    set /a MISSING_DIRS+=1
) else (
    echo [%time%] SUCCESS: WitcherScript files found >> "%LOG_FILE%"
    echo ✅ WitcherScript files found
)

REM List WitcherScript files
echo [%time%] WitcherScript files: >> "%LOG_FILE%"
echo WitcherScript files:
for %%f in ("mod\scripts\game\witcher3_mp\*.ws") do (
    echo [%time%]   - %%~nxf >> "%LOG_FILE%"
    echo   - %%~nxf
)
echo.

REM Check for configuration files
echo [%time%] Checking for configuration files... >> "%LOG_FILE%"
echo Checking for configuration files...

if exist "config" (
    echo [%time%] SUCCESS: config directory found >> "%LOG_FILE%"
    echo ✅ config directory found
    
    set "JSON_FILES=0"
    for %%f in ("config\*.json") do set /a JSON_FILES+=1
    
    echo [%time%] Found %JSON_FILES% JSON files >> "%LOG_FILE%"
    echo Found %JSON_FILES% JSON files
    
    if %JSON_FILES% gtr 0 (
        echo [%time%] JSON files: >> "%LOG_FILE%"
        echo JSON files:
        for %%f in ("config\*.json") do (
            echo [%time%]   - %%~nxf >> "%LOG_FILE%"
            echo   - %%~nxf
        )
    )
) else (
    echo [%time%] WARNING: config directory not found >> "%LOG_FILE%"
    echo ⚠️ WARNING: config directory not found
)
echo.

REM Check for documentation
echo [%time%] Checking for documentation... >> "%LOG_FILE%"
echo Checking for documentation...

if exist "docs" (
    echo [%time%] SUCCESS: docs directory found >> "%LOG_FILE%"
    echo ✅ docs directory found
    
    set "DOC_FILES=0"
    for %%f in ("docs\*.md") do set /a DOC_FILES+=1
    
    echo [%time%] Found %DOC_FILES% documentation files >> "%LOG_FILE%"
    echo Found %DOC_FILES% documentation files
) else (
    echo [%time%] WARNING: docs directory not found >> "%LOG_FILE%"
    echo ⚠️ WARNING: docs directory not found
)
echo.

REM Check system information
echo [%time%] Checking system information... >> "%LOG_FILE%"
echo Checking system information...

echo [%time%] Windows version: >> "%LOG_FILE%"
echo Windows version:
ver >> "%LOG_FILE%"
ver

echo [%time%] PowerShell version: >> "%LOG_FILE%"
echo PowerShell version:
powershell -Command "$PSVersionTable.PSVersion" >> "%LOG_FILE%" 2>&1
powershell -Command "$PSVersionTable.PSVersion" 2>&1

echo [%time%] Available disk space: >> "%LOG_FILE%"
echo Available disk space:
dir /-c >> "%LOG_FILE%"
dir /-c

echo.

REM Check for common issues
echo [%time%] Checking for common issues... >> "%LOG_FILE%"
echo Checking for common issues...

REM Check if running as administrator
net session >nul 2>&1
if %ERRORLEVEL% equ 0 (
    echo [%time%] INFO: Running as administrator >> "%LOG_FILE%"
    echo ℹ️ INFO: Running as administrator
) else (
    echo [%time%] WARNING: Not running as administrator >> "%LOG_FILE%"
    echo ⚠️ WARNING: Not running as administrator
)

REM Check for antivirus interference
echo [%time%] Checking for antivirus interference... >> "%LOG_FILE%"
echo Checking for antivirus interference...

tasklist /FI "IMAGENAME eq *antivirus*" /FO CSV | find /I "antivirus" >nul 2>&1
if %ERRORLEVEL% equ 0 (
    echo [%time%] WARNING: Antivirus software detected >> "%LOG_FILE%"
    echo ⚠️ WARNING: Antivirus software detected
) else (
    echo [%time%] INFO: No antivirus software detected >> "%LOG_FILE%"
    echo ℹ️ INFO: No antivirus software detected
)

echo.

REM Generate diagnosis report
echo [%time%] Generating diagnosis report... >> "%LOG_FILE%"
echo Generating diagnosis report...

echo ======================================== >> "%LOG_FILE%"
echo DIAGNOSIS SUMMARY >> "%LOG_FILE%"
echo ======================================== >> "%LOG_FILE%"
echo. >> "%LOG_FILE%"

echo Missing directories: %MISSING_DIRS% >> "%LOG_FILE%"
echo WitcherScript files: %WS_FILES% >> "%LOG_FILE%"
echo. >> "%LOG_FILE%"

if %MISSING_DIRS% equ 0 (
    echo [%time%] SUCCESS: All required directories found >> "%LOG_FILE%"
    echo ✅ SUCCESS: All required directories found
    echo. >> "%LOG_FILE%"
    echo The project structure appears to be correct. >> "%LOG_FILE%"
    echo The issue might be with the installation script itself. >> "%LOG_FILE%"
    echo. >> "%LOG_FILE%"
    echo Recommendations: >> "%LOG_FILE%"
    echo 1. Try running INSTALL_FOR_GAME_DEBUG.bat instead >> "%LOG_FILE%"
    echo 2. Check if antivirus is blocking the script >> "%LOG_FILE%"
    echo 3. Run as administrator >> "%LOG_FILE%"
    echo 4. Check Windows Event Viewer for errors >> "%LOG_FILE%"
) else (
    echo [%time%] ERROR: %MISSING_DIRS% required directories missing >> "%LOG_FILE%"
    echo ❌ ERROR: %MISSING_DIRS% required directories missing
    echo. >> "%LOG_FILE%"
    echo The project structure is incomplete. >> "%LOG_FILE%"
    echo Please ensure you are running this script from the correct directory. >> "%LOG_FILE%"
    echo. >> "%LOG_FILE%"
    echo Expected directory structure: >> "%LOG_FILE%"
    echo Witcher3-MP-Clean\ >> "%LOG_FILE%"
    echo ├── mod\scripts\game\witcher3_mp\ >> "%LOG_FILE%"
    echo ├── config\ >> "%LOG_FILE%"
    echo ├── docs\ >> "%LOG_FILE%"
    echo └── ... (other files) >> "%LOG_FILE%"
)

echo.
echo ========================================
echo Diagnosis Complete!
echo ========================================
echo.
echo Diagnosis Summary:
echo - Missing directories: %MISSING_DIRS%
echo - WitcherScript files: %WS_FILES%
echo - Log file: %LOG_FILE%
echo.

if %MISSING_DIRS% equ 0 (
    echo ✅ The project structure appears to be correct.
    echo The issue might be with the installation script itself.
    echo.
    echo Recommendations:
    echo 1. Try running INSTALL_FOR_GAME_DEBUG.bat instead
    echo 2. Check if antivirus is blocking the script
    echo 3. Run as administrator
    echo 4. Check Windows Event Viewer for errors
) else (
    echo ❌ The project structure is incomplete.
    echo Please ensure you are running this script from the correct directory.
    echo.
    echo Expected directory structure:
    echo Witcher3-MP-Clean\
    echo ├── mod\scripts\game\witcher3_mp\
    echo ├── config\
    echo ├── docs\
    echo └── ... (other files)
)

echo.
echo Press any key to continue...
pause >nul
