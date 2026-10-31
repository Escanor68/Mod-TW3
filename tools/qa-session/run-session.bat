@echo off
REM Windows wrapper for run-session.ps1
setlocal
set MODE=%~1
if "%MODE%"=="" set MODE=host
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0run-session.ps1" %MODE%
exit /b %ERRORLEVEL%
