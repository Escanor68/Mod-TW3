#pragma once

#include "Common.h"
#include "networking/MessageTypes.h"
#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <functional>

namespace Game
{
    // Quest states
    enum class QuestState
    {
        NotStarted = 0,
        Available = 1,
        InProgress = 2,
        Completed = 3,
        Failed = 4,
        Abandoned = 5
    };

    // Quest types
    enum class QuestType
    {
        Main = 0,
        Side = 1,
        Contract = 2,
        Treasure = 3,
        Exploration = 4,
        Combat = 5,
        Social = 6
    };

    // Quest objectives
    struct QuestObjective
    {
        uint32_t objectiveId;
        std::string description;
        std::string type;           // "kill", "collect", "talk", "reach", "use"
        std::string target;         // Target entity or location
        uint32_t requiredCount;
        uint32_t currentCount;
        bool isCompleted;
        bool isOptional;
        
        QuestObjective() : objectiveId(0), requiredCount(1), currentCount(0), 
                          isCompleted(false), isOptional(false) {}
    };

    // Quest data
    struct QuestData
    {
        uint32_t questId;
        std::string name;
        std::string description;
        QuestType type;
        QuestState state;
        uint32_t level;
        std::vector<QuestObjective> objectives;
        std::vector<uint32_t> requiredQuests;  // Prerequisites
        std::vector<uint32_t> rewards;         // Item IDs
        uint32_t experienceReward;
        uint32_t goldReward;
        std::string giverNpcId;
        std::string location;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point completionTime;
        bool isCooperative;
        uint32_t maxParticipants;
        std::vector<uint32_t> participants;
        uint32_t syncOwner;        // Player who controls quest progression
        
        QuestData() : questId(0), type(QuestType::Side), state(QuestState::NotStarted),
                     level(1), experienceReward(0), goldReward(0), isCooperative(false),
                     maxParticipants(1), syncOwner(0) {}
    };

    // Quest synchronization data
    struct QuestSyncData
    {
        uint32_t questId;
        QuestState state;
        std::vector<QuestObjective> objectives;
        std::vector<uint32_t> participants;
        uint32_t syncOwner;
        std::chrono::high_resolution_clock::time_point timestamp;
        
        QuestSyncData() : questId(0), state(QuestState::NotStarted), syncOwner(0) {}
    };

    // Quest group management
    struct QuestGroup
    {
        uint32_t groupId;
        std::string name;
        std::vector<uint32_t> members;
        uint32_t leader;
        std::vector<uint32_t> activeQuests;
        std::map<uint32_t, QuestState> questStates;
        std::chrono::high_resolution_clock::time_point createdTime;
        bool isActive;
        
        QuestGroup() : groupId(0), leader(0), isActive(true) {}
    };

    // Cooperative quest system
    class CooperativeQuestSystem
    {
    public:
        CooperativeQuestSystem();
        ~CooperativeQuestSystem();

        // Initialize system
        bool Initialize();
        void Shutdown();

        // Quest management
        void AddQuest(const QuestData& quest);
        void RemoveQuest(uint32_t questId);
        QuestData* GetQuest(uint32_t questId);
        std::vector<QuestData> GetAvailableQuests(uint32_t playerId) const;
        std::vector<QuestData> GetPlayerQuests(uint32_t playerId) const;
        std::vector<QuestData> GetGroupQuests(uint32_t groupId) const;

        // Quest progression
        bool StartQuest(uint32_t questId, uint32_t playerId);
        bool CompleteQuest(uint32_t questId, uint32_t playerId);
        bool AbandonQuest(uint32_t questId, uint32_t playerId);
        bool UpdateObjective(uint32_t questId, uint32_t objectiveId, uint32_t progress);
        
        // Cooperative features
        bool JoinQuest(uint32_t questId, uint32_t playerId);
        bool LeaveQuest(uint32_t questId, uint32_t playerId);
        bool TransferQuestOwnership(uint32_t questId, uint32_t fromPlayer, uint32_t toPlayer);
        
        // Group management
        uint32_t CreateGroup(const std::string& name, uint32_t leaderId);
        bool JoinGroup(uint32_t groupId, uint32_t playerId);
        bool LeaveGroup(uint32_t groupId, uint32_t playerId);
        bool DisbandGroup(uint32_t groupId);
        QuestGroup* GetGroup(uint32_t groupId);
        std::vector<QuestGroup> GetPlayerGroups(uint32_t playerId) const;

        // Synchronization
        void SynchronizeQuest(uint32_t questId, const QuestSyncData& syncData);
        QuestSyncData GetQuestSyncData(uint32_t questId) const;
        void BroadcastQuestUpdate(uint32_t questId);
        void ProcessQuestUpdate(const QuestSyncData& syncData);

        // Quest triggers (REDkit integration)
        void OnPlayerEnterArea(uint32_t playerId, const std::string& areaId);
        void OnPlayerKillMonster(uint32_t playerId, const std::string& monsterId);
        void OnPlayerCollectItem(uint32_t playerId, uint32_t itemId);
        void OnPlayerTalkToNpc(uint32_t playerId, const std::string& npcId);
        void OnPlayerUseObject(uint32_t playerId, const std::string& objectId);

        // Configuration
        void SetMaxGroupSize(uint32_t maxSize);
        void SetQuestSyncInterval(float interval);
        void EnableCooperativeMode(bool enable);
        void SetQuestDifficulty(float difficulty);

        // Statistics
        struct QuestStats
        {
            uint32_t totalQuests = 0;
            uint32_t completedQuests = 0;
            uint32_t failedQuests = 0;
            uint32_t activeGroups = 0;
            uint32_t totalParticipants = 0;
            float averageCompletionTime = 0.0f;
            float questSuccessRate = 0.0f;
            
            void Reset()
            {
                totalQuests = 0;
                completedQuests = 0;
                failedQuests = 0;
                activeGroups = 0;
                totalParticipants = 0;
                averageCompletionTime = 0.0f;
                questSuccessRate = 0.0f;
            }
        };
        
        QuestStats GetStats() const;
        void ResetStats();
        void PrintStats() const;

        // Callbacks
        using QuestStartedCallback = std::function<void(uint32_t, uint32_t)>;
        using QuestCompletedCallback = std::function<void(uint32_t, uint32_t)>;
        using QuestFailedCallback = std::function<void(uint32_t, uint32_t)>;
        using ObjectiveUpdatedCallback = std::function<void(uint32_t, uint32_t, uint32_t)>;
        using GroupCreatedCallback = std::function<void(uint32_t, uint32_t)>;
        
        void SetQuestStartedCallback(QuestStartedCallback callback);
        void SetQuestCompletedCallback(QuestCompletedCallback callback);
        void SetQuestFailedCallback(QuestFailedCallback callback);
        void SetObjectiveUpdatedCallback(ObjectiveUpdatedCallback callback);
        void SetGroupCreatedCallback(GroupCreatedCallback callback);

    private:
        // Internal methods
        bool ValidateQuestStart(uint32_t questId, uint32_t playerId) const;
        bool CheckQuestPrerequisites(uint32_t questId, uint32_t playerId) const;
        void UpdateQuestState(uint32_t questId, QuestState newState);
        void NotifyQuestUpdate(uint32_t questId, uint32_t playerId);
        void CleanupCompletedQuests();
        void ProcessQuestTriggers(uint32_t questId, const std::string& triggerType, uint32_t playerId);
        
        // Quest validation
        bool IsQuestAvailable(uint32_t questId, uint32_t playerId) const;
        bool CanPlayerJoinQuest(uint32_t questId, uint32_t playerId) const;
        bool IsQuestComplete(uint32_t questId) const;
        
        // Group validation
        bool IsPlayerInGroup(uint32_t playerId, uint32_t groupId) const;
        bool CanPlayerJoinGroup(uint32_t groupId, uint32_t playerId) const;
        
        // Member variables
        bool m_initialized;
        std::map<uint32_t, QuestData> m_quests;
        std::map<uint32_t, QuestGroup> m_groups;
        std::map<uint32_t, std::vector<uint32_t>> m_playerQuests;
        std::map<uint32_t, std::vector<uint32_t>> m_playerGroups;
        
        // Configuration
        uint32_t m_maxGroupSize;
        float m_questSyncInterval;
        bool m_cooperativeModeEnabled;
        float m_questDifficulty;
        
        // Statistics
        QuestStats m_stats;
        
        // Callbacks
        QuestStartedCallback m_questStartedCallback;
        QuestCompletedCallback m_questCompletedCallback;
        QuestFailedCallback m_questFailedCallback;
        ObjectiveUpdatedCallback m_objectiveUpdatedCallback;
        GroupCreatedCallback m_groupCreatedCallback;
        
        // Timing
        std::chrono::high_resolution_clock::time_point m_lastSyncTime;
        uint32_t m_nextQuestId;
        uint32_t m_nextGroupId;
    };

    // Quest database manager
    class QuestDatabaseManager
    {
    public:
        QuestDatabaseManager();
        ~QuestDatabaseManager();

        // Initialize database
        bool Initialize(const std::string& databasePath);
        void Shutdown();

        // Quest persistence
        bool SaveQuest(const QuestData& quest);
        bool LoadQuest(uint32_t questId, QuestData& quest);
        bool SaveAllQuests();
        bool LoadAllQuests();
        
        // Group persistence
        bool SaveGroup(const QuestGroup& group);
        bool LoadGroup(uint32_t groupId, QuestGroup& group);
        bool SaveAllGroups();
        bool LoadAllGroups();
        
        // Player progress
        bool SavePlayerProgress(uint32_t playerId, const std::vector<uint32_t>& questIds);
        bool LoadPlayerProgress(uint32_t playerId, std::vector<uint32_t>& questIds);
        
        // Database maintenance
        void CleanupOldData();
        void OptimizeDatabase();
        bool BackupDatabase(const std::string& backupPath);
        bool RestoreDatabase(const std::string& backupPath);

    private:
        std::string m_databasePath;
        bool m_initialized;
    };

    // Quest utilities
    namespace QuestUtils
    {
        // Quest validation
        bool ValidateQuestData(const QuestData& quest);
        bool ValidateObjective(const QuestObjective& objective);
        bool ValidateGroup(const QuestGroup& group);
        
        // Quest creation helpers
        QuestData CreateMainQuest(const std::string& name, const std::string& description, uint32_t level);
        QuestData CreateSideQuest(const std::string& name, const std::string& description, uint32_t level);
        QuestData CreateContractQuest(const std::string& name, const std::string& description, uint32_t level);
        
        // Objective creation
        QuestObjective CreateKillObjective(const std::string& description, const std::string& target, uint32_t count);
        QuestObjective CreateCollectObjective(const std::string& description, const std::string& target, uint32_t count);
        QuestObjective CreateTalkObjective(const std::string& description, const std::string& target);
        QuestObjective CreateReachObjective(const std::string& description, const std::string& target);
        
        // Quest analysis
        float CalculateQuestDifficulty(const QuestData& quest);
        uint32_t EstimateQuestDuration(const QuestData& quest);
        std::vector<std::string> GetQuestTags(const QuestData& quest);
        
        // Group management
        QuestGroup CreateGroup(const std::string& name, uint32_t leaderId);
        bool AddMemberToGroup(QuestGroup& group, uint32_t playerId);
        bool RemoveMemberFromGroup(QuestGroup& group, uint32_t playerId);
        
        // Statistics
        void PrintQuestAnalysis(const QuestData& quest);
        void PrintGroupAnalysis(const QuestGroup& group);
    }
}
