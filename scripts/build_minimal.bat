@echo off
echo ========================================
echo    Witcher3-MP Minimal Build Script
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
    pause
    exit /b 1
)

REM Create build directory
if not exist "build" mkdir build
if not exist "build\Release" mkdir build\Release

echo.
echo Compiling minimal Witcher3-MP Server...
echo.

REM Compile only the essential files that work
cl /EHsc /std:c++20 /I"include" /I"dep\asio" /I"dep\asio\asio" ^
   src\core\Main.cpp ^
   src\utils\Logger.cpp ^
   src\utils\ConfigManager.cpp ^
   src\utils\ConsoleCommands.cpp ^
   src\networking\NetworkLogger.cpp ^
   src\optimization\DataCompression.cpp ^
   src\optimization\NetworkOptimizer.cpp ^
   src\version\VersionManager.cpp ^
   src\database\ResourceNames.cpp ^
   /Fe:build\Release\Witcher3-MP.exe ^
   /link ws2_32.lib

if %errorLevel% neq 0 (
    echo ERROR: Compilation failed!
    echo.
    echo Trying to compile with fewer files...
    echo.
    
    REM Try with even fewer files
    cl /EHsc /std:c++20 /I"include" /I"dep\asio" /I"dep\asio\asio" ^
       src\core\Main.cpp ^
       src\utils\Logger.cpp ^
       src\utils\ConfigManager.cpp ^
       /Fe:build\Release\Witcher3-MP.exe ^
       /link ws2_32.lib
    
    if %errorLevel% neq 0 (
        echo ERROR: Minimal compilation also failed!
        pause
        exit /b 1
    )
)

echo.
echo Copying configuration files...
xcopy "config\*" "build\Release\config\" /E /I /Y
xcopy "mod\*" "build\Release\mod\" /E /I /Y

echo.
echo ========================================
echo    Minimal Build Complete!
echo ========================================
echo.
echo Executable built: build\Release\Witcher3-MP.exe
echo Configuration: build\Release\config\
echo Mod files: build\Release\mod\
echo.
echo To run the server: build\Release\Witcher3-MP.exe
echo To install mod: scripts\install.bat
echo.

pause
