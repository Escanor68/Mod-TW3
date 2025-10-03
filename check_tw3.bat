@echo off
echo Checking for The Witcher 3 installation...

if exist "C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3" (
    echo Found at: C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3
) else (
    echo Not found at: C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3
)

if exist "E:\SteamLibrary\steamapps\common\The Witcher 3" (
    echo Found at: E:\SteamLibrary\steamapps\common\The Witcher 3
) else (
    echo Not found at: E:\SteamLibrary\steamapps\common\The Witcher 3
)

if exist "D:\SteamLibrary\steamapps\common\The Witcher 3" (
    echo Found at: D:\SteamLibrary\steamapps\common\The Witcher 3
) else (
    echo Not found at: D:\SteamLibrary\steamapps\common\The Witcher 3
)

if exist "F:\SteamLibrary\steamapps\common\The Witcher 3" (
    echo Found at: F:\SteamLibrary\steamapps\common\The Witcher 3
) else (
    echo Not found at: F:\SteamLibrary\steamapps\common\The Witcher 3
)

pause
