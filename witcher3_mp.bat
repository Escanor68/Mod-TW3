@echo off
setlocal enabledelayedexpansion

echo.
echo ========================================
echo    Witcher3-MP - Script Unificado
echo ========================================
echo.

if "%~1"=="" goto :help

if /i "%~1"=="build"  goto :build
if /i "%~1"=="install" goto :install
if /i "%~1"=="start"  goto :start
if /i "%~1"=="test"   goto :test
if /i "%~1"=="config" goto :config
if /i "%~1"=="connect" goto :connect
if /i "%~1"=="clean"  goto :clean
if /i "%~1"=="help"   goto :help

goto :invalid

:build
echo ========================================
echo    Compilando Witcher3-MP...
echo ========================================
echo.

REM Detectar Visual Studio para compilación
set "VS_PATH="
for /f "usebackq tokens=*" %%i in (`"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -property installationPath`) do (
    set "VS_PATH=%%i"
)

if not defined VS_PATH (
    echo ERROR: Visual Studio installation not found.
    echo Please ensure Visual Studio 2022 is installed.
    pause
    exit /b 1
)

set "VCVARS_PATH=%VS_PATH%\VC\Auxiliary\Build\vcvarsall.bat"
if not exist "%VCVARS_PATH%" (
    echo ERROR: vcvarsall.bat not found at "%VCVARS_PATH%".
    pause
    exit /b 1
)

REM Configurar entorno de Visual Studio
call "%VCVARS_PATH%" x64 >nul 2>&1

REM Crear directorio de compilación
if not exist "build\Release" (
    mkdir "build\Release"
)

REM Compilar manualmente con vcpkg
echo Compilando servidor con librerías de vcpkg...
set VCPKG_ROOT=%~dp0vcpkg
set VCPKG_LIB=%VCPKG_ROOT%\installed\x64-windows\lib
set VCPKG_INCLUDE=%VCPKG_ROOT%\installed\x64-windows\include

cl /EHsc /W3 /std:c++20 /I"include" /I"%VCPKG_INCLUDE%" /I"dep\asio" ^
src\core\NextGenMain.cpp ^
src\optimization\NextGenServer.cpp ^
src\utils\Logger.cpp ^
src\version\NextGenVersionManager.cpp ^
src\version\DynamicVersionManager.cpp ^
src\version\VersionManager.cpp ^
/Fe:"build\Release\Witcher3-MP.exe" ^
/link "%VCPKG_LIB%\zlib.lib" "%VCPKG_LIB%\lz4.lib" ws2_32.lib

if %errorlevel% neq 0 (
    echo ERROR: Compilation failed!
    echo Please check the error messages above.
    pause
    exit /b 1
)

echo ✅ Compilation successful!
echo 📁 Executable built: build\Release\Witcher3-MP.exe
echo.
exit /b 0

:install
echo ========================================
echo    Instalando Mod en The Witcher 3...
echo ========================================
echo.

REM Verificar que el ejecutable existe
if not exist "build\Release\Witcher3-MP.exe" (
    echo ERROR: Server executable not found!
    echo Please run 'witcher3_mp.bat build' first.
    pause
    exit /b 1
)

REM Buscar instalación de The Witcher 3
set "TW3_PATH="
if exist "C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3" set "TW3_PATH=C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3"
if exist "E:\SteamLibrary\steamapps\common\The Witcher 3" set "TW3_PATH=E:\SteamLibrary\steamapps\common\The Witcher 3"

if not defined TW3_PATH (
    echo ERROR: The Witcher 3 installation not found!
    echo Please manually specify the path to your Witcher 3 installation:
    echo (Example: C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3)
    echo.
    set /p "TW3_PATH=Enter Witcher 3 path: "
    if not exist "%TW3_PATH%\bin\x64\witcher3.exe" (
        echo ERROR: Invalid path specified!
        pause
        exit /b 1
    )
)

set "MOD_INSTALL_DIR=%TW3_PATH%\mods\witcher3_mp"

REM Crear directorios
if not exist "%TW3_PATH%\mods" mkdir "%TW3_PATH%\mods"
if not exist "%MOD_INSTALL_DIR%" mkdir "%MOD_INSTALL_DIR%"
if not exist "%MOD_INSTALL_DIR%\config" mkdir "%MOD_INSTALL_DIR%\config"
if not exist "%MOD_INSTALL_DIR%\scripts\game\witcher3_mp" mkdir "%MOD_INSTALL_DIR%\scripts\game\witcher3_mp"

REM Copiar archivos del mod
echo 📁 Copying mod files...
xcopy "mod\config\mp_config.json" "%MOD_INSTALL_DIR%\config\" /Y >nul
xcopy "mod\scripts\game\witcher3_mp\*.ws" "%MOD_INSTALL_DIR%\scripts\game\witcher3_mp\" /Y >nul
xcopy "mod\modInfo.ws" "%MOD_INSTALL_DIR%\" /Y >nul
xcopy "build\Release\Witcher3-MP.exe" "%MOD_INSTALL_DIR%\" /Y >nul

echo ✅ Installation successful!
echo 📁 Mod installed to: %MOD_INSTALL_DIR%
echo.
exit /b 0

:start
echo ========================================
echo    Ejecutando Witcher3-MP...
echo ========================================
echo.

REM Verificar que el ejecutable existe
if not exist "build\Release\Witcher3-MP.exe" (
    echo ERROR: Server executable not found!
    echo Please run 'witcher3_mp.bat build' first.
    pause
    exit /b 1
)

echo 🚀 Starting Witcher3-MP Server...
echo.
echo Server will run in this window. Press Ctrl+C to stop the server.
echo.

REM Ejecutar el servidor directamente en esta ventana
cd "build\Release"
echo Starting server from: %CD%
echo.
Witcher3-MP.exe

REM Si llegamos aquí, el servidor se cerró
echo.
echo Server has stopped.
echo.
exit /b 0

:test
echo ========================================
echo    Ejecutando Tests de Witcher3-MP...
echo ========================================
echo.

REM Verificar que CMake esté disponible
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: CMake not found in PATH!
    echo Please install CMake and add it to your PATH.
    echo.
    echo You can download CMake from: https://cmake.org/download/
    echo After installation, add CMake to your system PATH.
    echo.
    exit /b 1
)

REM Crear directorio de build para tests si no existe
if not exist "build" mkdir "build"
if not exist "build\test" mkdir "build\test"

echo 🔧 Configurando tests con CMake...
cd "build\test"
cmake -DCMAKE_BUILD_TYPE=Debug ..\..\tests
if %errorlevel% neq 0 (
    echo ERROR: CMake configuration failed!
    echo.
    exit /b 1
)

echo.
echo 🏗️  Compilando tests...
cmake --build . --config Debug
if %errorlevel% neq 0 (
    echo ERROR: Test compilation failed!
    echo.
    exit /b 1
)

echo.
echo 🧪 Ejecutando tests...
echo.

REM Ejecutar tests individuales
if exist "Debug\test_logging_system.exe" (
    echo === Test: Logging System ===
    Debug\test_logging_system.exe
    echo.
)

if exist "Debug\test_network_connection.exe" (
    echo === Test: Network Connection ===
    Debug\test_network_connection.exe
    echo.
)

if exist "Debug\Witcher3-MP-Tests.exe" (
    echo === Test: All Tests ===
    Debug\Witcher3-MP-Tests.exe
    echo.
)

echo ✅ Tests completados!
echo.
cd ..\..
exit /b 0

:config
echo ========================================
echo    Configurando Servidor...
echo ========================================
echo.

call scripts\configure_server.bat
exit /b 0

:connect
echo ========================================
echo    Conectar a Servidor...
echo ========================================
echo.

call scripts\connect_to_server.bat
exit /b 0

:clean
echo ========================================
echo    Limpiando Archivos Temporales...
echo ========================================
echo.

REM Limpiar directorio de compilación
if exist "build" (
    echo 🗑️  Removing build directory...
    rmdir /s /q "build"
)

REM Limpiar archivos temporales
if exist "*.obj" del "*.obj" >nul 2>&1
if exist "*.pdb" del "*.pdb" >nul 2>&1
if exist "*.ilk" del "*.ilk" >nul 2>&1

echo ✅ Cleanup complete!
echo.
exit /b 0

:help
echo ========================================
echo    Ayuda - Witcher3-MP
echo ========================================
echo.
echo Uso: witcher3_mp.bat [comando]
echo.
echo Comandos disponibles:
echo.
echo   build   - Compilar el proyecto
echo            Crea el ejecutable Witcher3-MP.exe
echo.
echo   install - Instalar el mod en The Witcher 3
echo            Copia archivos del mod y crea scripts de lanzamiento
echo.
echo   start   - Ejecutar el servidor
echo            Inicia el servidor Witcher3-MP
echo.
echo   test    - Ejecutar los tests del proyecto
echo            Compila y ejecuta todos los tests unitarios
echo.
echo   clean   - Limpiar archivos temporales
echo            Elimina archivos de compilación y temporales
echo.
echo   help    - Mostrar esta ayuda
echo.
echo Ejemplos:
echo   witcher3_mp.bat build
echo   witcher3_mp.bat test
echo   witcher3_mp.bat start
echo.
exit /b 0

:invalid
echo ERROR: Comando inválido: %~1
echo.
echo Use 'witcher3_mp.bat help' para ver los comandos disponibles.
echo.
exit /b 1