@echo off
setlocal enabledelayedexpansion

echo.
echo ========================================
echo    Witcher3-MP - Script Unificado
echo ========================================
echo.

REM Verificar argumentos
if "%1"=="" (
    echo Uso: witcher3_mp.bat [comando]
    echo.
    echo Comandos disponibles:
    echo   build   - Compilar el proyecto
    echo   install - Instalar el mod en The Witcher 3
    echo   start   - Ejecutar el mod (servidor + juego)
    echo   config  - Configurar servidor (IP, puerto, etc.)
    echo   connect - Conectar a servidor existente
    echo   clean   - Limpiar archivos temporales
    echo   help    - Mostrar esta ayuda
    echo.
    pause
    exit /b 1
)

REM Detectar Visual Studio
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

REM Procesar comando
if /i "%1"=="build" goto :build
if /i "%1"=="install" goto :install
if /i "%1"=="start" goto :start
if /i "%1"=="config" goto :config
if /i "%1"=="connect" goto :connect
if /i "%1"=="clean" goto :clean
if /i "%1"=="help" goto :help
goto :invalid_command

:build
echo ========================================
echo    Compilando Witcher3-MP...
echo ========================================
echo.

REM Crear directorio de compilación
if not exist "build\Release" (
    mkdir "build\Release"
)

REM Compilar
echo Compilando servidor...
cl /EHsc /W3 /std:c++20 /I"include" src\core\MainUltraSimple.cpp /Fe:"build\Release\Witcher3-MP.exe" /link >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Compilation failed!
    echo Please check the error messages above.
    pause
    exit /b 1
)

echo ✅ Compilation successful!
echo 📁 Executable built: build\Release\Witcher3-MP.exe
echo.
goto :end

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

REM Crear scripts de lanzamiento
echo 📝 Creating launch scripts...

REM start_server.bat
(
echo @echo off
echo setlocal
echo.
echo echo ========================================
echo echo    Starting Witcher3-MP Server...
echo echo ========================================
echo echo.
echo.
echo set "SERVER_EXECUTABLE=Witcher3-MP.exe"
echo set "MOD_NAME=witcher3_mp"
echo set "SERVER_PORT=60000"
echo.
echo if not exist "mods\%MOD_NAME%\%SERVER_EXECUTABLE%" ^(
echo     echo ERROR: Server executable not found!
echo     pause
echo     exit /b 1
echo ^)
echo.
echo echo Starting server in background...
echo start /MIN "" "mods\%MOD_NAME%\%SERVER_EXECUTABLE%"
echo.
echo echo ✅ Server started on port %SERVER_PORT%.
echo echo You can now launch The Witcher 3.
echo echo.
echo pause
echo endlocal
) > "%TW3_PATH%\start_server.bat"

REM launch_game.bat
(
echo @echo off
echo setlocal
echo "bin\x64\witcher3.exe" -mod witcher3_mp
echo endlocal
) > "%TW3_PATH%\launch_game.bat"

REM start_nextgen.bat
(
echo @echo off
echo setlocal
echo.
echo echo ========================================
echo echo    Starting Witcher3-MP Server...
echo echo ========================================
echo echo.
echo.
echo REM Start the MP server in background
echo start /MIN "" "mods\witcher3_mp\Witcher3-MP.exe"
echo.
echo echo ✅ Server started on port 60000
echo echo Waiting 3 seconds for server to initialize...
echo timeout /t 3 /nobreak ^>nul
echo.
echo REM Launch The Witcher 3 with mod enabled
echo echo 🎮 Launching The Witcher 3 with Witcher3-MP mod...
echo.
echo if exist "bin\x64\witcher3.exe" ^(
echo     echo Found witcher3.exe, launching...
echo     start "" "bin\x64\witcher3.exe" -mod witcher3_mp
echo ^) else if exist "bin\x64\witcher3_dx12.exe" ^(
echo     echo Found witcher3_dx12.exe, launching...
echo     start "" "bin\x64\witcher3_dx12.exe" -mod witcher3_mp
echo ^) else ^(
echo     echo ERROR: Witcher 3 executable not found!
echo     pause
echo ^)
echo.
echo REM Cleanup: Kill server when game exits
echo taskkill /F /IM "Witcher3-MP.exe" ^>nul 2^>^&1
echo echo Server stopped.
echo.
echo pause
echo endlocal
) > "%TW3_PATH%\start_nextgen.bat"

echo ✅ Installation successful!
echo 📁 Mod installed to: %MOD_INSTALL_DIR%
echo.
echo To launch the mod, run: %TW3_PATH%\start_nextgen.bat
echo.
goto :end

:start
echo ========================================
echo    Ejecutando Witcher3-MP...
echo ========================================
echo.

REM Verificar que el mod esté instalado
set "TW3_PATH="
if exist "C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3" set "TW3_PATH=C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3"
if exist "E:\SteamLibrary\steamapps\common\The Witcher 3" set "TW3_PATH=E:\SteamLibrary\steamapps\common\The Witcher 3"

if not defined TW3_PATH (
    echo ERROR: The Witcher 3 installation not found!
    echo Please run 'witcher3_mp.bat install' first.
    pause
    exit /b 1
)

if not exist "%TW3_PATH%\mods\witcher3_mp\Witcher3-MP.exe" (
    echo ERROR: Mod not installed!
    echo Please run 'witcher3_mp.bat install' first.
    pause
    exit /b 1
)

echo 🚀 Starting Witcher3-MP...
echo.
echo 1. Starting server...
start /MIN "" "%TW3_PATH%\start_server.bat"

echo 2. Waiting for server to initialize...
timeout /t 3 /nobreak >nul

echo 3. Launching The Witcher 3...
start "" "%TW3_PATH%\launch_game.bat"

echo.
echo ✅ Witcher3-MP is now running!
echo 🖥️  Server: Running in background
echo 🎮 Game: The Witcher 3 with mod loaded
echo.
goto :end

:config
echo ========================================
echo    Configurando Servidor...
echo ========================================
echo.

call scripts\configure_server.bat
goto :end

:connect
echo ========================================
echo    Conectar a Servidor...
echo ========================================
echo.

call scripts\connect_to_server.bat
goto :end

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
goto :end

:help
echo ========================================
echo    Ayuda - Witcher3-MP
echo ========================================
echo.
echo Comandos disponibles:
echo.
echo   build   - Compilar el proyecto
echo            Crea el ejecutable Witcher3-MP.exe
echo.
echo   install - Instalar el mod en The Witcher 3
echo            Copia archivos del mod y crea scripts de lanzamiento
echo.
echo   start   - Ejecutar el mod
echo            Inicia el servidor y lanza The Witcher 3
echo.
echo   clean   - Limpiar archivos temporales
echo            Elimina archivos de compilación y temporales
echo.
echo   help    - Mostrar esta ayuda
echo.
echo Ejemplos:
echo   witcher3_mp.bat build
echo   witcher3_mp.bat install
echo   witcher3_mp.bat start
echo.
goto :end

:invalid_command
echo ERROR: Comando inválido: %1
echo.
echo Use 'witcher3_mp.bat help' para ver los comandos disponibles.
echo.
goto :end

:end
echo.
pause
endlocal
