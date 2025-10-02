@echo off
echo ========================================
echo  WITCHER 3 MULTIPLAYER - CLEAN
echo ========================================
echo.

echo [1/4] Cleaning build directories...
if exist "build" rmdir /s /q "build"
if exist "Debug" rmdir /s /q "Debug"
if exist "Release" rmdir /s /q "Release"
if exist "x64" rmdir /s /q "x64"
if exist "x86" rmdir /s /q "x86"
if exist "Win32" rmdir /s /q "Win32"

echo [2/4] Cleaning object files...
if exist "*.obj" del /q "*.obj"
if exist "*.o" del /q "*.o"

echo [3/4] Cleaning debug files...
if exist "*.pdb" del /q "*.pdb"
if exist "*.ilk" del /q "*.ilk"
if exist "*.exp" del /q "*.exp"
if exist "*.lib" del /q "*.lib"
if exist "*.idb" del /q "*.idb"
if exist "*.ipdb" del /q "*.ipdb"

echo [4/4] Cleaning executable files...
if exist "Witcher3-MP*.exe" del /q "Witcher3-MP*.exe"
if exist "Witcher3-MP*.dll" del /q "Witcher3-MP*.dll"

echo.
echo ========================================
echo  CLEAN COMPLETED SUCCESSFULLY!
echo ========================================
echo.
echo All build artifacts have been removed.
echo.
echo Press any key to exit...
pause >nul
