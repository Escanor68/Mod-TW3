#pragma once

#include "Common.h"
#include "game/GlobalEconomy.h"
#include "game/SyncedMonsterAI.h"
#include "game/SharedProgression.h"
#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <functional>

namespace Game
{
    // Exploration regions
    enum class ExplorationRegion
    {
        WhiteOrchard = 0,
        Velen = 1,
        Novigrad = 2,
        Skellige = 3,
        Toussaint = 4,
        KaerMorhen = 5,
        All = 6
    };

    // Fast travel point data
    struct FastTravelPoint
    {
        uint32_t pointId;
        std::string name;
        std::string location;
        ExplorationRegion region;
        Vector4F position;
        bool isUnlocked;
        bool isActive;
        std::chrono::high_resolution_clock::time_point unlockTime;
        
        FastTravelPoint() : pointId(0), region(ExplorationRegion::Velen), isUnlocked(false), isActive(true) {}
    };

    // Exploration session data
    struct ExplorationSessionData
    {
        uint32_t sessionId;
        std::string sessionName;
        uint32_t hostPlayerId;
        std::vector<uint32_t> participants;
        std::map<ExplorationRegion, bool> unlockedRegions;
        std::vector<FastTravelPoint> fastTravelPoints;
        std::map<uint32_t, Vector4F> playerPositions;
        std::map<uint32_t, ExplorationRegion> playerRegions;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point lastUpdate;
        bool isActive;
        
        ExplorationSessionData() : sessionId(0), hostPlayerId(0), isActive(false) {}
    };

    // Monster contract data
    struct MonsterContract
    {
        uint32_t contractId;
        std::string name;
        std::string description;
        std::string monsterType;
        Vector4F location;
        ExplorationRegion region;
        uint32_t level;
        uint32_t reward;
        std::vector<uint32_t> participants;
        bool isCompleted;
        bool isActive;
        std::chrono::high_resolution_clock::time_point createdTime;
        
        MonsterContract() : contractId(0), level(1), reward(0), isCompleted(false), isActive(true) {}
    };

    // Exploration activity data
    struct ExplorationActivity
    {
        uint32_t activityId;
        std::string name;
        std::string type;
        Vector4F location;
        ExplorationRegion region;
        std::vector<uint32_t> participants;
        bool isCompleted;
        bool isActive;
        std::chrono::high_resolution_clock::time_point startTime;
        
        ExplorationActivity() : activityId(0), isCompleted(false), isActive(true) {}
    };

    // Exploration statistics
    struct ExplorationStats
    {
        uint32_t totalSessions = 0;
        uint32_t activeSessions = 0;
        uint32_t totalContracts = 0;
        uint32_t completedContracts = 0;
        uint32_t totalActivities = 0;
        uint32_t completedActivities = 0;
        uint32_t totalDistanceTraveled = 0;
        float averageSessionTime = 0.0f;
        uint32_t mostExploredRegion = 0;
        uint32_t mostActivePlayer = 0;
        
        void Reset()
        {
            totalSessions = 0;
            activeSessions = 0;
            totalContracts = 0;
            completedContracts = 0;
            totalActivities = 0;
            completedActivities = 0;
            totalDistanceTraveled = 0;
            averageSessionTime = 0.0f;
            mostExploredRegion = 0;
            mostActivePlayer = 0;
        }
    };

    // Exploration mode system
    class ExplorationMode
    {
    public:
        ExplorationMode();
        ~ExplorationMode();

        // Initialize system
        bool Initialize();
        void Shutdown();

        // Session management
        uint32_t CreateExplorationSession(const std::string& sessionName, uint32_t hostPlayerId);
        bool JoinExplorationSession(uint32_t sessionId, uint32_t playerId);
        bool LeaveExplorationSession(uint32_t sessionId, uint32_t playerId);
        bool EndExplorationSession(uint32_t sessionId);
        ExplorationSessionData* GetExplorationSession(uint32_t sessionId);
        std::vector<ExplorationSessionData> GetActiveSessions() const;
        std::vector<ExplorationSessionData> GetPlayerSessions(uint32_t playerId) const;

        // Region management
        bool UnlockRegion(uint32_t sessionId, ExplorationRegion region);
        bool LockRegion(uint32_t sessionId, ExplorationRegion region);
        bool IsRegionUnlocked(uint32_t sessionId, ExplorationRegion region) const;
        std::vector<ExplorationRegion> GetUnlockedRegions(uint32_t sessionId) const;

        // Fast travel system
        bool UnlockFastTravelPoint(uint32_t sessionId, uint32_t pointId);
        bool LockFastTravelPoint(uint32_t sessionId, uint32_t pointId);
        bool FastTravelToPoint(uint32_t sessionId, uint32_t playerId, uint32_t pointId);
        std::vector<FastTravelPoint> GetAvailableFastTravelPoints(uint32_t sessionId) const;
        std::vector<FastTravelPoint> GetFastTravelPointsInRegion(uint32_t sessionId, ExplorationRegion region) const;

        // Monster contracts
        uint32_t CreateMonsterContract(const std::string& name, const std::string& description, 
                                     const std::string& monsterType, const Vector4F& location, 
                                     ExplorationRegion region, uint32_t level, uint32_t reward);
        bool JoinMonsterContract(uint32_t sessionId, uint32_t contractId, uint32_t playerId);
        bool LeaveMonsterContract(uint32_t sessionId, uint32_t contractId, uint32_t playerId);
        bool CompleteMonsterContract(uint32_t sessionId, uint32_t contractId);
        std::vector<MonsterContract> GetAvailableContracts(uint32_t sessionId) const;
        std::vector<MonsterContract> GetPlayerContracts(uint32_t sessionId, uint32_t playerId) const;

        // Exploration activities
        uint32_t CreateExplorationActivity(const std::string& name, const std::string& type, 
                                         const Vector4F& location, ExplorationRegion region);
        bool JoinExplorationActivity(uint32_t sessionId, uint32_t activityId, uint32_t playerId);
        bool LeaveExplorationActivity(uint32_t sessionId, uint32_t activityId, uint32_t playerId);
        bool CompleteExplorationActivity(uint32_t sessionId, uint32_t activityId);
        std::vector<ExplorationActivity> GetAvailableActivities(uint32_t sessionId) const;
        std::vector<ExplorationActivity> GetPlayerActivities(uint32_t sessionId, uint32_t playerId) const;

        // Player movement and positioning
        bool UpdatePlayerPosition(uint32_t sessionId, uint32_t playerId, const Vector4F& position);
        bool UpdatePlayerRegion(uint32_t sessionId, uint32_t playerId, ExplorationRegion region);
        Vector4F GetPlayerPosition(uint32_t sessionId, uint32_t playerId) const;
        ExplorationRegion GetPlayerRegion(uint32_t sessionId, uint32_t playerId) const;
        std::vector<uint32_t> GetPlayersInRegion(uint32_t sessionId, ExplorationRegion region) const;
        std::vector<uint32_t> GetNearbyPlayers(uint32_t sessionId, uint32_t playerId, float radius) const;

        // World state management
        bool SynchronizeWorldState(uint32_t sessionId, const std::string& stateId, const std::string& value);
        bool SynchronizeLocationState(uint32_t sessionId, const std::string& locationId, const std::string& state);
        std::map<std::string, std::string> GetWorldStates(uint32_t sessionId) const;
        std::map<std::string, std::string> GetLocationStates(uint32_t sessionId, const std::string& locationId) const;

        // DLC integration
        bool EnableDLCContent(uint32_t sessionId, const std::string& dlcName);
        bool DisableDLCContent(uint32_t sessionId, const std::string& dlcName);
        bool IsDLCContentEnabled(uint32_t sessionId, const std::string& dlcName) const;
        std::vector<std::string> GetEnabledDLCContent(uint32_t sessionId) const;

        // Economy integration
        bool EnableSharedEconomy(uint32_t sessionId, bool enable);
        bool IsSharedEconomyEnabled(uint32_t sessionId) const;
        bool TradeWithPlayer(uint32_t sessionId, uint32_t fromPlayerId, uint32_t toPlayerId, 
                           const std::map<uint32_t, uint32_t>& items);

        // Progression integration
        bool EnableSharedProgression(uint32_t sessionId, bool enable);
        bool IsSharedProgressionEnabled(uint32_t sessionId) const;
        bool ShareExperience(uint32_t sessionId, uint32_t amount);

        // Configuration
        void SetMaxSessionSize(uint32_t maxSize);
        void SetMaxContractParticipants(uint32_t maxParticipants);
        void SetMaxActivityParticipants(uint32_t maxParticipants);
        void SetFastTravelCooldown(float cooldown);
        void SetContractTimeout(float timeout);

        // Statistics
        ExplorationStats GetStats() const;
        void ResetStats();
        void PrintStats() const;

        // Callbacks
        using ExplorationSessionCreatedCallback = std::function<void(uint32_t, uint32_t)>;
        using PlayerJoinedCallback = std::function<void(uint32_t, uint32_t)>;
        using PlayerLeftCallback = std::function<void(uint32_t, uint32_t)>;
        using RegionUnlockedCallback = std::function<void(uint32_t, ExplorationRegion)>;
        using FastTravelPointUnlockedCallback = std::function<void(uint32_t, uint32_t)>;
        using MonsterContractCreatedCallback = std::function<void(uint32_t, uint32_t)>;
        using MonsterContractCompletedCallback = std::function<void(uint32_t, uint32_t)>;
        using ExplorationActivityCompletedCallback = std::function<void(uint32_t, uint32_t)>;
        
        void SetExplorationSessionCreatedCallback(ExplorationSessionCreatedCallback callback);
        void SetPlayerJoinedCallback(PlayerJoinedCallback callback);
        void SetPlayerLeftCallback(PlayerLeftCallback callback);
        void SetRegionUnlockedCallback(RegionUnlockedCallback callback);
        void SetFastTravelPointUnlockedCallback(FastTravelPointUnlockedCallback callback);
        void SetMonsterContractCreatedCallback(MonsterContractCreatedCallback callback);
        void SetMonsterContractCompletedCallback(MonsterContractCompletedCallback callback);
        void SetExplorationActivityCompletedCallback(ExplorationActivityCompletedCallback callback);

    private:
        // Internal methods
        void UpdateExplorationSession(uint32_t sessionId);
        void CleanupInactiveSessions();
        bool ValidateExplorationSession(uint32_t sessionId) const;
        bool ValidatePlayerInSession(uint32_t sessionId, uint32_t playerId) const;
        
        // Fast travel management
        void InitializeFastTravelPoints();
        bool IsFastTravelPointUnlocked(uint32_t sessionId, uint32_t pointId) const;
        void NotifyFastTravelUpdate(uint32_t sessionId, uint32_t playerId, uint32_t pointId);
        
        // Contract management
        void UpdateMonsterContracts();
        bool IsContractAvailable(uint32_t sessionId, uint32_t contractId) const;
        void NotifyContractUpdate(uint32_t sessionId, uint32_t contractId);
        
        // Activity management
        void UpdateExplorationActivities();
        bool IsActivityAvailable(uint32_t sessionId, uint32_t activityId) const;
        void NotifyActivityUpdate(uint32_t sessionId, uint32_t activityId);
        
        // World state management
        void SynchronizeWorldStateWithParticipants(uint32_t sessionId, const std::string& stateId, const std::string& value);
        void SynchronizeLocationStateWithParticipants(uint32_t sessionId, const std::string& locationId, const std::string& state);

        // Member variables
        bool m_initialized;
        std::map<uint32_t, ExplorationSessionData> m_explorationSessions;
        std::map<uint32_t, MonsterContract> m_monsterContracts;
        std::map<uint32_t, ExplorationActivity> m_explorationActivities;
        std::map<uint32_t, FastTravelPoint> m_fastTravelPoints;
        
        // Configuration
        uint32_t m_maxSessionSize;
        uint32_t m_maxContractParticipants;
        uint32_t m_maxActivityParticipants;
        float m_fastTravelCooldown;
        float m_contractTimeout;
        
        // Statistics
        ExplorationStats m_stats;
        
        // Callbacks
        ExplorationSessionCreatedCallback m_sessionCreatedCallback;
        PlayerJoinedCallback m_playerJoinedCallback;
        PlayerLeftCallback m_playerLeftCallback;
        RegionUnlockedCallback m_regionUnlockedCallback;
        FastTravelPointUnlockedCallback m_fastTravelPointUnlockedCallback;
        MonsterContractCreatedCallback m_contractCreatedCallback;
        MonsterContractCompletedCallback m_contractCompletedCallback;
        ExplorationActivityCompletedCallback m_activityCompletedCallback;
        
        // Timing
        std::chrono::high_resolution_clock::time_point m_lastUpdateTime;
        uint32_t m_nextSessionId;
        uint32_t m_nextContractId;
        uint32_t m_nextActivityId;
        uint32_t m_nextFastTravelPointId;
    };

    // Exploration utilities
    namespace ExplorationUtils
    {
        // Session creation helpers
        ExplorationSessionData CreateExplorationSession(const std::string& sessionName, uint32_t hostPlayerId);
        ExplorationSessionData CreateWhiteOrchardSession(const std::string& sessionName, uint32_t hostPlayerId);
        ExplorationSessionData CreateVelenSession(const std::string& sessionName, uint32_t hostPlayerId);
        ExplorationSessionData CreateNovigradSession(const std::string& sessionName, uint32_t hostPlayerId);
        ExplorationSessionData CreateSkelligeSession(const std::string& sessionName, uint32_t hostPlayerId);
        ExplorationSessionData CreateToussaintSession(const std::string& sessionName, uint32_t hostPlayerId);
        
        // Fast travel point creation
        FastTravelPoint CreateFastTravelPoint(const std::string& name, const std::string& location, 
                                            ExplorationRegion region, const Vector4F& position);
        std::vector<FastTravelPoint> CreateDefaultFastTravelPoints();
        
        // Monster contract creation
        MonsterContract CreateMonsterContract(const std::string& name, const std::string& description, 
                                            const std::string& monsterType, const Vector4F& location, 
                                            ExplorationRegion region, uint32_t level, uint32_t reward);
        std::vector<MonsterContract> CreateDefaultMonsterContracts();
        
        // Exploration activity creation
        ExplorationActivity CreateAlchemyActivity(const std::string& name, const Vector4F& location, ExplorationRegion region);
        ExplorationActivity CreateCraftingActivity(const std::string& name, const Vector4F& location, ExplorationRegion region);
        ExplorationActivity CreateGatheringActivity(const std::string& name, const Vector4F& location, ExplorationRegion region);
        std::vector<ExplorationActivity> CreateDefaultExplorationActivities();
        
        // Region analysis
        std::vector<std::string> GetRegionLocations(ExplorationRegion region);
        std::vector<std::string> GetRegionMonsters(ExplorationRegion region);
        std::vector<std::string> GetRegionActivities(ExplorationRegion region);
        
        // Distance calculation
        float CalculateDistance(const Vector4F& a, const Vector4F& b);
        bool IsInRange(const Vector4F& position, const Vector4F& target, float radius);
        
        // Validation
        bool ValidateExplorationSession(const ExplorationSessionData& session);
        bool ValidateMonsterContract(const MonsterContract& contract);
        bool ValidateExplorationActivity(const ExplorationActivity& activity);
        bool ValidateFastTravelPoint(const FastTravelPoint& point);
        
        // Statistics
        void PrintExplorationAnalysis(const ExplorationSessionData& session);
        void PrintMonsterContractAnalysis(const MonsterContract& contract);
        void PrintExplorationActivityAnalysis(const ExplorationActivity& activity);
    }
}

