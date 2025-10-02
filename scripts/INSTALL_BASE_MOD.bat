@echo off
echo ========================================
echo Installing Base Mod for Witcher3-MP
echo ========================================
echo.

echo This script will download and install the required base mod:
echo "Fix sounds of sword hits when adding effects"
echo.

REM Create temp directory
if not exist "temp" mkdir "temp"

echo Step 1: Downloading base mod...
echo.

REM Download the mod (using PowerShell for better compatibility)
powershell -Command "& {[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri 'https://github.com/ModdingCommunity/Witcher3-MP-BaseMod/archive/refs/heads/main.zip' -OutFile 'temp\base_mod.zip' -UseBasicParsing}"

if !ERRORLEVEL! neq 0 (
    echo ERROR: Failed to download base mod
    echo.
    echo Manual installation required:
    echo 1. Go to: https://www.nexusmods.com/witcher3/mods/11267
    echo 2. Download "Fix sounds of sword hits when adding effects"
    echo 3. Extract to: E:\SteamLibrary\steamapps\common\The Witcher 3\mods\
    echo.
    pause
    exit /b 1
)

echo ✅ Base mod downloaded successfully
echo.

echo Step 2: Extracting base mod...
powershell -Command "Expand-Archive -Path 'temp\base_mod.zip' -DestinationPath 'temp\base_mod_extracted' -Force"

if !ERRORLEVEL! neq 0 (
    echo ERROR: Failed to extract base mod
    pause
    exit /b 1
)

echo ✅ Base mod extracted successfully
echo.

echo Step 3: Installing base mod to The Witcher 3...
echo Target: E:\SteamLibrary\steamapps\common\The Witcher 3\mods\

REM Find the mod directory in extracted files
for /d %%d in ("temp\base_mod_extracted\*") do (
    echo Found mod directory: %%d
    xcopy "%%d\*" "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\" /E /I /Y
    if !ERRORLEVEL! equ 0 (
        echo ✅ Base mod installed successfully
        goto :success
    )
)

echo ERROR: Failed to install base mod
goto :cleanup

:success
echo.
echo Step 4: Verifying installation...
if exist "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits" (
    echo ✅ Base mod directory found
) else (
    echo ❌ Base mod directory not found
)

if exist "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\modWitcher3MP" (
    echo ✅ Our mod still present
) else (
    echo ❌ Our mod missing!
)

echo.
echo Step 5: Listing all installed mods...
dir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods" /B

echo.
echo ========================================
echo Installation Complete!
echo ========================================
echo.
echo Both mods are now installed:
echo - FixSoundsOfSwordHits (Base mod)
echo - modWitcher3MP (Our mod)
echo.
echo Next steps:
echo 1. Start The Witcher 3
echo 2. Check that both mods appear in the mods list
echo 3. Test the combat system in-game
echo.

:cleanup
echo Cleaning up temporary files...
rmdir /s /q "temp" 2>nul

echo.
echo Press any key to continue...
pause
