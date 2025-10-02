@echo off
echo ========================================
echo    Witcher3-MP Network Connection Test
echo ========================================
echo.

REM Create logs directory if it doesn't exist
if not exist "logs" mkdir logs

echo Starting network connection test...
echo.

REM Compile the test program
echo Compiling network test...
cd build
cmake --build . --target test_network_connection --config Release
if %errorLevel% neq 0 (
    echo ERROR: Failed to compile network test!
    pause
    exit /b 1
)

echo.
echo Running network connection tests...
echo.

REM Run the test
Release\test_network_connection.exe

echo.
echo ========================================
echo    Network Test Complete!
echo ========================================
echo.

REM Show network log file contents
echo Network log file contents:
echo ========================================
if exist "logs\network.log" (
    type "logs\network.log"
) else (
    echo No network log file found.
)

echo.
echo ========================================
echo    Test Results Summary
echo ========================================
echo.
echo The following tests were performed:
echo 1. Server creation and startup
echo 2. Client connection to localhost:7777
echo 3. Message types alignment verification
echo 4. Compression integration test
echo 5. Network logging test
echo 6. Localhost connection with two clients
echo 7. Compression performance test
echo.
echo Check the logs above for detailed results.
echo.

pause
