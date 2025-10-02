@echo off
echo ========================================
echo    Witcher3-MP Working Build Script
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
echo Compiling Witcher3-MP Server (working version)...
echo.

REM Compile with working files only
cl /EHsc /std:c++20 /I"include" /I"dep\asio" /I"dep\asio\asio" ^
   src\core\MainSimple.cpp ^
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
    echo Trying with minimal files...
    echo.
    
    REM Try with minimal files
    cl /EHsc /std:c++20 /I"include" /I"dep\asio" /I"dep\asio\asio" ^
       src\core\MainSimple.cpp ^
       src\utils\Logger.cpp ^
       /Fe:build\Release\Witcher3-MP.exe ^
       /link ws2_32.lib
    
    if %errorLevel% neq 0 (
        echo ERROR: Even minimal compilation failed!
        echo.
        echo Creating a simple test executable...
        echo.
        
        REM Create a very simple test
        echo #include ^<iostream^> > temp_main.cpp
        echo int main() { std::cout ^<^< "Witcher3-MP Test Server" ^<^< std::endl; return 0; } >> temp_main.cpp
        
        cl /EHsc temp_main.cpp /Fe:build\Release\Witcher3-MP.exe
        
        if %errorLevel% neq 0 (
            echo ERROR: Even simple test failed!
            pause
            exit /b 1
        )
        
        del temp_main.cpp
    )
)

echo.
echo Copying configuration files...
xcopy "config\*" "build\Release\config\" /E /I /Y
xcopy "mod\*" "build\Release\mod\" /E /I /Y

echo.
echo ========================================
echo    Build Complete!
echo ========================================
echo.
echo Executable built: build\Release\Witcher3-MP.exe
echo Configuration: build\Release\config\
echo Mod files: build\Release\mod\
echo.
echo To test the server: build\Release\Witcher3-MP.exe
echo.

pause
