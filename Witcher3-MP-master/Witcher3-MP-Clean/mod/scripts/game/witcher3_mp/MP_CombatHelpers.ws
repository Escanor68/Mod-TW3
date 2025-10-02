// Witcher3-MP Combat Helpers
// Helper functions for combat system integration

using game;
using game::player;
using game::stats;
using game::effects;

// Target detection helper
public func FindTargetAtPosition(position: Vector4, searchRadius: float, targetType: string) : string
{
    var allActors: array<CActor>;
    var target: CActor;
    var closestTarget: CActor;
    var closestDistance: float = 999999.0;
    var distance: float;
    var result: string;
    
    // Get all actors in the game
    allActors = theGame.GetAllActors();
    
    // Search for targets within radius
    for (let i: int32 = 0; i < ArraySize(allActors); i++)
    {
        target = allActors[i];
        if (!target || target == thePlayer) continue;
        
        // Calculate distance
        distance = Vector4.Distance(position, target.GetPosition());
        
        if (distance <= searchRadius && distance < closestDistance)
        {
            // Check target type
            if (targetType == "enemy" && target.IsEnemy())
            {
                closestTarget = target;
                closestDistance = distance;
            }
            else if (targetType == "player" && target == thePlayer)
            {
                closestTarget = target;
                closestDistance = distance;
            }
            else if (targetType == "any")
            {
                closestTarget = target;
                closestDistance = distance;
            }
        }
    }
    
    // Return result as JSON
    if (closestTarget)
    {
        result = "{\"targetId\":" + ToString(closestTarget.GetEntityID()) + ",\"distance\":" + ToString(closestDistance) + "}";
    }
    else
    {
        result = "{\"targetId\":0,\"distance\":0.0}";
    }
    
    return result;
}

// Player stats helper
public func GetPlayerStats(playerId: int32, stats: array<string>) : string
{
    var player: CActor;
    var result: string;
    var luck: float = 0.0;
    var criticalChance: float = 5.0;
    var level: int32 = 1;
    
    // Get player actor
    if (playerId == 0 || playerId == thePlayer.GetEntityID())
    {
        player = thePlayer;
    }
    else
    {
        player = GetActorById(playerId);
    }
    
    if (!player) return "{\"error\":\"Player not found\"}";
    
    // Get stats based on requested values
    for (let i: int32 = 0; i < ArraySize(stats); i++)
    {
        switch (stats[i])
        {
            case "luck":
                luck = player.GetStatValue(BCS_Luck);
                break;
            case "criticalChance":
                criticalChance = player.GetStatValue(BCS_CriticalChance);
                break;
            case "level":
                level = Cast<int32>(player.GetStatValue(BCS_Level));
                break;
        }
    }
    
    // Return result as JSON
    result = "{\"luck\":" + ToString(luck) + ",\"criticalChance\":" + ToString(criticalChance) + ",\"level\":" + ToString(level) + "}";
    return result;
}

// Actor lookup helper
private func GetActorById(actorId: int32) : CActor
{
    var allActors: array<CActor>;
    var actor: CActor;
    
    // Get all actors in the game
    allActors = theGame.GetAllActors();
    
    // Search for actor by ID
    for (let i: int32 = 0; i < ArraySize(allActors); i++)
    {
        actor = allActors[i];
        if (actor && actor.GetEntityID() == actorId)
        {
            return actor;
        }
    }
    
    return null;
}

// Combat state validation helper
public func ValidateCombatState(playerId: int32) : string
{
    var player: CActor;
    var canAct: bool = true;
    var reason: string = "";
    
    // Get player actor
    if (playerId == 0 || playerId == thePlayer.GetEntityID())
    {
        player = thePlayer;
    }
    else
    {
        player = GetActorById(playerId);
    }
    
    if (!player) return "{\"canAct\":false,\"reason\":\"Player not found\"}";
    
    // Check for debuffing effects
    if (player.HasBuff(EET_HeavyKnockdown))
    {
        canAct = false;
        reason = "Heavy Knockdown";
    }
    else if (player.HasBuff(EET_Knockdown))
    {
        canAct = false;
        reason = "Knockdown";
    }
    else if (player.HasBuff(EET_LongStagger))
    {
        canAct = false;
        reason = "Long Stagger";
    }
    else if (player.HasBuff(EET_Stagger))
    {
        canAct = false;
        reason = "Stagger";
    }
    else if (player.HasBuff(EET_Stun))
    {
        canAct = false;
        reason = "Stun";
    }
    else if (player.HasBuff(EET_Charm))
    {
        canAct = false;
        reason = "Charm";
    }
    else if (player.HasBuff(EET_Fear))
    {
        canAct = false;
        reason = "Fear";
    }
    
    // Check if player is dead
    if (player.IsDead())
    {
        canAct = false;
        reason = "Dead";
    }
    
    // Return result as JSON
    return "{\"canAct\":" + ToString(canAct) + ",\"reason\":\"" + reason + "\"}";
}

// Damage calculation helper
public func CalculateDamageValue(attackType: int32, weaponType: int32, attackerId: int32) : string
{
    var attacker: CActor;
    var baseDamage: float = 50.0;
    var weaponMultiplier: float = 1.0;
    var attackMultiplier: float = 1.0;
    var statMultiplier: float = 1.0;
    var result: string;
    
    // Get attacker actor
    if (attackerId == 0 || attackerId == thePlayer.GetEntityID())
    {
        attacker = thePlayer;
    }
    else
    {
        attacker = GetActorById(attackerId);
    }
    
    if (!attacker) return "{\"damage\":0.0}";
    
    // Base damage by weapon type
    switch (weaponType)
    {
        case 0: baseDamage = 20.0; break; // Fists
        case 1: baseDamage = 60.0; break; // Steel Sword
        case 2: baseDamage = 70.0; break; // Silver Sword
        case 3: baseDamage = 80.0; break; // Axe
        case 4: baseDamage = 75.0; break; // Mace
        case 5: baseDamage = 65.0; break; // Spear
        case 6: baseDamage = 90.0; break; // Crossbow
        case 7: baseDamage = 85.0; break; // Bow
        default: baseDamage = 50.0; break;
    }
    
    // Attack type multiplier
    switch (attackType)
    {
        case 0: attackMultiplier = 0.8; break;  // Light Attack
        case 1: attackMultiplier = 1.5; break;  // Heavy Attack
        case 2: attackMultiplier = 2.0; break;  // Strong Attack
        case 3: attackMultiplier = 1.2; break;  // Combo Attack
        case 4: attackMultiplier = 1.8; break;  // Counter Attack
        case 5: attackMultiplier = 2.5; break;  // Finisher Attack
        default: attackMultiplier = 1.0; break;
    }
    
    // Stat multiplier based on attacker's stats
    statMultiplier = 1.0 + (attacker.GetStatValue(BCS_AttackPower) * 0.01);
    
    // Calculate final damage
    var finalDamage: float = baseDamage * attackMultiplier * statMultiplier;
    
    // Add randomness (±10%)
    var randomFactor: float = 0.9 + (RandF() * 0.2);
    finalDamage = finalDamage * randomFactor;
    
    // Return result as JSON
    result = "{\"damage\":" + ToString(finalDamage) + "}";
    return result;
}

// Critical hit calculation helper
public func CalculateCriticalHitChance(attackerId: int32, victimId: int32) : string
{
    var attacker: CActor;
    var victim: CActor;
    var baseChance: float = 5.0;
    var luckBonus: float = 0.0;
    var levelBonus: float = 0.0;
    var result: string;
    
    // Get attacker actor
    if (attackerId == 0 || attackerId == thePlayer.GetEntityID())
    {
        attacker = thePlayer;
    }
    else
    {
        attacker = GetActorById(attackerId);
    }
    
    // Get victim actor
    if (victimId == 0 || victimId == thePlayer.GetEntityID())
    {
        victim = thePlayer;
    }
    else
    {
        victim = GetActorById(victimId);
    }
    
    if (!attacker) return "{\"criticalChance\":0.0}";
    
    // Calculate bonuses
    luckBonus = attacker.GetStatValue(BCS_Luck) * 0.1;
    levelBonus = attacker.GetStatValue(BCS_Level) * 0.05;
    
    // Calculate final critical hit chance
    var finalChance: float = baseChance + luckBonus + levelBonus;
    
    // Cap at 50% maximum
    if (finalChance > 50.0) finalChance = 50.0;
    
    // Return result as JSON
    result = "{\"criticalChance\":" + ToString(finalChance) + "}";
    return result;
}
