@echo off
echo ========================================
echo    Verificando Estado del Mod
echo ========================================
echo.

REM Verificar archivos del mod
echo Verificando archivos del mod...
if exist "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\witcher3_mp\modInfo.ws" (
    echo ✅ modInfo.ws encontrado
) else (
    echo ❌ modInfo.ws NO encontrado
)

if exist "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\witcher3_mp\mod.settings" (
    echo ✅ mod.settings encontrado
) else (
    echo ❌ mod.settings NO encontrado
)

if exist "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\witcher3_mp\Witcher3-MP.exe" (
    echo ✅ Witcher3-MP.exe encontrado
) else (
    echo ❌ Witcher3-MP.exe NO encontrado
)

echo.
echo Verificando scripts del mod...
if exist "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\witcher3_mp\scripts\game\witcher3_mp\MP_ModInitializer.ws" (
    echo ✅ MP_ModInitializer.ws encontrado
) else (
    echo ❌ MP_ModInitializer.ws NO encontrado
)

if exist "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\witcher3_mp\scripts\game\witcher3_mp\MP_GameHooks.ws" (
    echo ✅ MP_GameHooks.ws encontrado
) else (
    echo ❌ MP_GameHooks.ws NO encontrado
)

echo.
echo Verificando configuración XML...
if exist "E:\SteamLibrary\steamapps\common\The Witcher 3\bin\config\r4game\user_config_matrix\pc\witcher3_mp_config.xml" (
    echo ✅ witcher3_mp_config.xml encontrado
) else (
    echo ❌ witcher3_mp_config.xml NO encontrado
)

echo.
echo Verificando filelists...
if exist "E:\SteamLibrary\steamapps\common\The Witcher 3\bin\config\r4game\user_config_matrix\pc\dx11filelist.txt" (
    echo ✅ dx11filelist.txt encontrado
    findstr "witcher3_mp_config.xml" "E:\SteamLibrary\steamapps\common\The Witcher 3\bin\config\r4game\user_config_matrix\pc\dx11filelist.txt" >nul
    if %errorlevel%==0 (
        echo ✅ witcher3_mp_config.xml listado en dx11filelist.txt
    ) else (
        echo ❌ witcher3_mp_config.xml NO listado en dx11filelist.txt
    )
) else (
    echo ❌ dx11filelist.txt NO encontrado
)

if exist "E:\SteamLibrary\steamapps\common\The Witcher 3\bin\config\r4game\user_config_matrix\pc\dx12filelist.txt" (
    echo ✅ dx12filelist.txt encontrado
    findstr "witcher3_mp_config.xml" "E:\SteamLibrary\steamapps\common\The Witcher 3\bin\config\r4game\user_config_matrix\pc\dx12filelist.txt" >nul
    if %errorlevel%==0 (
        echo ✅ witcher3_mp_config.xml listado en dx12filelist.txt
    ) else (
        echo ❌ witcher3_mp_config.xml NO listado en dx12filelist.txt
    )
) else (
    echo ❌ dx12filelist.txt NO encontrado
)

echo.
echo ========================================
echo    Verificación Completada
echo ========================================
pause
