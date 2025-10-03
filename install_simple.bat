@echo off
setlocal enabledelayedexpansion

echo ========================================
echo    Installing Mod in The Witcher 3...
echo ========================================
echo.

REM Verificar que el ejecutable existe
if not exist "build\Release\Witcher3-MP.exe" (
    echo ERROR: Server executable not found!
    echo Please run 'witcher3_mp.bat build' first.
    pause
    exit /b 1
)

REM Detectar automáticamente la ubicación de The Witcher 3
set "TW3_PATH="

REM Buscar en ubicaciones comunes de Steam usando variables
set "PATH1=C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3"
set "PATH2=E:\SteamLibrary\steamapps\common\The Witcher 3"
set "PATH3=D:\SteamLibrary\steamapps\common\The Witcher 3"
set "PATH4=F:\SteamLibrary\steamapps\common\The Witcher 3"

if exist "!PATH1!" set "TW3_PATH=!PATH1!"
if exist "!PATH2!" set "TW3_PATH=!PATH2!"
if exist "!PATH3!" set "TW3_PATH=!PATH3!"
if exist "!PATH4!" set "TW3_PATH=!PATH4!"

if "!TW3_PATH!"=="" (
    echo ERROR: The Witcher 3 installation not found!
    echo Please ensure The Witcher 3 is installed via Steam.
    pause
    exit /b 1
)

echo Found The Witcher 3 at: !TW3_PATH!
set "MOD_INSTALL_DIR=!TW3_PATH!\mods\witcher3_mp"

REM Crear directorios
echo Creating directories...
if not exist "!TW3_PATH!\mods" mkdir "!TW3_PATH!\mods"
if not exist "!MOD_INSTALL_DIR!" mkdir "!MOD_INSTALL_DIR!"
if not exist "!MOD_INSTALL_DIR!\config" mkdir "!MOD_INSTALL_DIR!\config"
if not exist "!MOD_INSTALL_DIR!\scripts\game\witcher3_mp" mkdir "!MOD_INSTALL_DIR!\scripts\game\witcher3_mp"
if not exist "!TW3_PATH!\bin\config\r4game\user_config_matrix\pc" mkdir "!TW3_PATH!\bin\config\r4game\user_config_matrix\pc"

REM Copiar archivos del mod
echo Copying mod files...
copy "mod\config\mp_config.json" "!MOD_INSTALL_DIR!\config\" >nul
copy "mod\scripts\game\witcher3_mp\*.ws" "!MOD_INSTALL_DIR!\scripts\game\witcher3_mp\" >nul
copy "mod\modInfo.ws" "!MOD_INSTALL_DIR!\" >nul
copy "mod\mod.settings" "!MOD_INSTALL_DIR!\" >nul
copy "build\Release\Witcher3-MP.exe" "!MOD_INSTALL_DIR!\" >nul

REM Copiar archivos XML de configuración para el menú del juego
echo Copying XML configuration files...
copy "mod\config\witcher3_mp_config.xml" "!TW3_PATH!\bin\config\r4game\user_config_matrix\pc\" >nul
copy "mod\config\filelist.txt" "!TW3_PATH!\bin\config\r4game\user_config_matrix\pc\" >nul

REM Actualizar filelist.txt del juego
echo Updating game filelist...
set "FILELIST_PATH=!TW3_PATH!\bin\config\r4game\user_config_matrix\pc\dx11filelist.txt"
if exist "!FILELIST_PATH!" (
    echo witcher3_mp_config.xml; >> "!FILELIST_PATH!"
    echo Added to dx11filelist.txt
) else (
    echo witcher3_mp_config.xml; > "!FILELIST_PATH!"
    echo Created dx11filelist.txt
)

set "FILELIST_PATH=!TW3_PATH!\bin\config\r4game\user_config_matrix\pc\dx12filelist.txt"
if exist "!FILELIST_PATH!" (
    echo witcher3_mp_config.xml; >> "!FILELIST_PATH!"
    echo Added to dx12filelist.txt
) else (
    echo witcher3_mp_config.xml; > "!FILELIST_PATH!"
    echo Created dx12filelist.txt
)

echo.
echo Installation successful!
echo Mod installed to: !MOD_INSTALL_DIR!
echo XML config copied to: !TW3_PATH!\bin\config\r4game\user_config_matrix\pc\
echo.
pause
