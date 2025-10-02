#include "game/CooperativeQuests.h"
#include "utils/Logger.h"
#include "networking/MessageTypes.h"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace Game
{
    // CooperativeQuestSystem implementation
    CooperativeQuestSystem::CooperativeQuestSystem()
        : m_initialized(false), m_maxGroupSize(4), m_questSyncInterval(1.0f),
          m_cooperativeModeEnabled(true), m_questDifficulty(1.0f), m_nextQuestId(1), m_nextGroupId(1)
    {
        m_lastSyncTime = std::chrono::high_resolution_clock::now();
        LOG_INFO("Cooperative quest system created");
    }

    CooperativeQuestSystem::~CooperativeQuestSystem()
    {
        Shutdown();
        LOG_INFO("Cooperative quest system destroyed");
    }

    bool CooperativeQuestSystem::Initialize()
    {
        if (m_initialized)
        {
            return true;
        }

        LOG_INFO("Initializing cooperative quest system...");

        // Initialize default quests
        InitializeDefaultQuests();

        m_initialized = true;
        LOG_INFO("Cooperative quest system initialized");
        return true;
    }

    void CooperativeQuestSystem::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        LOG_INFO("Shutting down cooperative quest system...");
        
        // Save all quest data
        SaveAllQuests();
        
        // Clear all data
        m_quests.clear();
        m_groups.clear();
        m_playerQuests.clear();
        m_playerGroups.clear();
        
        m_initialized = false;
        LOG_INFO("Cooperative quest system shutdown complete");
    }

    void CooperativeQuestSystem::AddQuest(const QuestData& quest)
    {
        if (!m_initialized)
        {
            return;
        }

        QuestData questCopy = quest;
        if (questCopy.questId == 0)
        {
            questCopy.questId = m_nextQuestId++;
        }

        m_quests[questCopy.questId] = questCopy;
        m_stats.totalQuests++;

        LOG_INFO("Added quest: " + questCopy.name + " (ID: " + std::to_string(questCopy.questId) + ")");
    }

    void CooperativeQuestSystem::RemoveQuest(uint32_t questId)
    {
        auto it = m_quests.find(questId);
        if (it != m_quests.end())
        {
            m_quests.erase(it);
            LOG_INFO("Removed quest ID: " + std::to_string(questId));
        }
    }

    QuestData* CooperativeQuestSystem::GetQuest(uint32_t questId)
    {
        auto it = m_quests.find(questId);
        if (it != m_quests.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    std::vector<QuestData> CooperativeQuestSystem::GetAvailableQuests(uint32_t playerId) const
    {
        std::vector<QuestData> availableQuests;
        
        for (const auto& pair : m_quests)
        {
            const QuestData& quest = pair.second;
            if (IsQuestAvailable(quest.questId, playerId))
            {
                availableQuests.push_back(quest);
            }
        }
        
        return availableQuests;
    }

    std::vector<QuestData> CooperativeQuestSystem::GetPlayerQuests(uint32_t playerId) const
    {
        std::vector<QuestData> playerQuests;
        
        auto it = m_playerQuests.find(playerId);
        if (it != m_playerQuests.end())
        {
            for (uint32_t questId : it->second)
            {
                auto questIt = m_quests.find(questId);
                if (questIt != m_quests.end())
                {
                    playerQuests.push_back(questIt->second);
                }
            }
        }
        
        return playerQuests;
    }

    std::vector<QuestData> CooperativeQuestSystem::GetGroupQuests(uint32_t groupId) const
    {
        std::vector<QuestData> groupQuests;
        
        auto it = m_groups.find(groupId);
        if (it != m_groups.end())
        {
            for (uint32_t questId : it->second.activeQuests)
            {
                auto questIt = m_quests.find(questId);
                if (questIt != m_quests.end())
                {
                    groupQuests.push_back(questIt->second);
                }
            }
        }
        
        return groupQuests;
    }

    bool CooperativeQuestSystem::StartQuest(uint32_t questId, uint32_t playerId)
    {
        if (!m_initialized)
        {
            return false;
        }

        QuestData* quest = GetQuest(questId);
        if (!quest || !ValidateQuestStart(questId, playerId))
        {
            return false;
        }

        // Update quest state
        quest->state = QuestState::InProgress;
        quest->startTime = std::chrono::high_resolution_clock::now();
        quest->participants.push_back(playerId);
        quest->syncOwner = playerId;

        // Add to player's quest list
        m_playerQuests[playerId].push_back(questId);

        // Notify quest start
        if (m_questStartedCallback)
        {
            m_questStartedCallback(questId, playerId);
        }

        LOG_INFO("Player " + std::to_string(playerId) + " started quest: " + quest->name);
        return true;
    }

    bool CooperativeQuestSystem::CompleteQuest(uint32_t questId, uint32_t playerId)
    {
        QuestData* quest = GetQuest(questId);
        if (!quest || quest->state != QuestState::InProgress)
        {
            return false;
        }

        // Check if all objectives are completed
        if (!IsQuestComplete(questId))
        {
            return false;
        }

        // Update quest state
        quest->state = QuestState::Completed;
        quest->completionTime = std::chrono::high_resolution_clock::now();

        // Calculate completion time
        float completionTime = std::chrono::duration<float>(quest->completionTime - quest->startTime).count();
        m_stats.averageCompletionTime = (m_stats.averageCompletionTime + completionTime) / 2.0f;

        // Update statistics
        m_stats.completedQuests++;
        m_stats.questSuccessRate = static_cast<float>(m_stats.completedQuests) / static_cast<float>(m_stats.totalQuests);

        // Notify quest completion
        if (m_questCompletedCallback)
        {
            m_questCompletedCallback(questId, playerId);
        }

        LOG_INFO("Player " + std::to_string(playerId) + " completed quest: " + quest->name);
        return true;
    }

    bool CooperativeQuestSystem::AbandonQuest(uint32_t questId, uint32_t playerId)
    {
        QuestData* quest = GetQuest(questId);
        if (!quest || quest->state != QuestState::InProgress)
        {
            return false;
        }

        // Update quest state
        quest->state = QuestState::Abandoned;

        // Remove from player's quest list
        auto it = std::find(m_playerQuests[playerId].begin(), m_playerQuests[playerId].end(), questId);
        if (it != m_playerQuests[playerId].end())
        {
            m_playerQuests[playerId].erase(it);
        }

        // Remove from participants
        auto partIt = std::find(quest->participants.begin(), quest->participants.end(), playerId);
        if (partIt != quest->participants.end())
        {
            quest->participants.erase(partIt);
        }

        // Notify quest failure
        if (m_questFailedCallback)
        {
            m_questFailedCallback(questId, playerId);
        }

        LOG_INFO("Player " + std::to_string(playerId) + " abandoned quest: " + quest->name);
        return true;
    }

    bool CooperativeQuestSystem::UpdateObjective(uint32_t questId, uint32_t objectiveId, uint32_t progress)
    {
        QuestData* quest = GetQuest(questId);
        if (!quest || quest->state != QuestState::InProgress)
        {
            return false;
        }

        // Find and update objective
        for (auto& objective : quest->objectives)
        {
            if (objective.objectiveId == objectiveId)
            {
                objective.currentCount = std::min(objective.currentCount + progress, objective.requiredCount);
                objective.isCompleted = objective.currentCount >= objective.requiredCount;

                // Notify objective update
                if (m_objectiveUpdatedCallback)
                {
                    m_objectiveUpdatedCallback(questId, objectiveId, objective.currentCount);
                }

                LOG_DEBUG("Updated objective " + std::to_string(objectiveId) + " for quest " + std::to_string(questId) + 
                         " (progress: " + std::to_string(objective.currentCount) + "/" + std::to_string(objective.requiredCount) + ")");
                return true;
            }
        }

        return false;
    }

    bool CooperativeQuestSystem::JoinQuest(uint32_t questId, uint32_t playerId)
    {
        QuestData* quest = GetQuest(questId);
        if (!quest || !quest->isCooperative || !CanPlayerJoinQuest(questId, playerId))
        {
            return false;
        }

        // Add player to quest participants
        quest->participants.push_back(playerId);
        m_playerQuests[playerId].push_back(questId);

        LOG_INFO("Player " + std::to_string(playerId) + " joined cooperative quest: " + quest->name);
        return true;
    }

    bool CooperativeQuestSystem::LeaveQuest(uint32_t questId, uint32_t playerId)
    {
        QuestData* quest = GetQuest(questId);
        if (!quest)
        {
            return false;
        }

        // Remove player from participants
        auto it = std::find(quest->participants.begin(), quest->participants.end(), playerId);
        if (it != quest->participants.end())
        {
            quest->participants.erase(it);
        }

        // Remove from player's quest list
        auto playerIt = std::find(m_playerQuests[playerId].begin(), m_playerQuests[playerId].end(), questId);
        if (playerIt != m_playerQuests[playerId].end())
        {
            m_playerQuests[playerId].erase(playerIt);
        }

        LOG_INFO("Player " + std::to_string(playerId) + " left quest: " + quest->name);
        return true;
    }

    bool CooperativeQuestSystem::TransferQuestOwnership(uint32_t questId, uint32_t fromPlayer, uint32_t toPlayer)
    {
        QuestData* quest = GetQuest(questId);
        if (!quest || quest->syncOwner != fromPlayer)
        {
            return false;
        }

        // Check if toPlayer is a participant
        auto it = std::find(quest->participants.begin(), quest->participants.end(), toPlayer);
        if (it == quest->participants.end())
        {
            return false;
        }

        quest->syncOwner = toPlayer;
        LOG_INFO("Transferred quest ownership from " + std::to_string(fromPlayer) + " to " + std::to_string(toPlayer));
        return true;
    }

    uint32_t CooperativeQuestSystem::CreateGroup(const std::string& name, uint32_t leaderId)
    {
        QuestGroup group;
        group.groupId = m_nextGroupId++;
        group.name = name;
        group.leader = leaderId;
        group.members.push_back(leaderId);
        group.createdTime = std::chrono::high_resolution_clock::now();
        group.isActive = true;

        m_groups[group.groupId] = group;
        m_playerGroups[leaderId].push_back(group.groupId);
        m_stats.activeGroups++;

        if (m_groupCreatedCallback)
        {
            m_groupCreatedCallback(group.groupId, leaderId);
        }

        LOG_INFO("Created quest group: " + name + " (ID: " + std::to_string(group.groupId) + ")");
        return group.groupId;
    }

    bool CooperativeQuestSystem::JoinGroup(uint32_t groupId, uint32_t playerId)
    {
        QuestGroup* group = GetGroup(groupId);
        if (!group || !CanPlayerJoinGroup(groupId, playerId))
        {
            return false;
        }

        group->members.push_back(playerId);
        m_playerGroups[playerId].push_back(groupId);
        m_stats.totalParticipants++;

        LOG_INFO("Player " + std::to_string(playerId) + " joined group: " + group->name);
        return true;
    }

    bool CooperativeQuestSystem::LeaveGroup(uint32_t groupId, uint32_t playerId)
    {
        QuestGroup* group = GetGroup(groupId);
        if (!group)
        {
            return false;
        }

        // Remove from group members
        auto it = std::find(group->members.begin(), group->members.end(), playerId);
        if (it != group->members.end())
        {
            group->members.erase(it);
        }

        // Remove from player's groups
        auto playerIt = std::find(m_playerGroups[playerId].begin(), m_playerGroups[playerId].end(), groupId);
        if (playerIt != m_playerGroups[playerId].end())
        {
            m_playerGroups[playerId].erase(playerIt);
        }

        // If leader left, transfer leadership or disband
        if (group->leader == playerId)
        {
            if (group->members.empty())
            {
                DisbandGroup(groupId);
            }
            else
            {
                group->leader = group->members[0];
            }
        }

        LOG_INFO("Player " + std::to_string(playerId) + " left group: " + group->name);
        return true;
    }

    bool CooperativeQuestSystem::DisbandGroup(uint32_t groupId)
    {
        QuestGroup* group = GetGroup(groupId);
        if (!group)
        {
            return false;
        }

        // Remove all members from their group lists
        for (uint32_t memberId : group->members)
        {
            auto it = std::find(m_playerGroups[memberId].begin(), m_playerGroups[memberId].end(), groupId);
            if (it != m_playerGroups[memberId].end())
            {
                m_playerGroups[memberId].erase(it);
            }
        }

        m_groups.erase(groupId);
        m_stats.activeGroups--;

        LOG_INFO("Disbanded group: " + group->name);
        return true;
    }

    QuestGroup* CooperativeQuestSystem::GetGroup(uint32_t groupId)
    {
        auto it = m_groups.find(groupId);
        if (it != m_groups.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    std::vector<QuestGroup> CooperativeQuestSystem::GetPlayerGroups(uint32_t playerId) const
    {
        std::vector<QuestGroup> playerGroups;
        
        auto it = m_playerGroups.find(playerId);
        if (it != m_playerGroups.end())
        {
            for (uint32_t groupId : it->second)
            {
                auto groupIt = m_groups.find(groupId);
                if (groupIt != m_groups.end())
                {
                    playerGroups.push_back(groupIt->second);
                }
            }
        }
        
        return playerGroups;
    }

    void CooperativeQuestSystem::SynchronizeQuest(uint32_t questId, const QuestSyncData& syncData)
    {
        QuestData* quest = GetQuest(questId);
        if (!quest)
        {
            return;
        }

        // Update quest with sync data
        quest->state = syncData.state;
        quest->objectives = syncData.objectives;
        quest->participants = syncData.participants;
        quest->syncOwner = syncData.syncOwner;

        LOG_DEBUG("Synchronized quest " + std::to_string(questId) + " with " + std::to_string(syncData.participants.size()) + " participants");
    }

    QuestSyncData CooperativeQuestSystem::GetQuestSyncData(uint32_t questId) const
    {
        QuestSyncData syncData;
        
        auto it = m_quests.find(questId);
        if (it != m_quests.end())
        {
            const QuestData& quest = it->second;
            syncData.questId = quest.questId;
            syncData.state = quest.state;
            syncData.objectives = quest.objectives;
            syncData.participants = quest.participants;
            syncData.syncOwner = quest.syncOwner;
            syncData.timestamp = std::chrono::high_resolution_clock::now();
        }
        
        return syncData;
    }

    void CooperativeQuestSystem::BroadcastQuestUpdate(uint32_t questId)
    {
        QuestSyncData syncData = GetQuestSyncData(questId);
        if (syncData.questId != 0)
        {
            // This would broadcast to all participants
            // In a real implementation, this would use the networking system
            LOG_DEBUG("Broadcasting quest update for quest " + std::to_string(questId));
        }
    }

    void CooperativeQuestSystem::ProcessQuestUpdate(const QuestSyncData& syncData)
    {
        SynchronizeQuest(syncData.questId, syncData);
    }

    // Quest triggers (REDkit integration)
    void CooperativeQuestSystem::OnPlayerEnterArea(uint32_t playerId, const std::string& areaId)
    {
        // Process area-based quest triggers
        for (const auto& pair : m_quests)
        {
            const QuestData& quest = pair.second;
            if (quest.state == QuestState::InProgress && quest.location == areaId)
            {
                ProcessQuestTriggers(quest.questId, "enter_area", playerId);
            }
        }
    }

    void CooperativeQuestSystem::OnPlayerKillMonster(uint32_t playerId, const std::string& monsterId)
    {
        // Process kill-based quest triggers
        for (const auto& pair : m_quests)
        {
            const QuestData& quest = pair.second;
            if (quest.state == QuestState::InProgress)
            {
                for (const auto& objective : quest.objectives)
                {
                    if (objective.type == "kill" && objective.target == monsterId)
                    {
                        UpdateObjective(quest.questId, objective.objectiveId, 1);
                    }
                }
            }
        }
    }

    void CooperativeQuestSystem::OnPlayerCollectItem(uint32_t playerId, uint32_t itemId)
    {
        // Process collect-based quest triggers
        for (const auto& pair : m_quests)
        {
            const QuestData& quest = pair.second;
            if (quest.state == QuestState::InProgress)
            {
                for (const auto& objective : quest.objectives)
                {
                    if (objective.type == "collect" && std::to_string(itemId) == objective.target)
                    {
                        UpdateObjective(quest.questId, objective.objectiveId, 1);
                    }
                }
            }
        }
    }

    void CooperativeQuestSystem::OnPlayerTalkToNpc(uint32_t playerId, const std::string& npcId)
    {
        // Process talk-based quest triggers
        for (const auto& pair : m_quests)
        {
            const QuestData& quest = pair.second;
            if (quest.state == QuestState::InProgress)
            {
                for (const auto& objective : quest.objectives)
                {
                    if (objective.type == "talk" && objective.target == npcId)
                    {
                        UpdateObjective(quest.questId, objective.objectiveId, 1);
                    }
                }
            }
        }
    }

    void CooperativeQuestSystem::OnPlayerUseObject(uint32_t playerId, const std::string& objectId)
    {
        // Process use-based quest triggers
        for (const auto& pair : m_quests)
        {
            const QuestData& quest = pair.second;
            if (quest.state == QuestState::InProgress)
            {
                for (const auto& objective : quest.objectives)
                {
                    if (objective.type == "use" && objective.target == objectId)
                    {
                        UpdateObjective(quest.questId, objective.objectiveId, 1);
                    }
                }
            }
        }
    }

    // Configuration methods
    void CooperativeQuestSystem::SetMaxGroupSize(uint32_t maxSize)
    {
        m_maxGroupSize = maxSize;
    }

    void CooperativeQuestSystem::SetQuestSyncInterval(float interval)
    {
        m_questSyncInterval = interval;
    }

    void CooperativeQuestSystem::EnableCooperativeMode(bool enable)
    {
        m_cooperativeModeEnabled = enable;
    }

    void CooperativeQuestSystem::SetQuestDifficulty(float difficulty)
    {
        m_questDifficulty = std::max(0.1f, std::min(5.0f, difficulty));
    }

    CooperativeQuestSystem::QuestStats CooperativeQuestSystem::GetStats() const
    {
        return m_stats;
    }

    void CooperativeQuestSystem::ResetStats()
    {
        m_stats.Reset();
    }

    void CooperativeQuestSystem::PrintStats() const
    {
        LOG_INFO("=== Cooperative Quest System Statistics ===");
        LOG_INFO("Total quests: " + std::to_string(m_stats.totalQuests));
        LOG_INFO("Completed quests: " + std::to_string(m_stats.completedQuests));
        LOG_INFO("Failed quests: " + std::to_string(m_stats.failedQuests));
        LOG_INFO("Active groups: " + std::to_string(m_stats.activeGroups));
        LOG_INFO("Total participants: " + std::to_string(m_stats.totalParticipants));
        LOG_INFO("Average completion time: " + std::to_string(m_stats.averageCompletionTime) + "s");
        LOG_INFO("Quest success rate: " + std::to_string(m_stats.questSuccessRate * 100.0f) + "%");
        LOG_INFO("==========================================");
    }

    // Callback setters
    void CooperativeQuestSystem::SetQuestStartedCallback(QuestStartedCallback callback)
    {
        m_questStartedCallback = callback;
    }

    void CooperativeQuestSystem::SetQuestCompletedCallback(QuestCompletedCallback callback)
    {
        m_questCompletedCallback = callback;
    }

    void CooperativeQuestSystem::SetQuestFailedCallback(QuestFailedCallback callback)
    {
        m_questFailedCallback = callback;
    }

    void CooperativeQuestSystem::SetObjectiveUpdatedCallback(ObjectiveUpdatedCallback callback)
    {
        m_objectiveUpdatedCallback = callback;
    }

    void CooperativeQuestSystem::SetGroupCreatedCallback(GroupCreatedCallback callback)
    {
        m_groupCreatedCallback = callback;
    }

    // Private methods
    void CooperativeQuestSystem::InitializeDefaultQuests()
    {
        // Create some default cooperative quests
        QuestData mainQuest = QuestUtils::CreateMainQuest("The Witcher's Call", "A cooperative adventure begins", 1);
        mainQuest.isCooperative = true;
        mainQuest.maxParticipants = 4;
        mainQuest.objectives.push_back(QuestUtils::CreateKillObjective("Defeat the Griffin", "griffin", 1));
        mainQuest.objectives.push_back(QuestUtils::CreateCollectObjective("Gather Griffin Feathers", "griffin_feather", 3));
        AddQuest(mainQuest);

        QuestData sideQuest = QuestUtils::CreateSideQuest("Bandit Hunt", "Clear the bandit camp together", 2);
        sideQuest.isCooperative = true;
        sideQuest.maxParticipants = 3;
        sideQuest.objectives.push_back(QuestUtils::CreateKillObjective("Eliminate Bandits", "bandit", 10));
        sideQuest.objectives.push_back(QuestUtils::CreateReachObjective("Reach the Bandit Camp", "bandit_camp"));
        AddQuest(sideQuest);
    }

    bool CooperativeQuestSystem::ValidateQuestStart(uint32_t questId, uint32_t playerId) const
    {
        const QuestData* quest = GetQuest(questId);
        if (!quest)
        {
            return false;
        }

        // Check if quest is available
        if (quest->state != QuestState::Available)
        {
            return false;
        }

        // Check prerequisites
        if (!CheckQuestPrerequisites(questId, playerId))
        {
            return false;
        }

        // Check if player can join
        if (quest->isCooperative && !CanPlayerJoinQuest(questId, playerId))
        {
            return false;
        }

        return true;
    }

    bool CooperativeQuestSystem::CheckQuestPrerequisites(uint32_t questId, uint32_t playerId) const
    {
        const QuestData* quest = GetQuest(questId);
        if (!quest)
        {
            return false;
        }

        // Check required quests
        for (uint32_t requiredQuestId : quest->requiredQuests)
        {
            const QuestData* requiredQuest = GetQuest(requiredQuestId);
            if (!requiredQuest || requiredQuest->state != QuestState::Completed)
            {
                return false;
            }
        }

        return true;
    }

    void CooperativeQuestSystem::UpdateQuestState(uint32_t questId, QuestState newState)
    {
        QuestData* quest = GetQuest(questId);
        if (quest)
        {
            quest->state = newState;
            BroadcastQuestUpdate(questId);
        }
    }

    void CooperativeQuestSystem::NotifyQuestUpdate(uint32_t questId, uint32_t playerId)
    {
        // Notify all participants of quest update
        QuestData* quest = GetQuest(questId);
        if (quest)
        {
            for (uint32_t participantId : quest->participants)
            {
                if (participantId != playerId)
                {
                    // Send update to participant
                    LOG_DEBUG("Notifying participant " + std::to_string(participantId) + " of quest update");
                }
            }
        }
    }

    void CooperativeQuestSystem::CleanupCompletedQuests()
    {
        // Remove old completed quests
        auto it = m_quests.begin();
        while (it != m_quests.end())
        {
            if (it->second.state == QuestState::Completed || it->second.state == QuestState::Failed)
            {
                auto now = std::chrono::high_resolution_clock::now();
                float age = std::chrono::duration<float>(now - it->second.completionTime).count();
                
                if (age > 3600.0f) // 1 hour
                {
                    it = m_quests.erase(it);
                }
                else
                {
                    ++it;
                }
            }
            else
            {
                ++it;
            }
        }
    }

    void CooperativeQuestSystem::ProcessQuestTriggers(uint32_t questId, const std::string& triggerType, uint32_t playerId)
    {
        // Process quest-specific triggers
        QuestData* quest = GetQuest(questId);
        if (!quest)
        {
            return;
        }

        // Update relevant objectives based on trigger type
        for (auto& objective : quest->objectives)
        {
            if (objective.type == triggerType)
            {
                UpdateObjective(questId, objective.objectiveId, 1);
            }
        }
    }

    bool CooperativeQuestSystem::IsQuestAvailable(uint32_t questId, uint32_t playerId) const
    {
        const QuestData* quest = GetQuest(questId);
        if (!quest)
        {
            return false;
        }

        return quest->state == QuestState::Available && CheckQuestPrerequisites(questId, playerId);
    }

    bool CooperativeQuestSystem::CanPlayerJoinQuest(uint32_t questId, uint32_t playerId) const
    {
        const QuestData* quest = GetQuest(questId);
        if (!quest || !quest->isCooperative)
        {
            return false;
        }

        // Check if quest has space for more participants
        return quest->participants.size() < quest->maxParticipants;
    }

    bool CooperativeQuestSystem::IsQuestComplete(uint32_t questId) const
    {
        const QuestData* quest = GetQuest(questId);
        if (!quest)
        {
            return false;
        }

        // Check if all required objectives are completed
        for (const auto& objective : quest->objectives)
        {
            if (!objective.isOptional && !objective.isCompleted)
            {
                return false;
            }
        }

        return true;
    }

    bool CooperativeQuestSystem::IsPlayerInGroup(uint32_t playerId, uint32_t groupId) const
    {
        const QuestGroup* group = GetGroup(groupId);
        if (!group)
        {
            return false;
        }

        return std::find(group->members.begin(), group->members.end(), playerId) != group->members.end();
    }

    bool CooperativeQuestSystem::CanPlayerJoinGroup(uint32_t groupId, uint32_t playerId) const
    {
        const QuestGroup* group = GetGroup(groupId);
        if (!group || !group->isActive)
        {
            return false;
        }

        // Check if group has space
        if (group->members.size() >= m_maxGroupSize)
        {
            return false;
        }

        // Check if player is already in group
        return !IsPlayerInGroup(playerId, groupId);
    }

    // QuestDatabaseManager implementation
    QuestDatabaseManager::QuestDatabaseManager()
        : m_initialized(false)
    {
        LOG_INFO("Quest database manager created");
    }

    QuestDatabaseManager::~QuestDatabaseManager()
    {
        Shutdown();
        LOG_INFO("Quest database manager destroyed");
    }

    bool QuestDatabaseManager::Initialize(const std::string& databasePath)
    {
        if (m_initialized)
        {
            return true;
        }

        m_databasePath = databasePath;
        m_initialized = true;
        
        LOG_INFO("Quest database manager initialized with path: " + databasePath);
        return true;
    }

    void QuestDatabaseManager::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        m_initialized = false;
        LOG_INFO("Quest database manager shutdown complete");
    }

    // Quest utilities implementation
    namespace QuestUtils
    {
        QuestData CreateMainQuest(const std::string& name, const std::string& description, uint32_t level)
        {
            QuestData quest;
            quest.name = name;
            quest.description = description;
            quest.type = QuestType::Main;
            quest.level = level;
            quest.state = QuestState::Available;
            quest.experienceReward = level * 100;
            quest.goldReward = level * 50;
            return quest;
        }

        QuestData CreateSideQuest(const std::string& name, const std::string& description, uint32_t level)
        {
            QuestData quest;
            quest.name = name;
            quest.description = description;
            quest.type = QuestType::Side;
            quest.level = level;
            quest.state = QuestState::Available;
            quest.experienceReward = level * 50;
            quest.goldReward = level * 25;
            return quest;
        }

        QuestData CreateContractQuest(const std::string& name, const std::string& description, uint32_t level)
        {
            QuestData quest;
            quest.name = name;
            quest.description = description;
            quest.type = QuestType::Contract;
            quest.level = level;
            quest.state = QuestState::Available;
            quest.experienceReward = level * 75;
            quest.goldReward = level * 100;
            return quest;
        }

        QuestObjective CreateKillObjective(const std::string& description, const std::string& target, uint32_t count)
        {
            QuestObjective objective;
            objective.description = description;
            objective.type = "kill";
            objective.target = target;
            objective.requiredCount = count;
            return objective;
        }

        QuestObjective CreateCollectObjective(const std::string& description, const std::string& target, uint32_t count)
        {
            QuestObjective objective;
            objective.description = description;
            objective.type = "collect";
            objective.target = target;
            objective.requiredCount = count;
            return objective;
        }

        QuestObjective CreateTalkObjective(const std::string& description, const std::string& target)
        {
            QuestObjective objective;
            objective.description = description;
            objective.type = "talk";
            objective.target = target;
            objective.requiredCount = 1;
            return objective;
        }

        QuestObjective CreateReachObjective(const std::string& description, const std::string& target)
        {
            QuestObjective objective;
            objective.description = description;
            objective.type = "reach";
            objective.target = target;
            objective.requiredCount = 1;
            return objective;
        }
    }
}
