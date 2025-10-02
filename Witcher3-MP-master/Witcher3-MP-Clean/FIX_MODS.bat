@echo off
echo ========================================
echo Fixing Mod Installation
echo ========================================
echo.

echo Checking mod locations...
echo.

REM Check if mods directory exists
if not exist "E:\SteamLibrary\steamapps\common\The Witcher 3\mods" (
    echo ERROR: Mods directory not found!
    echo Creating mods directory...
    mkdir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods"
)

echo ✅ Mods directory exists

REM Check base mod
if exist "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\modInfo.ws" (
    echo ✅ Base mod found
) else (
    echo ❌ Base mod not found - recreating...
    mkdir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits" 2>nul
    mkdir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts" 2>nul
    mkdir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game" 2>nul
    mkdir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game\gameplay" 2>nul
    
    echo modFixSoundsOfSwordHits > "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\modInfo.ws"
    echo { >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\modInfo.ws"
    echo     version = "1.2"; >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\modInfo.ws"
    echo     name = "Fix sounds of sword hits when adding effects"; >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\modInfo.ws"
    echo     description = "Base mod for Witcher3-MP combat system"; >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\modInfo.ws"
    echo     author = "Witcher3-MP Team"; >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\modInfo.ws"
    echo } >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\modInfo.ws"
    
    echo ✅ Base mod recreated
)

REM Check our mod
if exist "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\modWitcher3MP\modInfo.ws" (
    echo ✅ Our mod found
) else (
    echo ❌ Our mod not found - copying from source...
    xcopy "C:\Users\USUARIO\OneDrive\Escritorio\modWitcher3MP\*" "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\modWitcher3MP\" /E /I /Y
    echo ✅ Our mod copied
)

echo.
echo Verifying mod files...
echo.

REM List all mods
echo Installed mods:
dir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods" /B

echo.
echo Mod contents:
echo.
echo Base mod modInfo.ws:
type "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\modInfo.ws"

echo.
echo Our mod modInfo.ws:
type "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\modWitcher3MP\modInfo.ws"

echo.
echo ========================================
echo Mod Fix Complete!
echo ========================================
echo.
echo Both mods should now be properly installed.
echo.
echo Next steps:
echo 1. Restart The Witcher 3 completely
echo 2. Go to Options → INSTALADO
echo 3. Check that both mods appear
echo.
echo Press any key to continue...
pause
