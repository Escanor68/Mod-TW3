@echo off
echo Emergency Installation - Minimal Setup
echo ========================================
echo.

echo Creating mod directory...
mkdir "..\..\modWitcher3MP" 2>nul
mkdir "..\..\modWitcher3MP\scripts" 2>nul
mkdir "..\..\modWitcher3MP\scripts\game" 2>nul
mkdir "..\..\modWitcher3MP\scripts\game\witcher3_mp" 2>nul

echo Copying files...
copy "mod\scripts\game\witcher3_mp\*.ws" "..\..\modWitcher3MP\scripts\game\witcher3_mp\" /Y >nul

echo Creating modInfo.ws...
echo modWitcher3MP > "..\..\modWitcher3MP\modInfo.ws"
echo { >> "..\..\modWitcher3MP\modInfo.ws"
echo     version = "1.0.0"; >> "..\..\modWitcher3MP\modInfo.ws"
echo     name = "Witcher3-MP Multiplayer Mod"; >> "..\..\modWitcher3MP\modInfo.ws"
echo     description = "Multiplayer combat system for The Witcher 3 Next-Gen"; >> "..\..\modWitcher3MP\modInfo.ws"
echo     author = "Witcher3-MP Team"; >> "..\..\modWitcher3MP\modInfo.ws"
echo } >> "..\..\modWitcher3MP\modInfo.ws"

echo.
echo Emergency installation complete!
echo Mod directory: ..\..\modWitcher3MP
echo.
pause
