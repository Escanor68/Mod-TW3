@echo off
echo ========================================
echo    Witcher3-MP Complete Setup
echo ========================================
echo.

echo This script will:
echo 1. Compile the Witcher3-MP server
echo 2. Install the mod to The Witcher 3
echo 3. Create the startup script
echo 4. Configure for coop story mode
echo.

pause

echo.
echo Step 1: Compiling Witcher3-MP server...
call scripts\build_ultra_simple.bat

if %errorlevel% neq 0 (
    echo ERROR: Compilation failed!
    pause
    exit /b 1
)

echo.
echo Step 2: Installing mod to The Witcher 3...
call scripts\install_mod.bat

if %errorlevel% neq 0 (
    echo ERROR: Mod installation failed!
    pause
    exit /b 1
)

echo.
echo Step 3: Creating startup script...
call scripts\create_startup_script.bat

if %errorlevel% neq 0 (
    echo ERROR: Startup script creation failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo    Setup Complete!
echo ========================================
echo.
echo Witcher3-MP is now ready to use!
echo.
echo Configuration:
echo - Server port: 60000
echo - Max players: 4 (coop story mode)
echo - Quest sharing: Enabled
echo - Progression sync: Enabled
echo.
echo To launch:
echo 1. Go to your Witcher 3 installation directory
echo 2. Run: start_nextgen.bat
echo.
echo The mod will automatically:
echo - Start the server
echo - Launch The Witcher 3 with mod enabled
echo - Stop the server when you exit the game
echo.

pause
