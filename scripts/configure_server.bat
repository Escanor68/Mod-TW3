@echo off
setlocal

echo ========================================
echo    Configurar Servidor Witcher3-MP
echo ========================================
echo.

echo Selecciona el tipo de configuración:
echo 1. Servidor Local (mismo PC)
echo 2. Servidor LAN (red local)
echo 3. Servidor Internet (WAN)
echo.

set /p "choice=Ingresa tu opción (1-3): "

if "%choice%"=="1" goto :local
if "%choice%"=="2" goto :lan
if "%choice%"=="3" goto :wan
goto :invalid

:local
echo.
echo Configurando para servidor local...
echo IP: 127.0.0.1
echo Puerto: 60000
echo.

REM Crear configuración local
(
echo {
echo   "server": {
echo     "ip": "127.0.0.1",
echo     "port": 60000,
echo     "max_connections": 4,
echo     "timeout": 30,
echo     "keep_alive": true,
echo     "auto_start": true
echo   },
echo   "client": {
echo     "server_ip": "127.0.0.1",
echo     "server_port": 60000,
echo     "auto_connect": true
echo   }
echo }
) > "mod\config\mp_config.json"

echo ✅ Configuración local aplicada.
goto :end

:lan
echo.
echo Configurando para servidor LAN...
echo.

REM Obtener IP local
for /f "tokens=2 delims=:" %%a in ('ipconfig ^| findstr /i "IPv4"') do (
    set "local_ip=%%a"
    set "local_ip=!local_ip: =!"
    goto :found_ip
)

:found_ip
echo IP local detectada: %local_ip%
echo Puerto: 60000
echo.

set /p "custom_ip=¿Usar esta IP? (s/n): "
if /i "%custom_ip%"=="n" (
    set /p "local_ip=Ingresa la IP del servidor: "
)

REM Crear configuración LAN
(
echo {
echo   "server": {
echo     "ip": "%local_ip%",
echo     "port": 60000,
echo     "max_connections": 4,
echo     "timeout": 30,
echo     "keep_alive": true,
echo     "auto_start": true
echo   },
echo   "client": {
echo     "server_ip": "%local_ip%",
echo     "server_port": 60000,
echo     "auto_connect": true
echo   }
echo }
) > "mod\config\mp_config.json"

echo ✅ Configuración LAN aplicada.
echo.
echo Para que otros jugadores se conecten:
echo 1. Comparte esta IP: %local_ip%
echo 2. Diles que ejecuten: witcher3_mp.bat install
echo 3. Diles que ejecuten: launch_game.bat
goto :end

:wan
echo.
echo Configurando para servidor Internet...
echo.

set /p "public_ip=Ingresa tu IP pública: "
set /p "port=Ingresa el puerto (default 60000): "
if "%port%"=="" set "port=60000"

REM Crear configuración WAN
(
echo {
echo   "server": {
echo     "ip": "%public_ip%",
echo     "port": %port%,
echo     "max_connections": 4,
echo     "timeout": 30,
echo     "keep_alive": true,
echo     "auto_start": true
echo   },
echo   "client": {
echo     "server_ip": "%public_ip%",
echo     "server_port": %port%,
echo     "auto_connect": true
echo   }
echo }
) > "mod\config\mp_config.json"

echo ✅ Configuración Internet aplicada.
echo.
echo IMPORTANTE: Configura tu router para redirigir el puerto %port% a tu PC.
echo.
echo Para que otros jugadores se conecten:
echo 1. Comparte esta IP: %public_ip%
echo 2. Diles que ejecuten: witcher3_mp.bat install
echo 3. Diles que ejecuten: launch_game.bat
goto :end

:invalid
echo ERROR: Opción inválida.
goto :end

:end
echo.
pause
endlocal
