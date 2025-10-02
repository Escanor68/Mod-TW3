@echo off
setlocal enabledelayedexpansion

echo ========================================
echo Witcher3-MP Log Analysis Tool
echo ========================================
echo.

REM Check if logs directory exists
if not exist "logs" (
    echo ❌ No logs directory found. Run INSTALL_FOR_GAME.bat first.
    pause
    exit /b 1
)

REM Find the most recent log file
set "LATEST_LOG="
for /f "delims=" %%i in ('dir /b /o-d logs\installation_*.log 2^>nul') do (
    set "LATEST_LOG=logs\%%i"
    goto :found
)
:found

if "%LATEST_LOG%"=="" (
    echo ❌ No installation logs found.
    pause
    exit /b 1
)

echo Analyzing log file: %LATEST_LOG%
echo.

REM Create analysis report
set "ANALYSIS_FILE=logs\analysis_%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%.txt"
set "ANALYSIS_FILE=%ANALYSIS_FILE: =0%"

echo ======================================== > "%ANALYSIS_FILE%"
echo Witcher3-MP Log Analysis Report >> "%ANALYSIS_FILE%"
echo Generated: %date% %time% >> "%ANALYSIS_FILE%"
echo ======================================== >> "%ANALYSIS_FILE%"
echo. >> "%ANALYSIS_FILE%"

REM Count errors and warnings
set "ERROR_COUNT=0"
set "WARNING_COUNT=0"
set "SUCCESS_COUNT=0"

for /f "tokens=*" %%a in ('findstr /i "ERROR" "%LATEST_LOG%"') do set /a ERROR_COUNT+=1
for /f "tokens=*" %%a in ('findstr /i "WARNING" "%LATEST_LOG%"') do set /a WARNING_COUNT+=1
for /f "tokens=*" %%a in ('findstr /i "SUCCESS" "%LATEST_LOG%"') do set /a SUCCESS_COUNT+=1

echo Log Analysis Summary: >> "%ANALYSIS_FILE%"
echo - Errors: %ERROR_COUNT% >> "%ANALYSIS_FILE%"
echo - Warnings: %WARNING_COUNT% >> "%ANALYSIS_FILE%"
echo - Success operations: %SUCCESS_COUNT% >> "%ANALYSIS_FILE%"
echo. >> "%ANALYSIS_FILE%"

echo Log Analysis Summary:
echo - Errors: %ERROR_COUNT%
echo - Warnings: %WARNING_COUNT%
echo - Success operations: %SUCCESS_COUNT%
echo.

REM Analyze specific issues
echo Analyzing specific issues... >> "%ANALYSIS_FILE%"
echo Analyzing specific issues...

if %ERROR_COUNT% gtr 0 (
    echo ERROR ANALYSIS: >> "%ANALYSIS_FILE%"
    echo ================ >> "%ANALYSIS_FILE%"
    for /f "tokens=*" %%a in ('findstr /i "ERROR" "%LATEST_LOG%"') do (
        echo %%a >> "%ANALYSIS_FILE%"
    )
    echo. >> "%ANALYSIS_FILE%"
    
    echo ERROR ANALYSIS:
    echo ================
    for /f "tokens=*" %%a in ('findstr /i "ERROR" "%LATEST_LOG%"') do (
        echo %%a
    )
    echo.
)

if %WARNING_COUNT% gtr 0 (
    echo WARNING ANALYSIS: >> "%ANALYSIS_FILE%"
    echo ================== >> "%ANALYSIS_FILE%"
    for /f "tokens=*" %%a in ('findstr /i "WARNING" "%LATEST_LOG%"') do (
        echo %%a >> "%ANALYSIS_FILE%"
    )
    echo. >> "%ANALYSIS_FILE%"
    
    echo WARNING ANALYSIS:
    echo ==================
    for /f "tokens=*" %%a in ('findstr /i "WARNING" "%LATEST_LOG%"') do (
        echo %%a
    )
    echo.
)

REM Generate optimization recommendations
echo OPTIMIZATION RECOMMENDATIONS: >> "%ANALYSIS_FILE%"
echo =============================== >> "%ANALYSIS_FILE%"
echo. >> "%ANALYSIS_FILE%"

echo OPTIMIZATION RECOMMENDATIONS:
echo ===============================

if %ERROR_COUNT% equ 0 (
    echo ✅ No errors found - installation was successful >> "%ANALYSIS_FILE%"
    echo ✅ No errors found - installation was successful
) else (
    echo ⚠️ %ERROR_COUNT% errors found - review and fix before testing >> "%ANALYSIS_FILE%"
    echo ⚠️ %ERROR_COUNT% errors found - review and fix before testing
)

if %WARNING_COUNT% gtr 0 (
    echo ⚠️ %WARNING_COUNT% warnings found - consider addressing for optimal performance >> "%ANALYSIS_FILE%"
    echo ⚠️ %WARNING_COUNT% warnings found - consider addressing for optimal performance
)

REM Check for specific optimization opportunities
echo. >> "%ANALYSIS_FILE%"
echo Specific Recommendations: >> "%ANALYSIS_FILE%"

echo. >> "%ANALYSIS_FILE%"
echo 1. File System Optimization: >> "%ANALYSIS_FILE%"
echo    - Ensure all WitcherScript files are properly copied >> "%ANALYSIS_FILE%"
echo    - Verify file permissions are correct >> "%ANALYSIS_FILE%"
echo    - Check for any missing dependencies >> "%ANALYSIS_FILE%"

echo. >> "%ANALYSIS_FILE%"
echo 2. Mod Integration: >> "%ANALYSIS_FILE%"
echo    - Verify base mod "Fix sounds of sword hits when adding effects" is installed >> "%ANALYSIS_FILE%"
echo    - Check REDkit compatibility >> "%ANALYSIS_FILE%"
echo    - Ensure proper mod loading order >> "%ANALYSIS_FILE%"

echo. >> "%ANALYSIS_FILE%"
echo 3. Performance Optimization: >> "%ANALYSIS_FILE%"
echo    - Monitor FPS during combat >> "%ANALYSIS_FILE%"
echo    - Check memory usage >> "%ANALYSIS_FILE%"
echo    - Verify network performance if testing multiplayer >> "%ANALYSIS_FILE%"

echo. >> "%ANALYSIS_FILE%"
echo 4. Testing Recommendations: >> "%ANALYSIS_FILE%"
echo    - Test with different weapon types >> "%ANALYSIS_FILE%"
echo    - Verify damage calculations >> "%ANALYSIS_FILE%"
echo    - Check critical hit system >> "%ANALYSIS_FILE%"
echo    - Test with multiple enemies >> "%ANALYSIS_FILE%"

echo.
echo 1. File System Optimization:
echo    - Ensure all WitcherScript files are properly copied
echo    - Verify file permissions are correct
echo    - Check for any missing dependencies

echo.
echo 2. Mod Integration:
echo    - Verify base mod "Fix sounds of sword hits when adding effects" is installed
echo    - Check REDkit compatibility
echo    - Ensure proper mod loading order

echo.
echo 3. Performance Optimization:
echo    - Monitor FPS during combat
echo    - Check memory usage
echo    - Verify network performance if testing multiplayer

echo.
echo 4. Testing Recommendations:
echo    - Test with different weapon types
echo    - Verify damage calculations
echo    - Check critical hit system
echo    - Test with multiple enemies

echo.
echo Analysis report saved to: %ANALYSIS_FILE%
echo.
pause
