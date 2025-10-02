@echo off
echo ========================================
echo Collecting Witcher 3 Logs
echo ========================================
echo.

REM Create logs directory
if not exist "logs" mkdir "logs"

echo Collecting logs from The Witcher 3...
echo.

REM Copy TW3 logs
echo Copying TW3 game logs...
xcopy "E:\SteamLibrary\steamapps\common\The Witcher 3\bin\config\r4game\user_config_matrix\pc\*.log" "logs\" /Y /Q 2>nul
if !ERRORLEVEL! equ 0 (
    echo ✅ TW3 logs copied
) else (
    echo ⚠️ No TW3 logs found or error copying
)

REM Copy TW3 text files
echo Copying TW3 text files...
xcopy "E:\SteamLibrary\steamapps\common\The Witcher 3\bin\config\r4game\user_config_matrix\pc\*.txt" "logs\" /Y /Q 2>nul

REM Copy Steam logs
echo Copying Steam logs...
xcopy "C:\Users\USUARIO\AppData\Local\Steam\logs\*.log" "logs\" /Y /Q 2>nul
if !ERRORLEVEL! equ 0 (
    echo ✅ Steam logs copied
) else (
    echo ⚠️ No Steam logs found or error copying
)

REM Copy crash dumps
echo Copying crash dumps...
xcopy "C:\Users\USUARIO\AppData\Local\CrashDumps\*.dmp" "logs\" /Y /Q 2>nul
if !ERRORLEVEL! equ 0 (
    echo ✅ Crash dumps copied
) else (
    echo ℹ️ No crash dumps found
)

REM List collected files
echo.
echo Collected log files:
dir "logs" /B

echo.
echo ========================================
echo Log Collection Complete!
echo ========================================
echo.
echo Logs saved to: logs\
echo.
echo You can now share these logs for analysis.
echo.
echo Press any key to continue...
pause
