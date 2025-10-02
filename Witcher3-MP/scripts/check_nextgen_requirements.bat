@echo off
echo ========================================
echo  NEXT-GEN REQUIREMENTS CHECKER
echo ========================================
echo.

set REQUIREMENTS_MET=1

echo Checking system requirements...
echo.

REM Check Windows version
echo [1/8] Checking Windows version...
for /f "tokens=4-5 delims=. " %%i in ('ver') do set VERSION=%%i.%%j
if "%VERSION%" geq "10.0" (
    echo ✓ Windows 10/11 detected: %VERSION%
) else (
    echo ✗ Windows version too old: %VERSION%
    echo   Required: Windows 10/11
    set REQUIREMENTS_MET=0
)

REM Check if running on 64-bit
echo [2/8] Checking system architecture...
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    echo ✓ 64-bit system detected
) else (
    echo ✗ 32-bit system detected
    echo   Required: 64-bit system
    set REQUIREMENTS_MET=0
)

REM Check RAM
echo [3/8] Checking available RAM...
for /f "skip=1" %%p in ('wmic computersystem get TotalPhysicalMemory') do (
    if not "%%p"=="" (
        set RAM_BYTES=%%p
        goto :ram_check
    )
)
:ram_check
if %RAM_BYTES% geq 8589934592 (
    echo ✓ RAM: 8GB+ detected (Minimum: 8GB)
) else (
    echo ! RAM: Less than 8GB detected (Minimum: 8GB recommended)
    echo   Current system may work but performance may be limited
)

REM Check Visual Studio installation
echo [4/8] Checking Visual Studio installation...
set VS_FOUND=0
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" (
    echo ✓ Visual Studio 2022 Community found
    set VS_FOUND=1
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" (
    echo ✓ Visual Studio 2022 Professional found
    set VS_FOUND=1
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe" (
    echo ✓ Visual Studio 2022 Enterprise found
    set VS_FOUND=1
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" (
    echo ✓ Visual Studio 2019 Community found
    set VS_FOUND=1
) else (
    echo ✗ Visual Studio not found
    echo   Required: Visual Studio 2019/2022
    set REQUIREMENTS_MET=0
)

REM Check Windows SDK
echo [5/8] Checking Windows SDK...
if exist "C:\Program Files (x86)\Windows Kits\10\Include" (
    echo ✓ Windows SDK found
) else (
    echo ✗ Windows SDK not found
    echo   Required: Windows 10/11 SDK
    set REQUIREMENTS_MET=0
)

REM Check GPU (basic check)
echo [6/8] Checking GPU...
wmic path win32_VideoController get name | findstr /i "nvidia" >nul
if %ERRORLEVEL%==0 (
    echo ✓ NVIDIA GPU detected
    wmic path win32_VideoController get name | findstr /i "rtx" >nul
    if %ERRORLEVEL%==0 (
        echo ✓ RTX GPU detected - Ray Tracing and DLSS supported
    ) else (
        echo ! NVIDIA GPU without RTX - Limited Next-Gen features
    )
) else (
    wmic path win32_VideoController get name | findstr /i "amd" >nul
    if %ERRORLEVEL%==0 (
        echo ✓ AMD GPU detected
        wmic path win32_VideoController get name | findstr /i "rx" >nul
        if %ERRORLEVEL%==0 (
            echo ✓ AMD RX GPU detected - FSR supported
        ) else (
            echo ! AMD GPU without RX - Limited Next-Gen features
        )
    ) else (
        echo ! Unknown GPU - Next-Gen features may not work
    )
)

REM Check The Witcher 3 Next-Gen installation
echo [7/8] Checking The Witcher 3 Next-Gen installation...
set TW3_FOUND=0
if exist "C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3 Wild Hunt\bin\x64\witcher3.exe" (
    echo ✓ The Witcher 3 found in Steam
    set TW3_FOUND=1
) else if exist "C:\GOG Games\The Witcher 3 Wild Hunt\bin\x64\witcher3.exe" (
    echo ✓ The Witcher 3 found in GOG
    set TW3_FOUND=1
) else (
    echo ! The Witcher 3 Next-Gen not found in common locations
    echo   Please ensure The Witcher 3 Next-Gen is installed
)

REM Check project files
echo [8/8] Checking project files...
if exist "Witcher3-MP.vcxproj" (
    echo ✓ Visual Studio project file found
) else (
    echo ✗ Visual Studio project file not found
    set REQUIREMENTS_MET=0
)

if exist "include\optimization\NextGenServer.h" (
    echo ✓ Next-Gen server header found
) else (
    echo ✗ Next-Gen server header not found
    set REQUIREMENTS_MET=0
)

if exist "src\core\NextGenMain.cpp" (
    echo ✓ Next-Gen main source found
) else (
    echo ✗ Next-Gen main source not found
    set REQUIREMENTS_MET=0
)

echo.
echo ========================================
echo  REQUIREMENTS CHECK COMPLETE
echo ========================================
echo.

if %REQUIREMENTS_MET%==1 (
    echo ✓ All requirements met! Ready to build Next-Gen server.
    echo.
    echo Next steps:
    echo 1. Open Witcher3-MP.vcxproj in Visual Studio
    echo 2. Select Release x64 configuration
    echo 3. Build the project
    echo 4. Run Witcher3MP_NextGen.exe
    echo.
    echo The server will automatically:
    echo - Detect The Witcher 3 Next-Gen version
    echo - Apply optimal Next-Gen settings
    echo - Enable Ray Tracing, DLSS, FSR, HDR, RTX
    echo - Optimize for low latency
    echo - Monitor performance in real-time
) else (
    echo ✗ Some requirements not met. Please address the issues above.
    echo.
    echo Missing components:
    if %VS_FOUND%==0 echo - Visual Studio 2019/2022
    if %TW3_FOUND%==0 echo - The Witcher 3 Next-Gen
    echo.
    echo Download links:
    echo - Visual Studio: https://visualstudio.microsoft.com/downloads/
    echo - The Witcher 3: https://store.steampowered.com/app/292030/
)

echo.
echo Press any key to continue...
pause >nul