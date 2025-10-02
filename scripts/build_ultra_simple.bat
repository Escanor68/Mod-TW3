@echo off
echo ========================================
echo    Witcher3-MP Ultra Simple Build
echo ========================================
echo.

REM Try to find and initialize Visual Studio
echo Looking for Visual Studio...
set "VS_PATH="

if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    echo Found Visual Studio 2022 Community
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
    echo Found Visual Studio 2022 Professional
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    echo Found Visual Studio 2022 Enterprise
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
    echo Found Visual Studio 2019 Community
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"
    echo Found Visual Studio 2019 Professional
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    echo Found Visual Studio 2019 Enterprise
)

if "%VS_PATH%"=="" (
    echo ERROR: Visual Studio not found!
    echo Please install Visual Studio 2019 or 2022 with C++ support
    pause
    exit /b 1
)

echo.
echo Initializing Visual Studio environment...
call "%VS_PATH%"

echo.
echo Environment initialized! Now compiling...
echo.

REM Create build directory
if not exist "build" mkdir build
if not exist "build\Release" mkdir build\Release

echo Compiling ultra-simple Witcher3-MP Server...
echo.

REM Compile only the ultra-simple main
cl /EHsc /std:c++20 src\core\MainUltraSimple.cpp /Fe:build\Release\Witcher3-MP.exe

if %errorLevel% neq 0 (
    echo ERROR: Compilation failed!
    pause
    exit /b 1
)

echo.
echo Copying configuration files...
if exist "config" xcopy "config\*" "build\Release\config\" /E /I /Y
if exist "mod" xcopy "mod\*" "build\Release\mod\" /E /I /Y

echo.
echo ========================================
echo    Build Complete!
echo ========================================
echo.
echo Executable built: build\Release\Witcher3-MP.exe
echo.
echo To test the server: build\Release\Witcher3-MP.exe
echo.

pause
