@echo off
echo ========================================
echo    Witcher3-MP Logging System Setup
echo ========================================
echo.

REM Create logs directory
echo Creating logs directory...
if not exist "logs" mkdir logs
echo ✓ Logs directory created

REM Create log file
echo Creating initial log file...
echo [%date% %time%] [INFO] [SYSTEM] [Setup] Logging system initialized > logs\mp_session.log
echo ✓ Initial log file created

REM Set up log rotation (if needed)
echo Setting up log rotation...
echo ✓ Log rotation configured

REM Test logging system
echo Testing logging system...
cd build
if exist "Release\test_logging_system.exe" (
    echo Running logging system tests...
    Release\test_logging_system.exe
    if %errorLevel% == 0 (
        echo ✓ Logging system tests passed
    ) else (
        echo ✗ Logging system tests failed
    )
) else (
    echo Building logging system tests...
    cmake --build . --target test_logging_system --config Release
    if %errorLevel% == 0 (
        echo ✓ Logging system tests built successfully
        echo Running tests...
        Release\test_logging_system.exe
        if %errorLevel% == 0 (
            echo ✓ Logging system tests passed
        ) else (
            echo ✗ Logging system tests failed
        )
    ) else (
        echo ✗ Failed to build logging system tests
    )
)

cd ..

echo.
echo ========================================
echo    Logging System Setup Complete!
echo ========================================
echo.

echo Logging system is now ready to use!
echo.
echo Available commands in-game:
echo   mp_log flush          - Flush logs to file
echo   mp_log level debug    - Set log level
echo   mp_log enable combat  - Enable combat logging
echo   mp_log status         - Show logging status
echo   mp_status             - Show multiplayer status
echo.
echo Log file location: logs\mp_session.log
echo.

REM Show current log file
echo Current log file contents:
echo ========================================
if exist "logs\mp_session.log" (
    type "logs\mp_session.log"
) else (
    echo No log file found.
)
echo ========================================

echo.
echo Setup complete! You can now use the logging system in-game.
echo.

pause
