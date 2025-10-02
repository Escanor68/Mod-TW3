@echo off
echo ========================================
echo  WITCHER 3 MULTIPLAYER - BUILD ALL
echo ========================================
echo.

REM Check if Visual Studio is available
where cl >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Visual Studio compiler not found!
    echo Please run this from a Visual Studio Developer Command Prompt
    pause
    exit /b 1
)

echo [1/8] Cleaning previous build...
if exist "build" rmdir /s /q "build"
if exist "Debug" rmdir /s /q "Debug"
if exist "Release" rmdir /s /q "Release"
if exist "*.obj" del /q "*.obj"
if exist "*.pdb" del /q "*.pdb"
if exist "*.ilk" del /q "*.ilk"
if exist "*.exp" del /q "*.exp"
if exist "*.lib" del /q "*.lib"

echo [2/8] Creating output directories...
mkdir "build" 2>nul
mkdir "build\Debug" 2>nul
mkdir "build\Release" 2>nul

echo [3/8] Compiling Standard Server (Debug)...
cl /EHsc /std:c++20 /I"include" /I"dep\asio" /D"_DEBUG" /D"_CONSOLE" /D"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING" /Zi /Od /RTC1 /W3 /nologo ^
    src\core\Main.cpp ^
    src\networking\net_server.cpp ^
    src\networking\net_client.cpp ^
    src\networking\net_connection.cpp ^
    src\networking\net_message.cpp ^
    src\networking\net_tsqueue.cpp ^
    src\utils\Logger.cpp ^
    src\utils\ConfigManager.cpp ^
    src\game\Entities\Npc\Npc.cpp ^
    src\game\Entities\Player\Player.cpp ^
    /Fe:build\Debug\Witcher3-MP-Standard-Debug.exe /link /SUBSYSTEM:CONSOLE

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Standard Server Debug compilation failed!
    pause
    exit /b 1
)

echo [4/8] Compiling Standard Server (Release)...
cl /EHsc /std:c++20 /I"include" /I"dep\asio" /D"NDEBUG" /D"_CONSOLE" /D"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING" /O2 /Ot /GL /W3 /nologo ^
    src\core\Main.cpp ^
    src\networking\net_server.cpp ^
    src\networking\net_client.cpp ^
    src\networking\net_connection.cpp ^
    src\networking\net_message.cpp ^
    src\networking\net_tsqueue.cpp ^
    src\utils\Logger.cpp ^
    src\utils\ConfigManager.cpp ^
    src\game\Entities\Npc\Npc.cpp ^
    src\game\Entities\Player\Player.cpp ^
    /Fe:build\Release\Witcher3-MP-Standard-Release.exe /link /SUBSYSTEM:CONSOLE /LTCG /OPT:REF /OPT:ICF

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Standard Server Release compilation failed!
    pause
    exit /b 1
)

echo [5/8] Compiling Low Latency Server (Debug)...
cl /EHsc /std:c++20 /I"include" /I"dep\asio" /D"_DEBUG" /D"_CONSOLE" /D"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING" /Zi /Od /RTC1 /W3 /nologo ^
    src\core\LowLatencyMain.cpp ^
    src\networking\net_server.cpp ^
    src\networking\net_client.cpp ^
    src\networking\net_connection.cpp ^
    src\networking\net_message.cpp ^
    src\networking\net_tsqueue.cpp ^
    src\optimization\NetworkOptimizer.cpp ^
    src\optimization\LowLatencyServer.cpp ^
    src\utils\Logger.cpp ^
    src\utils\ConfigManager.cpp ^
    src\version\VersionManager.cpp ^
    src\game\Entities\Npc\Npc.cpp ^
    src\game\Entities\Player\Player.cpp ^
    /Fe:build\Debug\Witcher3-MP-LowLatency-Debug.exe /link /SUBSYSTEM:CONSOLE

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Low Latency Server Debug compilation failed!
    pause
    exit /b 1
)

echo [6/8] Compiling Low Latency Server (Release)...
cl /EHsc /std:c++20 /I"include" /I"dep\asio" /D"NDEBUG" /D"_CONSOLE" /D"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING" /O2 /Ot /GL /W3 /nologo ^
    src\core\LowLatencyMain.cpp ^
    src\networking\net_server.cpp ^
    src\networking\net_client.cpp ^
    src\networking\net_connection.cpp ^
    src\networking\net_message.cpp ^
    src\networking\net_tsqueue.cpp ^
    src\optimization\NetworkOptimizer.cpp ^
    src\optimization\LowLatencyServer.cpp ^
    src\utils\Logger.cpp ^
    src\utils\ConfigManager.cpp ^
    src\version\VersionManager.cpp ^
    src\game\Entities\Npc\Npc.cpp ^
    src\game\Entities\Player\Player.cpp ^
    /Fe:build\Release\Witcher3-MP-LowLatency-Release.exe /link /SUBSYSTEM:CONSOLE /LTCG /OPT:REF /OPT:ICF

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Low Latency Server Release compilation failed!
    pause
    exit /b 1
)

echo [7/8] Compiling Dynamic Server (Debug)...
cl /EHsc /std:c++20 /I"include" /I"dep\asio" /D"_DEBUG" /D"_CONSOLE" /D"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING" /Zi /Od /RTC1 /W3 /nologo ^
    src\core\DynamicMain.cpp ^
    src\networking\net_server.cpp ^
    src\networking\net_client.cpp ^
    src\networking\net_connection.cpp ^
    src\networking\net_message.cpp ^
    src\networking\net_tsqueue.cpp ^
    src\optimization\NetworkOptimizer.cpp ^
    src\optimization\LowLatencyServer.cpp ^
    src\optimization\DynamicLowLatencyServer.cpp ^
    src\optimization\DynamicNetworkOptimizer.cpp ^
    src\utils\Logger.cpp ^
    src\utils\ConfigManager.cpp ^
    src\version\VersionManager.cpp ^
    src\version\DynamicVersionManager.cpp ^
    src\game\Entities\Npc\Npc.cpp ^
    src\game\Entities\Player\Player.cpp ^
    /Fe:build\Debug\Witcher3-MP-Dynamic-Debug.exe /link /SUBSYSTEM:CONSOLE

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Dynamic Server Debug compilation failed!
    pause
    exit /b 1
)

echo [8/8] Compiling Dynamic Server (Release)...
cl /EHsc /std:c++20 /I"include" /I"dep\asio" /D"NDEBUG" /D"_CONSOLE" /D"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING" /O2 /Ot /GL /W3 /nologo ^
    src\core\DynamicMain.cpp ^
    src\networking\net_server.cpp ^
    src\networking\net_client.cpp ^
    src\networking\net_connection.cpp ^
    src\networking\net_message.cpp ^
    src\networking\net_tsqueue.cpp ^
    src\optimization\NetworkOptimizer.cpp ^
    src\optimization\LowLatencyServer.cpp ^
    src\optimization\DynamicLowLatencyServer.cpp ^
    src\optimization\DynamicNetworkOptimizer.cpp ^
    src\utils\Logger.cpp ^
    src\utils\ConfigManager.cpp ^
    src\version\VersionManager.cpp ^
    src\version\DynamicVersionManager.cpp ^
    src\game\Entities\Npc\Npc.cpp ^
    src\game\Entities\Player\Player.cpp ^
    /Fe:build\Release\Witcher3-MP-Dynamic-Release.exe /link /SUBSYSTEM:CONSOLE /LTCG /OPT:REF /OPT:ICF

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Dynamic Server Release compilation failed!
    pause
    exit /b 1
)

echo [9/9] Copying configuration files...
xcopy "config\*" "build\Debug\" /E /I /Y >nul 2>nul
xcopy "config\*" "build\Release\" /E /I /Y >nul 2>nul

echo.
echo ========================================
echo  BUILD ALL COMPLETED SUCCESSFULLY!
echo ========================================
echo.
echo Debug builds:
echo   Standard:    build\Debug\Witcher3-MP-Standard-Debug.exe
echo   Low Latency: build\Debug\Witcher3-MP-LowLatency-Debug.exe
echo   Dynamic:     build\Debug\Witcher3-MP-Dynamic-Debug.exe
echo.
echo Release builds:
echo   Standard:    build\Release\Witcher3-MP-Standard-Release.exe
echo   Low Latency: build\Release\Witcher3-MP-LowLatency-Release.exe
echo   Dynamic:     build\Release\Witcher3-MP-Dynamic-Release.exe
echo   Next-Gen:    build\Release\Witcher3-MP-NextGen.exe
echo.
echo RECOMMENDED: Use Next-Gen for The Witcher 3 Next-Gen, Dynamic for other versions
echo.
echo Configuration files copied to build directories.
echo.
echo Press any key to exit...
pause >nul
