@echo off
echo ========================================
echo    Witcher3-MP Build Script
echo ========================================
echo.

REM Check for Visual Studio
echo Checking for Visual Studio...
where cl >nul 2>&1
if %errorLevel% == 0 (
    echo Visual Studio compiler found... OK
) else (
    echo ERROR: Visual Studio compiler not found!
    echo Please run this script from a Visual Studio Developer Command Prompt
    echo or install Visual Studio 2019/2022 with C++ support
    pause
    exit /b 1
)

REM Check for CMake
echo Checking for CMake...
where cmake >nul 2>&1
if %errorLevel% == 0 (
    echo CMake found... OK
) else (
    echo ERROR: CMake not found!
    echo Please install CMake and add it to your PATH
    pause
    exit /b 1
)

REM Create build directory
if not exist "build" (
    echo Creating build directory...
    mkdir build
)

cd build

REM Configure with CMake
echo Configuring with CMake...
cmake .. -G "Visual Studio 17 2022" -A x64
if %errorLevel% neq 0 (
    echo ERROR: CMake configuration failed!
    pause
    exit /b 1
)

REM Build the project
echo Building project...
cmake --build . --config Release
if %errorLevel% neq 0 (
    echo ERROR: Build failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo    Build Complete!
echo ========================================
echo.
echo Executables built in: build\Release\
echo.
echo Available executables:
echo - Witcher3-MP-Standard-Release.exe
echo - Witcher3-MP-LowLatency-Release.exe
echo - Witcher3-MP-Dynamic-Release.exe
echo - Witcher3-MP-NextGen-Release.exe
echo.
echo To run tests: scripts\run_tests.bat
echo To install mod: scripts\install.bat
echo.
pause
