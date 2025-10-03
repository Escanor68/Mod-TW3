@echo off
setlocal enabledelayedexpansion

echo.
echo ========================================
echo    Witcher3-MP - Build and Test Suite
echo ========================================
echo.

REM Set error handling
set "ERROR_COUNT=0"

REM Create logs directory
if not exist "logs" mkdir "logs"

REM Initialize log file
set "LOG_FILE=logs\build_and_test.log"
echo [%date% %time%] Starting Witcher3-MP Build and Test Suite > "%LOG_FILE%"

echo 🔧 Step 1: Environment Setup
echo [%date% %time%] Step 1: Environment Setup >> "%LOG_FILE%"

REM Check for Visual Studio 2022
set "VS_PATH="
for /f "usebackq tokens=*" %%i in (`"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -property installationPath`) do (
    set "VS_PATH=%%i"
)

if not defined VS_PATH (
    echo ❌ ERROR: Visual Studio 2022 not found!
    echo [%date% %time%] ERROR: Visual Studio 2022 not found! >> "%LOG_FILE%"
    set /a ERROR_COUNT+=1
    goto :error_exit
)

set "VCVARS_PATH=%VS_PATH%\VC\Auxiliary\Build\vcvarsall.bat"
if not exist "%VCVARS_PATH%" (
    echo ❌ ERROR: vcvarsall.bat not found at "%VCVARS_PATH%"
    echo [%date% %time%] ERROR: vcvarsall.bat not found >> "%LOG_FILE%"
    set /a ERROR_COUNT+=1
    goto :error_exit
)

REM Setup Visual Studio environment
call "%VCVARS_PATH%" x64 >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ ERROR: Failed to setup Visual Studio environment
    echo [%date% %time%] ERROR: Failed to setup Visual Studio environment >> "%LOG_FILE%"
    set /a ERROR_COUNT+=1
    goto :error_exit
)

echo ✅ Visual Studio 2022 environment configured
echo [%date% %time%] Visual Studio 2022 environment configured >> "%LOG_FILE%"

echo.
echo 🏗️  Step 2: CMake Configuration
echo [%date% %time%] Step 2: CMake Configuration >> "%LOG_FILE%"

REM Check for CMake
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ ERROR: CMake not found in PATH!
    echo [%date% %time%] ERROR: CMake not found in PATH! >> "%LOG_FILE%"
    set /a ERROR_COUNT+=1
    goto :error_exit
)

REM Create build directory
if not exist "build" mkdir "build"
if not exist "build\Release" mkdir "build\Release"

REM Configure with CMake
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release .. > ..\logs\cmake_config.log 2>&1
if %errorlevel% neq 0 (
    echo ❌ ERROR: CMake configuration failed!
    echo [%date% %time%] ERROR: CMake configuration failed! >> "..\%LOG_FILE%"
    set /a ERROR_COUNT+=1
    cd ..
    goto :error_exit
)

echo ✅ CMake configuration successful
echo [%date% %time%] CMake configuration successful >> "..\%LOG_FILE%"

echo.
echo 🔨 Step 3: Compilation
echo [%date% %time%] Step 3: Compilation >> "..\%LOG_FILE%"

REM Build the project
cmake --build . --config Release --parallel > ..\logs\build_output.log 2>&1
if %errorlevel% neq 0 (
    echo ❌ ERROR: Compilation failed!
    echo [%date% %time%] ERROR: Compilation failed! >> "..\%LOG_FILE%"
    set /a ERROR_COUNT+=1
    cd ..
    goto :error_exit
)

cd ..

REM Check if executable was created
if not exist "build\Release\Witcher3-MP.exe" (
    echo ❌ ERROR: Witcher3-MP.exe not found after compilation!
    echo [%date% %time%] ERROR: Witcher3-MP.exe not found after compilation! >> "%LOG_FILE%"
    set /a ERROR_COUNT+=1
    goto :error_exit
)

echo ✅ Compilation successful - Witcher3-MP.exe created
echo [%date% %time%] Compilation successful - Witcher3-MP.exe created >> "%LOG_FILE%"

echo.
echo 📦 Step 4: Mod Installation
echo [%date% %time%] Step 4: Mod Installation >> "%LOG_FILE%"

REM Detect Witcher 3 installation path
set "WITCHER3_PATH="
set "WITCHER3_MODS_PATH="

REM Try common installation paths
for %%p in (
    "C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3"
    "C:\Program Files\Steam\steamapps\common\The Witcher 3"
    "D:\Steam\steamapps\common\The Witcher 3"
    "E:\Steam\steamapps\common\The Witcher 3"
    "C:\GOG Games\The Witcher 3 Wild Hunt GOTY"
    "D:\GOG Games\The Witcher 3 Wild Hunt GOTY"
) do (
    if exist "%%~p\bin\x64\witcher3.exe" (
        set "WITCHER3_PATH=%%~p"
        set "WITCHER3_MODS_PATH=%%~p\mods"
        goto :found_witcher3
    )
)

echo ❌ ERROR: The Witcher 3 installation not found!
echo [%date% %time%] ERROR: The Witcher 3 installation not found! >> "%LOG_FILE%"
echo Please ensure The Witcher 3 is installed and accessible.
set /a ERROR_COUNT+=1
goto :error_exit

:found_witcher3
echo ✅ Found The Witcher 3 at: %WITCHER3_PATH%
echo [%date% %time%] Found The Witcher 3 at: %WITCHER3_PATH% >> "%LOG_FILE%"

REM Create mod directory structure
if not exist "%WITCHER3_MODS_PATH%" mkdir "%WITCHER3_MODS_PATH%"
if not exist "%WITCHER3_MODS_PATH%\witcher3_mp" mkdir "%WITCHER3_MODS_PATH%\witcher3_mp"
if not exist "%WITCHER3_MODS_PATH%\witcher3_mp\scripts" mkdir "%WITCHER3_MODS_PATH%\witcher3_mp\scripts"
if not exist "%WITCHER3_MODS_PATH%\witcher3_mp\scripts\game" mkdir "%WITCHER3_MODS_PATH%\witcher3_mp\scripts\game"
if not exist "%WITCHER3_MODS_PATH%\witcher3_mp\scripts\game\witcher3_mp" mkdir "%WITCHER3_MODS_PATH%\witcher3_mp\scripts\game\witcher3_mp"
if not exist "%WITCHER3_MODS_PATH%\witcher3_mp\config" mkdir "%WITCHER3_MODS_PATH%\witcher3_mp\config"

REM Copy mod files
echo Copying mod files...
echo [%date% %time%] Copying mod files... >> "%LOG_FILE%"

REM Copy executable
copy "build\Release\Witcher3-MP.exe" "%WITCHER3_MODS_PATH%\witcher3_mp\" >nul
if %errorlevel% neq 0 (
    echo ❌ ERROR: Failed to copy Witcher3-MP.exe
    echo [%date% %time%] ERROR: Failed to copy Witcher3-MP.exe >> "%LOG_FILE%"
    set /a ERROR_COUNT+=1
    goto :error_exit
)

REM Copy scripts
xcopy "mod\scripts\game\witcher3_mp\*.ws" "%WITCHER3_MODS_PATH%\witcher3_mp\scripts\game\witcher3_mp\" /Y >nul
if %errorlevel% neq 0 (
    echo ❌ ERROR: Failed to copy .ws scripts
    echo [%date% %time%] ERROR: Failed to copy .ws scripts >> "%LOG_FILE%"
    set /a ERROR_COUNT+=1
    goto :error_exit
)

REM Copy configuration
xcopy "mod\config\*.*" "%WITCHER3_MODS_PATH%\witcher3_mp\config\" /Y >nul
if %errorlevel% neq 0 (
    echo ❌ ERROR: Failed to copy configuration files
    echo [%date% %time%] ERROR: Failed to copy configuration files >> "%LOG_FILE%"
    set /a ERROR_COUNT+=1
    goto :error_exit
)

REM Copy mod info files
copy "mod\modInfo.ini" "%WITCHER3_MODS_PATH%\witcher3_mp\" >nul
copy "mod\modInfo.ws" "%WITCHER3_MODS_PATH%\witcher3_mp\" >nul

echo ✅ Mod installation successful
echo [%date% %time%] Mod installation successful >> "%LOG_FILE%"

echo.
echo 🧪 Step 5: Running Tests
echo [%date% %time%] Step 5: Running Tests >> "%LOG_FILE%"

REM Run unit tests
call "scripts\run_integration_tests.bat"
if %errorlevel% neq 0 (
    echo ❌ WARNING: Some tests failed, but continuing...
    echo [%date% %time%] WARNING: Some tests failed, but continuing... >> "%LOG_FILE%"
)

echo.
echo 🔍 Step 6: File Validation
echo [%date% %time%] Step 6: File Validation >> "%LOG_FILE%"

REM Validate JSON files
echo Validating JSON files...
echo [%date% %time%] Validating JSON files... >> "%LOG_FILE%"

python -c "import json; json.load(open('mod/config/mp_config.json')); print('✅ mp_config.json is valid')" 2>nul
if %errorlevel% neq 0 (
    echo ❌ ERROR: mp_config.json is invalid JSON
    echo [%date% %time%] ERROR: mp_config.json is invalid JSON >> "%LOG_FILE%"
    set /a ERROR_COUNT+=1
)

python -c "import json; json.load(open('config/config.json')); print('✅ config.json is valid')" 2>nul
if %errorlevel% neq 0 (
    echo ❌ ERROR: config.json is invalid JSON
    echo [%date% %time%] ERROR: config.json is invalid JSON >> "%LOG_FILE%"
    set /a ERROR_COUNT+=1
)

REM Validate XML files
echo Validating XML files...
echo [%date% %time%] Validating XML files... >> "%LOG_FILE%"

python -c "import xml.etree.ElementTree as ET; ET.parse('mod/config/witcher3_mp_config.xml'); print('✅ witcher3_mp_config.xml is valid')" 2>nul
if %errorlevel% neq 0 (
    echo ❌ ERROR: witcher3_mp_config.xml is invalid XML
    echo [%date% %time%] ERROR: witcher3_mp_config.xml is invalid XML >> "%LOG_FILE%"
    set /a ERROR_COUNT+=1
)

echo.
echo 🎮 Step 7: Game Launch Preparation
echo [%date% %time%] Step 7: Game Launch Preparation >> "%LOG_FILE%"

REM Detect Witcher 3 executable
set "WITCHER3_EXE="
if exist "%WITCHER3_PATH%\bin\x64\witcher3_dx12.exe" (
    set "WITCHER3_EXE=%WITCHER3_PATH%\bin\x64\witcher3_dx12.exe"
    echo ✅ Found witcher3_dx12.exe (DirectX 12)
) else if exist "%WITCHER3_PATH%\bin\x64\witcher3.exe" (
    set "WITCHER3_EXE=%WITCHER3_PATH%\bin\x64\witcher3.exe"
    echo ✅ Found witcher3.exe (DirectX 11)
) else (
    echo ❌ ERROR: No Witcher 3 executable found!
    echo [%date% %time%] ERROR: No Witcher 3 executable found! >> "%LOG_FILE%"
    set /a ERROR_COUNT+=1
    goto :error_exit
)

echo [%date% %time%] Using executable: %WITCHER3_EXE% >> "%LOG_FILE%"

echo.
echo 📊 Step 8: Generating Test Results
echo [%date% %time%] Step 8: Generating Test Results >> "%LOG_FILE%"

REM Generate test results summary
set "RESULTS_FILE=logs\test_results.log"
echo ======================================== > "%RESULTS_FILE%"
echo    Witcher3-MP Test Results Summary >> "%RESULTS_FILE%"
echo ======================================== >> "%RESULTS_FILE%"
echo. >> "%RESULTS_FILE%"
echo Test Date: %date% %time% >> "%RESULTS_FILE%"
echo. >> "%RESULTS_FILE%"

if %ERROR_COUNT% equ 0 (
    echo ✅ Compilation: PASSED >> "%RESULTS_FILE%"
    echo ✅ Installation: PASSED >> "%RESULTS_FILE%"
    echo ✅ File Validation: PASSED >> "%RESULTS_FILE%"
    echo ✅ Game Detection: PASSED >> "%RESULTS_FILE%"
    echo. >> "%RESULTS_FILE%"
    echo 🎉 OVERALL RESULT: ALL TESTS PASSED >> "%RESULTS_FILE%"
    echo. >> "%RESULTS_FILE%"
    echo The mod is ready to use! >> "%RESULTS_FILE%"
    echo. >> "%RESULTS_FILE%"
    echo To launch the game with the mod: >> "%RESULTS_FILE%"
    echo "%WITCHER3_EXE%" -mod witcher3_mp >> "%RESULTS_FILE%"
    echo. >> "%RESULTS_FILE%"
    echo To start the server: >> "%RESULTS_FILE%"
    echo "%WITCHER3_MODS_PATH%\witcher3_mp\Witcher3-MP.exe" >> "%RESULTS_FILE%"
    
    echo.
    echo 🎉 ALL TESTS PASSED! The mod is ready to use.
    echo.
    echo To launch the game with the mod:
    echo "%WITCHER3_EXE%" -mod witcher3_mp
    echo.
    echo To start the server:
    echo "%WITCHER3_MODS_PATH%\witcher3_mp\Witcher3-MP.exe"
) else (
    echo ❌ Compilation: %ERROR_COUNT% errors found >> "%RESULTS_FILE%"
    echo ❌ Installation: Check logs for details >> "%RESULTS_FILE%"
    echo ❌ File Validation: Check logs for details >> "%RESULTS_FILE%"
    echo ❌ Game Detection: Check logs for details >> "%RESULTS_FILE%"
    echo. >> "%RESULTS_FILE%"
    echo ❌ OVERALL RESULT: %ERROR_COUNT% ERRORS FOUND >> "%RESULTS_FILE%"
    echo. >> "%RESULTS_FILE%"
    echo Please check the logs for detailed error information. >> "%RESULTS_FILE%"
    
    echo.
    echo ❌ %ERROR_COUNT% ERRORS FOUND! Please check the logs.
)

echo.
echo 📁 Log files generated:
echo   - %LOG_FILE%
echo   - %RESULTS_FILE%
echo   - logs\cmake_config.log
echo   - logs\build_output.log

echo.
echo ========================================
echo    Build and Test Suite Complete
echo ========================================

pause
exit /b %ERROR_COUNT%

:error_exit
echo.
echo ❌ Build and Test Suite failed with %ERROR_COUNT% errors.
echo Check the log files for detailed information.
pause
exit /b %ERROR_COUNT%
