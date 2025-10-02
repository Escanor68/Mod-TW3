@echo off
echo ========================================
echo Witcher3-MP Simple Installation
echo ========================================
echo.

echo Current directory: %CD%
echo.

echo Checking if we're in the right directory...
if not exist "mod" (
    echo ERROR: mod directory not found
    echo Please run this script from the Witcher3-MP-Clean directory
    pause
    exit /b 1
)

echo ✅ mod directory found

if not exist "mod\scripts\game\witcher3_mp" (
    echo ERROR: mod\scripts\game\witcher3_mp directory not found
    pause
    exit /b 1
)

echo ✅ mod\scripts\game\witcher3_mp directory found

echo.
echo Counting WitcherScript files...
set "WS_COUNT=0"
for %%f in ("mod\scripts\game\witcher3_mp\*.ws") do set /a WS_COUNT+=1

echo Found %WS_COUNT% WitcherScript files

if %WS_COUNT% equ 0 (
    echo ERROR: No WitcherScript files found
    pause
    exit /b 1
)

echo ✅ WitcherScript files found

echo.
echo Listing WitcherScript files:
for %%f in ("mod\scripts\game\witcher3_mp\*.ws") do echo   - %%~nxf

echo.
echo Creating mod directory...
set "MOD_DIR=..\..\modWitcher3MP"

if not exist "%MOD_DIR%" (
    echo Creating %MOD_DIR%...
    mkdir "%MOD_DIR%"
    if !ERRORLEVEL! neq 0 (
        echo ERROR: Failed to create mod directory
        pause
        exit /b 1
    )
    echo ✅ Mod directory created
) else (
    echo ✅ Mod directory already exists
)

echo.
echo Creating subdirectories...
if not exist "%MOD_DIR%\scripts" mkdir "%MOD_DIR%\scripts"
if not exist "%MOD_DIR%\scripts\game" mkdir "%MOD_DIR%\scripts\game"
if not exist "%MOD_DIR%\scripts\game\witcher3_mp" mkdir "%MOD_DIR%\scripts\game\witcher3_mp"

echo ✅ Subdirectories created

echo.
echo Copying WitcherScript files...
xcopy "mod\scripts\game\witcher3_mp\*.ws" "%MOD_DIR%\scripts\game\witcher3_mp\" /Y /Q
if !ERRORLEVEL! neq 0 (
    echo ERROR: Failed to copy WitcherScript files
    pause
    exit /b 1
)

echo ✅ WitcherScript files copied

echo.
echo Verifying copied files...
set "COPIED_COUNT=0"
for %%f in ("%MOD_DIR%\scripts\game\witcher3_mp\*.ws") do set /a COPIED_COUNT+=1

echo Files copied: %COPIED_COUNT%

if %COPIED_COUNT% neq %WS_COUNT% (
    echo WARNING: File count mismatch - expected %WS_COUNT%, got %COPIED_COUNT%
) else (
    echo ✅ All files copied successfully
)

echo.
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
echo Press any key to continue...
pause
