@echo off
echo Building Witcher3-MP mod for REDkit...

REM Set paths
set REDKIT_PATH="C:\Program Files (x86)\Steam\steamapps\common\REDkit"
set WCC_LITE_PATH="%REDKIT_PATH%\bin\wcc_lite.exe"
set MOD_PATH="%~dp0"
set OUTPUT_PATH="%MOD_PATH%dist"

REM Create output directory
if not exist "%OUTPUT_PATH%" mkdir "%OUTPUT_PATH%"

REM Check if REDkit is installed
if not exist "%WCC_LITE_PATH%" (
    echo ERROR: REDkit not found at %REDKIT_PATH%
    echo Please install REDkit from Steam or official website
    pause
    exit /b 1
)

REM Build the mod
echo Building mod with wcc_lite...
"%WCC_LITE_PATH%" -depot -root "%MOD_PATH%" -out "%OUTPUT_PATH%"

if %ERRORLEVEL% neq 0 (
    echo ERROR: Build failed
    pause
    exit /b 1
)

echo Build completed successfully!
echo Output: %OUTPUT_PATH%
pause
