#include "game/SharedStoryMode.h"
#include "utils/Logger.h"
#include <algorithm>

namespace Game
{
    // SharedStoryMode implementation
    SharedStoryMode::SharedStoryMode()
        : m_initialized(false), m_maxSessionSize(4), m_decisionTimeout(30.0f),
          m_cutsceneTimeout(300.0f), m_autoSaveEnabled(true), m_autoSaveInterval(60.0f),
          m_nextSessionId(1), m_nextDecisionId(1), m_nextCutsceneId(1)
    {
        m_lastUpdateTime = std::chrono::high_resolution_clock::now();
        m_lastAutoSave = m_lastUpdateTime;
        
        LOG_INFO("Shared story mode system created");
    }

    SharedStoryMode::~SharedStoryMode()
    {
        Shutdown();
        LOG_INFO("Shared story mode system destroyed");
    }

    bool SharedStoryMode::Initialize()
    {
        if (m_initialized)
        {
            return true;
        }

        LOG_INFO("Initializing shared story mode system...");

        m_initialized = true;
        LOG_INFO("Shared story mode system initialized");
        return true;
    }

    void SharedStoryMode::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        LOG_INFO("Shutting down shared story mode system...");
        
        // Save all active sessions
        for (auto& pair : m_storySessions)
        {
            if (pair.second.isActive)
            {
                SaveStoryProgression(pair.first);
            }
        }
        
        // Clear all data
        m_storySessions.clear();
        m_storyProgressions.clear();
        
        m_initialized = false;
        LOG_INFO("Shared story mode system shutdown complete");
    }

    uint32_t SharedStoryMode::CreateStorySession(const std::string& sessionName, StoryModeType storyType, uint32_t hostPlayerId)
    {
        if (!m_initialized)
        {
            return 0;
        }

        StorySessionData session;
        session.sessionId = m_nextSessionId++;
        session.sessionName = sessionName;
        session.storyType = storyType;
        session.hostPlayerId = hostPlayerId;
        session.participants.push_back(hostPlayerId);
        session.playerRoles[hostPlayerId] = StoryRole::Host;
        session.startTime = std::chrono::high_resolution_clock::now();
        session.lastUpdate = session.startTime;
        session.isActive = true;

        m_storySessions[session.sessionId] = session;
        m_stats.totalSessions++;
        m_stats.activeSessions++;

        // Create story progression data
        StoryProgressionData progression;
        progression.sessionId = session.sessionId;
        progression.storyType = std::to_string(static_cast<int>(storyType));
        progression.lastSave = std::chrono::high_resolution_clock::now();
        m_storyProgressions[session.sessionId] = progression;

        if (m_sessionCreatedCallback)
        {
            m_sessionCreatedCallback(session.sessionId, hostPlayerId);
        }

        LOG_INFO("Created story session: " + sessionName + " (ID: " + std::to_string(session.sessionId) + 
                ", Type: " + std::to_string(static_cast<int>(storyType)) + ")");
        return session.sessionId;
    }

    bool SharedStoryMode::JoinStorySession(uint32_t sessionId, uint32_t playerId, StoryRole role)
    {
        StorySessionData* session = GetStorySession(sessionId);
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
        session->playerRoles[playerId] = role;
        session->lastUpdate = std::chrono::high_resolution_clock::now();

        if (m_playerJoinedCallback)
        {
            m_playerJoinedCallback(sessionId, playerId);
        }

        LOG_INFO("Player " + std::to_string(playerId) + " joined story session " + std::to_string(sessionId));
        return true;
    }

    bool SharedStoryMode::LeaveStorySession(uint32_t sessionId, uint32_t playerId)
    {
        StorySessionData* session = GetStorySession(sessionId);
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

        // Remove player role
        session->playerRoles.erase(playerId);
        session->lastUpdate = std::chrono::high_resolution_clock::now();

        // If host left, transfer host to another player
        if (session->hostPlayerId == playerId && !session->participants.empty())
        {
            session->hostPlayerId = session->participants[0];
            session->playerRoles[session->hostPlayerId] = StoryRole::Host;
        }

        // If no participants left, end session
        if (session->participants.empty())
        {
            EndStorySession(sessionId);
        }

        if (m_playerLeftCallback)
        {
            m_playerLeftCallback(sessionId, playerId);
        }

        LOG_INFO("Player " + std::to_string(playerId) + " left story session " + std::to_string(sessionId));
        return true;
    }

    bool SharedStoryMode::EndStorySession(uint32_t sessionId)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session)
        {
            return false;
        }

        // Save progression before ending
        if (m_autoSaveEnabled)
        {
            SaveStoryProgression(sessionId);
        }

        // Mark session as inactive
        session->isActive = false;
        m_stats.activeSessions--;

        LOG_INFO("Ended story session: " + std::to_string(sessionId));
        return true;
    }

    StorySessionData* SharedStoryMode::GetStorySession(uint32_t sessionId)
    {
        auto it = m_storySessions.find(sessionId);
        if (it != m_storySessions.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    std::vector<StorySessionData> SharedStoryMode::GetActiveSessions() const
    {
        std::vector<StorySessionData> activeSessions;
        for (const auto& pair : m_storySessions)
        {
            if (pair.second.isActive)
            {
                activeSessions.push_back(pair.second);
            }
        }
        return activeSessions;
    }

    std::vector<StorySessionData> SharedStoryMode::GetPlayerSessions(uint32_t playerId) const
    {
        std::vector<StorySessionData> playerSessions;
        for (const auto& pair : m_storySessions)
        {
            const StorySessionData& session = pair.second;
            if (session.isActive && std::find(session.participants.begin(), session.participants.end(), playerId) != session.participants.end())
            {
                playerSessions.push_back(session);
            }
        }
        return playerSessions;
    }

    bool SharedStoryMode::StartStoryQuest(uint32_t sessionId, const std::string& questId)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        session->currentQuestId = questId;
        session->lastUpdate = std::chrono::high_resolution_clock::now();

        // Integrate with quest system
        IntegrateWithQuestSystem(sessionId, questId);

        LOG_INFO("Started story quest: " + questId + " in session " + std::to_string(sessionId));
        return true;
    }

    bool SharedStoryMode::CompleteStoryQuest(uint32_t sessionId, const std::string& questId)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Add to completed quests
        auto& progression = m_storyProgressions[sessionId];
        if (std::find(progression.completedQuests.begin(), progression.completedQuests.end(), questId) == progression.completedQuests.end())
        {
            progression.completedQuests.push_back(questId);
        }

        // Remove from available quests
        auto it = std::find(progression.availableQuests.begin(), progression.availableQuests.end(), questId);
        if (it != progression.availableQuests.end())
        {
            progression.availableQuests.erase(it);
        }

        session->lastUpdate = std::chrono::high_resolution_clock::now();

        if (m_questCompletedCallback)
        {
            m_questCompletedCallback(sessionId, questId);
        }

        LOG_INFO("Completed story quest: " + questId + " in session " + std::to_string(sessionId));
        return true;
    }

    bool SharedStoryMode::AdvanceStoryProgression(uint32_t sessionId, const std::string& nextQuestId)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Add next quest to available quests
        auto& progression = m_storyProgressions[sessionId];
        if (std::find(progression.availableQuests.begin(), progression.availableQuests.end(), nextQuestId) == progression.availableQuests.end())
        {
            progression.availableQuests.push_back(nextQuestId);
        }

        session->lastUpdate = std::chrono::high_resolution_clock::now();

        LOG_INFO("Advanced story progression to quest: " + nextQuestId + " in session " + std::to_string(sessionId));
        return true;
    }

    bool SharedStoryMode::SetStoryLocation(uint32_t sessionId, const std::string& locationId)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        session->currentLocation = locationId;
        session->lastUpdate = std::chrono::high_resolution_clock::now();

        LOG_INFO("Set story location to: " + locationId + " in session " + std::to_string(sessionId));
        return true;
    }

    bool SharedStoryMode::PresentStoryDecision(uint32_t sessionId, const StoryDecision& decision)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        StoryDecision decisionCopy = decision;
        decisionCopy.decisionId = m_nextDecisionId++;
        decisionCopy.hostPlayerId = session->hostPlayerId;
        decisionCopy.timestamp = std::chrono::high_resolution_clock::now();

        session->decisions.push_back(decisionCopy);
        session->lastUpdate = std::chrono::high_resolution_clock::now();

        LOG_INFO("Presented story decision: " + std::to_string(decisionCopy.decisionId) + " in session " + std::to_string(sessionId));
        return true;
    }

    bool SharedStoryMode::MakeStoryDecision(uint32_t sessionId, uint32_t decisionId, uint32_t optionIndex)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Find decision
        for (auto& decision : session->decisions)
        {
            if (decision.decisionId == decisionId && !decision.isCompleted)
            {
                // Check if player can make decision
                if (!CanPlayerMakeDecision(sessionId, decision.hostPlayerId))
                {
                    return false;
                }

                decision.selectedOption = optionIndex;
                decision.isCompleted = true;
                session->lastUpdate = std::chrono::high_resolution_clock::now();

                // Add to story progression
                auto& progression = m_storyProgressions[sessionId];
                progression.storyDecisions.push_back(decision);

                if (m_decisionMadeCallback)
                {
                    m_decisionMadeCallback(sessionId, decisionId, optionIndex);
                }

                LOG_INFO("Made story decision: " + std::to_string(decisionId) + " option " + std::to_string(optionIndex) + 
                        " in session " + std::to_string(sessionId));
                return true;
            }
        }

        return false;
    }

    bool SharedStoryMode::SkipStoryDecision(uint32_t sessionId, uint32_t decisionId)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Find decision
        for (auto& decision : session->decisions)
        {
            if (decision.decisionId == decisionId && !decision.isCompleted)
            {
                decision.isCompleted = true;
                session->lastUpdate = std::chrono::high_resolution_clock::now();

                LOG_INFO("Skipped story decision: " + std::to_string(decisionId) + " in session " + std::to_string(sessionId));
                return true;
            }
        }

        return false;
    }

    std::vector<StoryDecision> SharedStoryMode::GetStoryDecisions(uint32_t sessionId) const
    {
        const StorySessionData* session = GetStorySession(sessionId);
        if (!session)
        {
            return std::vector<StoryDecision>();
        }
        return session->decisions;
    }

    bool SharedStoryMode::StartCutscene(uint32_t sessionId, const std::string& cutsceneName, const std::string& questId)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        CutsceneSyncData cutscene;
        cutscene.cutsceneId = m_nextCutsceneId++;
        cutscene.cutsceneName = cutsceneName;
        cutscene.questId = questId;
        cutscene.hostPlayerId = session->hostPlayerId;
        cutscene.participants = session->participants;
        cutscene.startTime = std::chrono::high_resolution_clock::now();
        cutscene.isPlaying = true;
        cutscene.canSkip = true;

        session->cutscenes.push_back(cutscene);
        session->lastUpdate = std::chrono::high_resolution_clock::now();

        if (m_cutsceneStartedCallback)
        {
            m_cutsceneStartedCallback(sessionId, cutscene.cutsceneId);
        }

        LOG_INFO("Started cutscene: " + cutsceneName + " in session " + std::to_string(sessionId));
        return true;
    }

    bool SharedStoryMode::EndCutscene(uint32_t sessionId, uint32_t cutsceneId)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Find cutscene
        for (auto& cutscene : session->cutscenes)
        {
            if (cutscene.cutsceneId == cutsceneId && cutscene.isPlaying)
            {
                cutscene.isPlaying = false;
                cutscene.endTime = std::chrono::high_resolution_clock::now();
                session->lastUpdate = std::chrono::high_resolution_clock::now();

                if (m_cutsceneEndedCallback)
                {
                    m_cutsceneEndedCallback(sessionId, cutsceneId);
                }

                LOG_INFO("Ended cutscene: " + std::to_string(cutsceneId) + " in session " + std::to_string(sessionId));
                return true;
            }
        }

        return false;
    }

    bool SharedStoryMode::SkipCutscene(uint32_t sessionId, uint32_t cutsceneId)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Find cutscene
        for (auto& cutscene : session->cutscenes)
        {
            if (cutscene.cutsceneId == cutsceneId && cutscene.isPlaying && cutscene.canSkip)
            {
                cutscene.isSkipped = true;
                cutscene.isPlaying = false;
                cutscene.endTime = std::chrono::high_resolution_clock::now();
                session->lastUpdate = std::chrono::high_resolution_clock::now();

                LOG_INFO("Skipped cutscene: " + std::to_string(cutsceneId) + " in session " + std::to_string(sessionId));
                return true;
            }
        }

        return false;
    }

    bool SharedStoryMode::PauseCutscene(uint32_t sessionId, uint32_t cutsceneId)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Find cutscene
        for (auto& cutscene : session->cutscenes)
        {
            if (cutscene.cutsceneId == cutsceneId && cutscene.isPlaying)
            {
                cutscene.isPlaying = false;
                session->lastUpdate = std::chrono::high_resolution_clock::now();

                LOG_INFO("Paused cutscene: " + std::to_string(cutsceneId) + " in session " + std::to_string(sessionId));
                return true;
            }
        }

        return false;
    }

    bool SharedStoryMode::ResumeCutscene(uint32_t sessionId, uint32_t cutsceneId)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Find cutscene
        for (auto& cutscene : session->cutscenes)
        {
            if (cutscene.cutsceneId == cutsceneId && !cutscene.isPlaying && !cutscene.isSkipped)
            {
                cutscene.isPlaying = true;
                session->lastUpdate = std::chrono::high_resolution_clock::now();

                LOG_INFO("Resumed cutscene: " + std::to_string(cutsceneId) + " in session " + std::to_string(sessionId));
                return true;
            }
        }

        return false;
    }

    CutsceneSyncData* SharedStoryMode::GetCutsceneData(uint32_t sessionId, uint32_t cutsceneId)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session)
        {
            return nullptr;
        }

        for (auto& cutscene : session->cutscenes)
        {
            if (cutscene.cutsceneId == cutsceneId)
            {
                return &cutscene;
            }
        }
        return nullptr;
    }

    bool SharedStoryMode::SynchronizeQuestState(uint32_t sessionId, const std::string& questId, const std::string& state)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Synchronize quest state with all participants
        for (uint32_t playerId : session->participants)
        {
            // This would send quest state update to each player
            LOG_DEBUG("Synchronizing quest state for player " + std::to_string(playerId) + 
                     ": " + questId + " -> " + state);
        }

        session->lastUpdate = std::chrono::high_resolution_clock::now();
        return true;
    }

    bool SharedStoryMode::SynchronizeQuestObjective(uint32_t sessionId, const std::string& questId, const std::string& objectiveId, uint32_t progress)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Synchronize quest objective with all participants
        for (uint32_t playerId : session->participants)
        {
            // This would send quest objective update to each player
            LOG_DEBUG("Synchronizing quest objective for player " + std::to_string(playerId) + 
                     ": " + questId + " objective " + objectiveId + " -> " + std::to_string(progress));
        }

        session->lastUpdate = std::chrono::high_resolution_clock::now();
        return true;
    }

    bool SharedStoryMode::SynchronizeQuestReward(uint32_t sessionId, const std::string& questId, const std::vector<uint32_t>& rewards)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Synchronize quest rewards with all participants
        for (uint32_t playerId : session->participants)
        {
            // This would send quest rewards to each player
            LOG_DEBUG("Synchronizing quest rewards for player " + std::to_string(playerId) + 
                     ": " + questId + " -> " + std::to_string(rewards.size()) + " rewards");
        }

        session->lastUpdate = std::chrono::high_resolution_clock::now();
        return true;
    }

    bool SharedStoryMode::SynchronizeWorldState(uint32_t sessionId, const std::string& stateId, const std::string& value)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Update world state
        auto& progression = m_storyProgressions[sessionId];
        progression.worldStates[stateId] = value;

        // Synchronize with all participants
        for (uint32_t playerId : session->participants)
        {
            // This would send world state update to each player
            LOG_DEBUG("Synchronizing world state for player " + std::to_string(playerId) + 
                     ": " + stateId + " -> " + value);
        }

        session->lastUpdate = std::chrono::high_resolution_clock::now();
        return true;
    }

    bool SharedStoryMode::SynchronizeLocationState(uint32_t sessionId, const std::string& locationId, const std::string& state)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Update location state
        auto& progression = m_storyProgressions[sessionId];
        progression.unlockedLocations[locationId].push_back(state);

        // Synchronize with all participants
        for (uint32_t playerId : session->participants)
        {
            // This would send location state update to each player
            LOG_DEBUG("Synchronizing location state for player " + std::to_string(playerId) + 
                     ": " + locationId + " -> " + state);
        }

        session->lastUpdate = std::chrono::high_resolution_clock::now();
        return true;
    }

    std::map<std::string, std::string> SharedStoryMode::GetWorldStates(uint32_t sessionId) const
    {
        auto it = m_storyProgressions.find(sessionId);
        if (it != m_storyProgressions.end())
        {
            return it->second.worldStates;
        }
        return std::map<std::string, std::string>();
    }

    bool SharedStoryMode::SetPlayerRole(uint32_t sessionId, uint32_t playerId, StoryRole role)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Check if player is in session
        if (std::find(session->participants.begin(), session->participants.end(), playerId) == session->participants.end())
        {
            return false;
        }

        session->playerRoles[playerId] = role;
        session->lastUpdate = std::chrono::high_resolution_clock::now();

        LOG_INFO("Set player " + std::to_string(playerId) + " role to " + std::to_string(static_cast<int>(role)) + 
                " in session " + std::to_string(sessionId));
        return true;
    }

    bool SharedStoryMode::TransferHost(uint32_t sessionId, uint32_t fromPlayerId, uint32_t toPlayerId)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Check if fromPlayerId is current host
        if (session->hostPlayerId != fromPlayerId)
        {
            return false;
        }

        // Check if toPlayerId is in session
        if (std::find(session->participants.begin(), session->participants.end(), toPlayerId) == session->participants.end())
        {
            return false;
        }

        // Transfer host
        session->hostPlayerId = toPlayerId;
        session->playerRoles[fromPlayerId] = StoryRole::Participant;
        session->playerRoles[toPlayerId] = StoryRole::Host;
        session->lastUpdate = std::chrono::high_resolution_clock::now();

        LOG_INFO("Transferred host from " + std::to_string(fromPlayerId) + " to " + std::to_string(toPlayerId) + 
                " in session " + std::to_string(sessionId));
        return true;
    }

    bool SharedStoryMode::KickPlayer(uint32_t sessionId, uint32_t playerId)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Check if player is in session
        if (std::find(session->participants.begin(), session->participants.end(), playerId) == session->participants.end())
        {
            return false;
        }

        // Remove player
        LeaveStorySession(sessionId, playerId);

        LOG_INFO("Kicked player " + std::to_string(playerId) + " from session " + std::to_string(sessionId));
        return true;
    }

    std::vector<uint32_t> SharedStoryMode::GetSessionParticipants(uint32_t sessionId) const
    {
        const StorySessionData* session = GetStorySession(sessionId);
        if (!session)
        {
            return std::vector<uint32_t>();
        }
        return session->participants;
    }

    bool SharedStoryMode::SaveStoryProgression(uint32_t sessionId)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // Update progression data
        auto& progression = m_storyProgressions[sessionId];
        progression.lastSave = std::chrono::high_resolution_clock::now();

        // This would save to file using SharedSaveSystem
        LOG_DEBUG("Saved story progression for session " + std::to_string(sessionId));
        return true;
    }

    bool SharedStoryMode::LoadStoryProgression(uint32_t sessionId)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return false;
        }

        // This would load from file using SharedSaveSystem
        LOG_DEBUG("Loaded story progression for session " + std::to_string(sessionId));
        return true;
    }

    bool SharedStoryMode::ExportStoryData(uint32_t sessionId, const std::string& filePath)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session)
        {
            return false;
        }

        // This would export story data to file
        LOG_DEBUG("Exported story data for session " + std::to_string(sessionId) + " to " + filePath);
        return true;
    }

    bool SharedStoryMode::ImportStoryData(uint32_t sessionId, const std::string& filePath)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session)
        {
            return false;
        }

        // This would import story data from file
        LOG_DEBUG("Imported story data for session " + std::to_string(sessionId) + " from " + filePath);
        return true;
    }

    // Configuration methods
    void SharedStoryMode::SetMaxSessionSize(uint32_t maxSize)
    {
        m_maxSessionSize = std::max(1u, std::min(8u, maxSize));
    }

    void SharedStoryMode::SetDecisionTimeout(float timeout)
    {
        m_decisionTimeout = std::max(5.0f, std::min(300.0f, timeout));
    }

    void SharedStoryMode::SetCutsceneTimeout(float timeout)
    {
        m_cutsceneTimeout = std::max(30.0f, std::min(1800.0f, timeout));
    }

    void SharedStoryMode::EnableAutoSave(bool enable)
    {
        m_autoSaveEnabled = enable;
    }

    void SharedStoryMode::SetAutoSaveInterval(float interval)
    {
        m_autoSaveInterval = std::max(10.0f, std::min(3600.0f, interval));
    }

    SharedStoryMode::StoryStats SharedStoryMode::GetStats() const
    {
        return m_stats;
    }

    void SharedStoryMode::ResetStats()
    {
        m_stats.Reset();
    }

    void SharedStoryMode::PrintStats() const
    {
        LOG_INFO("=== Shared Story Mode Statistics ===");
        LOG_INFO("Total sessions: " + std::to_string(m_stats.totalSessions));
        LOG_INFO("Active sessions: " + std::to_string(m_stats.activeSessions));
        LOG_INFO("Total decisions: " + std::to_string(m_stats.totalDecisions));
        LOG_INFO("Total cutscenes: " + std::to_string(m_stats.totalCutscenes));
        LOG_INFO("Completed stories: " + std::to_string(m_stats.completedStories));
        LOG_INFO("Average session time: " + std::to_string(m_stats.averageSessionTime) + "s");
        LOG_INFO("Average decisions per session: " + std::to_string(m_stats.averageDecisionsPerSession));
        LOG_INFO("Most popular story: " + std::to_string(m_stats.mostPopularStory));
        LOG_INFO("===================================");
    }

    // Callback setters
    void SharedStoryMode::SetStorySessionCreatedCallback(StorySessionCreatedCallback callback)
    {
        m_sessionCreatedCallback = callback;
    }

    void SharedStoryMode::SetPlayerJoinedCallback(PlayerJoinedCallback callback)
    {
        m_playerJoinedCallback = callback;
    }

    void SharedStoryMode::SetPlayerLeftCallback(PlayerLeftCallback callback)
    {
        m_playerLeftCallback = callback;
    }

    void SharedStoryMode::SetStoryDecisionMadeCallback(StoryDecisionMadeCallback callback)
    {
        m_decisionMadeCallback = callback;
    }

    void SharedStoryMode::SetCutsceneStartedCallback(CutsceneStartedCallback callback)
    {
        m_cutsceneStartedCallback = callback;
    }

    void SharedStoryMode::SetCutsceneEndedCallback(CutsceneEndedCallback callback)
    {
        m_cutsceneEndedCallback = callback;
    }

    void SharedStoryMode::SetQuestCompletedCallback(QuestCompletedCallback callback)
    {
        m_questCompletedCallback = callback;
    }

    // Private methods
    void SharedStoryMode::UpdateStorySession(uint32_t sessionId)
    {
        StorySessionData* session = GetStorySession(sessionId);
        if (!session || !session->isActive)
        {
            return;
        }

        // Update session state
        session->lastUpdate = std::chrono::high_resolution_clock::now();

        // Process pending decisions
        ProcessPendingDecisions();

        // Update cutscene states
        UpdateCutsceneStates();

        // Auto-save if enabled
        if (m_autoSaveEnabled)
        {
            auto now = std::chrono::high_resolution_clock::now();
            float timeSinceLastSave = std::chrono::duration<float>(now - m_lastAutoSave).count();
            if (timeSinceLastSave >= m_autoSaveInterval)
            {
                SaveStoryProgression(sessionId);
                m_lastAutoSave = now;
            }
        }
    }

    void SharedStoryMode::CleanupInactiveSessions()
    {
        auto it = m_storySessions.begin();
        while (it != m_storySessions.end())
        {
            if (!it->second.isActive)
            {
                it = m_storySessions.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    bool SharedStoryMode::ValidateStorySession(uint32_t sessionId) const
    {
        return m_storySessions.find(sessionId) != m_storySessions.end();
    }

    bool SharedStoryMode::ValidatePlayerInSession(uint32_t sessionId, uint32_t playerId) const
    {
        const StorySessionData* session = GetStorySession(sessionId);
        if (!session)
        {
            return false;
        }

        return std::find(session->participants.begin(), session->participants.end(), playerId) != session->participants.end();
    }

    bool SharedStoryMode::CanPlayerMakeDecision(uint32_t sessionId, uint32_t playerId) const
    {
        const StorySessionData* session = GetStorySession(sessionId);
        if (!session)
        {
            return false;
        }

        // Only host can make decisions
        return session->hostPlayerId == playerId;
    }

    bool SharedStoryMode::IntegrateWithQuestSystem(uint32_t sessionId, const std::string& questId)
    {
        // This would integrate with the CooperativeQuests system
        LOG_DEBUG("Integrating quest " + questId + " with story session " + std::to_string(sessionId));
        return true;
    }

    bool SharedStoryMode::SynchronizeQuestWithStory(uint32_t sessionId, const std::string& questId)
    {
        // This would synchronize quest state with story progression
        LOG_DEBUG("Synchronizing quest " + questId + " with story session " + std::to_string(sessionId));
        return true;
    }

    void SharedStoryMode::UpdateCutsceneStates()
    {
        // Update cutscene states for all active sessions
        for (auto& pair : m_storySessions)
        {
            if (pair.second.isActive)
            {
                for (auto& cutscene : pair.second.cutscenes)
                {
                    if (cutscene.isPlaying)
                    {
                        // Check if cutscene should timeout
                        auto now = std::chrono::high_resolution_clock::now();
                        float duration = std::chrono::duration<float>(now - cutscene.startTime).count();
                        if (duration >= m_cutsceneTimeout)
                        {
                            cutscene.isPlaying = false;
                            cutscene.endTime = now;
                        }
                    }
                }
            }
        }
    }

    bool SharedStoryMode::AllPlayersReadyForCutscene(uint32_t sessionId, uint32_t cutsceneId) const
    {
        const StorySessionData* session = GetStorySession(sessionId);
        if (!session)
        {
            return false;
        }

        // Check if all participants are ready
        // This would implement actual readiness checking
        return true;
    }

    void SharedStoryMode::NotifyCutsceneUpdate(uint32_t sessionId, uint32_t cutsceneId)
    {
        // Notify all participants of cutscene update
        const StorySessionData* session = GetStorySession(sessionId);
        if (session)
        {
            for (uint32_t playerId : session->participants)
            {
                // This would send cutscene update to each player
                LOG_DEBUG("Notifying player " + std::to_string(playerId) + " of cutscene update");
            }
        }
    }

    void SharedStoryMode::ProcessPendingDecisions()
    {
        // Process pending decisions for all active sessions
        for (auto& pair : m_storySessions)
        {
            if (pair.second.isActive)
            {
                for (auto& decision : pair.second.decisions)
                {
                    if (!decision.isCompleted && IsDecisionTimeout(pair.first, decision.decisionId))
                    {
                        // Auto-complete decision with default option
                        decision.selectedOption = 0;
                        decision.isCompleted = true;
                        
                        LOG_DEBUG("Auto-completed decision " + std::to_string(decision.decisionId) + 
                                 " due to timeout in session " + std::to_string(pair.first));
                    }
                }
            }
        }
    }

    bool SharedStoryMode::IsDecisionTimeout(uint32_t sessionId, uint32_t decisionId) const
    {
        const StorySessionData* session = GetStorySession(sessionId);
        if (!session)
        {
            return false;
        }

        // Find decision
        for (const auto& decision : session->decisions)
        {
            if (decision.decisionId == decisionId)
            {
                auto now = std::chrono::high_resolution_clock::now();
                float duration = std::chrono::duration<float>(now - decision.timestamp).count();
                return duration >= m_decisionTimeout;
            }
        }

        return false;
    }

    void SharedStoryMode::NotifyDecisionUpdate(uint32_t sessionId, uint32_t decisionId)
    {
        // Notify all participants of decision update
        const StorySessionData* session = GetStorySession(sessionId);
        if (session)
        {
            for (uint32_t playerId : session->participants)
            {
                // This would send decision update to each player
                LOG_DEBUG("Notifying player " + std::to_string(playerId) + " of decision update");
            }
        }
    }

    // Story utilities implementation
    namespace StoryUtils
    {
        StorySessionData CreateMainCampaignSession(const std::string& sessionName, uint32_t hostPlayerId)
        {
            StorySessionData session;
            session.sessionName = sessionName;
            session.storyType = StoryModeType::MainCampaign;
            session.hostPlayerId = hostPlayerId;
            session.participants.push_back(hostPlayerId);
            session.playerRoles[hostPlayerId] = StoryRole::Host;
            session.isActive = true;
            return session;
        }

        StorySessionData CreateHeartsOfStoneSession(const std::string& sessionName, uint32_t hostPlayerId)
        {
            StorySessionData session;
            session.sessionName = sessionName;
            session.storyType = StoryModeType::HeartsOfStone;
            session.hostPlayerId = hostPlayerId;
            session.participants.push_back(hostPlayerId);
            session.playerRoles[hostPlayerId] = StoryRole::Host;
            session.isActive = true;
            return session;
        }

        StorySessionData CreateBloodAndWineSession(const std::string& sessionName, uint32_t hostPlayerId)
        {
            StorySessionData session;
            session.sessionName = sessionName;
            session.storyType = StoryModeType::BloodAndWine;
            session.hostPlayerId = hostPlayerId;
            session.participants.push_back(hostPlayerId);
            session.playerRoles[hostPlayerId] = StoryRole::Host;
            session.isActive = true;
            return session;
        }

        StoryDecision CreateDialogueDecision(const std::string& questId, const std::string& dialogueId, 
                                           const std::string& text, const std::vector<std::string>& options)
        {
            StoryDecision decision;
            decision.questId = questId;
            decision.dialogueId = dialogueId;
            decision.decisionText = text;
            decision.options = options;
            return decision;
        }

        StoryDecision CreateQuestDecision(const std::string& questId, const std::string& text, 
                                        const std::vector<std::string>& options)
        {
            StoryDecision decision;
            decision.questId = questId;
            decision.decisionText = text;
            decision.options = options;
            return decision;
        }

        CutsceneSyncData CreateCutscene(const std::string& cutsceneName, const std::string& questId, 
                                       uint32_t hostPlayerId, const std::vector<uint32_t>& participants)
        {
            CutsceneSyncData cutscene;
            cutscene.cutsceneName = cutsceneName;
            cutscene.questId = questId;
            cutscene.hostPlayerId = hostPlayerId;
            cutscene.participants = participants;
            cutscene.isPlaying = true;
            cutscene.canSkip = true;
            return cutscene;
        }

        float CalculateStoryProgress(const StoryProgressionData& progression)
        {
            if (progression.availableQuests.empty() && progression.completedQuests.empty())
            {
                return 0.0f;
            }

            float totalQuests = progression.availableQuests.size() + progression.completedQuests.size();
            float completedQuests = progression.completedQuests.size();
            
            return completedQuests / totalQuests;
        }

        std::vector<std::string> GetAvailableQuests(const StoryProgressionData& progression)
        {
            return progression.availableQuests;
        }

        std::vector<std::string> GetCompletedQuests(const StoryProgressionData& progression)
        {
            return progression.completedQuests;
        }

        bool ValidateStorySession(const StorySessionData& session)
        {
            return !session.sessionName.empty() && session.hostPlayerId != 0 && !session.participants.empty();
        }

        bool ValidateStoryDecision(const StoryDecision& decision)
        {
            return !decision.questId.empty() && !decision.decisionText.empty() && !decision.options.empty();
        }

        bool ValidateCutsceneData(const CutsceneSyncData& cutscene)
        {
            return !cutscene.cutsceneName.empty() && !cutscene.questId.empty() && cutscene.hostPlayerId != 0;
        }

        void PrintStoryAnalysis(const StorySessionData& session)
        {
            LOG_INFO("=== Story Session Analysis ===");
            LOG_INFO("Session ID: " + std::to_string(session.sessionId));
            LOG_INFO("Name: " + session.sessionName);
            LOG_INFO("Type: " + std::to_string(static_cast<int>(session.storyType)));
            LOG_INFO("Host: " + std::to_string(session.hostPlayerId));
            LOG_INFO("Participants: " + std::to_string(session.participants.size()));
            LOG_INFO("Decisions: " + std::to_string(session.decisions.size()));
            LOG_INFO("Cutscenes: " + std::to_string(session.cutscenes.size()));
            LOG_INFO("Active: " + (session.isActive ? "Yes" : "No"));
            LOG_INFO("=============================");
        }

        void PrintStoryProgressionAnalysis(const StoryProgressionData& progression)
        {
            LOG_INFO("=== Story Progression Analysis ===");
            LOG_INFO("Session ID: " + std::to_string(progression.sessionId));
            LOG_INFO("Story Type: " + progression.storyType);
            LOG_INFO("Completed Quests: " + std::to_string(progression.completedQuests.size()));
            LOG_INFO("Available Quests: " + std::to_string(progression.availableQuests.size()));
            LOG_INFO("World States: " + std::to_string(progression.worldStates.size()));
            LOG_INFO("Decisions: " + std::to_string(progression.storyDecisions.size()));
            LOG_INFO("Progress: " + std::to_string(CalculateStoryProgress(progression) * 100.0f) + "%");
            LOG_INFO("=================================");
        }
    }
}

