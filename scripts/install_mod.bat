@echo off
echo ========================================
echo    Witcher3-MP Mod Installation
echo ========================================
echo.

REM Common Witcher 3 installation paths
set "WITCHER3_PATHS[0]=C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3"
set "WITCHER3_PATHS[1]=C:\Program Files\Steam\steamapps\common\The Witcher 3"
set "WITCHER3_PATHS[2]=C:\GOG Games\The Witcher 3 Wild Hunt GOTY"
set "WITCHER3_PATHS[3]=C:\Games\The Witcher 3 Wild Hunt"
set "WITCHER3_PATHS[4]=D:\Steam\steamapps\common\The Witcher 3"
set "WITCHER3_PATHS[5]=D:\GOG Games\The Witcher 3 Wild Hunt GOTY"

echo Looking for The Witcher 3 installation...
set "WITCHER3_PATH="

for /L %%i in (0,1,5) do (
    if exist "!WITCHER3_PATHS[%%i]!" (
        set "WITCHER3_PATH=!WITCHER3_PATHS[%%i]!"
        echo Found The Witcher 3 at: !WITCHER3_PATH!
        goto :found
    )
)

echo.
echo ERROR: The Witcher 3 installation not found!
echo.
echo Please manually specify the path to your Witcher 3 installation:
echo (Example: C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3)
echo.
set /p "WITCHER3_PATH=Enter Witcher 3 path: "

if not exist "%WITCHER3_PATH%" (
    echo ERROR: Invalid path specified!
    pause
    exit /b 1
)

:found
echo.
echo Installing Witcher3-MP mod to: %WITCHER3_PATH%\mods\witcher3_mp
echo.

REM Create mods directory if it doesn't exist
if not exist "%WITCHER3_PATH%\mods" (
    echo Creating mods directory...
    mkdir "%WITCHER3_PATH%\mods"
)

REM Create witcher3_mp mod directory
if not exist "%WITCHER3_PATH%\mods\witcher3_mp" (
    echo Creating witcher3_mp mod directory...
    mkdir "%WITCHER3_PATH%\mods\witcher3_mp"
)

REM Copy mod files
echo Copying mod files...
xcopy "mod\*" "%WITCHER3_PATH%\mods\witcher3_mp\" /E /I /Y

if %errorlevel% neq 0 (
    echo ERROR: Failed to copy mod files!
    pause
    exit /b 1
)

echo.
echo Copying server executable...
copy "build\Release\Witcher3-MP.exe" "%WITCHER3_PATH%\mods\witcher3_mp\"

if %errorlevel% neq 0 (
    echo WARNING: Failed to copy server executable!
    echo You can manually copy build\Release\Witcher3-MP.exe to the mod directory
)

echo.
echo ========================================
echo    Installation Complete!
echo ========================================
echo.
echo Mod installed to: %WITCHER3_PATH%\mods\witcher3_mp
echo.
echo Next steps:
echo 1. Run scripts\create_startup_script.bat to create start_nextgen.bat
echo 2. Launch The Witcher 3 with the mod enabled
echo 3. The server will start automatically on port 60000
echo.

pause
