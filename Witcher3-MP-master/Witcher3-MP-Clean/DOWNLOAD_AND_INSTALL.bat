@echo off
echo ========================================
echo Auto-Install Base Mod for Witcher3-MP
echo ========================================
echo.

echo This will automatically download and install the base mod.
echo.

REM Create temp directory
if not exist "temp" mkdir "temp"

echo Downloading base mod from GitHub...
powershell -Command "Invoke-WebRequest -Uri 'https://github.com/ModdingCommunity/Witcher3-MP-BaseMod/archive/refs/heads/main.zip' -OutFile 'temp\base_mod.zip'"

if exist "temp\base_mod.zip" (
    echo ✅ Download successful
) else (
    echo ❌ Download failed - trying alternative method...
    echo.
    echo Please download manually from:
    echo https://www.nexusmods.com/witcher3/mods/11267
    echo.
    echo Then extract to: E:\SteamLibrary\steamapps\common\The Witcher 3\mods\
    pause
    exit /b 1
)

echo Extracting mod...
powershell -Command "Expand-Archive -Path 'temp\base_mod.zip' -DestinationPath 'temp\extracted' -Force"

echo Installing to The Witcher 3...
xcopy "temp\extracted\*\*" "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\" /E /I /Y

echo Verifying installation...
if exist "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits" (
    echo ✅ Base mod installed successfully
) else (
    echo ❌ Installation failed
)

echo.
echo All mods installed:
dir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods" /B

echo.
echo Cleaning up...
rmdir /s /q "temp" 2>nul

echo.
echo Installation complete! Start The Witcher 3 to test.
pause
