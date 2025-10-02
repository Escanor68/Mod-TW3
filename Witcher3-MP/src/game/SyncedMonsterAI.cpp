#include "game/SyncedMonsterAI.h"
#include "utils/Logger.h"
#include <algorithm>
#include <cmath>

namespace Game
{
    // SyncedMonsterAI implementation
    SyncedMonsterAI::SyncedMonsterAI()
        : m_initialized(false), m_aiDifficulty(1.0f), m_syncInterval(0.1f),
          m_groupBehaviorEnabled(true), m_maxMonsters(100), m_nextMonsterId(1), m_nextGroupId(1)
    {
        m_lastUpdateTime = std::chrono::high_resolution_clock::now();
        m_lastSyncTime = m_lastUpdateTime;
        
        LOG_INFO("Synced monster AI system created");
    }

    SyncedMonsterAI::~SyncedMonsterAI()
    {
        Shutdown();
        LOG_INFO("Synced monster AI system destroyed");
    }

    bool SyncedMonsterAI::Initialize()
    {
        if (m_initialized)
        {
            return true;
        }

        LOG_INFO("Initializing synced monster AI system...");

        m_initialized = true;
        LOG_INFO("Synced monster AI system initialized");
        return true;
    }

    void SyncedMonsterAI::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        LOG_INFO("Shutting down synced monster AI system...");
        
        // Clear all data
        m_monsters.clear();
        m_groups.clear();
        m_playerThreats.clear();
        
        m_initialized = false;
        LOG_INFO("Synced monster AI system shutdown complete");
    }

    void SyncedMonsterAI::AddMonster(const MonsterAIData& monsterData)
    {
        if (!m_initialized)
        {
            return;
        }

        MonsterAIData monster = monsterData;
        if (monster.monsterId == 0)
        {
            monster.monsterId = m_nextMonsterId++;
        }

        monster.lastSyncTime = std::chrono::high_resolution_clock::now();
        m_monsters[monster.monsterId] = monster;
        m_stats.totalMonsters++;
        m_stats.activeMonsters++;

        LOG_INFO("Added monster: " + monster.monsterName + " (ID: " + std::to_string(monster.monsterId) + ")");
    }

    void SyncedMonsterAI::RemoveMonster(uint32_t monsterId)
    {
        auto it = m_monsters.find(monsterId);
        if (it != m_monsters.end())
        {
            m_monsters.erase(it);
            m_stats.activeMonsters--;
            LOG_INFO("Removed monster ID: " + std::to_string(monsterId));
        }
    }

    MonsterAIData* SyncedMonsterAI::GetMonster(uint32_t monsterId)
    {
        auto it = m_monsters.find(monsterId);
        if (it != m_monsters.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    std::vector<MonsterAIData> SyncedMonsterAI::GetAllMonsters() const
    {
        std::vector<MonsterAIData> monsters;
        for (const auto& pair : m_monsters)
        {
            monsters.push_back(pair.second);
        }
        return monsters;
    }

    std::vector<MonsterAIData> SyncedMonsterAI::GetMonstersInRange(const Vector4F& position, float range) const
    {
        std::vector<MonsterAIData> monstersInRange;
        
        for (const auto& pair : m_monsters)
        {
            const MonsterAIData& monster = pair.second;
            if (CalculateDistance(monster.position, position) <= range)
            {
                monstersInRange.push_back(monster);
            }
        }
        
        return monstersInRange;
    }

    void SyncedMonsterAI::UpdateAI(float deltaTime)
    {
        if (!m_initialized)
        {
            return;
        }

        auto now = std::chrono::high_resolution_clock::now();
        
        // Update all monsters
        for (auto& pair : m_monsters)
        {
            ProcessMonsterAI(pair.first, deltaTime);
        }
        
        // Update groups
        if (m_groupBehaviorEnabled)
        {
            for (auto& pair : m_groups)
            {
                ProcessGroupBehavior(pair.first, deltaTime);
            }
        }
        
        // Cleanup dead monsters
        CleanupDeadMonsters();
        
        m_lastUpdateTime = now;
    }

    void SyncedMonsterAI::ProcessMonsterAI(uint32_t monsterId, float deltaTime)
    {
        MonsterAIData* monster = GetMonster(monsterId);
        if (!monster || monster->currentState == MonsterAIState::Dead)
        {
            return;
        }

        // Get nearby players
        std::vector<uint32_t> nearbyPlayers;
        for (const auto& pair : m_playerThreats)
        {
            if (IsInRange(monster->position, Vector4F{0.0f, 0.0f, 0.0f, 1.0f}, monster->detectionRange))
            {
                nearbyPlayers.push_back(pair.first);
            }
        }

        // Process current state
        switch (monster->currentState)
        {
            case MonsterAIState::Idle:
                ProcessIdleState(monsterId, deltaTime);
                break;
            case MonsterAIState::Patrolling:
                ProcessPatrollingState(monsterId, deltaTime);
                break;
            case MonsterAIState::Alert:
                ProcessAlertState(monsterId, deltaTime);
                break;
            case MonsterAIState::Aggressive:
                ProcessAggressiveState(monsterId, deltaTime);
                break;
            case MonsterAIState::Attacking:
                ProcessAttackingState(monsterId, deltaTime);
                break;
            case MonsterAIState::Stunned:
                ProcessStunnedState(monsterId, deltaTime);
                break;
            case MonsterAIState::Fleeing:
                ProcessFleeingState(monsterId, deltaTime);
                break;
        }

        // Make decision if needed
        if (monster->needsSync)
        {
            AIDecision decision = MakeDecision(monsterId, nearbyPlayers);
            if (decision.priority > 0.0f)
            {
                // Apply decision
                ChangeMonsterState(monsterId, decision.newState);
                if (decision.targetPlayerId != 0)
                {
                    SetMonsterTarget(monsterId, decision.targetPlayerId);
                }
                monster->targetPosition = decision.targetPosition;
            }
        }
    }

    AIDecision SyncedMonsterAI::MakeDecision(uint32_t monsterId, const std::vector<uint32_t>& nearbyPlayers)
    {
        MonsterAIData* monster = GetMonster(monsterId);
        if (!monster)
        {
            return AIDecision();
        }

        AIDecision bestDecision;
        bestDecision.priority = 0.0f;

        // Evaluate different options based on current state
        switch (monster->currentState)
        {
            case MonsterAIState::Idle:
            case MonsterAIState::Patrolling:
                bestDecision = EvaluateIdleOptions(monsterId, nearbyPlayers);
                break;
            case MonsterAIState::Alert:
            case MonsterAIState::Aggressive:
            case MonsterAIState::Attacking:
                bestDecision = EvaluateCombatOptions(monsterId, nearbyPlayers);
                break;
            case MonsterAIState::Fleeing:
                bestDecision = EvaluateEscapeOptions(monsterId, nearbyPlayers);
                break;
        }

        m_stats.totalDecisions++;
        return bestDecision;
    }

    void SyncedMonsterAI::ChangeMonsterState(uint32_t monsterId, MonsterAIState newState)
    {
        MonsterAIData* monster = GetMonster(monsterId);
        if (!monster || monster->currentState == newState)
        {
            return;
        }

        MonsterAIState oldState = monster->currentState;
        monster->previousState = oldState;
        monster->currentState = newState;
        monster->lastStateChange = std::chrono::high_resolution_clock::now();
        monster->needsSync = true;

        // Update statistics
        if (newState == MonsterAIState::Aggressive || newState == MonsterAIState::Attacking)
        {
            m_stats.aggressiveMonsters++;
        }

        // Call callback
        if (m_stateChangedCallback)
        {
            m_stateChangedCallback(monsterId, newState, oldState);
        }

        LOG_DEBUG("Monster " + std::to_string(monsterId) + " changed state from " + 
                 std::to_string(static_cast<int>(oldState)) + " to " + 
                 std::to_string(static_cast<int>(newState)));
    }

    void SyncedMonsterAI::SetMonsterTarget(uint32_t monsterId, uint32_t playerId)
    {
        MonsterAIData* monster = GetMonster(monsterId);
        if (!monster)
        {
            return;
        }

        monster->targetPlayerId = playerId;
        monster->needsSync = true;

        LOG_DEBUG("Monster " + std::to_string(monsterId) + " targeting player " + std::to_string(playerId));
    }

    void SyncedMonsterAI::ClearMonsterTarget(uint32_t monsterId)
    {
        MonsterAIData* monster = GetMonster(monsterId);
        if (!monster)
        {
            return;
        }

        monster->targetPlayerId = 0;
        monster->needsSync = true;

        LOG_DEBUG("Monster " + std::to_string(monsterId) + " cleared target");
    }

    void SyncedMonsterAI::SynchronizeMonster(uint32_t monsterId, const MonsterAIData& syncData)
    {
        MonsterAIData* monster = GetMonster(monsterId);
        if (!monster)
        {
            return;
        }

        // Check for conflicts
        if (monster->syncOwner != syncData.syncOwner && monster->needsSync)
        {
            ResolveSyncConflict(monsterId, *monster, syncData);
            m_stats.syncConflicts++;
        }
        else
        {
            // Apply sync data
            *monster = syncData;
            monster->lastSyncTime = std::chrono::high_resolution_clock::now();
        }

        LOG_DEBUG("Synchronized monster " + std::to_string(monsterId));
    }

    void SyncedMonsterAI::BroadcastMonsterUpdate(uint32_t monsterId)
    {
        MonsterAIData* monster = GetMonster(monsterId);
        if (!monster)
        {
            return;
        }

        // This would broadcast to all clients
        // In a real implementation, this would use the networking system
        LOG_DEBUG("Broadcasting monster update for " + std::to_string(monsterId));
    }

    void SyncedMonsterAI::ProcessMonsterUpdate(const MonsterAIData& syncData)
    {
        SynchronizeMonster(syncData.monsterId, syncData);
    }

    void SyncedMonsterAI::SetSyncOwner(uint32_t monsterId, uint32_t playerId)
    {
        MonsterAIData* monster = GetMonster(monsterId);
        if (!monster)
        {
            return;
        }

        monster->syncOwner = playerId;
        monster->needsSync = true;

        LOG_DEBUG("Set sync owner for monster " + std::to_string(monsterId) + " to player " + std::to_string(playerId));
    }

    uint32_t SyncedMonsterAI::CreateMonsterGroup(const std::string& name, BehaviorPattern pattern)
    {
        MonsterGroup group;
        group.groupId = m_nextGroupId++;
        group.name = name;
        group.groupPattern = pattern;
        group.isActive = true;

        m_groups[group.groupId] = group;
        m_stats.totalGroups++;

        LOG_INFO("Created monster group: " + name + " (ID: " + std::to_string(group.groupId) + ")");
        return group.groupId;
    }

    void SyncedMonsterAI::AddMonsterToGroup(uint32_t groupId, uint32_t monsterId)
    {
        MonsterGroup* group = GetMonsterGroup(groupId);
        MonsterAIData* monster = GetMonster(monsterId);
        
        if (!group || !monster)
        {
            return;
        }

        group->members.push_back(monsterId);
        if (group->leader == 0)
        {
            group->leader = monsterId;
        }

        LOG_DEBUG("Added monster " + std::to_string(monsterId) + " to group " + std::to_string(groupId));
    }

    void SyncedMonsterAI::RemoveMonsterFromGroup(uint32_t groupId, uint32_t monsterId)
    {
        MonsterGroup* group = GetMonsterGroup(groupId);
        if (!group)
        {
            return;
        }

        auto it = std::find(group->members.begin(), group->members.end(), monsterId);
        if (it != group->members.end())
        {
            group->members.erase(it);
        }

        // If leader was removed, assign new leader
        if (group->leader == monsterId && !group->members.empty())
        {
            group->leader = group->members[0];
        }

        LOG_DEBUG("Removed monster " + std::to_string(monsterId) + " from group " + std::to_string(groupId));
    }

    MonsterGroup* SyncedMonsterAI::GetMonsterGroup(uint32_t groupId)
    {
        auto it = m_groups.find(groupId);
        if (it != m_groups.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    void SyncedMonsterAI::UpdateGroupBehavior(uint32_t groupId)
    {
        MonsterGroup* group = GetMonsterGroup(groupId);
        if (!group || !group->isActive)
        {
            return;
        }

        // Update group center
        Vector4F center{0.0f, 0.0f, 0.0f, 1.0f};
        for (uint32_t monsterId : group->members)
        {
            MonsterAIData* monster = GetMonster(monsterId);
            if (monster)
            {
                center.x += monster->position.x;
                center.y += monster->position.y;
                center.z += monster->position.z;
            }
        }
        
        if (!group->members.empty())
        {
            center.x /= group->members.size();
            center.y /= group->members.size();
            center.z /= group->members.size();
        }
        
        group->groupCenter = center;
    }

    // Player interaction methods
    void SyncedMonsterAI::OnPlayerEnterRange(uint32_t playerId, const Vector4F& position)
    {
        // Find monsters that can detect the player
        for (auto& pair : m_monsters)
        {
            MonsterAIData& monster = pair.second;
            if (IsInRange(monster.position, position, monster.detectionRange))
            {
                // Add to threat list
                if (std::find(monster.threatList.begin(), monster.threatList.end(), playerId) == monster.threatList.end())
                {
                    monster.threatList.push_back(playerId);
                }

                // Change state to alert
                if (monster.currentState == MonsterAIState::Idle || monster.currentState == MonsterAIState::Patrolling)
                {
                    ChangeMonsterState(pair.first, MonsterAIState::Alert);
                }
            }
        }
    }

    void SyncedMonsterAI::OnPlayerLeaveRange(uint32_t playerId, const Vector4F& position)
    {
        // Remove from threat lists
        for (auto& pair : m_monsters)
        {
            MonsterAIData& monster = pair.second;
            auto it = std::find(monster.threatList.begin(), monster.threatList.end(), playerId);
            if (it != monster.threatList.end())
            {
                monster.threatList.erase(it);
            }

            // Clear target if it was this player
            if (monster.targetPlayerId == playerId)
            {
                ClearMonsterTarget(pair.first);
            }
        }
    }

    void SyncedMonsterAI::OnPlayerAttack(uint32_t playerId, uint32_t monsterId, float damage)
    {
        MonsterAIData* monster = GetMonster(monsterId);
        if (!monster)
        {
            return;
        }

        // Apply damage
        monster->health = std::max(0.0f, monster->health - damage);
        
        // Add to threat list
        if (std::find(monster->threatList.begin(), monster->threatList.end(), playerId) == monster->threatList.end())
        {
            monster->threatList.push_back(playerId);
        }

        // Set as target
        SetMonsterTarget(monsterId, playerId);

        // Change to aggressive state
        if (monster->currentState == MonsterAIState::Idle || monster->currentState == MonsterAIState::Patrolling)
        {
            ChangeMonsterState(monsterId, MonsterAIState::Aggressive);
        }

        // Check if monster died
        if (monster->health <= 0.0f)
        {
            ChangeMonsterState(monsterId, MonsterAIState::Dead);
            if (m_diedCallback)
            {
                m_diedCallback(monsterId);
            }
        }

        // Call attack callback
        if (m_attackedCallback)
        {
            m_attackedCallback(monsterId, playerId, damage);
        }

        LOG_DEBUG("Player " + std::to_string(playerId) + " attacked monster " + std::to_string(monsterId) + 
                 " for " + std::to_string(damage) + " damage");
    }

    void SyncedMonsterAI::OnPlayerDeath(uint32_t playerId)
    {
        // Clear player from all threat lists
        for (auto& pair : m_monsters)
        {
            MonsterAIData& monster = pair.second;
            auto it = std::find(monster.threatList.begin(), monster.threatList.end(), playerId);
            if (it != monster.threatList.end())
            {
                monster.threatList.erase(it);
            }

            // Clear target if it was this player
            if (monster.targetPlayerId == playerId)
            {
                ClearMonsterTarget(pair.first);
            }
        }

        LOG_DEBUG("Player " + std::to_string(playerId) + " died, clearing from monster threat lists");
    }

    // Configuration methods
    void SyncedMonsterAI::SetAIDifficulty(float difficulty)
    {
        m_aiDifficulty = std::max(0.1f, std::min(5.0f, difficulty));
    }

    void SyncedMonsterAI::SetSyncInterval(float interval)
    {
        m_syncInterval = std::max(0.01f, interval);
    }

    void SyncedMonsterAI::EnableGroupBehavior(bool enable)
    {
        m_groupBehaviorEnabled = enable;
    }

    void SyncedMonsterAI::SetMaxMonsters(uint32_t maxMonsters)
    {
        m_maxMonsters = maxMonsters;
    }

    SyncedMonsterAI::AIStats SyncedMonsterAI::GetStats() const
    {
        return m_stats;
    }

    void SyncedMonsterAI::ResetStats()
    {
        m_stats.Reset();
    }

    void SyncedMonsterAI::PrintStats() const
    {
        LOG_INFO("=== Synced Monster AI Statistics ===");
        LOG_INFO("Total monsters: " + std::to_string(m_stats.totalMonsters));
        LOG_INFO("Active monsters: " + std::to_string(m_stats.activeMonsters));
        LOG_INFO("Aggressive monsters: " + std::to_string(m_stats.aggressiveMonsters));
        LOG_INFO("Total groups: " + std::to_string(m_stats.totalGroups));
        LOG_INFO("Total decisions: " + std::to_string(m_stats.totalDecisions));
        LOG_INFO("Average decision time: " + std::to_string(m_stats.averageDecisionTime) + "ms");
        LOG_INFO("Average sync time: " + std::to_string(m_stats.averageSyncTime) + "ms");
        LOG_INFO("Sync conflicts: " + std::to_string(m_stats.syncConflicts));
        LOG_INFO("====================================");
    }

    // Callback setters
    void SyncedMonsterAI::SetMonsterStateChangedCallback(MonsterStateChangedCallback callback)
    {
        m_stateChangedCallback = callback;
    }

    void SyncedMonsterAI::SetMonsterAttackedCallback(MonsterAttackedCallback callback)
    {
        m_attackedCallback = callback;
    }

    void SyncedMonsterAI::SetMonsterDiedCallback(MonsterDiedCallback callback)
    {
        m_diedCallback = callback;
    }

    void SyncedMonsterAI::SetGroupFormedCallback(GroupFormedCallback callback)
    {
        m_groupFormedCallback = callback;
    }

    // Private methods implementation
    void SyncedMonsterAI::ProcessIdleState(uint32_t monsterId, float deltaTime)
    {
        MonsterAIData* monster = GetMonster(monsterId);
        if (!monster)
        {
            return;
        }

        // Simple idle behavior - just stand still
        monster->velocity = Vector4F{0.0f, 0.0f, 0.0f, 0.0f};
    }

    void SyncedMonsterAI::ProcessPatrollingState(uint32_t monsterId, float deltaTime)
    {
        MonsterAIData* monster = GetMonster(monsterId);
        if (!monster)
        {
            return;
        }

        // Simple patrolling - move towards target position
        Vector4F direction = CalculateDirection(monster->position, monster->targetPosition);
        monster->velocity.x = direction.x * monster->speed;
        monster->velocity.y = direction.y * monster->speed;
        monster->velocity.z = direction.z * monster->speed;
    }

    void SyncedMonsterAI::ProcessAlertState(uint32_t monsterId, float deltaTime)
    {
        MonsterAIData* monster = GetMonster(monsterId);
        if (!monster)
        {
            return;
        }

        // Look around for threats
        if (!monster->threatList.empty())
        {
            // Found a threat, become aggressive
            ChangeMonsterState(monsterId, MonsterAIState::Aggressive);
        }
    }

    void SyncedMonsterAI::ProcessAggressiveState(uint32_t monsterId, float deltaTime)
    {
        MonsterAIData* monster = GetMonster(monsterId);
        if (!monster)
        {
            return;
        }

        if (monster->targetPlayerId != 0)
        {
            // Move towards target
            Vector4F direction = CalculateDirection(monster->position, monster->targetPosition);
            monster->velocity.x = direction.x * monster->speed;
            monster->velocity.y = direction.y * monster->speed;
            monster->velocity.z = direction.z * monster->speed;

            // Check if in attack range
            if (IsInRange(monster->position, monster->targetPosition, monster->attackRange))
            {
                ChangeMonsterState(monsterId, MonsterAIState::Attacking);
            }
        }
    }

    void SyncedMonsterAI::ProcessAttackingState(uint32_t monsterId, float deltaTime)
    {
        MonsterAIData* monster = GetMonster(monsterId);
        if (!monster)
        {
            return;
        }

        // Attack the target
        auto now = std::chrono::high_resolution_clock::now();
        float timeSinceLastAttack = std::chrono::duration<float>(now - monster->lastAttackTime).count();
        
        if (timeSinceLastAttack >= 1.0f) // 1 second attack cooldown
        {
            // Perform attack
            monster->lastAttackTime = now;
            LOG_DEBUG("Monster " + std::to_string(monsterId) + " attacks player " + std::to_string(monster->targetPlayerId));
        }
    }

    void SyncedMonsterAI::ProcessStunnedState(uint32_t monsterId, float deltaTime)
    {
        MonsterAIData* monster = GetMonster(monsterId);
        if (!monster)
        {
            return;
        }

        // Stunned - can't move or attack
        monster->velocity = Vector4F{0.0f, 0.0f, 0.0f, 0.0f};
    }

    void SyncedMonsterAI::ProcessFleeingState(uint32_t monsterId, float deltaTime)
    {
        MonsterAIData* monster = GetMonster(monsterId);
        if (!monster)
        {
            return;
        }

        // Move away from threats
        if (!monster->threatList.empty())
        {
            // Simple flee behavior - move away from nearest threat
            Vector4F fleeDirection{0.0f, 0.0f, 0.0f, 0.0f};
            for (uint32_t threatId : monster->threatList)
            {
                // This would calculate direction away from threat
                // For now, just move in a random direction
                fleeDirection.x = 1.0f;
                fleeDirection.y = 0.0f;
                fleeDirection.z = 0.0f;
                break;
            }
            
            monster->velocity.x = fleeDirection.x * monster->speed;
            monster->velocity.y = fleeDirection.y * monster->speed;
            monster->velocity.z = fleeDirection.z * monster->speed;
        }
    }

    AIDecision SyncedMonsterAI::EvaluateIdleOptions(uint32_t monsterId, const std::vector<uint32_t>& nearbyPlayers)
    {
        AIDecision decision;
        
        if (!nearbyPlayers.empty())
        {
            // Found players nearby, become alert
            decision.newState = MonsterAIState::Alert;
            decision.priority = 0.8f;
        }
        
        return decision;
    }

    AIDecision SyncedMonsterAI::EvaluateCombatOptions(uint32_t monsterId, const std::vector<uint32_t>& nearbyPlayers)
    {
        AIDecision decision;
        
        if (!nearbyPlayers.empty())
        {
            // Attack nearest player
            decision.newState = MonsterAIState::Attacking;
            decision.targetPlayerId = nearbyPlayers[0];
            decision.priority = 1.0f;
        }
        
        return decision;
    }

    AIDecision SyncedMonsterAI::EvaluateEscapeOptions(uint32_t monsterId, const std::vector<uint32_t>& nearbyPlayers)
    {
        AIDecision decision;
        
        // Continue fleeing
        decision.newState = MonsterAIState::Fleeing;
        decision.priority = 0.9f;
        
        return decision;
    }

    void SyncedMonsterAI::ProcessGroupBehavior(uint32_t groupId, float deltaTime)
    {
        MonsterGroup* group = GetMonsterGroup(groupId);
        if (!group || !group->isActive)
        {
            return;
        }

        // Update group center
        UpdateGroupBehavior(groupId);

        // Coordinate group actions
        if (group->groupPattern == BehaviorPattern::Pack)
        {
            // Pack behavior - coordinate attacks
            for (uint32_t monsterId : group->members)
            {
                MonsterAIData* monster = GetMonster(monsterId);
                if (monster && monster->currentState == MonsterAIState::Aggressive)
                {
                    // Coordinate with other pack members
                    // This would implement pack coordination logic
                }
            }
        }
    }

    float SyncedMonsterAI::CalculateDistance(const Vector4F& a, const Vector4F& b) const
    {
        float dx = a.x - b.x;
        float dy = a.y - b.y;
        float dz = a.z - b.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    Vector4F SyncedMonsterAI::CalculateDirection(const Vector4F& from, const Vector4F& to) const
    {
        Vector4F direction;
        direction.x = to.x - from.x;
        direction.y = to.y - from.y;
        direction.z = to.z - from.z;
        direction.w = 0.0f;
        
        // Normalize
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
        if (length > 0.0f)
        {
            direction.x /= length;
            direction.y /= length;
            direction.z /= length;
        }
        
        return direction;
    }

    bool SyncedMonsterAI::IsInRange(const Vector4F& position, const Vector4F& target, float range) const
    {
        return CalculateDistance(position, target) <= range;
    }

    uint32_t SyncedMonsterAI::FindNearestPlayer(const Vector4F& position, const std::vector<uint32_t>& players) const
    {
        if (players.empty())
        {
            return 0;
        }

        uint32_t nearestPlayer = players[0];
        float nearestDistance = CalculateDistance(position, Vector4F{0.0f, 0.0f, 0.0f, 1.0f}); // Placeholder

        for (uint32_t playerId : players)
        {
            // This would calculate actual distance to player
            // For now, just return first player
        }

        return nearestPlayer;
    }

    bool SyncedMonsterAI::CanSeeTarget(uint32_t monsterId, uint32_t playerId) const
    {
        // This would implement line-of-sight checking
        // For now, just return true
        return true;
    }

    void SyncedMonsterAI::ResolveSyncConflict(uint32_t monsterId, const MonsterAIData& localData, const MonsterAIData& remoteData)
    {
        // Simple conflict resolution - use the data with the higher priority
        // In a real implementation, this would be more sophisticated
        LOG_DEBUG("Resolving sync conflict for monster " + std::to_string(monsterId));
    }

    bool SyncedMonsterAI::ShouldSyncMonster(uint32_t monsterId) const
    {
        MonsterAIData* monster = GetMonster(monsterId);
        if (!monster)
        {
            return false;
        }

        return monster->needsSync;
    }

    void SyncedMonsterAI::CleanupDeadMonsters()
    {
        auto it = m_monsters.begin();
        while (it != m_monsters.end())
        {
            if (it->second.currentState == MonsterAIState::Dead)
            {
                // Remove from groups
                for (auto& groupPair : m_groups)
                {
                    RemoveMonsterFromGroup(groupPair.first, it->first);
                }
                
                it = m_monsters.erase(it);
                m_stats.activeMonsters--;
            }
            else
            {
                ++it;
            }
        }
    }

    // Monster AI utilities implementation
    namespace MonsterAIUtils
    {
        MonsterAIData CreateMonster(const std::string& name, MonsterType type, const Vector4F& position)
        {
            MonsterAIData monster;
            monster.monsterName = name;
            monster.type = type;
            monster.position = position;
            monster.currentState = MonsterAIState::Idle;
            return monster;
        }

        MonsterAIData CreateBeast(const std::string& name, const Vector4F& position, float health, float attack)
        {
            MonsterAIData monster = CreateMonster(name, MonsterType::Beast, position);
            monster.maxHealth = health;
            monster.health = health;
            monster.attackPower = attack;
            monster.pattern = BehaviorPattern::Aggressive;
            return monster;
        }

        MonsterAIData CreateHumanoid(const std::string& name, const Vector4F& position, float health, float attack)
        {
            MonsterAIData monster = CreateMonster(name, MonsterType::Humanoid, position);
            monster.maxHealth = health;
            monster.health = health;
            monster.attackPower = attack;
            monster.pattern = BehaviorPattern::Territorial;
            return monster;
        }

        MonsterAIData CreateDragon(const std::string& name, const Vector4F& position, float health, float attack)
        {
            MonsterAIData monster = CreateMonster(name, MonsterType::Dragon, position);
            monster.maxHealth = health;
            monster.health = health;
            monster.attackPower = attack;
            monster.pattern = BehaviorPattern::Guardian;
            monster.detectionRange = 20.0f;
            monster.aggroRange = 15.0f;
            return monster;
        }

        void SetPassiveBehavior(MonsterAIData& monster)
        {
            monster.pattern = BehaviorPattern::Passive;
            monster.aggressionLevel = 0.1f;
            monster.detectionRange = 5.0f;
        }

        void SetAggressiveBehavior(MonsterAIData& monster)
        {
            monster.pattern = BehaviorPattern::Aggressive;
            monster.aggressionLevel = 0.9f;
            monster.detectionRange = 15.0f;
        }

        void SetTerritorialBehavior(MonsterAIData& monster)
        {
            monster.pattern = BehaviorPattern::Territorial;
            monster.aggressionLevel = 0.7f;
            monster.detectionRange = 10.0f;
        }

        void SetPackBehavior(MonsterAIData& monster)
        {
            monster.pattern = BehaviorPattern::Pack;
            monster.aggressionLevel = 0.8f;
            monster.detectionRange = 12.0f;
        }

        float CalculateThreatLevel(const MonsterAIData& monster, uint32_t playerId)
        {
            // Simple threat calculation
            return monster.aggressionLevel * monster.attackPower;
        }

        float CalculateEscapeChance(const MonsterAIData& monster, const std::vector<uint32_t>& threats)
        {
            // Simple escape chance calculation
            return monster.fearLevel * (1.0f - static_cast<float>(threats.size()) * 0.1f);
        }

        Vector4F CalculateOptimalPosition(const MonsterAIData& monster, const Vector4F& target)
        {
            // Simple optimal position calculation
            Vector4F direction;
            direction.x = target.x - monster.position.x;
            direction.y = target.y - monster.position.y;
            direction.z = target.z - monster.position.z;
            direction.w = 0.0f;
            
            // Normalize and scale by attack range
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
            if (length > 0.0f)
            {
                direction.x = (direction.x / length) * monster.attackRange;
                direction.y = (direction.y / length) * monster.attackRange;
                direction.z = (direction.z / length) * monster.attackRange;
            }
            
            Vector4F optimalPosition;
            optimalPosition.x = target.x - direction.x;
            optimalPosition.y = target.y - direction.y;
            optimalPosition.z = target.z - direction.z;
            optimalPosition.w = 1.0f;
            
            return optimalPosition;
        }

        MonsterGroup CreatePack(const std::string& name, const std::vector<uint32_t>& members)
        {
            MonsterGroup group;
            group.name = name;
            group.members = members;
            group.groupPattern = BehaviorPattern::Pack;
            group.isActive = true;
            
            if (!members.empty())
            {
                group.leader = members[0];
            }
            
            return group;
        }

        MonsterGroup CreateTerritory(const std::string& name, const Vector4F& center, float radius)
        {
            MonsterGroup group;
            group.name = name;
            group.groupCenter = center;
            group.groupRadius = radius;
            group.groupPattern = BehaviorPattern::Territorial;
            group.isActive = true;
            return group;
        }

        bool ValidateMonsterData(const MonsterAIData& monster)
        {
            return !monster.monsterName.empty() && monster.maxHealth > 0.0f && monster.attackPower >= 0.0f;
        }

        bool ValidateGroupData(const MonsterGroup& group)
        {
            return !group.name.empty() && group.groupRadius > 0.0f;
        }

        void PrintMonsterAnalysis(const MonsterAIData& monster)
        {
            LOG_INFO("=== Monster Analysis ===");
            LOG_INFO("Name: " + monster.monsterName);
            LOG_INFO("Type: " + std::to_string(static_cast<int>(monster.type)));
            LOG_INFO("State: " + std::to_string(static_cast<int>(monster.currentState)));
            LOG_INFO("Health: " + std::to_string(monster.health) + "/" + std::to_string(monster.maxHealth));
            LOG_INFO("Attack Power: " + std::to_string(monster.attackPower));
            LOG_INFO("Aggression: " + std::to_string(monster.aggressionLevel));
            LOG_INFO("======================");
        }

        void PrintGroupAnalysis(const MonsterGroup& group)
        {
            LOG_INFO("=== Group Analysis ===");
            LOG_INFO("Name: " + group.name);
            LOG_INFO("Members: " + std::to_string(group.members.size()));
            LOG_INFO("Leader: " + std::to_string(group.leader));
            LOG_INFO("Pattern: " + std::to_string(static_cast<int>(group.groupPattern)));
            LOG_INFO("Radius: " + std::to_string(group.groupRadius));
            LOG_INFO("====================");
        }
    }
}
