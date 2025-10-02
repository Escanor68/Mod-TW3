@echo off
echo ========================================
echo  WITCHER 3 MULTIPLAYER - DYNAMIC BUILD
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

echo [1/6] Cleaning previous build...
if exist "Debug" rmdir /s /q "Debug"
if exist "Release" rmdir /s /q "Release"
if exist "*.obj" del /q "*.obj"
if exist "*.pdb" del /q "*.pdb"
if exist "*.ilk" del /q "*.ilk"
if exist "*.exp" del /q "*.exp"
if exist "*.lib" del /q "*.lib"

echo [2/6] Creating output directories...
mkdir "Debug" 2>nul
mkdir "Release" 2>nul

echo [3/6] Compiling Debug version...
cl /EHsc /std:c++20 /I"dep\asio" /D"_DEBUG" /D"_CONSOLE" /D"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING" /Zi /Od /RTC1 /W3 /nologo ^
    Main.cpp LowLatencyMain.cpp DynamicMain.cpp ^
    LowLatencyServer.cpp DynamicLowLatencyServer.cpp ^
    NetworkOptimizer.cpp DynamicNetworkOptimizer.cpp ^
    ConfigManager.cpp VersionManager.cpp DynamicVersionManager.cpp Logger.cpp ^
    game\Entities\Npc\Npc.cpp game\Entities\Player\Player.cpp ^
    /Fe:Debug\Witcher3-MP-Debug.exe /link /SUBSYSTEM:CONSOLE

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Debug compilation failed!
    pause
    exit /b 1
)

echo [4/6] Compiling Release version...
cl /EHsc /std:c++20 /I"dep\asio" /D"NDEBUG" /D"_CONSOLE" /D"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING" /O2 /Ot /GL /W3 /nologo ^
    Main.cpp LowLatencyMain.cpp DynamicMain.cpp ^
    LowLatencyServer.cpp DynamicLowLatencyServer.cpp ^
    NetworkOptimizer.cpp DynamicNetworkOptimizer.cpp ^
    ConfigManager.cpp VersionManager.cpp DynamicVersionManager.cpp Logger.cpp ^
    game\Entities\Npc\Npc.cpp game\Entities\Player\Player.cpp ^
    /Fe:Release\Witcher3-MP-Release.exe /link /SUBSYSTEM:CONSOLE /LTCG /OPT:REF /OPT:ICF

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Release compilation failed!
    pause
    exit /b 1
)

echo [5/6] Compiling Dynamic Low Latency version...
cl /EHsc /std:c++20 /I"dep\asio" /D"NDEBUG" /D"_CONSOLE" /D"_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING" /O2 /Ot /GL /W3 /nologo ^
    DynamicMain.cpp ^
    DynamicLowLatencyServer.cpp ^
    DynamicNetworkOptimizer.cpp ^
    DynamicVersionManager.cpp ^
    LowLatencyServer.cpp ^
    NetworkOptimizer.cpp ^
    ConfigManager.cpp VersionManager.cpp Logger.cpp ^
    game\Entities\Npc\Npc.cpp game\Entities\Player\Player.cpp ^
    /Fe:Release\Witcher3-MP-Dynamic.exe /link /SUBSYSTEM:CONSOLE /LTCG /OPT:REF /OPT:ICF

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Dynamic compilation failed!
    pause
    exit /b 1
)

echo [6/6] Copying configuration files...
copy "config.json" "Debug\" >nul 2>nul
copy "config.json" "Release\" >nul 2>nul
copy "version.json" "Debug\" >nul 2>nul
copy "version.json" "Release\" >nul 2>nul
copy "network_config.json" "Debug\" >nul 2>nul
copy "network_config.json" "Release\" >nul 2>nul
copy "version_database.json" "Debug\" >nul 2>nul
copy "version_database.json" "Release\" >nul 2>nul
copy "dynamic_server_config.json" "Debug\" >nul 2>nul
copy "dynamic_server_config.json" "Release\" >nul 2>nul

echo.
echo ========================================
echo  DYNAMIC BUILD COMPLETED SUCCESSFULLY!
echo ========================================
echo.
echo Debug version:        Debug\Witcher3-MP-Debug.exe
echo Release version:      Release\Witcher3-MP-Release.exe
echo Low Latency version:  Release\Witcher3-MP-LowLatency.exe
echo Dynamic version:      Release\Witcher3-MP-Dynamic.exe
echo.
echo RECOMMENDED: Use Dynamic version for best performance
echo.
echo Configuration files copied to output directories.
echo.
echo Press any key to exit...
pause >nul
