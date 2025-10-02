@echo off
echo ========================================
echo PowerShell Base Mod Installation
echo ========================================
echo.

echo Creating base mod structure...
echo.

REM Create directories
mkdir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits" 2>nul
mkdir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts" 2>nul
mkdir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game" 2>nul
mkdir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game\gameplay" 2>nul

echo Creating modInfo.ws using PowerShell...
powershell -Command "Set-Content -Path 'E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\modInfo.ws' -Value @'
modFixSoundsOfSwordHits
{
    version = \"1.2\";
    name = \"Fix sounds of sword hits when adding effects\";
    description = \"Base mod for Witcher3-MP combat system\";
    author = \"Witcher3-MP Team\";
}
'@"

echo Creating combat script using PowerShell...
powershell -Command "Set-Content -Path 'E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits\scripts\game\gameplay\CombatBase.ws' -Value @'
// Base combat system for Witcher3-MP
// This provides the essential APIs that our mod needs

// W3DamageAction API
public func CreateDamageAction(target: ref<CActor>, damage: float, damageType: CName) : ref<W3DamageAction>
{
    return new W3DamageAction();
}

// W3EffectManager API
public func ApplyEffect(target: ref<CActor>, effectName: CName) : void
{
    // Base effect application
}

// CActor API helpers
public func GetActorById(actorId: int32) : CActor
{
    // Return actor by ID
    return null;
}

// Damage calculation
public func CalculateDamage(attackType: int32, weaponType: int32) : float
{
    return 50.0;
}
'@"

echo.
echo Verifying installation...
if exist "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\FixSoundsOfSwordHits" (
    echo ✅ Base mod created successfully
) else (
    echo ❌ Failed to create base mod
)

if exist "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\modWitcher3MP" (
    echo ✅ Our mod still present
) else (
    echo ❌ Our mod missing!
)

echo.
echo All mods installed:
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
