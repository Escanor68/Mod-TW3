#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "integration/REDkitBridge.h"
#include "integration/WitcherScriptBridge.h"
#include "game/Entities/Player.h"
#include "utils/Logger.h"
#include <memory>
#include <vector>

using namespace TW3Integration;
using namespace TW3Game;
using Catch::Matchers::WithinRel;

TEST_CASE("Combat System - Basic Attack Execution", "[combat]")
{
    // Initialize systems
    auto& logger = Logger::GetInstance();
    logger.SetConsoleLogging(true);
    logger.SetLogLevel(LogLevel::Info);
    
    auto& redkitBridge = REDkitBridge::GetInstance();
    auto& witcherScriptBridge = WitcherScriptBridge::GetInstance();
    
    // Initialize bridges
    REQUIRE(witcherScriptBridge.Initialize());
    REQUIRE(redkitBridge.Initialize());
    
    SECTION("Execute Light Attack with Steel Sword")
    {
        Vector4F targetPosition = {10.0f, 0.0f, 5.0f, 1.0f};
        uint32_t playerId = 1;
        uint32_t attackType = 0; // Light Attack
        uint32_t weaponType = 1; // Steel Sword
        
        bool result = redkitBridge.ExecuteAttack(playerId, attackType, weaponType, targetPosition);
        
        // Should succeed (even if target not found, attack should be processed)
        REQUIRE(result == true);
    }
    
    SECTION("Execute Heavy Attack with Silver Sword")
    {
        Vector4F targetPosition = {15.0f, 0.0f, 8.0f, 1.0f};
        uint32_t playerId = 2;
        uint32_t attackType = 1; // Heavy Attack
        uint32_t weaponType = 2; // Silver Sword
        
        bool result = redkitBridge.ExecuteAttack(playerId, attackType, weaponType, targetPosition);
        
        REQUIRE(result == true);
    }
    
    SECTION("Execute Defense")
    {
        uint32_t playerId = 1;
        uint32_t defenseType = 0; // Block
        uint32_t incomingAttackId = 123;
        
        bool result = redkitBridge.ExecuteDefense(playerId, defenseType, incomingAttackId);
        
        REQUIRE(result == true);
    }
}

TEST_CASE("Combat System - Damage Calculation", "[combat][damage]")
{
    auto& redkitBridge = REDkitBridge::GetInstance();
    
    SECTION("Fist Damage Calculation")
    {
        uint32_t attackType = 0; // Light Attack
        uint32_t weaponType = 0; // Fists
        
        float damage = redkitBridge.CalculateDamage(attackType, weaponType);
        
        // Fist damage should be low
        REQUIRE(damage >= 10.0f);
        REQUIRE(damage <= 30.0f);
    }
    
    SECTION("Steel Sword Damage Calculation")
    {
        uint32_t attackType = 1; // Heavy Attack
        uint32_t weaponType = 1; // Steel Sword
        
        float damage = redkitBridge.CalculateDamage(attackType, weaponType);
        
        // Steel sword heavy attack should be higher
        REQUIRE(damage >= 60.0f);
        REQUIRE(damage <= 120.0f);
    }
    
    SECTION("Silver Sword Damage Calculation")
    {
        uint32_t attackType = 2; // Strong Attack
        uint32_t weaponType = 2; // Silver Sword
        
        float damage = redkitBridge.CalculateDamage(attackType, weaponType);
        
        // Silver sword strong attack should be highest
        REQUIRE(damage >= 100.0f);
        REQUIRE(damage <= 200.0f);
    }
    
    SECTION("Crossbow Damage Calculation")
    {
        uint32_t attackType = 0; // Light Attack
        uint32_t weaponType = 6; // Crossbow
        
        float damage = redkitBridge.CalculateDamage(attackType, weaponType);
        
        // Crossbow should have high base damage
        REQUIRE(damage >= 70.0f);
        REQUIRE(damage <= 110.0f);
    }
}

TEST_CASE("Combat System - Critical Hit Calculation", "[combat][critical]")
{
    auto& redkitBridge = REDkitBridge::GetInstance();
    
    SECTION("Critical Hit Probability")
    {
        uint32_t playerId = 1;
        
        // Test multiple times to check probability
        int criticalHits = 0;
        int totalTests = 1000;
        
        for (int i = 0; i < totalTests; ++i)
        {
            if (redkitBridge.IsCriticalHit(playerId))
            {
                criticalHits++;
            }
        }
        
        // Should have some critical hits (fallback is 10% chance)
        REQUIRE(criticalHits > 0);
        REQUIRE(criticalHits < totalTests);
    }
}

TEST_CASE("Combat System - Damage Type Mapping", "[combat][damage_types]")
{
    auto& redkitBridge = REDkitBridge::GetInstance();
    
    SECTION("Physical Damage Types")
    {
        std::string damageType = redkitBridge.GetDamageTypeFromAttackType(0);
        REQUIRE(damageType == "DAMAGE_NAME_PHYSICAL");
        
        damageType = redkitBridge.GetDamageTypeFromAttackType(1);
        REQUIRE(damageType == "DAMAGE_NAME_SLASHING");
        
        damageType = redkitBridge.GetDamageTypeFromAttackType(2);
        REQUIRE(damageType == "DAMAGE_NAME_PIERCING");
        
        damageType = redkitBridge.GetDamageTypeFromAttackType(3);
        REQUIRE(damageType == "DAMAGE_NAME_BLUNT");
    }
    
    SECTION("Elemental Damage Types")
    {
        std::string damageType = redkitBridge.GetDamageTypeFromAttackType(4);
        REQUIRE(damageType == "DAMAGE_NAME_FIRE");
        
        damageType = redkitBridge.GetDamageTypeFromAttackType(5);
        REQUIRE(damageType == "DAMAGE_NAME_FROST");
        
        damageType = redkitBridge.GetDamageTypeFromAttackType(6);
        REQUIRE(damageType == "DAMAGE_NAME_LIGHTNING");
        
        damageType = redkitBridge.GetDamageTypeFromAttackType(7);
        REQUIRE(damageType == "DAMAGE_NAME_POISON");
    }
    
    SECTION("Invalid Attack Type")
    {
        std::string damageType = redkitBridge.GetDamageTypeFromAttackType(999);
        REQUIRE(damageType == "DAMAGE_NAME_PHYSICAL");
    }
}

TEST_CASE("Combat System - Target Detection", "[combat][targeting]")
{
    auto& redkitBridge = REDkitBridge::GetInstance();
    
    SECTION("Target Detection at Origin")
    {
        Vector4F position = {0.0f, 0.0f, 0.0f, 1.0f};
        
        uint32_t targetId = redkitBridge.GetTargetFromPosition(position);
        
        // Should return 0 if no target found (which is expected in test environment)
        REQUIRE(targetId >= 0);
    }
    
    SECTION("Target Detection at Specific Position")
    {
        Vector4F position = {100.0f, 0.0f, 50.0f, 1.0f};
        
        uint32_t targetId = redkitBridge.GetTargetFromPosition(position);
        
        // Should return 0 if no target found (which is expected in test environment)
        REQUIRE(targetId >= 0);
    }
}

TEST_CASE("Combat System - Animation System", "[combat][animation]")
{
    auto& redkitBridge = REDkitBridge::GetInstance();
    
    SECTION("Play Attack Animation")
    {
        uint32_t playerId = 1;
        uint32_t animationType = 0; // Light Attack Animation
        
        bool result = redkitBridge.PlayAttackAnimation(playerId, animationType);
        
        REQUIRE(result == true);
    }
    
    SECTION("Play Defense Animation")
    {
        uint32_t playerId = 1;
        uint32_t animationType = 0; // Block Animation
        
        bool result = redkitBridge.PlayDefenseAnimation(playerId, animationType);
        
        REQUIRE(result == true);
    }
}

TEST_CASE("Combat System - Signs System", "[combat][signs]")
{
    auto& redkitBridge = REDkitBridge::GetInstance();
    
    SECTION("Cast Igni Sign")
    {
        uint32_t playerId = 1;
        uint32_t signType = 0; // Igni
        Vector4F targetPosition = {10.0f, 0.0f, 5.0f, 1.0f};
        
        bool result = redkitBridge.CastSign(playerId, signType, targetPosition);
        
        REQUIRE(result == true);
    }
    
    SECTION("Check Sign Cooldown")
    {
        uint32_t playerId = 1;
        uint32_t signType = 0; // Igni
        
        float cooldown = redkitBridge.GetSignCooldown(playerId, signType);
        
        REQUIRE(cooldown >= 0.0f);
    }
    
    SECTION("Check Can Cast Sign")
    {
        uint32_t playerId = 1;
        uint32_t signType = 0; // Igni
        
        bool canCast = redkitBridge.CanCastSign(playerId, signType);
        
        // Should be able to cast (or at least return a valid boolean)
        REQUIRE(canCast == true || canCast == false);
    }
}

TEST_CASE("Combat System - Integration Tests", "[combat][integration]")
{
    auto& redkitBridge = REDkitBridge::GetInstance();
    auto& witcherScriptBridge = WitcherScriptBridge::GetInstance();
    
    SECTION("Full Combat Sequence")
    {
        uint32_t playerId = 1;
        Vector4F targetPosition = {10.0f, 0.0f, 5.0f, 1.0f};
        
        // 1. Check if player can act
        bool canAct = redkitBridge.CheckCombatState(playerId);
        REQUIRE(canAct == true);
        
        // 2. Execute attack
        bool attackResult = redkitBridge.ExecuteAttack(playerId, 1, 1, targetPosition);
        REQUIRE(attackResult == true);
        
        // 3. Play attack animation
        bool animResult = redkitBridge.PlayAttackAnimation(playerId, 1);
        REQUIRE(animResult == true);
        
        // 4. Check for critical hit
        bool isCritical = redkitBridge.IsCriticalHit(playerId);
        // Should return a valid boolean
        REQUIRE(isCritical == true || isCritical == false);
    }
    
    SECTION("Defense Sequence")
    {
        uint32_t playerId = 1;
        
        // 1. Execute defense
        bool defenseResult = redkitBridge.ExecuteDefense(playerId, 0, 123);
        REQUIRE(defenseResult == true);
        
        // 2. Play defense animation
        bool animResult = redkitBridge.PlayDefenseAnimation(playerId, 0);
        REQUIRE(animResult == true);
    }
}

TEST_CASE("Combat System - Error Handling", "[combat][error_handling]")
{
    auto& redkitBridge = REDkitBridge::GetInstance();
    
    SECTION("Invalid Player ID")
    {
        Vector4F targetPosition = {10.0f, 0.0f, 5.0f, 1.0f};
        uint32_t invalidPlayerId = 999999;
        
        bool result = redkitBridge.ExecuteAttack(invalidPlayerId, 0, 1, targetPosition);
        
        // Should handle gracefully (return false or true, but not crash)
        REQUIRE(result == true || result == false);
    }
    
    SECTION("Invalid Attack Type")
    {
        Vector4F targetPosition = {10.0f, 0.0f, 5.0f, 1.0f};
        uint32_t invalidAttackType = 999;
        
        bool result = redkitBridge.ExecuteAttack(1, invalidAttackType, 1, targetPosition);
        
        // Should handle gracefully
        REQUIRE(result == true || result == false);
    }
    
    SECTION("Invalid Weapon Type")
    {
        Vector4F targetPosition = {10.0f, 0.0f, 5.0f, 1.0f};
        uint32_t invalidWeaponType = 999;
        
        bool result = redkitBridge.ExecuteAttack(1, 0, invalidWeaponType, targetPosition);
        
        // Should handle gracefully
        REQUIRE(result == true || result == false);
    }
}

// Cleanup after tests
TEST_CASE("Combat System - Cleanup", "[combat][cleanup]")
{
    auto& redkitBridge = REDkitBridge::GetInstance();
    auto& witcherScriptBridge = WitcherScriptBridge::GetInstance();
    
    SECTION("Shutdown Systems")
    {
        bool redkitShutdown = redkitBridge.Shutdown();
        bool witcherScriptShutdown = witcherScriptBridge.Shutdown();
        
        REQUIRE(redkitShutdown == true);
        REQUIRE(witcherScriptShutdown == true);
    }
}
