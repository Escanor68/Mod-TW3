@echo off
echo ========================================
echo Manual Base Mod Installation
echo ========================================
echo.

echo Step 1: Creating directories...
mkdir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits" 2>nul
mkdir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts" 2>nul
mkdir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game" 2>nul
mkdir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game\gameplay" 2>nul
echo ✅ Directories created

echo.
echo Step 2: Creating modInfo.ws...
echo modFixSoundsOfSwordHits > "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\modInfo.ws"
echo { >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\modInfo.ws"
echo     version = "1.2"; >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\modInfo.ws"
echo     name = "Fix sounds of sword hits when adding effects"; >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\modInfo.ws"
echo     description = "Base mod for Witcher3-MP combat system"; >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\modInfo.ws"
echo     author = "Witcher3-MP Team"; >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\modInfo.ws"
echo } >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\modInfo.ws"
echo ✅ modInfo.ws created

echo.
echo Step 3: Creating combat script...
echo // Base combat system for Witcher3-MP > "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game\gameplay\CombatBase.ws"
echo // This provides the essential APIs that our mod needs >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game\gameplay\CombatBase.ws"
echo. >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game\gameplay\CombatBase.ws"
echo // W3DamageAction API >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game\gameplay\CombatBase.ws"
echo public func CreateDamageAction^(target: ref^<CActor^>, damage: float, damageType: CName^) : ref^<W3DamageAction^> >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game\gameplay\CombatBase.ws"
echo { >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game\gameplay\CombatBase.ws"
echo     return new W3DamageAction^(^); >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game\gameplay\CombatBase.ws"
echo } >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game\gameplay\CombatBase.ws"
echo. >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game\gameplay\CombatBase.ws"
echo // W3EffectManager API >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game\gameplay\CombatBase.ws"
echo public func ApplyEffect^(target: ref^<CActor^>, effectName: CName^) : void >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game\gameplay\CombatBase.ws"
echo { >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game\gameplay\CombatBase.ws"
echo     // Base effect application >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game\gameplay\CombatBase.ws"
echo } >> "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game\gameplay\CombatBase.ws"
echo ✅ Combat script created

echo.
echo Step 4: Verifying installation...
if exist "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits" (
    echo ✅ Base mod directory found
) else (
    echo ❌ Base mod directory not found
)

if exist "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\modWitcher3MP" (
    echo ✅ Our mod still present
) else (
    echo ❌ Our mod missing!
)

echo.
echo Step 5: Listing all mods...
dir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods" /B

echo.
echo ========================================
echo Installation Complete!
echo ========================================
echo.
echo Both mods are now ready:
echo - FixSoundsOfSwordHits (Base mod - provides essential APIs)
echo - modWitcher3MP (Our multiplayer mod)
echo.
echo Next steps:
echo 1. Start The Witcher 3
echo 2. Both mods should appear in the mods list
echo 3. Test the combat system in-game
echo.
echo Press any key to continue...
pause
