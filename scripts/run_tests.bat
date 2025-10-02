@echo off
echo === Witcher3-MP Test Runner ===
echo.

echo Compiling simple test...
g++ -std=c++20 -O2 -o test_combat_simple.exe test_combat_simple.cpp
if %ERRORLEVEL% neq 0 (
    echo Compilation failed!
    pause
    exit /b 1
)

echo.
echo Running combat system test...
echo.
test_combat_simple.exe

echo.
echo Test completed!
pause
