@echo off
setlocal enabledelayedexpansion

echo ========================================
echo    Desinstalando Mod de The Witcher 3...
echo ========================================
echo.

REM Detectar automáticamente la ubicación de The Witcher 3
set "TW3_PATH="

REM Buscar en ubicaciones comunes de Steam
if exist "C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3" set "TW3_PATH=C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3"
if exist "E:\SteamLibrary\steamapps\common\The Witcher 3" set "TW3_PATH=E:\SteamLibrary\steamapps\common\The Witcher 3"
if exist "D:\SteamLibrary\steamapps\common\The Witcher 3" set "TW3_PATH=D:\SteamLibrary\steamapps\common\The Witcher 3"
if exist "F:\SteamLibrary\steamapps\common\The Witcher 3" set "TW3_PATH=F:\SteamLibrary\steamapps\common\The Witcher 3"

if not defined TW3_PATH (
    echo ERROR: The Witcher 3 installation not found!
    echo Please ensure The Witcher 3 is installed via Steam.
    echo Searched locations:
    echo   - C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3
    echo   - E:\SteamLibrary\steamapps\common\The Witcher 3
    echo   - D:\SteamLibrary\steamapps\common\The Witcher 3
    echo   - F:\SteamLibrary\steamapps\common\The Witcher 3
    pause
    exit /b 1
)

echo Found The Witcher 3 at: %TW3_PATH%
set "MOD_INSTALL_DIR=%TW3_PATH%\mods\witcher3_mp"

REM Eliminar archivos del mod
echo 🗑️ Removing mod files...
if exist "%MOD_INSTALL_DIR%" (
    rmdir /s /q "%MOD_INSTALL_DIR%"
    echo ✅ Mod directory removed
) else (
    echo ⚠️  Mod directory not found
)

REM Eliminar archivos XML de configuración
echo 🗑️ Removing XML configuration files...
set "XML_PATH=%TW3_PATH%\bin\config\r4game\user_config_matrix\pc\witcher3_mp_config.xml"
if exist "%XML_PATH%" (
    del "%XML_PATH%"
    echo ✅ XML config file removed
) else (
    echo ⚠️  XML config file not found
)

REM Limpiar filelist.txt del juego
echo 📝 Cleaning game filelist...
set "FILELIST_PATH=%TW3_PATH%\bin\config\r4game\user_config_matrix\pc\dx11filelist.txt"
if exist "%FILELIST_PATH%" (
    findstr /v "witcher3_mp_config.xml" "%FILELIST_PATH%" > "%FILELIST_PATH%.tmp"
    move "%FILELIST_PATH%.tmp" "%FILELIST_PATH%"
    echo ✅ Cleaned dx11filelist.txt
)

set "FILELIST_PATH=%TW3_PATH%\bin\config\r4game\user_config_matrix\pc\dx12filelist.txt"
if exist "%FILELIST_PATH%" (
    findstr /v "witcher3_mp_config.xml" "%FILELIST_PATH%" > "%FILELIST_PATH%.tmp"
    move "%FILELIST_PATH%.tmp" "%FILELIST_PATH%"
    echo ✅ Cleaned dx12filelist.txt
)

echo ✅ Uninstallation successful!
echo 📁 Mod completely removed from The Witcher 3
echo.
pause
