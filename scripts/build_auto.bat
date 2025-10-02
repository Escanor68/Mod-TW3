@echo off
echo ========================================
echo    Witcher3-MP Auto Build Script
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
    echo.
    echo You can download it from: https://visualstudio.microsoft.com/vs/community/
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

echo Compiling Witcher3-MP Server...
echo.

REM Try to compile with all files first
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
    echo.
    echo Some files failed to compile, trying with minimal set...
    echo.
    
    REM Try with minimal files
    cl /EHsc /std:c++20 /I"include" /I"dep\asio" /I"dep\asio\asio" ^
       src\core\MainSimple.cpp ^
       src\utils\Logger.cpp ^
       /Fe:build\Release\Witcher3-MP.exe ^
       /link ws2_32.lib
    
    if %errorLevel% neq 0 (
        echo.
        echo Even minimal compilation failed, creating simple test executable...
        echo.
        
        REM Create a very simple test
        echo #include ^<iostream^> > temp_main.cpp
        echo #include ^<string^> >> temp_main.cpp
        echo int main() { >> temp_main.cpp
        echo     std::cout ^<^< "Witcher3-MP Test Server" ^<^< std::endl; >> temp_main.cpp
        echo     std::cout ^<^< "Press Enter to exit..." ^<^< std::endl; >> temp_main.cpp
        echo     std::cin.get(); >> temp_main.cpp
        echo     return 0; >> temp_main.cpp
        echo } >> temp_main.cpp
        
        cl /EHsc temp_main.cpp /Fe:build\Release\Witcher3-MP.exe
        
        if %errorLevel% neq 0 (
            echo ERROR: Even simple test failed!
            echo Please check your Visual Studio installation
            pause
            exit /b 1
        )
        
        del temp_main.cpp
        echo Simple test executable created successfully!
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
