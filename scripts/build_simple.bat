@echo off
echo ========================================
echo    Witcher3-MP Simple Build Script
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

REM Create build directory
if not exist "build" (
    echo Creating build directory...
    mkdir build
)

if not exist "build\Release" (
    echo Creating Release directory...
    mkdir build\Release
)

echo.
echo Compiling Witcher3-MP Server...
echo.

REM Compile main server executable
cl /EHsc /std:c++20 /I"include" /I"dep\asio" /I"dep\asio\asio" ^
   src\core\Main.cpp ^
   src\integration\TW3ModInterface.cpp ^
   src\integration\REDkitBridge.cpp ^
   src\integration\WitcherScriptBridge.cpp ^
   src\integration\AssetLoader.cpp ^
   src\integration\CombatSystemIntegration.cpp ^
   src\integration\LoggerIntegration.cpp ^
   src\networking\net_client.cpp ^
   src\networking\net_server.cpp ^
   src\networking\NetworkLogger.cpp ^
   src\optimization\DataCompression.cpp ^
   src\optimization\NetworkOptimizer.cpp ^
   src\optimization\MovementPrediction.cpp ^
   src\optimization\MessagePrioritySystem.cpp ^
   src\optimization\PositionInterpolation.cpp ^
   src\optimization\SmartBatching.cpp ^
   src\optimization\DynamicNetworkOptimizer.cpp ^
   src\optimization\DynamicLowLatencyServer.cpp ^
   src\optimization\LowLatencyServer.cpp ^
   src\optimization\NextGenServer.cpp ^
   src\optimization\OptimizedNetworkProtocol.cpp ^
   src\optimization\CombatOptimizer.cpp ^
   src\utils\Logger.cpp ^
   src\utils\ConfigManager.cpp ^
   src\utils\ConsoleCommands.cpp ^
   src\version\VersionManager.cpp ^
   src\version\DynamicVersionManager.cpp ^
   src\version\NextGenVersionManager.cpp ^
   src\database\ResourceNames.cpp ^
   src\game\CooperativeQuests.cpp ^
   src\game\ExplorationMode.cpp ^
   src\game\GlobalEconomy.cpp ^
   src\game\SharedProgression.cpp ^
   src\game\SharedSaveSystem.cpp ^
   src\game\SharedStoryMode.cpp ^
   src\game\SyncedMonsterAI.cpp ^
   src\game\Entities\Player.cpp ^
   src\game\Entities\Npc.cpp ^
   /Fe:build\Release\Witcher3-MP.exe ^
   /link ws2_32.lib

if %errorLevel% neq 0 (
    echo ERROR: Compilation failed!
    pause
    exit /b 1
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
echo To run the server: build\Release\Witcher3-MP.exe
echo To install mod: scripts\install.bat
echo.
pause
