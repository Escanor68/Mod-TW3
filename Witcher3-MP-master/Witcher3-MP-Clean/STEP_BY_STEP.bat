@echo off
setlocal enabledelayedexpansion

echo ========================================
echo Witcher3-MP Step-by-Step Installation
echo ========================================
echo.

echo This script will guide you through the installation process step by step.
echo Press any key to continue after each step.
echo.

pause

echo Step 1: Checking current directory
echo ========================================
echo Current directory: %CD%
echo.
echo Is this the correct directory? (Y/N)
set /p "response=Enter your choice: "
if /i "%response%" neq "Y" (
    echo Please navigate to the Witcher3-MP-Clean directory and run this script again.
    pause
    exit /b 1
)
echo ✅ Directory confirmed
echo.
pause

echo Step 2: Checking project structure
echo ========================================
echo Checking for required directories...

set "ERRORS=0"

if not exist "mod" (
    echo ❌ ERROR: mod directory not found
    set /a ERRORS+=1
) else (
    echo ✅ mod directory found
)

if not exist "mod\scripts" (
    echo ❌ ERROR: mod\scripts directory not found
    set /a ERRORS+=1
) else (
    echo ✅ mod\scripts directory found
)

if not exist "mod\scripts\game" (
    echo ❌ ERROR: mod\scripts\game directory not found
    set /a ERRORS+=1
) else (
    echo ✅ mod\scripts\game directory found
)

if not exist "mod\scripts\game\witcher3_mp" (
    echo ❌ ERROR: mod\scripts\game\witcher3_mp directory not found
    set /a ERRORS+=1
) else (
    echo ✅ mod\scripts\game\witcher3_mp directory found
)

if %ERRORS% gtr 0 (
    echo.
    echo ❌ %ERRORS% errors found. Please fix these issues before continuing.
    pause
    exit /b 1
)

echo ✅ All required directories found
echo.
pause

echo Step 3: Checking WitcherScript files
echo ========================================
echo Checking for WitcherScript files...

set "WS_COUNT=0"
for %%f in ("mod\scripts\game\witcher3_mp\*.ws") do set /a WS_COUNT+=1

echo Found %WS_COUNT% WitcherScript files

if %WS_COUNT% equ 0 (
    echo ❌ ERROR: No WitcherScript files found
    pause
    exit /b 1
)

echo ✅ WitcherScript files found
echo.
echo Listing files:
for %%f in ("mod\scripts\game\witcher3_mp\*.ws") do echo   - %%~nxf
echo.
pause

echo Step 4: Creating mod directory
echo ========================================
set "MOD_DIR=..\..\modWitcher3MP"
echo Target directory: %MOD_DIR%

if not exist "%MOD_DIR%" (
    echo Creating mod directory...
    mkdir "%MOD_DIR%"
    if !ERRORLEVEL! neq 0 (
        echo ❌ ERROR: Failed to create mod directory
        pause
        exit /b 1
    )
    echo ✅ Mod directory created
) else (
    echo ✅ Mod directory already exists
)
echo.
pause

echo Step 5: Creating subdirectories
echo ========================================
echo Creating subdirectories...

if not exist "%MOD_DIR%\scripts" (
    echo Creating scripts directory...
    mkdir "%MOD_DIR%\scripts"
    if !ERRORLEVEL! neq 0 (
        echo ❌ ERROR: Failed to create scripts directory
        pause
        exit /b 1
    )
    echo ✅ Scripts directory created
) else (
    echo ✅ Scripts directory already exists
)

if not exist "%MOD_DIR%\scripts\game" (
    echo Creating game directory...
    mkdir "%MOD_DIR%\scripts\game"
    if !ERRORLEVEL! neq 0 (
        echo ❌ ERROR: Failed to create game directory
        pause
        exit /b 1
    )
    echo ✅ Game directory created
) else (
    echo ✅ Game directory already exists
)

if not exist "%MOD_DIR%\scripts\game\witcher3_mp" (
    echo Creating witcher3_mp directory...
    mkdir "%MOD_DIR%\scripts\game\witcher3_mp"
    if !ERRORLEVEL! neq 0 (
        echo ❌ ERROR: Failed to create witcher3_mp directory
        pause
        exit /b 1
    )
    echo ✅ witcher3_mp directory created
) else (
    echo ✅ witcher3_mp directory already exists
)
echo.
pause

echo Step 6: Copying WitcherScript files
echo ========================================
echo Copying WitcherScript files...
echo Source: mod\scripts\game\witcher3_mp\*.ws
echo Destination: %MOD_DIR%\scripts\game\witcher3_mp\

xcopy "mod\scripts\game\witcher3_mp\*.ws" "%MOD_DIR%\scripts\game\witcher3_mp\" /Y /Q
if !ERRORLEVEL! neq 0 (
    echo ❌ ERROR: Failed to copy WitcherScript files
    echo Error code: !ERRORLEVEL!
    pause
    exit /b 1
)

echo ✅ WitcherScript files copied
echo.
pause

echo Step 7: Verifying copied files
echo ========================================
echo Verifying copied files...

set "COPIED_COUNT=0"
for %%f in ("%MOD_DIR%\scripts\game\witcher3_mp\*.ws") do set /a COPIED_COUNT+=1

echo Files copied: %COPIED_COUNT%
echo Files expected: %WS_COUNT%

if %COPIED_COUNT% neq %WS_COUNT% (
    echo ⚠️ WARNING: File count mismatch - expected %WS_COUNT%, got %COPIED_COUNT%
    echo.
    echo Copied files:
    for %%f in ("%MOD_DIR%\scripts\game\witcher3_mp\*.ws") do echo   - %%~nxf
) else (
    echo ✅ All files copied successfully
)
echo.
pause

echo Step 8: Creating modInfo.ws
echo ========================================
echo Creating modInfo.ws...

echo // Witcher3-MP Mod Info > "%MOD_DIR%\modInfo.ws"
echo modWitcher3MP >> "%MOD_DIR%\modInfo.ws"
echo { >> "%MOD_DIR%\modInfo.ws"
echo     version = "1.0.0"; >> "%MOD_DIR%\modInfo.ws"
echo     name = "Witcher3-MP Multiplayer Mod"; >> "%MOD_DIR%\modInfo.ws"
echo     description = "Multiplayer combat system for The Witcher 3 Next-Gen"; >> "%MOD_DIR%\modInfo.ws"
echo     author = "Witcher3-MP Team"; >> "%MOD_DIR%\modInfo.ws"
echo } >> "%MOD_DIR%\modInfo.ws"

echo ✅ modInfo.ws created
echo.
pause

echo Step 9: Final verification
echo ========================================
echo Final verification...

echo Checking mod directory structure:
if exist "%MOD_DIR%\scripts\game\witcher3_mp" (
    echo ✅ witcher3_mp directory exists
) else (
    echo ❌ witcher3_mp directory missing
)

if exist "%MOD_DIR%\modInfo.ws" (
    echo ✅ modInfo.ws exists
) else (
    echo ❌ modInfo.ws missing
)

echo.
echo Checking file count:
echo Expected: %WS_COUNT%
echo Actual: %COPIED_COUNT%

if %COPIED_COUNT% equ %WS_COUNT% (
    echo ✅ File count matches
) else (
    echo ⚠️ File count mismatch
)
echo.
pause

echo ========================================
echo Installation Complete!
echo ========================================
echo.
echo Mod directory: %MOD_DIR%
echo Files copied: %COPIED_COUNT%
echo.
echo Next steps:
echo 1. Copy the modWitcher3MP folder to your The Witcher 3 mods directory
echo 2. Install the base mod "Fix sounds of sword hits when adding effects"
echo 3. Start The Witcher 3 and test the combat system
echo.
echo Press any key to exit...
pause
