@echo off
echo ========================================
echo    Instalacion de MinGW-w64
echo ========================================
echo.

echo Esta opcion instalara MinGW-w64 que es una alternativa
echo ligera a Visual Studio para compilar C++
echo.

echo Descargando MinGW-w64...
echo.

REM Create temp directory
if not exist "temp" mkdir temp
cd temp

REM Download MinGW-w64
powershell -Command "& {Invoke-WebRequest -Uri 'https://github.com/niXman/mingw-builds-binaries/releases/download/13.2.0-rt_v11-rev1/winlibs-x86_64-posix-seh-gcc-13.2.0-mingw-w64-11.0.0-r1.zip' -OutFile 'mingw64.zip'}"

if not exist "mingw64.zip" (
    echo ERROR: No se pudo descargar MinGW-w64
    echo Por favor, descarga manualmente desde: https://www.mingw-w64.org/downloads/
    pause
    exit /b 1
)

echo.
echo Extrayendo MinGW-w64...
powershell -Command "& {Expand-Archive -Path 'mingw64.zip' -DestinationPath '.' -Force}"

echo.
echo Configurando MinGW-w64...
if exist "mingw64" (
    xcopy "mingw64" "C:\mingw64\" /E /I /Y
    setx PATH "%PATH%;C:\mingw64\bin" /M
    echo MinGW-w64 instalado en C:\mingw64\
    echo PATH actualizado
) else (
    echo ERROR: No se pudo extraer MinGW-w64
    pause
    exit /b 1
)

cd ..

echo.
echo Instalacion completada!
echo.
echo Ahora puedes ejecutar:
echo 1. Cerrar y reabrir esta ventana de comandos
echo 2. Ejecutar: scripts\build_mingw.bat
echo.

REM Clean up
rmdir /s /q temp

pause
