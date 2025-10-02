#include "game/ExplorationMode.h"
#include "utils/Logger.h"
#include <algorithm>
#include <cmath>

namespace Game
{
    // ExplorationMode implementation
    ExplorationMode::ExplorationMode()
        : m_initialized(false), m_maxSessionSize(8), m_maxContractParticipants(4),
          m_maxActivityParticipants(6), m_fastTravelCooldown(5.0f), m_contractTimeout(3600.0f),
          m_nextSessionId(1), m_nextContractId(1), m_nextActivityId(1), m_nextFastTravelPointId(1)
    {
        m_lastUpdateTime = std::chrono::high_resolution_clock::now();
        
        LOG_INFO("Exploration mode system created");
    }

    ExplorationMode::~ExplorationMode()
    {
        Shutdown();
        LOG_INFO("Exploration mode system destroyed");
    }

    bool ExplorationMode::Initialize()
    {
        if (m_initialized)
        {
            return true;
        }

        LOG_INFO("Initializing exploration mode system...");

        // Initialize fast travel points
        InitializeFastTravelPoints();

        m_initialized = true;
        LOG_INFO("Exploration mode system initialized");
        return true;
    }

    void ExplorationMode::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        LOG_INFO("Shutting down exploration mode system...");
        
        // Clear all data
        m_explorationSessions.clear();
        m_monsterContracts.clear();
        m_explorationActivities.clear();
        m_fastTravelPoints.clear();
        
        m_initialized = false;
        LOG_INFO("Exploration mode system shutdown complete");
    }

    uint32_t ExplorationMode::CreateExplorationSession(const std::string& sessionName, uint32_t hostPlayerId)
    {
        if (!m_initialized)
        {
            return 0;
        }

        ExplorationSessionData session;
        session.sessionId = m_nextSessionId++;
        session.sessionName = sessionName;
        session.hostPlayerId = hostPlayerId;
        session.participants.push_back(hostPlayerId);
        session.startTime = std::chrono::high_resolution_clock::now();
        session.lastUpdate = session.startTime;
        session.isActive = true;

        // Unlock all regions by default in exploration mode
        for (int i = 0; i < static_cast<int>(ExplorationRegion::All); ++i)
        {
            session.unlockedRegions[static_cast<ExplorationRegion>(i)] = true;
        }

        m_explorationSessions[session.sessionId] = session;
        m_stats.totalSessions++;
        m_stats.activeSessions++;

        if (m_sessionCreatedCallback)
        {
            m_sessionCreatedCallback(session.sessionId, hostPlayerId);
        }

        LOG_INFO("Created exploration session: " + sessionName + " (ID: " + std::to_string(session.sessionId) + ")");
        return session.sessionId;
    }

    bool ExplorationMode::JoinExplorationSession(uint32_t sessionId, uint32_t playerId)
    {
        ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Check if session is full
        if (session->participants.size() >= m_maxSessionSize)
        {
            return false;
        }

        // Check if player is already in session
        if (std::find(session->participants.begin(), session->participants.end(), playerId) != session->participants.end())
        {
            return false;
        }

        // Add player to session
        session->participants.push_back(playerId);
        session->lastUpdate = std::chrono::high_resolution_clock::now();

        if (m_playerJoinedCallback)
        {
            m_playerJoinedCallback(sessionId, playerId);
        }

        LOG_INFO("Player " + std::to_string(playerId) + " joined exploration session " + std::to_string(sessionId));
        return true;
    }

    bool ExplorationMode::LeaveExplorationSession(uint32_t sessionId, uint32_t playerId)
    {
        ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session)
        {
            return false;
        }

        // Remove player from participants
        auto it = std::find(session->participants.begin(), session->participants.end(), playerId);
        if (it != session->participants.end())
        {
            session->participants.erase(it);
        }

        // Remove player position and region
        session->playerPositions.erase(playerId);
        session->playerRegions.erase(playerId);
        session->lastUpdate = std::chrono::high_resolution_clock::now();

        // If host left, transfer host to another player
        if (session->hostPlayerId == playerId && !session->participants.empty())
        {
            session->hostPlayerId = session->participants[0];
        }

        // If no participants left, end session
        if (session->participants.empty())
        {
            EndExplorationSession(sessionId);
        }

        if (m_playerLeftCallback)
        {
            m_playerLeftCallback(sessionId, playerId);
        }

        LOG_INFO("Player " + std::to_string(playerId) + " left exploration session " + std::to_string(sessionId));
        return true;
    }

    bool ExplorationMode::EndExplorationSession(uint32_t sessionId)
    {
        ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session)
        {
            return false;
        }

        // Mark session as inactive
        session->isActive = false;
        m_stats.activeSessions--;

        LOG_INFO("Ended exploration session: " + std::to_string(sessionId));
        return true;
    }

    ExplorationSessionData* ExplorationMode::GetExplorationSession(uint32_t sessionId)
    {
        auto it = m_explorationSessions.find(sessionId);
        if (it != m_explorationSessions.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    std::vector<ExplorationSessionData> ExplorationMode::GetActiveSessions() const
    {
        std::vector<ExplorationSessionData> activeSessions;
        for (const auto& pair : m_explorationSessions)
        {
            if (pair.second.isActive)
            {
                activeSessions.push_back(pair.second);
            }
        }
        return activeSessions;
    }

    std::vector<ExplorationSessionData> ExplorationMode::GetPlayerSessions(uint32_t playerId) const
    {
        std::vector<ExplorationSessionData> playerSessions;
        for (const auto& pair : m_explorationSessions)
        {
            const ExplorationSessionData& session = pair.second;
            if (session.isActive && std::find(session.participants.begin(), session.participants.end(), playerId) != session.participants.end())
            {
                playerSessions.push_back(session);
            }
        }
        return playerSessions;
    }

    bool ExplorationMode::UnlockRegion(uint32_t sessionId, ExplorationRegion region)
    {
        ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        session->unlockedRegions[region] = true;
        session->lastUpdate = std::chrono::high_resolution_clock::now();

        if (m_regionUnlockedCallback)
        {
            m_regionUnlockedCallback(sessionId, region);
        }

        LOG_INFO("Unlocked region " + std::to_string(static_cast<int>(region)) + " in session " + std::to_string(sessionId));
        return true;
    }

    bool ExplorationMode::LockRegion(uint32_t sessionId, ExplorationRegion region)
    {
        ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        session->unlockedRegions[region] = false;
        session->lastUpdate = std::chrono::high_resolution_clock::now();

        LOG_INFO("Locked region " + std::to_string(static_cast<int>(region)) + " in session " + std::to_string(sessionId));
        return true;
    }

    bool ExplorationMode::IsRegionUnlocked(uint32_t sessionId, ExplorationRegion region) const
    {
        const ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session)
        {
            return false;
        }

        auto it = session->unlockedRegions.find(region);
        return it != session->unlockedRegions.end() && it->second;
    }

    std::vector<ExplorationRegion> ExplorationMode::GetUnlockedRegions(uint32_t sessionId) const
    {
        std::vector<ExplorationRegion> unlockedRegions;
        const ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session)
        {
            return unlockedRegions;
        }

        for (const auto& pair : session->unlockedRegions)
        {
            if (pair.second)
            {
                unlockedRegions.push_back(pair.first);
            }
        }
        return unlockedRegions;
    }

    bool ExplorationMode::UnlockFastTravelPoint(uint32_t sessionId, uint32_t pointId)
    {
        ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Find fast travel point
        auto it = m_fastTravelPoints.find(pointId);
        if (it == m_fastTravelPoints.end())
        {
            return false;
        }

        // Check if already unlocked
        for (const auto& point : session->fastTravelPoints)
        {
            if (point.pointId == pointId && point.isUnlocked)
            {
                return true;
            }
        }

        // Unlock fast travel point
        FastTravelPoint point = it->second;
        point.isUnlocked = true;
        point.unlockTime = std::chrono::high_resolution_clock::now();
        session->fastTravelPoints.push_back(point);
        session->lastUpdate = std::chrono::high_resolution_clock::now();

        if (m_fastTravelPointUnlockedCallback)
        {
            m_fastTravelPointUnlockedCallback(sessionId, pointId);
        }

        LOG_INFO("Unlocked fast travel point " + std::to_string(pointId) + " in session " + std::to_string(sessionId));
        return true;
    }

    bool ExplorationMode::LockFastTravelPoint(uint32_t sessionId, uint32_t pointId)
    {
        ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Find and lock fast travel point
        for (auto& point : session->fastTravelPoints)
        {
            if (point.pointId == pointId)
            {
                point.isUnlocked = false;
                session->lastUpdate = std::chrono::high_resolution_clock::now();
                LOG_INFO("Locked fast travel point " + std::to_string(pointId) + " in session " + std::to_string(sessionId));
                return true;
            }
        }

        return false;
    }

    bool ExplorationMode::FastTravelToPoint(uint32_t sessionId, uint32_t playerId, uint32_t pointId)
    {
        ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Check if player is in session
        if (std::find(session->participants.begin(), session->participants.end(), playerId) == session->participants.end())
        {
            return false;
        }

        // Find fast travel point
        for (const auto& point : session->fastTravelPoints)
        {
            if (point.pointId == pointId && point.isUnlocked && point.isActive)
            {
                // Update player position
                session->playerPositions[playerId] = point.position;
                session->lastUpdate = std::chrono::high_resolution_clock::now();

                LOG_INFO("Player " + std::to_string(playerId) + " fast traveled to point " + std::to_string(pointId) + 
                        " in session " + std::to_string(sessionId));
                return true;
            }
        }

        return false;
    }

    std::vector<FastTravelPoint> ExplorationMode::GetAvailableFastTravelPoints(uint32_t sessionId) const
    {
        const ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session)
        {
            return std::vector<FastTravelPoint>();
        }

        std::vector<FastTravelPoint> availablePoints;
        for (const auto& point : session->fastTravelPoints)
        {
            if (point.isUnlocked && point.isActive)
            {
                availablePoints.push_back(point);
            }
        }
        return availablePoints;
    }

    std::vector<FastTravelPoint> ExplorationMode::GetFastTravelPointsInRegion(uint32_t sessionId, ExplorationRegion region) const
    {
        const ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session)
        {
            return std::vector<FastTravelPoint>();
        }

        std::vector<FastTravelPoint> regionPoints;
        for (const auto& point : session->fastTravelPoints)
        {
            if (point.region == region && point.isUnlocked && point.isActive)
            {
                regionPoints.push_back(point);
            }
        }
        return regionPoints;
    }

    uint32_t ExplorationMode::CreateMonsterContract(const std::string& name, const std::string& description, 
                                                   const std::string& monsterType, const Vector4F& location, 
                                                   ExplorationRegion region, uint32_t level, uint32_t reward)
    {
        MonsterContract contract;
        contract.contractId = m_nextContractId++;
        contract.name = name;
        contract.description = description;
        contract.monsterType = monsterType;
        contract.location = location;
        contract.region = region;
        contract.level = level;
        contract.reward = reward;
        contract.createdTime = std::chrono::high_resolution_clock::now();

        m_monsterContracts[contract.contractId] = contract;
        m_stats.totalContracts++;

        LOG_INFO("Created monster contract: " + name + " (ID: " + std::to_string(contract.contractId) + ")");
        return contract.contractId;
    }

    bool ExplorationMode::JoinMonsterContract(uint32_t sessionId, uint32_t contractId, uint32_t playerId)
    {
        ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Check if player is in session
        if (std::find(session->participants.begin(), session->participants.end(), playerId) == session->participants.end())
        {
            return false;
        }

        // Find contract
        auto it = m_monsterContracts.find(contractId);
        if (it == m_monsterContracts.end() || !it->second.isActive)
        {
            return false;
        }

        // Check if contract is full
        if (it->second.participants.size() >= m_maxContractParticipants)
        {
            return false;
        }

        // Check if player is already in contract
        if (std::find(it->second.participants.begin(), it->second.participants.end(), playerId) != it->second.participants.end())
        {
            return false;
        }

        // Add player to contract
        it->second.participants.push_back(playerId);
        session->lastUpdate = std::chrono::high_resolution_clock::now();

        LOG_INFO("Player " + std::to_string(playerId) + " joined monster contract " + std::to_string(contractId));
        return true;
    }

    bool ExplorationMode::LeaveMonsterContract(uint32_t sessionId, uint32_t contractId, uint32_t playerId)
    {
        ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Find contract
        auto it = m_monsterContracts.find(contractId);
        if (it == m_monsterContracts.end())
        {
            return false;
        }

        // Remove player from contract
        auto playerIt = std::find(it->second.participants.begin(), it->second.participants.end(), playerId);
        if (playerIt != it->second.participants.end())
        {
            it->second.participants.erase(playerIt);
            session->lastUpdate = std::chrono::high_resolution_clock::now();

            LOG_INFO("Player " + std::to_string(playerId) + " left monster contract " + std::to_string(contractId));
            return true;
        }

        return false;
    }

    bool ExplorationMode::CompleteMonsterContract(uint32_t sessionId, uint32_t contractId)
    {
        ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Find contract
        auto it = m_monsterContracts.find(contractId);
        if (it == m_monsterContracts.end() || !it->second.isActive)
        {
            return false;
        }

        // Mark contract as completed
        it->second.isCompleted = true;
        it->second.isActive = false;
        session->lastUpdate = std::chrono::high_resolution_clock::now();

        // Distribute rewards to participants
        for (uint32_t playerId : it->second.participants)
        {
            // This would distribute rewards to each player
            LOG_DEBUG("Distributed reward to player " + std::to_string(playerId) + " for contract " + std::to_string(contractId));
        }

        m_stats.completedContracts++;

        if (m_contractCompletedCallback)
        {
            m_contractCompletedCallback(sessionId, contractId);
        }

        LOG_INFO("Completed monster contract " + std::to_string(contractId) + " in session " + std::to_string(sessionId));
        return true;
    }

    std::vector<MonsterContract> ExplorationMode::GetAvailableContracts(uint32_t sessionId) const
    {
        std::vector<MonsterContract> availableContracts;
        for (const auto& pair : m_monsterContracts)
        {
            if (pair.second.isActive && !pair.second.isCompleted)
            {
                availableContracts.push_back(pair.second);
            }
        }
        return availableContracts;
    }

    std::vector<MonsterContract> ExplorationMode::GetPlayerContracts(uint32_t sessionId, uint32_t playerId) const
    {
        std::vector<MonsterContract> playerContracts;
        for (const auto& pair : m_monsterContracts)
        {
            if (std::find(pair.second.participants.begin(), pair.second.participants.end(), playerId) != pair.second.participants.end())
            {
                playerContracts.push_back(pair.second);
            }
        }
        return playerContracts;
    }

    // Placeholder implementations for other methods
    uint32_t ExplorationMode::CreateExplorationActivity(const std::string& name, const std::string& type, 
                                                       const Vector4F& location, ExplorationRegion region)
    {
        ExplorationActivity activity;
        activity.activityId = m_nextActivityId++;
        activity.name = name;
        activity.type = type;
        activity.location = location;
        activity.region = region;
        activity.startTime = std::chrono::high_resolution_clock::now();

        m_explorationActivities[activity.activityId] = activity;
        m_stats.totalActivities++;

        LOG_INFO("Created exploration activity: " + name + " (ID: " + std::to_string(activity.activityId) + ")");
        return activity.activityId;
    }

    bool ExplorationMode::JoinExplorationActivity(uint32_t sessionId, uint32_t activityId, uint32_t playerId) { return true; }
    bool ExplorationMode::LeaveExplorationActivity(uint32_t sessionId, uint32_t activityId, uint32_t playerId) { return true; }
    bool ExplorationMode::CompleteExplorationActivity(uint32_t sessionId, uint32_t activityId) { return true; }
    std::vector<ExplorationActivity> ExplorationMode::GetAvailableActivities(uint32_t sessionId) const { return std::vector<ExplorationActivity>(); }
    std::vector<ExplorationActivity> ExplorationMode::GetPlayerActivities(uint32_t sessionId, uint32_t playerId) const { return std::vector<ExplorationActivity>(); }

    bool ExplorationMode::UpdatePlayerPosition(uint32_t sessionId, uint32_t playerId, const Vector4F& position)
    {
        ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        session->playerPositions[playerId] = position;
        session->lastUpdate = std::chrono::high_resolution_clock::now();
        return true;
    }

    bool ExplorationMode::UpdatePlayerRegion(uint32_t sessionId, uint32_t playerId, ExplorationRegion region)
    {
        ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        session->playerRegions[playerId] = region;
        session->lastUpdate = std::chrono::high_resolution_clock::now();
        return true;
    }

    Vector4F ExplorationMode::GetPlayerPosition(uint32_t sessionId, uint32_t playerId) const
    {
        const ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session)
        {
            return Vector4F{0.0f, 0.0f, 0.0f, 1.0f};
        }

        auto it = session->playerPositions.find(playerId);
        if (it != session->playerPositions.end())
        {
            return it->second;
        }
        return Vector4F{0.0f, 0.0f, 0.0f, 1.0f};
    }

    ExplorationRegion ExplorationMode::GetPlayerRegion(uint32_t sessionId, uint32_t playerId) const
    {
        const ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session)
        {
            return ExplorationRegion::Velen;
        }

        auto it = session->playerRegions.find(playerId);
        if (it != session->playerRegions.end())
        {
            return it->second;
        }
        return ExplorationRegion::Velen;
    }

    std::vector<uint32_t> ExplorationMode::GetPlayersInRegion(uint32_t sessionId, ExplorationRegion region) const
    {
        std::vector<uint32_t> playersInRegion;
        const ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session)
        {
            return playersInRegion;
        }

        for (const auto& pair : session->playerRegions)
        {
            if (pair.second == region)
            {
                playersInRegion.push_back(pair.first);
            }
        }
        return playersInRegion;
    }

    std::vector<uint32_t> ExplorationMode::GetNearbyPlayers(uint32_t sessionId, uint32_t playerId, float radius) const
    {
        std::vector<uint32_t> nearbyPlayers;
        const ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session)
        {
            return nearbyPlayers;
        }

        Vector4F playerPos = GetPlayerPosition(sessionId, playerId);
        for (const auto& pair : session->playerPositions)
        {
            if (pair.first != playerId)
            {
                float distance = ExplorationUtils::CalculateDistance(playerPos, pair.second);
                if (distance <= radius)
                {
                    nearbyPlayers.push_back(pair.first);
                }
            }
        }
        return nearbyPlayers;
    }

    // Placeholder implementations for other methods
    bool ExplorationMode::SynchronizeWorldState(uint32_t sessionId, const std::string& stateId, const std::string& value) { return true; }
    bool ExplorationMode::SynchronizeLocationState(uint32_t sessionId, const std::string& locationId, const std::string& state) { return true; }
    std::map<std::string, std::string> ExplorationMode::GetWorldStates(uint32_t sessionId) const { return std::map<std::string, std::string>(); }
    std::map<std::string, std::string> ExplorationMode::GetLocationStates(uint32_t sessionId, const std::string& locationId) const { return std::map<std::string, std::string>(); }

    bool ExplorationMode::EnableDLCContent(uint32_t sessionId, const std::string& dlcName) { return true; }
    bool ExplorationMode::DisableDLCContent(uint32_t sessionId, const std::string& dlcName) { return true; }
    bool ExplorationMode::IsDLCContentEnabled(uint32_t sessionId, const std::string& dlcName) const { return true; }
    std::vector<std::string> ExplorationMode::GetEnabledDLCContent(uint32_t sessionId) const { return std::vector<std::string>(); }

    bool ExplorationMode::EnableSharedEconomy(uint32_t sessionId, bool enable) { return true; }
    bool ExplorationMode::IsSharedEconomyEnabled(uint32_t sessionId) const { return true; }
    bool ExplorationMode::TradeWithPlayer(uint32_t sessionId, uint32_t fromPlayerId, uint32_t toPlayerId, const std::map<uint32_t, uint32_t>& items) { return true; }

    bool ExplorationMode::EnableSharedProgression(uint32_t sessionId, bool enable) { return true; }
    bool ExplorationMode::IsSharedProgressionEnabled(uint32_t sessionId) const { return true; }
    bool ExplorationMode::ShareExperience(uint32_t sessionId, uint32_t amount) { return true; }

    // Configuration methods
    void ExplorationMode::SetMaxSessionSize(uint32_t maxSize)
    {
        m_maxSessionSize = std::max(1u, std::min(16u, maxSize));
    }

    void ExplorationMode::SetMaxContractParticipants(uint32_t maxParticipants)
    {
        m_maxContractParticipants = std::max(1u, std::min(8u, maxParticipants));
    }

    void ExplorationMode::SetMaxActivityParticipants(uint32_t maxParticipants)
    {
        m_maxActivityParticipants = std::max(1u, std::min(12u, maxParticipants));
    }

    void ExplorationMode::SetFastTravelCooldown(float cooldown)
    {
        m_fastTravelCooldown = std::max(0.0f, std::min(60.0f, cooldown));
    }

    void ExplorationMode::SetContractTimeout(float timeout)
    {
        m_contractTimeout = std::max(60.0f, std::min(7200.0f, timeout));
    }

    ExplorationMode::ExplorationStats ExplorationMode::GetStats() const
    {
        return m_stats;
    }

    void ExplorationMode::ResetStats()
    {
        m_stats.Reset();
    }

    void ExplorationMode::PrintStats() const
    {
        LOG_INFO("=== Exploration Mode Statistics ===");
        LOG_INFO("Total sessions: " + std::to_string(m_stats.totalSessions));
        LOG_INFO("Active sessions: " + std::to_string(m_stats.activeSessions));
        LOG_INFO("Total contracts: " + std::to_string(m_stats.totalContracts));
        LOG_INFO("Completed contracts: " + std::to_string(m_stats.completedContracts));
        LOG_INFO("Total activities: " + std::to_string(m_stats.totalActivities));
        LOG_INFO("Completed activities: " + std::to_string(m_stats.completedActivities));
        LOG_INFO("Total distance traveled: " + std::to_string(m_stats.totalDistanceTraveled));
        LOG_INFO("Average session time: " + std::to_string(m_stats.averageSessionTime) + "s");
        LOG_INFO("Most explored region: " + std::to_string(m_stats.mostExploredRegion));
        LOG_INFO("Most active player: " + std::to_string(m_stats.mostActivePlayer));
        LOG_INFO("===================================");
    }

    // Callback setters
    void ExplorationMode::SetExplorationSessionCreatedCallback(ExplorationSessionCreatedCallback callback)
    {
        m_sessionCreatedCallback = callback;
    }

    void ExplorationMode::SetPlayerJoinedCallback(PlayerJoinedCallback callback)
    {
        m_playerJoinedCallback = callback;
    }

    void ExplorationMode::SetPlayerLeftCallback(PlayerLeftCallback callback)
    {
        m_playerLeftCallback = callback;
    }

    void ExplorationMode::SetRegionUnlockedCallback(RegionUnlockedCallback callback)
    {
        m_regionUnlockedCallback = callback;
    }

    void ExplorationMode::SetFastTravelPointUnlockedCallback(FastTravelPointUnlockedCallback callback)
    {
        m_fastTravelPointUnlockedCallback = callback;
    }

    void ExplorationMode::SetMonsterContractCreatedCallback(MonsterContractCreatedCallback callback)
    {
        m_contractCreatedCallback = callback;
    }

    void ExplorationMode::SetMonsterContractCompletedCallback(MonsterContractCompletedCallback callback)
    {
        m_contractCompletedCallback = callback;
    }

    void ExplorationMode::SetExplorationActivityCompletedCallback(ExplorationActivityCompletedCallback callback)
    {
        m_activityCompletedCallback = callback;
    }

    // Private methods
    void ExplorationMode::InitializeFastTravelPoints()
    {
        // Create default fast travel points for each region
        // White Orchard
        FastTravelPoint whiteOrchardInn = ExplorationUtils::CreateFastTravelPoint("White Orchard Inn", "White Orchard", 
                                                                                 ExplorationRegion::WhiteOrchard, 
                                                                                 Vector4F{1000.0f, 0.0f, 1000.0f, 1.0f});
        m_fastTravelPoints[whiteOrchardInn.pointId] = whiteOrchardInn;

        // Velen
        FastTravelPoint crowPerch = ExplorationUtils::CreateFastTravelPoint("Crow's Perch", "Velen", 
                                                                           ExplorationRegion::Velen, 
                                                                           Vector4F{2000.0f, 0.0f, 2000.0f, 1.0f});
        m_fastTravelPoints[crowPerch.pointId] = crowPerch;

        // Novigrad
        FastTravelPoint novigradSquare = ExplorationUtils::CreateFastTravelPoint("Novigrad Square", "Novigrad", 
                                                                                ExplorationRegion::Novigrad, 
                                                                                Vector4F{3000.0f, 0.0f, 3000.0f, 1.0f});
        m_fastTravelPoints[novigradSquare.pointId] = novigradSquare;

        // Skellige
        FastTravelPoint kaerTrolde = ExplorationUtils::CreateFastTravelPoint("Kaer Trolde", "Skellige", 
                                                                             ExplorationRegion::Skellige, 
                                                                             Vector4F{4000.0f, 0.0f, 4000.0f, 1.0f});
        m_fastTravelPoints[kaerTrolde.pointId] = kaerTrolde;

        // Toussaint
        FastTravelPoint beauclair = ExplorationUtils::CreateFastTravelPoint("Beauclair", "Toussaint", 
                                                                           ExplorationRegion::Toussaint, 
                                                                           Vector4F{5000.0f, 0.0f, 5000.0f, 1.0f});
        m_fastTravelPoints[beauclair.pointId] = beauclair;

        LOG_INFO("Initialized " + std::to_string(m_fastTravelPoints.size()) + " fast travel points");
    }

    void ExplorationMode::UpdateExplorationSession(uint32_t sessionId)
    {
        ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session || !session->isActive)
        {
            return;
        }

        session->lastUpdate = std::chrono::high_resolution_clock::now();
    }

    void ExplorationMode::CleanupInactiveSessions()
    {
        auto it = m_explorationSessions.begin();
        while (it != m_explorationSessions.end())
        {
            if (!it->second.isActive)
            {
                it = m_explorationSessions.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    bool ExplorationMode::ValidateExplorationSession(uint32_t sessionId) const
    {
        return m_explorationSessions.find(sessionId) != m_explorationSessions.end();
    }

    bool ExplorationMode::ValidatePlayerInSession(uint32_t sessionId, uint32_t playerId) const
    {
        const ExplorationSessionData* session = GetExplorationSession(sessionId);
        if (!session)
        {
            return false;
        }

        return std::find(session->participants.begin(), session->participants.end(), playerId) != session->participants.end();
    }

    // Placeholder implementations for other private methods
    void ExplorationMode::UpdateMonsterContracts() {}
    bool ExplorationMode::IsContractAvailable(uint32_t sessionId, uint32_t contractId) const { return true; }
    void ExplorationMode::NotifyContractUpdate(uint32_t sessionId, uint32_t contractId) {}
    void ExplorationMode::UpdateExplorationActivities() {}
    bool ExplorationMode::IsActivityAvailable(uint32_t sessionId, uint32_t activityId) const { return true; }
    void ExplorationMode::NotifyActivityUpdate(uint32_t sessionId, uint32_t activityId) {}
    void ExplorationMode::SynchronizeWorldStateWithParticipants(uint32_t sessionId, const std::string& stateId, const std::string& value) {}
    void ExplorationMode::SynchronizeLocationStateWithParticipants(uint32_t sessionId, const std::string& locationId, const std::string& state) {}

    // Exploration utilities implementation
    namespace ExplorationUtils
    {
        ExplorationSessionData CreateExplorationSession(const std::string& sessionName, uint32_t hostPlayerId)
        {
            ExplorationSessionData session;
            session.sessionName = sessionName;
            session.hostPlayerId = hostPlayerId;
            session.participants.push_back(hostPlayerId);
            session.isActive = true;
            return session;
        }

        FastTravelPoint CreateFastTravelPoint(const std::string& name, const std::string& location, 
                                            ExplorationRegion region, const Vector4F& position)
        {
            FastTravelPoint point;
            point.name = name;
            point.location = location;
            point.region = region;
            point.position = position;
            point.isUnlocked = false;
            point.isActive = true;
            return point;
        }

        MonsterContract CreateMonsterContract(const std::string& name, const std::string& description, 
                                            const std::string& monsterType, const Vector4F& location, 
                                            ExplorationRegion region, uint32_t level, uint32_t reward)
        {
            MonsterContract contract;
            contract.name = name;
            contract.description = description;
            contract.monsterType = monsterType;
            contract.location = location;
            contract.region = region;
            contract.level = level;
            contract.reward = reward;
            contract.isActive = true;
            return contract;
        }

        ExplorationActivity CreateAlchemyActivity(const std::string& name, const Vector4F& location, ExplorationRegion region)
        {
            ExplorationActivity activity;
            activity.name = name;
            activity.type = "Alchemy";
            activity.location = location;
            activity.region = region;
            activity.isActive = true;
            return activity;
        }

        ExplorationActivity CreateCraftingActivity(const std::string& name, const Vector4F& location, ExplorationRegion region)
        {
            ExplorationActivity activity;
            activity.name = name;
            activity.type = "Crafting";
            activity.location = location;
            activity.region = region;
            activity.isActive = true;
            return activity;
        }

        ExplorationActivity CreateGatheringActivity(const std::string& name, const Vector4F& location, ExplorationRegion region)
        {
            ExplorationActivity activity;
            activity.name = name;
            activity.type = "Gathering";
            activity.location = location;
            activity.region = region;
            activity.isActive = true;
            return activity;
        }

        float CalculateDistance(const Vector4F& a, const Vector4F& b)
        {
            float dx = a.x - b.x;
            float dy = a.y - b.y;
            float dz = a.z - b.z;
            return std::sqrt(dx * dx + dy * dy + dz * dz);
        }

        bool IsInRange(const Vector4F& position, const Vector4F& target, float radius)
        {
            return CalculateDistance(position, target) <= radius;
        }

        bool ValidateExplorationSession(const ExplorationSessionData& session)
        {
            return !session.sessionName.empty() && session.hostPlayerId != 0 && !session.participants.empty();
        }

        bool ValidateMonsterContract(const MonsterContract& contract)
        {
            return !contract.name.empty() && !contract.description.empty() && !contract.monsterType.empty();
        }

        bool ValidateExplorationActivity(const ExplorationActivity& activity)
        {
            return !activity.name.empty() && !activity.type.empty();
        }

        bool ValidateFastTravelPoint(const FastTravelPoint& point)
        {
            return !point.name.empty() && !point.location.empty();
        }

        void PrintExplorationAnalysis(const ExplorationSessionData& session)
        {
            LOG_INFO("=== Exploration Session Analysis ===");
            LOG_INFO("Session ID: " + std::to_string(session.sessionId));
            LOG_INFO("Name: " + session.sessionName);
            LOG_INFO("Host: " + std::to_string(session.hostPlayerId));
            LOG_INFO("Participants: " + std::to_string(session.participants.size()));
            LOG_INFO("Unlocked Regions: " + std::to_string(session.unlockedRegions.size()));
            LOG_INFO("Fast Travel Points: " + std::to_string(session.fastTravelPoints.size()));
            LOG_INFO("Active: " + (session.isActive ? "Yes" : "No"));
            LOG_INFO("===================================");
        }

        void PrintMonsterContractAnalysis(const MonsterContract& contract)
        {
            LOG_INFO("=== Monster Contract Analysis ===");
            LOG_INFO("Contract ID: " + std::to_string(contract.contractId));
            LOG_INFO("Name: " + contract.name);
            LOG_INFO("Description: " + contract.description);
            LOG_INFO("Monster Type: " + contract.monsterType);
            LOG_INFO("Level: " + std::to_string(contract.level));
            LOG_INFO("Reward: " + std::to_string(contract.reward));
            LOG_INFO("Participants: " + std::to_string(contract.participants.size()));
            LOG_INFO("Completed: " + (contract.isCompleted ? "Yes" : "No"));
            LOG_INFO("================================");
        }

        void PrintExplorationActivityAnalysis(const ExplorationActivity& activity)
        {
            LOG_INFO("=== Exploration Activity Analysis ===");
            LOG_INFO("Activity ID: " + std::to_string(activity.activityId));
            LOG_INFO("Name: " + activity.name);
            LOG_INFO("Type: " + activity.type);
            LOG_INFO("Region: " + std::to_string(static_cast<int>(activity.region)));
            LOG_INFO("Participants: " + std::to_string(activity.participants.size()));
            LOG_INFO("Completed: " + (activity.isCompleted ? "Yes" : "No"));
            LOG_INFO("====================================");
        }
    }
}

