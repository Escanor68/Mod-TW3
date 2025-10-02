@echo off
echo ========================================
echo    Creating start_nextgen.bat
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
echo Creating start_nextgen.bat in: %WITCHER3_PATH%
echo.

REM Create start_nextgen.bat
echo @echo off > "%WITCHER3_PATH%\start_nextgen.bat"
echo echo ======================================== >> "%WITCHER3_PATH%\start_nextgen.bat"
echo echo    Starting Witcher3-MP Server... >> "%WITCHER3_PATH%\start_nextgen.bat"
echo echo ======================================== >> "%WITCHER3_PATH%\start_nextgen.bat"
echo echo. >> "%WITCHER3_PATH%\start_nextgen.bat"
echo. >> "%WITCHER3_PATH%\start_nextgen.bat"
echo REM Start the MP server in background >> "%WITCHER3_PATH%\start_nextgen.bat"
echo start /B "" "mods\witcher3_mp\Witcher3-MP.exe" >> "%WITCHER3_PATH%\start_nextgen.bat"
echo. >> "%WITCHER3_PATH%\start_nextgen.bat"
echo echo Server started on port 60000 >> "%WITCHER3_PATH%\start_nextgen.bat"
echo echo Waiting 3 seconds for server to initialize... >> "%WITCHER3_PATH%\start_nextgen.bat"
echo timeout /t 3 /nobreak ^>nul >> "%WITCHER3_PATH%\start_nextgen.bat"
echo. >> "%WITCHER3_PATH%\start_nextgen.bat"
echo REM Launch The Witcher 3 with mod enabled >> "%WITCHER3_PATH%\start_nextgen.bat"
echo echo Launching The Witcher 3 with Witcher3-MP mod... >> "%WITCHER3_PATH%\start_nextgen.bat"
echo. >> "%WITCHER3_PATH%\start_nextgen.bat"
echo REM Try different executable names >> "%WITCHER3_PATH%\start_nextgen.bat"
echo if exist "bin\x64\witcher3.exe" ( >> "%WITCHER3_PATH%\start_nextgen.bat"
echo     "bin\x64\witcher3.exe" -mod witcher3_mp >> "%WITCHER3_PATH%\start_nextgen.bat"
echo ^) else if exist "bin\x64\witcher3_dx12.exe" ( >> "%WITCHER3_PATH%\start_nextgen.bat"
echo     "bin\x64\witcher3_dx12.exe" -mod witcher3_mp >> "%WITCHER3_PATH%\start_nextgen.bat"
echo ^) else if exist "witcher3.exe" ( >> "%WITCHER3_PATH%\start_nextgen.bat"
echo     "witcher3.exe" -mod witcher3_mp >> "%WITCHER3_PATH%\start_nextgen.bat"
echo ^) else ( >> "%WITCHER3_PATH%\start_nextgen.bat"
echo     echo ERROR: Witcher 3 executable not found! >> "%WITCHER3_PATH%\start_nextgen.bat"
echo     echo Please check your installation. >> "%WITCHER3_PATH%\start_nextgen.bat"
echo     pause >> "%WITCHER3_PATH%\start_nextgen.bat"
echo ^) >> "%WITCHER3_PATH%\start_nextgen.bat"
echo. >> "%WITCHER3_PATH%\start_nextgen.bat"
echo REM Cleanup: Kill server when game exits >> "%WITCHER3_PATH%\start_nextgen.bat"
echo taskkill /F /IM "Witcher3-MP.exe" ^>nul 2^>^&1 >> "%WITCHER3_PATH%\start_nextgen.bat"
echo echo Server stopped. >> "%WITCHER3_PATH%\start_nextgen.bat"

echo.
echo ========================================
echo    start_nextgen.bat Created!
echo ========================================
echo.
echo File created: %WITCHER3_PATH%\start_nextgen.bat
echo.
echo To launch The Witcher 3 with the mod:
echo 1. Navigate to: %WITCHER3_PATH%
echo 2. Double-click: start_nextgen.bat
echo.
echo The script will:
echo - Start the Witcher3-MP server on port 60000
echo - Launch The Witcher 3 with the mod enabled
echo - Stop the server when the game exits
echo.

pause
