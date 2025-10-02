#pragma once

#include "Common.h"
#include "game/CooperativeQuests.h"
#include "game/SharedProgression.h"
#include "game/SharedSaveSystem.h"
#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <functional>

namespace Game
{
    // Story mode types
    enum class StoryModeType
    {
        MainCampaign = 0,
        HeartsOfStone = 1,
        BloodAndWine = 2,
        CustomStory = 3
    };

    // Player roles in story
    enum class StoryRole
    {
        Host = 0,           // Controls story decisions
        Participant = 1,    // Views and participates
        Spectator = 2       // Views only
    };

    // Story decision data
    struct StoryDecision
    {
        uint32_t decisionId;
        std::string questId;
        std::string dialogueId;
        std::string decisionText;
        std::vector<std::string> options;
        uint32_t selectedOption;
        uint32_t hostPlayerId;
        std::chrono::high_resolution_clock::time_point timestamp;
        bool isCompleted;
        
        StoryDecision() : decisionId(0), selectedOption(0), hostPlayerId(0), isCompleted(false) {}
    };

    // Cutscene synchronization data
    struct CutsceneSyncData
    {
        uint32_t cutsceneId;
        std::string cutsceneName;
        std::string questId;
        uint32_t hostPlayerId;
        std::vector<uint32_t> participants;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        bool isPlaying;
        bool canSkip;
        bool isSkipped;
        
        CutsceneSyncData() : cutsceneId(0), hostPlayerId(0), isPlaying(false), canSkip(false), isSkipped(false) {}
    };

    // Story session data
    struct StorySessionData
    {
        uint32_t sessionId;
        std::string sessionName;
        StoryModeType storyType;
        uint32_t hostPlayerId;
        std::vector<uint32_t> participants;
        std::map<uint32_t, StoryRole> playerRoles;
        std::string currentQuestId;
        std::string currentLocation;
        std::vector<StoryDecision> decisions;
        std::vector<CutsceneSyncData> cutscenes;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point lastUpdate;
        bool isActive;
        
        StorySessionData() : sessionId(0), storyType(StoryModeType::MainCampaign), 
                           hostPlayerId(0), isActive(false) {}
    };

    // Story progression data
    struct StoryProgressionData
    {
        uint32_t sessionId;
        std::string storyType;
        std::vector<std::string> completedQuests;
        std::vector<std::string> availableQuests;
        std::map<std::string, std::string> worldStates;
        std::map<std::string, std::vector<std::string>> unlockedLocations;
        std::vector<StoryDecision> storyDecisions;
        std::chrono::high_resolution_clock::time_point lastSave;
        
        StoryProgressionData() : sessionId(0) {}
    };

    // Story statistics
    struct StoryStats
    {
        uint32_t totalSessions = 0;
        uint32_t activeSessions = 0;
        uint32_t totalDecisions = 0;
        uint32_t totalCutscenes = 0;
        uint32_t completedStories = 0;
        float averageSessionTime = 0.0f;
        float averageDecisionsPerSession = 0.0f;
        uint32_t mostPopularStory = 0;
        
        void Reset()
        {
            totalSessions = 0;
            activeSessions = 0;
            totalDecisions = 0;
            totalCutscenes = 0;
            completedStories = 0;
            averageSessionTime = 0.0f;
            averageDecisionsPerSession = 0.0f;
            mostPopularStory = 0;
        }
    };

    // Shared story mode system
    class SharedStoryMode
    {
    public:
        SharedStoryMode();
        ~SharedStoryMode();

        // Initialize system
        bool Initialize();
        void Shutdown();

        // Session management
        uint32_t CreateStorySession(const std::string& sessionName, StoryModeType storyType, uint32_t hostPlayerId);
        bool JoinStorySession(uint32_t sessionId, uint32_t playerId, StoryRole role = StoryRole::Participant);
        bool LeaveStorySession(uint32_t sessionId, uint32_t playerId);
        bool EndStorySession(uint32_t sessionId);
        StorySessionData* GetStorySession(uint32_t sessionId);
        std::vector<StorySessionData> GetActiveSessions() const;
        std::vector<StorySessionData> GetPlayerSessions(uint32_t playerId) const;

        // Story progression
        bool StartStoryQuest(uint32_t sessionId, const std::string& questId);
        bool CompleteStoryQuest(uint32_t sessionId, const std::string& questId);
        bool AdvanceStoryProgression(uint32_t sessionId, const std::string& nextQuestId);
        bool SetStoryLocation(uint32_t sessionId, const std::string& locationId);

        // Decision making
        bool PresentStoryDecision(uint32_t sessionId, const StoryDecision& decision);
        bool MakeStoryDecision(uint32_t sessionId, uint32_t decisionId, uint32_t optionIndex);
        bool SkipStoryDecision(uint32_t sessionId, uint32_t decisionId);
        std::vector<StoryDecision> GetStoryDecisions(uint32_t sessionId) const;

        // Cutscene synchronization
        bool StartCutscene(uint32_t sessionId, const std::string& cutsceneName, const std::string& questId);
        bool EndCutscene(uint32_t sessionId, uint32_t cutsceneId);
        bool SkipCutscene(uint32_t sessionId, uint32_t cutsceneId);
        bool PauseCutscene(uint32_t sessionId, uint32_t cutsceneId);
        bool ResumeCutscene(uint32_t sessionId, uint32_t cutsceneId);
        CutsceneSyncData* GetCutsceneData(uint32_t sessionId, uint32_t cutsceneId);

        // Quest synchronization
        bool SynchronizeQuestState(uint32_t sessionId, const std::string& questId, const std::string& state);
        bool SynchronizeQuestObjective(uint32_t sessionId, const std::string& questId, const std::string& objectiveId, uint32_t progress);
        bool SynchronizeQuestReward(uint32_t sessionId, const std::string& questId, const std::vector<uint32_t>& rewards);

        // World state synchronization
        bool SynchronizeWorldState(uint32_t sessionId, const std::string& stateId, const std::string& value);
        bool SynchronizeLocationState(uint32_t sessionId, const std::string& locationId, const std::string& state);
        std::map<std::string, std::string> GetWorldStates(uint32_t sessionId) const;

        // Player management
        bool SetPlayerRole(uint32_t sessionId, uint32_t playerId, StoryRole role);
        bool TransferHost(uint32_t sessionId, uint32_t fromPlayerId, uint32_t toPlayerId);
        bool KickPlayer(uint32_t sessionId, uint32_t playerId);
        std::vector<uint32_t> GetSessionParticipants(uint32_t sessionId) const;

        // Story data management
        bool SaveStoryProgression(uint32_t sessionId);
        bool LoadStoryProgression(uint32_t sessionId);
        bool ExportStoryData(uint32_t sessionId, const std::string& filePath);
        bool ImportStoryData(uint32_t sessionId, const std::string& filePath);

        // Configuration
        void SetMaxSessionSize(uint32_t maxSize);
        void SetDecisionTimeout(float timeout);
        void SetCutsceneTimeout(float timeout);
        void EnableAutoSave(bool enable);
        void SetAutoSaveInterval(float interval);

        // Statistics
        StoryStats GetStats() const;
        void ResetStats();
        void PrintStats() const;

        // Callbacks
        using StorySessionCreatedCallback = std::function<void(uint32_t, uint32_t)>;
        using PlayerJoinedCallback = std::function<void(uint32_t, uint32_t)>;
        using PlayerLeftCallback = std::function<void(uint32_t, uint32_t)>;
        using StoryDecisionMadeCallback = std::function<void(uint32_t, uint32_t, uint32_t)>;
        using CutsceneStartedCallback = std::function<void(uint32_t, uint32_t)>;
        using CutsceneEndedCallback = std::function<void(uint32_t, uint32_t)>;
        using QuestCompletedCallback = std::function<void(uint32_t, const std::string&)>;
        
        void SetStorySessionCreatedCallback(StorySessionCreatedCallback callback);
        void SetPlayerJoinedCallback(PlayerJoinedCallback callback);
        void SetPlayerLeftCallback(PlayerLeftCallback callback);
        void SetStoryDecisionMadeCallback(StoryDecisionMadeCallback callback);
        void SetCutsceneStartedCallback(CutsceneStartedCallback callback);
        void SetCutsceneEndedCallback(CutsceneEndedCallback callback);
        void SetQuestCompletedCallback(QuestCompletedCallback callback);

    private:
        // Internal methods
        void UpdateStorySession(uint32_t sessionId);
        void CleanupInactiveSessions();
        bool ValidateStorySession(uint32_t sessionId) const;
        bool ValidatePlayerInSession(uint32_t sessionId, uint32_t playerId) const;
        bool CanPlayerMakeDecision(uint32_t sessionId, uint32_t playerId) const;
        
        // Quest integration
        bool IntegrateWithQuestSystem(uint32_t sessionId, const std::string& questId);
        bool SynchronizeQuestWithStory(uint32_t sessionId, const std::string& questId);
        
        // Cutscene management
        void UpdateCutsceneStates();
        bool AllPlayersReadyForCutscene(uint32_t sessionId, uint32_t cutsceneId) const;
        void NotifyCutsceneUpdate(uint32_t sessionId, uint32_t cutsceneId);
        
        // Decision management
        void ProcessPendingDecisions();
        bool IsDecisionTimeout(uint32_t sessionId, uint32_t decisionId) const;
        void NotifyDecisionUpdate(uint32_t sessionId, uint32_t decisionId);

        // Member variables
        bool m_initialized;
        std::map<uint32_t, StorySessionData> m_storySessions;
        std::map<uint32_t, StoryProgressionData> m_storyProgressions;
        
        // Configuration
        uint32_t m_maxSessionSize;
        float m_decisionTimeout;
        float m_cutsceneTimeout;
        bool m_autoSaveEnabled;
        float m_autoSaveInterval;
        
        // Statistics
        StoryStats m_stats;
        
        // Callbacks
        StorySessionCreatedCallback m_sessionCreatedCallback;
        PlayerJoinedCallback m_playerJoinedCallback;
        PlayerLeftCallback m_playerLeftCallback;
        StoryDecisionMadeCallback m_decisionMadeCallback;
        CutsceneStartedCallback m_cutsceneStartedCallback;
        CutsceneEndedCallback m_cutsceneEndedCallback;
        QuestCompletedCallback m_questCompletedCallback;
        
        // Timing
        std::chrono::high_resolution_clock::time_point m_lastUpdateTime;
        std::chrono::high_resolution_clock::time_point m_lastAutoSave;
        uint32_t m_nextSessionId;
        uint32_t m_nextDecisionId;
        uint32_t m_nextCutsceneId;
    };

    // Story utilities
    namespace StoryUtils
    {
        // Story creation helpers
        StorySessionData CreateMainCampaignSession(const std::string& sessionName, uint32_t hostPlayerId);
        StorySessionData CreateHeartsOfStoneSession(const std::string& sessionName, uint32_t hostPlayerId);
        StorySessionData CreateBloodAndWineSession(const std::string& sessionName, uint32_t hostPlayerId);
        
        // Decision creation
        StoryDecision CreateDialogueDecision(const std::string& questId, const std::string& dialogueId, 
                                           const std::string& text, const std::vector<std::string>& options);
        StoryDecision CreateQuestDecision(const std::string& questId, const std::string& text, 
                                        const std::vector<std::string>& options);
        
        // Cutscene creation
        CutsceneSyncData CreateCutscene(const std::string& cutsceneName, const std::string& questId, 
                                       uint32_t hostPlayerId, const std::vector<uint32_t>& participants);
        
        // Story analysis
        float CalculateStoryProgress(const StoryProgressionData& progression);
        std::vector<std::string> GetAvailableQuests(const StoryProgressionData& progression);
        std::vector<std::string> GetCompletedQuests(const StoryProgressionData& progression);
        
        // Validation
        bool ValidateStorySession(const StorySessionData& session);
        bool ValidateStoryDecision(const StoryDecision& decision);
        bool ValidateCutsceneData(const CutsceneSyncData& cutscene);
        
        // Statistics
        void PrintStoryAnalysis(const StorySessionData& session);
        void PrintStoryProgressionAnalysis(const StoryProgressionData& progression);
    }
}
