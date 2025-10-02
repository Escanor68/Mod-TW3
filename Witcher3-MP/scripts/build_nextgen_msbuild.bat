@echo off
echo ========================================
echo  WITCHER 3 MULTIPLAYER - NEXT-GEN BUILD
echo ========================================
echo.

REM Find MSBuild
set MSBUILD_PATH=""
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set MSBUILD_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe" (
    set MSBUILD_PATH="C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe" (
    set MSBUILD_PATH="C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe" (
    set MSBUILD_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe" (
    set MSBUILD_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\MSBuild\Current\Bin\MSBuild.exe"
) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\MSBuild.exe" (
    set MSBUILD_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
)

if "%MSBUILD_PATH%"=="" (
    echo Error: MSBuild not found!
    echo Please install Visual Studio or Visual Studio Build Tools
    echo Download from: https://visualstudio.microsoft.com/downloads/
    pause
    exit /b 1
)

echo Found MSBuild: %MSBUILD_PATH%
echo.

REM Build the project
echo Building Next-Gen server...
%MSBUILD_PATH% Witcher3-MP.vcxproj /p:Configuration=Release /p:Platform=x64 /p:OutDir=build\nextgen\ /p:TargetName=Witcher3MP_NextGen

if %ERRORLEVEL% neq 0 (
    echo.
    echo ERROR: Build failed!
    echo Please check the error messages above.
    pause
    exit /b 1
)

echo.
echo ========================================
echo  NEXT-GEN BUILD COMPLETED SUCCESSFULLY!
echo ========================================
echo.
echo Executable: build\nextgen\Witcher3MP_NextGen.exe
echo.

REM Copy configuration files
echo Copying configuration files...
if not exist "build\nextgen\config" mkdir "build\nextgen\config"
copy "config\*.json" "build\nextgen\config\" >nul 2>nul

REM Copy ASIO library
echo Copying ASIO library...
if not exist "build\nextgen\dep" mkdir "build\nextgen\dep"
if not exist "build\nextgen\dep\asio" mkdir "build\nextgen\dep\asio"
xcopy "dep\asio\*" "build\nextgen\dep\asio\" /E /I /Q >nul 2>nul

echo.
echo ========================================
echo  NEXT-GEN SERVER READY TO RUN!
echo ========================================
echo.
echo To start the Next-Gen server:
echo   1. Navigate to: build\nextgen
echo   2. Run: Witcher3MP_NextGen.exe
echo.
echo The server will automatically:
echo   - Detect The Witcher 3 Next-Gen version
echo   - Apply optimal Next-Gen settings
echo   - Enable Ray Tracing, DLSS, FSR, HDR, RTX
echo   - Optimize for low latency
echo   - Monitor performance in real-time
echo.
echo Press any key to continue...
pause >nul
