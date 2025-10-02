@echo off
echo ========================================
echo  WITCHER 3 MULTIPLAYER - NEXT-GEN BUILD
echo ========================================
echo.

REM Check if Visual Studio is available
where cl >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: Visual Studio compiler not found!
    echo Please run this from a Visual Studio Developer Command Prompt
    echo or install Visual Studio Build Tools
    pause
    exit /b 1
)

echo Building Next-Gen server...
echo.

REM Set build directory
set BUILD_DIR=build\nextgen
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

REM Set source directory
set SRC_DIR=src
set INCLUDE_DIR=include
set CONFIG_DIR=config

REM Compiler flags
set CFLAGS=/std:c++17 /EHsc /O2 /I"%INCLUDE_DIR%" /I"dep\asio" /D_WIN32_WINNT=0x0A00 /DASIO_STANDALONE

REM Source files
set SOURCES=^
"%SRC_DIR%\core\NextGenMain.cpp" ^
"%SRC_DIR%\networking\net_server.cpp" ^
"%SRC_DIR%\networking\net_client.cpp" ^
"%SRC_DIR%\networking\net_connection.cpp" ^
"%SRC_DIR%\networking\net_message.cpp" ^
"%SRC_DIR%\networking\net_tsqueue.cpp" ^
"%SRC_DIR%\optimization\NetworkOptimizer.cpp" ^
"%SRC_DIR%\optimization\LowLatencyServer.cpp" ^
"%SRC_DIR%\optimization\DynamicLowLatencyServer.cpp" ^
"%SRC_DIR%\optimization\DynamicNetworkOptimizer.cpp" ^
"%SRC_DIR%\optimization\NextGenServer.cpp" ^
"%SRC_DIR%\version\VersionManager.cpp" ^
"%SRC_DIR%\version\DynamicVersionManager.cpp" ^
"%SRC_DIR%\version\NextGenVersionManager.cpp" ^
"%SRC_DIR%\utils\Logger.cpp" ^
"%SRC_DIR%\utils\ConfigManager.cpp" ^
"%SRC_DIR%\game\Entities\Npc\Npc.cpp" ^
"%SRC_DIR%\game\Entities\Player\Player.cpp"

REM Libraries
set LIBS=ws2_32.lib

REM Compile Next-Gen server
echo Compiling Next-Gen server...
cl %CFLAGS% %SOURCES% /Fe:"%BUILD_DIR%\Witcher3MP_NextGen.exe" /link %LIBS%

if %ERRORLEVEL% neq 0 (
    echo.
    echo ERROR: Compilation failed!
    echo Please check the error messages above.
    pause
    exit /b 1
)

echo.
echo ========================================
echo  NEXT-GEN BUILD COMPLETED SUCCESSFULLY!
echo ========================================
echo.
echo Executable: %BUILD_DIR%\Witcher3MP_NextGen.exe
echo.

REM Copy configuration files
echo Copying configuration files...
if not exist "%BUILD_DIR%\config" mkdir "%BUILD_DIR%\config"
copy "%CONFIG_DIR%\*.json" "%BUILD_DIR%\config\" >nul 2>nul
copy "%CONFIG_DIR%\nextgen_*.json" "%BUILD_DIR%\config\" >nul 2>nul

REM Copy ASIO library
echo Copying ASIO library...
if not exist "%BUILD_DIR%\dep" mkdir "%BUILD_DIR%\dep"
if not exist "%BUILD_DIR%\dep\asio" mkdir "%BUILD_DIR%\dep\asio"
xcopy "dep\asio\*" "%BUILD_DIR%\dep\asio\" /E /I /Q >nul 2>nul

echo.
echo ========================================
echo  NEXT-GEN SERVER READY TO RUN!
echo ========================================
echo.
echo To start the Next-Gen server:
echo   1. Navigate to: %BUILD_DIR%
echo   2. Run: Witcher3MP_NextGen.exe
echo.
echo The server will automatically:
echo   - Detect The Witcher 3 Next-Gen version
echo   - Apply optimal Next-Gen settings
echo   - Enable Ray Tracing, DLSS, FSR, HDR, RTX
echo   - Optimize for low latency
echo   - Monitor performance in real-time
echo.
echo Press any key to continue...
pause >nul
