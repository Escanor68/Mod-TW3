@echo off
setlocal

echo ========================================
echo    Conectar a Servidor Witcher3-MP
echo ========================================
echo.

echo Este script configura el cliente para conectarse a un servidor.
echo.

set /p "server_ip=Ingresa la IP del servidor: "
set /p "server_port=Ingresa el puerto (default 60000): "
if "%server_port%"=="" set "server_port=60000"

echo.
echo Configurando cliente para conectarse a:
echo IP: %server_ip%
echo Puerto: %server_port%
echo.

REM Crear configuración de cliente
(
echo {
echo   "server": {
echo     "ip": "127.0.0.1",
echo     "port": 60000,
echo     "max_connections": 4,
echo     "timeout": 30,
echo     "keep_alive": true,
echo     "auto_start": false
echo   },
echo   "client": {
echo     "server_ip": "%server_ip%",
echo     "server_port": %server_port%,
echo     "auto_connect": true
echo   }
echo }
) > "mod\config\mp_config.json"

echo ✅ Configuración de cliente aplicada.
echo.

REM Buscar instalación de The Witcher 3
set "TW3_PATH="
if exist "C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3" set "TW3_PATH=C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3"
if exist "E:\SteamLibrary\steamapps\common\The Witcher 3" set "TW3_PATH=E:\SteamLibrary\steamapps\common\The Witcher 3"

if not defined TW3_PATH (
    echo ERROR: The Witcher 3 installation not found!
    echo Please manually specify the path to your Witcher 3 installation:
    echo.
    set /p "TW3_PATH=Enter Witcher 3 path: "
    if not exist "%TW3_PATH%\bin\x64\witcher3.exe" (
        echo ERROR: Invalid path specified!
        pause
        exit /b 1
    )
)

REM Crear script de lanzamiento para cliente
(
echo @echo off
echo setlocal
echo.
echo echo ========================================
echo echo    Conectando a Servidor Witcher3-MP
echo echo ========================================
echo echo.
echo echo Servidor: %server_ip%:%server_port%
echo echo.
echo echo Lanzando The Witcher 3...
echo "bin\x64\witcher3.exe" -mod witcher3_mp
echo endlocal
) > "%TW3_PATH%\connect_to_server.bat"

echo ✅ Script de conexión creado: %TW3_PATH%\connect_to_server.bat
echo.
echo Para conectarte al servidor, ejecuta:
echo %TW3_PATH%\connect_to_server.bat
echo.
pause
endlocal
