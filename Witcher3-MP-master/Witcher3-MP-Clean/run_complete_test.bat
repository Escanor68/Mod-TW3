@echo off
echo ========================================
echo Witcher3-MP Complete Validation Test
echo ========================================
echo.

echo Compiling complete validation test...
g++ -std=c++20 -O2 -o COMPLETE_VALIDATION_TEST.exe COMPLETE_VALIDATION_TEST.cpp
if %ERRORLEVEL% neq 0 (
    echo.
    echo ❌ Compilation failed!
    echo Please check your C++ compiler installation.
    pause
    exit /b 1
)

echo.
echo ✅ Compilation successful!
echo.
echo Running complete validation test...
echo.

COMPLETE_VALIDATION_TEST.exe

echo.
echo ========================================
echo Test completed!
echo ========================================
pause
