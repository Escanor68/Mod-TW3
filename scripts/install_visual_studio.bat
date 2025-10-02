@echo off
echo ========================================
echo    Instalacion de Visual Studio Community
echo ========================================
echo.

echo Esta opcion instalara Visual Studio Community 2022
echo que incluye el compilador C++ necesario para Witcher3-MP
echo.

echo Descargando Visual Studio Community 2022...
echo.

REM Download Visual Studio Community 2022
powershell -Command "& {Invoke-WebRequest -Uri 'https://aka.ms/vs/17/release/vs_community.exe' -OutFile 'vs_community.exe'}"

if not exist "vs_community.exe" (
    echo ERROR: No se pudo descargar Visual Studio Community
    echo Por favor, descarga manualmente desde: https://visualstudio.microsoft.com/vs/community/
    pause
    exit /b 1
)

echo.
echo Iniciando instalacion de Visual Studio Community...
echo.
echo IMPORTANTE: Durante la instalacion, asegurate de seleccionar:
echo - "Desktop development with C++"
echo - "Windows 10/11 SDK"
echo - "CMake tools for Visual Studio"
echo.

start /wait vs_community.exe --quiet --wait --add Microsoft.VisualStudio.Workload.NativeDesktop --add Microsoft.VisualStudio.Component.VC.CMake.Project --add Microsoft.VisualStudio.Component.Windows10SDK.19041

echo.
echo Instalacion completada!
echo.
echo Ahora puedes ejecutar:
echo 1. Abrir "Developer Command Prompt for VS 2022"
echo 2. Navegar a este directorio
echo 3. Ejecutar: scripts\build_simple.bat
echo.

REM Clean up
del vs_community.exe

pause
