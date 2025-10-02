@echo off
echo ========================================
echo    Witcher3-MP Logging System Demo
echo ========================================
echo.

REM Create logs directory if it doesn't exist
if not exist "logs" mkdir logs

echo Starting logging system demonstration...
echo.

REM Compile the test program
echo Compiling logging system test...
cd build
cmake --build . --target test_logging_system --config Release
if %errorLevel% neq 0 (
    echo ERROR: Failed to compile logging system test!
    pause
    exit /b 1
)

echo.
echo Running logging system tests...
echo.

REM Run the test
Release\test_logging_system.exe

echo.
echo ========================================
echo    Logging Demo Complete!
echo ========================================
echo.

REM Show log file contents
echo Log file contents:
echo ========================================
if exist "logs\mp_session.log" (
    type "logs\mp_session.log"
) else (
    echo No log file found.
)

echo.
echo ========================================
echo    Console Commands Demo
echo ========================================
echo.
echo You can now test the console commands:
echo.
echo 1. Start the game and open the console (~ key)
echo 2. Try these commands:
echo    - mp_log status
echo    - mp_log flush
echo    - mp_log level debug
echo    - mp_log enable combat
echo    - mp_log disable network
echo    - mp_status
echo    - mp_quest list
echo    - mp_inventory status
echo    - mp_network status
echo    - help
echo.
echo 3. Check the log file: logs\mp_session.log
echo.

pause
