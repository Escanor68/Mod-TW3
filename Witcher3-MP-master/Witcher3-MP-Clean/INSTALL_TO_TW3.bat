@echo off
echo ========================================
echo Installing Witcher3-MP to The Witcher 3
echo ========================================
echo.

echo Target directory: E:\SteamLibrary\steamapps\common\The Witcher 3
echo.

REM Check if source mod exists
if not exist "C:\Users\USUARIO\OneDrive\Escritorio\modWitcher3MP" (
    echo ERROR: Source mod not found!
    echo Please run EMERGENCY_INSTALL.bat first
    pause
    exit /b 1
)

echo ✅ Source mod found

REM Check if TW3 directory exists
if not exist "E:\SteamLibrary\steamapps\common\The Witcher 3" (
    echo ERROR: The Witcher 3 directory not found!
    echo Please check the path: E:\SteamLibrary\steamapps\common\The Witcher 3
    pause
    exit /b 1
)

echo ✅ The Witcher 3 directory found

REM Create mods directory if it doesn't exist
if not exist "E:\SteamLibrary\steamapps\common\The Witcher 3\mods" (
    echo Creating mods directory...
    mkdir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods"
    if !ERRORLEVEL! neq 0 (
        echo ERROR: Failed to create mods directory
        pause
        exit /b 1
    )
    echo ✅ Mods directory created
) else (
    echo ✅ Mods directory already exists
)

REM Copy the mod
echo Copying mod to The Witcher 3...
xcopy "C:\Users\USUARIO\OneDrive\Escritorio\modWitcher3MP\*" "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\modWitcher3MP\" /E /I /Y
if !ERRORLEVEL! neq 0 (
    echo ERROR: Failed to copy mod files
    pause
    exit /b 1
)

echo ✅ Mod copied successfully

REM Verify installation
echo Verifying installation...

set "WS_COUNT=0"
for %%f in ("E:\SteamLibrary\steamapps\common\The Witcher 3\mods\modWitcher3MP\scripts\game\witcher3_mp\*.ws") do set /a WS_COUNT+=1

echo Found %WS_COUNT% WitcherScript files

if %WS_COUNT% equ 14 (
    echo ✅ All 14 WitcherScript files copied successfully
) else (
    echo ⚠️ WARNING: Expected 14 files, found %WS_COUNT%
)

REM Check for modInfo.ws
if exist "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\modWitcher3MP\modInfo.ws" (
    echo ✅ modInfo.ws found
) else (
    echo ❌ ERROR: modInfo.ws not found
)

REM List installed mods
echo.
echo Installed mods:
dir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods" /B

echo.
echo ========================================
echo Installation Complete!
echo ========================================
echo.
echo Mod installed to: E:\SteamLibrary\steamapps\common\The Witcher 3\mods\modWitcher3MP
echo WitcherScript files: %WS_COUNT%
echo.
echo Next steps:
echo 1. Install the base mod "Fix sounds of sword hits when adding effects"
echo 2. Start The Witcher 3
echo 3. Check that both mods appear in the mods list
echo 4. Test the combat system in-game
echo.
echo Press any key to continue...
pause
