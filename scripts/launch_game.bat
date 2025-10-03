@echo off
setlocal enabledelayedexpansion

echo.
echo ========================================
echo    Witcher3-MP - Game Launcher
echo ========================================
echo.

REM Set error handling
set "ERROR_COUNT=0"

REM Create logs directory
if not exist "logs" mkdir "logs"

REM Initialize session log
set "SESSION_LOG=logs\session.log"
echo [%date% %time%] Starting Witcher3-MP Game Session > "%SESSION_LOG%"

echo 🔍 Step 1: Detecting Witcher 3 Installation
echo [%date% %time%] Step 1: Detecting Witcher 3 Installation >> "%SESSION_LOG%"

REM Detect Witcher 3 installation path
set "WITCHER3_PATH="
set "WITCHER3_EXE="
set "WITCHER3_MODS_PATH="

REM Try common installation paths
for %%p in (
    "C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3"
    "C:\Program Files\Steam\steamapps\common\The Witcher 3"
    "D:\Steam\steamapps\common\The Witcher 3"
    "E:\Steam\steamapps\common\The Witcher 3"
    "C:\GOG Games\The Witcher 3 Wild Hunt GOTY"
    "D:\GOG Games\The Witcher 3 Wild Hunt GOTY"
) do (
    if exist "%%~p\bin\x64\witcher3_dx12.exe" (
        set "WITCHER3_PATH=%%~p"
        set "WITCHER3_EXE=%%~p\bin\x64\witcher3_dx12.exe"
        set "WITCHER3_MODS_PATH=%%~p\mods"
        echo ✅ Found witcher3_dx12.exe (DirectX 12)
        goto :found_witcher3
    ) else if exist "%%~p\bin\x64\witcher3.exe" (
        set "WITCHER3_PATH=%%~p"
        set "WITCHER3_EXE=%%~p\bin\x64\witcher3.exe"
        set "WITCHER3_MODS_PATH=%%~p\mods"
        echo ✅ Found witcher3.exe (DirectX 11)
        goto :found_witcher3
    )
)

echo ❌ ERROR: The Witcher 3 installation not found!
echo [%date% %time%] ERROR: The Witcher 3 installation not found! >> "%SESSION_LOG%"
echo Please ensure The Witcher 3 is installed and accessible.
set /a ERROR_COUNT+=1
goto :error_exit

:found_witcher3
echo ✅ Found The Witcher 3 at: %WITCHER3_PATH%
echo [%date% %time%] Found The Witcher 3 at: %WITCHER3_PATH% >> "%SESSION_LOG%"
echo [%date% %time%] Using executable: %WITCHER3_EXE% >> "%SESSION_LOG%"

echo.
echo 🔍 Step 2: Checking Mod Installation
echo [%date% %time%] Step 2: Checking Mod Installation >> "%SESSION_LOG%"

REM Check if mod is installed
if not exist "%WITCHER3_MODS_PATH%\witcher3_mp" (
    echo ❌ ERROR: Mod not installed! Please run build_and_test.bat first.
    echo [%date% %time%] ERROR: Mod not installed! >> "%SESSION_LOG%"
    set /a ERROR_COUNT+=1
    goto :error_exit
)

if not exist "%WITCHER3_MODS_PATH%\witcher3_mp\Witcher3-MP.exe" (
    echo ❌ ERROR: Witcher3-MP.exe not found in mod directory!
    echo [%date% %time%] ERROR: Witcher3-MP.exe not found in mod directory! >> "%SESSION_LOG%"
    set /a ERROR_COUNT+=1
    goto :error_exit
)

echo ✅ Mod installation verified
echo [%date% %time%] Mod installation verified >> "%SESSION_LOG%"

echo.
echo ⚙️  Step 3: Reading Game Mode Configuration
echo [%date% %time%] Step 3: Reading Game Mode Configuration >> "%SESSION_LOG%"

REM Read game mode from config
set "GAME_MODE=story"
if exist "%WITCHER3_MODS_PATH%\witcher3_mp\config\mp_config.json" (
    for /f "tokens=2 delims=:" %%a in ('findstr "mode" "%WITCHER3_MODS_PATH%\witcher3_mp\config\mp_config.json"') do (
        set "MODE_LINE=%%a"
        set "MODE_LINE=!MODE_LINE: =!"
        set "MODE_LINE=!MODE_LINE:"=!"
        set "MODE_LINE=!MODE_LINE:,=!"
        if "!MODE_LINE!" neq "" (
            set "GAME_MODE=!MODE_LINE!"
        )
    )
)

echo ✅ Game mode detected: %GAME_MODE%
echo [%date% %time%] Game mode detected: %GAME_MODE% >> "%SESSION_LOG%"

echo.
echo 🚀 Step 4: Starting Multiplayer Server
echo [%date% %time%] Step 4: Starting Multiplayer Server >> "%SESSION_LOG%"

REM Start the server in background
echo Starting Witcher3-MP server...
start /B "" "%WITCHER3_MODS_PATH%\witcher3_mp\Witcher3-MP.exe" --server --mode=%GAME_MODE% --log-file="%SESSION_LOG%"

REM Wait a moment for server to start
timeout /t 3 /nobreak >nul

REM Check if server is running
tasklist /FI "IMAGENAME eq Witcher3-MP.exe" 2>NUL | find /I /N "Witcher3-MP.exe" >nul
if %errorlevel% equ 0 (
    echo ✅ Server started successfully
    echo [%date% %time%] Server started successfully >> "%SESSION_LOG%"
) else (
    echo ❌ WARNING: Server may not have started properly
    echo [%date% %time%] WARNING: Server may not have started properly >> "%SESSION_LOG%"
)

echo.
echo 🎮 Step 5: Launching The Witcher 3
echo [%date% %time%] Step 5: Launching The Witcher 3 >> "%SESSION_LOG%"

REM Launch The Witcher 3 with mod
echo Launching The Witcher 3 with witcher3_mp mod...
echo [%date% %time%] Launching The Witcher 3 with witcher3_mp mod... >> "%SESSION_LOG%"

REM Change to Witcher 3 directory
cd /d "%WITCHER3_PATH%\bin\x64"

REM Launch the game
start "" "%WITCHER3_EXE%" -mod witcher3_mp

echo ✅ Game launched with mod
echo [%date% %time%] Game launched with mod >> "%SESSION_LOG%"

echo.
echo 📊 Step 6: Session Summary
echo [%date% %time%] Step 6: Session Summary >> "%SESSION_LOG%"

echo ======================================== >> "%SESSION_LOG%"
echo    Witcher3-MP Session Summary >> "%SESSION_LOG%"
echo ======================================== >> "%SESSION_LOG%"
echo. >> "%SESSION_LOG%"
echo Session Date: %date% %time% >> "%SESSION_LOG%"
echo Game Mode: %GAME_MODE% >> "%SESSION_LOG%"
echo Witcher 3 Path: %WITCHER3_PATH% >> "%SESSION_LOG%"
echo Executable: %WITCHER3_EXE% >> "%SESSION_LOG%"
echo Mod Path: %WITCHER3_MODS_PATH%\witcher3_mp >> "%SESSION_LOG%"
echo. >> "%SESSION_LOG%"
echo Status: Game launched successfully >> "%SESSION_LOG%"
echo. >> "%SESSION_LOG%"

echo.
echo 🎉 SUCCESS! The Witcher 3 has been launched with the witcher3_mp mod.
echo.
echo Session Information:
echo   - Game Mode: %GAME_MODE%
echo   - Server: Running in background
echo   - Log File: %SESSION_LOG%
echo.
echo The mod should now be visible in the game's main menu under "Mods".
echo.
echo Press any key to close this window...
pause >nul

exit /b 0

:error_exit
echo.
echo ❌ Game launch failed with %ERROR_COUNT% errors.
echo Check the session log for detailed information: %SESSION_LOG%
echo.
pause
exit /b %ERROR_COUNT%
