#pragma once

#include "Common.h"
#include "game/Entities/Npc/Npc.h"
#include <vector>
#include <map>
#include <queue>
#include <chrono>
#include <functional>

namespace Game
{
    // Monster AI states
    enum class MonsterAIState
    {
        Idle = 0,
        Patrolling = 1,
        Alert = 2,
        Aggressive = 3,
        Attacking = 4,
        Stunned = 5,
        Dead = 6,
        Fleeing = 7
    };

    // Monster types
    enum class MonsterType
    {
        Humanoid = 0,
        Beast = 1,
        Elemental = 2,
        Undead = 3,
        Dragon = 4,
        Insectoid = 5,
        Hybrid = 6
    };

    // Monster behavior patterns
    enum class BehaviorPattern
    {
        Passive = 0,
        Aggressive = 1,
        Territorial = 2,
        Pack = 3,
        Guardian = 4,
        Predator = 5
    };

    // Monster AI data
    struct MonsterAIData
    {
        uint32_t monsterId;
        std::string monsterName;
        MonsterType type;
        BehaviorPattern pattern;
        MonsterAIState currentState;
        MonsterAIState previousState;
        
        // Position and movement
        Vector4F position;
        Vector4F targetPosition;
        Vector4F velocity;
        float rotation;
        float targetRotation;
        
        // Combat data
        float health;
        float maxHealth;
        float stamina;
        float maxStamina;
        float attackPower;
        float defense;
        float speed;
        float attackRange;
        float detectionRange;
        float aggroRange;
        
        // AI parameters
        float aggressionLevel;
        float fearLevel;
        float intelligence;
        float memory;
        uint32_t targetPlayerId;
        std::vector<uint32_t> threatList;
        std::chrono::high_resolution_clock::time_point lastAttackTime;
        std::chrono::high_resolution_clock::time_point lastStateChange;
        
        // Synchronization
        uint32_t syncOwner;
        bool needsSync;
        std::chrono::high_resolution_clock::time_point lastSyncTime;
        
        MonsterAIData() : monsterId(0), type(MonsterType::Beast), pattern(BehaviorPattern::Passive),
                         currentState(MonsterAIState::Idle), previousState(MonsterAIState::Idle),
                         rotation(0.0f), targetRotation(0.0f), health(100.0f), maxHealth(100.0f),
                         stamina(100.0f), maxStamina(100.0f), attackPower(10.0f), defense(5.0f),
                         speed(1.0f), attackRange(2.0f), detectionRange(10.0f), aggroRange(5.0f),
                         aggressionLevel(0.5f), fearLevel(0.0f), intelligence(0.5f), memory(0.7f),
                         targetPlayerId(0), syncOwner(0), needsSync(false) {}
    };

    // AI decision
    struct AIDecision
    {
        MonsterAIState newState;
        Vector4F targetPosition;
        uint32_t targetPlayerId;
        std::string action;
        float priority;
        std::chrono::high_resolution_clock::time_point timestamp;
        
        AIDecision() : newState(MonsterAIState::Idle), targetPlayerId(0), priority(0.0f) {}
    };

    // Monster group
    struct MonsterGroup
    {
        uint32_t groupId;
        std::string name;
        std::vector<uint32_t> members;
        uint32_t leader;
        BehaviorPattern groupPattern;
        Vector4F groupCenter;
        float groupRadius;
        bool isActive;
        
        MonsterGroup() : groupId(0), leader(0), groupPattern(BehaviorPattern::Pack),
                        groupRadius(10.0f), isActive(true) {}
    };

    // Synchronized monster AI system
    class SyncedMonsterAI
    {
    public:
        SyncedMonsterAI();
        ~SyncedMonsterAI();

        // Initialize system
        bool Initialize();
        void Shutdown();

        // Monster management
        void AddMonster(const MonsterAIData& monsterData);
        void RemoveMonster(uint32_t monsterId);
        MonsterAIData* GetMonster(uint32_t monsterId);
        std::vector<MonsterAIData> GetAllMonsters() const;
        std::vector<MonsterAIData> GetMonstersInRange(const Vector4F& position, float range) const;

        // AI processing
        void UpdateAI(float deltaTime);
        void ProcessMonsterAI(uint32_t monsterId, float deltaTime);
        AIDecision MakeDecision(uint32_t monsterId, const std::vector<uint32_t>& nearbyPlayers);
        
        // State management
        void ChangeMonsterState(uint32_t monsterId, MonsterAIState newState);
        void SetMonsterTarget(uint32_t monsterId, uint32_t playerId);
        void ClearMonsterTarget(uint32_t monsterId);
        
        // Synchronization
        void SynchronizeMonster(uint32_t monsterId, const MonsterAIData& syncData);
        void BroadcastMonsterUpdate(uint32_t monsterId);
        void ProcessMonsterUpdate(const MonsterAIData& syncData);
        void SetSyncOwner(uint32_t monsterId, uint32_t playerId);
        
        // Group management
        uint32_t CreateMonsterGroup(const std::string& name, BehaviorPattern pattern);
        void AddMonsterToGroup(uint32_t groupId, uint32_t monsterId);
        void RemoveMonsterFromGroup(uint32_t groupId, uint32_t monsterId);
        MonsterGroup* GetMonsterGroup(uint32_t groupId);
        void UpdateGroupBehavior(uint32_t groupId);

        // Player interaction
        void OnPlayerEnterRange(uint32_t playerId, const Vector4F& position);
        void OnPlayerLeaveRange(uint32_t playerId, const Vector4F& position);
        void OnPlayerAttack(uint32_t playerId, uint32_t monsterId, float damage);
        void OnPlayerDeath(uint32_t playerId);

        // Configuration
        void SetAIDifficulty(float difficulty);
        void SetSyncInterval(float interval);
        void EnableGroupBehavior(bool enable);
        void SetMaxMonsters(uint32_t maxMonsters);

        // Statistics
        struct AIStats
        {
            uint32_t totalMonsters = 0;
            uint32_t activeMonsters = 0;
            uint32_t aggressiveMonsters = 0;
            uint32_t totalGroups = 0;
            uint32_t totalDecisions = 0;
            float averageDecisionTime = 0.0f;
            float averageSyncTime = 0.0f;
            uint32_t syncConflicts = 0;
            
            void Reset()
            {
                totalMonsters = 0;
                activeMonsters = 0;
                aggressiveMonsters = 0;
                totalGroups = 0;
                totalDecisions = 0;
                averageDecisionTime = 0.0f;
                averageSyncTime = 0.0f;
                syncConflicts = 0;
            }
        };
        
        AIStats GetStats() const;
        void ResetStats();
        void PrintStats() const;

        // Callbacks
        using MonsterStateChangedCallback = std::function<void(uint32_t, MonsterAIState, MonsterAIState)>;
        using MonsterAttackedCallback = std::function<void(uint32_t, uint32_t, float)>;
        using MonsterDiedCallback = std::function<void(uint32_t)>;
        using GroupFormedCallback = std::function<void(uint32_t, const std::vector<uint32_t>&)>;
        
        void SetMonsterStateChangedCallback(MonsterStateChangedCallback callback);
        void SetMonsterAttackedCallback(MonsterAttackedCallback callback);
        void SetMonsterDiedCallback(MonsterDiedCallback callback);
        void SetGroupFormedCallback(GroupFormedCallback callback);

    private:
        // Internal AI methods
        void ProcessIdleState(uint32_t monsterId, float deltaTime);
        void ProcessPatrollingState(uint32_t monsterId, float deltaTime);
        void ProcessAlertState(uint32_t monsterId, float deltaTime);
        void ProcessAggressiveState(uint32_t monsterId, float deltaTime);
        void ProcessAttackingState(uint32_t monsterId, float deltaTime);
        void ProcessStunnedState(uint32_t monsterId, float deltaTime);
        void ProcessFleeingState(uint32_t monsterId, float deltaTime);
        
        // Decision making
        AIDecision EvaluateIdleOptions(uint32_t monsterId, const std::vector<uint32_t>& nearbyPlayers);
        AIDecision EvaluateCombatOptions(uint32_t monsterId, const std::vector<uint32_t>& nearbyPlayers);
        AIDecision EvaluateEscapeOptions(uint32_t monsterId, const std::vector<uint32_t>& nearbyPlayers);
        
        // Group behavior
        void ProcessGroupBehavior(uint32_t groupId, float deltaTime);
        void CoordinateGroupAttack(uint32_t groupId, uint32_t targetPlayerId);
        void CoordinateGroupRetreat(uint32_t groupId);
        
        // Utility methods
        float CalculateDistance(const Vector4F& a, const Vector4F& b) const;
        Vector4F CalculateDirection(const Vector4F& from, const Vector4F& to) const;
        bool IsInRange(const Vector4F& position, const Vector4F& target, float range) const;
        uint32_t FindNearestPlayer(const Vector4F& position, const std::vector<uint32_t>& players) const;
        bool CanSeeTarget(uint32_t monsterId, uint32_t playerId) const;
        
        // Synchronization
        void ResolveSyncConflict(uint32_t monsterId, const MonsterAIData& localData, const MonsterAIData& remoteData);
        bool ShouldSyncMonster(uint32_t monsterId) const;
        void CleanupDeadMonsters();

        // Member variables
        bool m_initialized;
        std::map<uint32_t, MonsterAIData> m_monsters;
        std::map<uint32_t, MonsterGroup> m_groups;
        std::map<uint32_t, std::vector<uint32_t>> m_playerThreats;
        
        // Configuration
        float m_aiDifficulty;
        float m_syncInterval;
        bool m_groupBehaviorEnabled;
        uint32_t m_maxMonsters;
        
        // Statistics
        AIStats m_stats;
        
        // Callbacks
        MonsterStateChangedCallback m_stateChangedCallback;
        MonsterAttackedCallback m_attackedCallback;
        MonsterDiedCallback m_diedCallback;
        GroupFormedCallback m_groupFormedCallback;
        
        // Timing
        std::chrono::high_resolution_clock::time_point m_lastUpdateTime;
        std::chrono::high_resolution_clock::time_point m_lastSyncTime;
        uint32_t m_nextMonsterId;
        uint32_t m_nextGroupId;
    };

    // Monster AI utilities
    namespace MonsterAIUtils
    {
        // Monster creation
        MonsterAIData CreateMonster(const std::string& name, MonsterType type, const Vector4F& position);
        MonsterAIData CreateBeast(const std::string& name, const Vector4F& position, float health, float attack);
        MonsterAIData CreateHumanoid(const std::string& name, const Vector4F& position, float health, float attack);
        MonsterAIData CreateDragon(const std::string& name, const Vector4F& position, float health, float attack);
        
        // Behavior patterns
        void SetPassiveBehavior(MonsterAIData& monster);
        void SetAggressiveBehavior(MonsterAIData& monster);
        void SetTerritorialBehavior(MonsterAIData& monster);
        void SetPackBehavior(MonsterAIData& monster);
        
        // AI calculations
        float CalculateThreatLevel(const MonsterAIData& monster, uint32_t playerId);
        float CalculateEscapeChance(const MonsterAIData& monster, const std::vector<uint32_t>& threats);
        Vector4F CalculateOptimalPosition(const MonsterAIData& monster, const Vector4F& target);
        
        // Group management
        MonsterGroup CreatePack(const std::string& name, const std::vector<uint32_t>& members);
        MonsterGroup CreateTerritory(const std::string& name, const Vector4F& center, float radius);
        
        // Validation
        bool ValidateMonsterData(const MonsterAIData& monster);
        bool ValidateGroupData(const MonsterGroup& group);
        
        // Statistics
        void PrintMonsterAnalysis(const MonsterAIData& monster);
        void PrintGroupAnalysis(const MonsterGroup& group);
    }
}
