@echo off
echo ========================================
echo  WITCHER 3 MULTIPLAYER - NEXT-GEN SERVER
echo ========================================
echo.

REM Check if Next-Gen server exists
if not exist "build\nextgen\Witcher3MP_NextGen.exe" (
    echo Error: Next-Gen server not found!
    echo Please run build_nextgen.bat first to compile the server.
    pause
    exit /b 1
)

REM Check if The Witcher 3 Next-Gen is running
echo Checking for The Witcher 3 Next-Gen...
tasklist /FI "IMAGENAME eq witcher3.exe" 2>NUL | find /I /N "witcher3.exe">NUL
if "%ERRORLEVEL%"=="0" (
    echo The Witcher 3 Next-Gen detected! Starting optimized server...
) else (
    echo Warning: The Witcher 3 Next-Gen not detected.
    echo The server will start with default Next-Gen settings.
)

echo.
echo Starting Next-Gen server...
echo.

REM Change to build directory
cd /d "build\nextgen"

REM Start Next-Gen server
echo ========================================
echo  NEXT-GEN SERVER STARTING...
echo ========================================
echo.
echo Features enabled:
echo   - Ray Tracing optimization
echo   - DLSS support
echo   - FSR support
echo   - HDR support
echo   - RTX features
echo   - Low latency networking
echo   - Real-time adaptation
echo   - Performance monitoring
echo.
echo Press Ctrl+C to stop the server
echo.

Witcher3MP_NextGen.exe

echo.
echo Next-Gen server stopped.
pause
