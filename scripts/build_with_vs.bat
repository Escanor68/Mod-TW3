@echo off
echo ========================================
echo    Witcher3-MP Build with Visual Studio
echo ========================================
echo.

echo Buscando Visual Studio Developer Command Prompt...
echo.

REM Try to find Visual Studio 2022
set "VS_PATH="
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    echo Encontrado Visual Studio 2022 Community
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
    echo Encontrado Visual Studio 2022 Professional
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    echo Encontrado Visual Studio 2022 Enterprise
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
    echo Encontrado Visual Studio 2019 Community
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"
    echo Encontrado Visual Studio 2019 Professional
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    set "VS_PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    echo Encontrado Visual Studio 2019 Enterprise
)

if "%VS_PATH%"=="" (
    echo ERROR: No se encontro Visual Studio instalado
    echo Por favor, instala Visual Studio 2019 o 2022 con soporte C++
    pause
    exit /b 1
)

echo.
echo Inicializando entorno de Visual Studio...
call "%VS_PATH%"

echo.
echo Compilando Witcher3-MP...
echo.

REM Create build directory
if not exist "build" mkdir build
if not exist "build\Release" mkdir build\Release

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
    echo ERROR: Compilacion fallo!
    pause
    exit /b 1
)

echo.
echo Copiando archivos de configuracion...
xcopy "config\*" "build\Release\config\" /E /I /Y
xcopy "mod\*" "build\Release\mod\" /E /I /Y

echo.
echo ========================================
echo    Compilacion Completada!
echo ========================================
echo.
echo Ejecutable creado: build\Release\Witcher3-MP.exe
echo Configuracion: build\Release\config\
echo Archivos del mod: build\Release\mod\
echo.
echo Para ejecutar el servidor: build\Release\Witcher3-MP.exe
echo Para instalar el mod: scripts\install.bat
echo.

pause
