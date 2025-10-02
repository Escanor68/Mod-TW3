@echo off
echo ========================================
echo  WITCHER 3 MULTIPLAYER - NEXT-GEN BUILD
echo ========================================
echo.

REM Set MSBuild path directly
set MSBUILD_PATH="C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe"

REM Check if MSBuild exists
if not exist %MSBUILD_PATH% (
    echo Error: MSBuild not found at expected location!
    echo Expected: %MSBUILD_PATH%
    echo Please check your Visual Studio 2022 installation.
    pause
    exit /b 1
)

echo Found MSBuild: %MSBUILD_PATH%
echo.

REM Create build directory
if not exist "build\nextgen" mkdir "build\nextgen"

REM Build the project
echo Building Next-Gen server...
%MSBUILD_PATH% Witcher3-MP.vcxproj /p:Configuration=Release /p:Platform=x64 /p:OutDir=build\nextgen\ /p:TargetName=Witcher3MP_NextGen /verbosity:minimal

if %ERRORLEVEL% neq 0 (
    echo.
    echo ERROR: Build failed!
    echo Please check the error messages above.
    echo.
    echo Common solutions:
    echo 1. Make sure all source files exist
    echo 2. Check include paths in project file
    echo 3. Verify Visual Studio 2022 is properly installed
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
