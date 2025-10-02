#include "game/SharedProgression.h"
#include "utils/Logger.h"
#include <algorithm>
#include <cmath>

namespace Game
{
    // SharedProgression implementation
    SharedProgression::SharedProgression()
        : m_initialized(false), m_experienceMultiplier(1.0f), m_groupExperienceBonus(0.2f),
          m_maxLevel(100), m_skillPointsPerLevel(1), m_groupProgressionEnabled(true),
          m_nextPlayerId(1), m_nextGroupId(1), m_nextSkillId(1), m_nextAchievementId(1)
    {
        m_lastUpdateTime = std::chrono::high_resolution_clock::now();
        
        LOG_INFO("Shared progression system created");
    }

    SharedProgression::~SharedProgression()
    {
        Shutdown();
        LOG_INFO("Shared progression system destroyed");
    }

    bool SharedProgression::Initialize()
    {
        if (m_initialized)
        {
            return true;
        }

        LOG_INFO("Initializing shared progression system...");

        // Initialize default skills
        InitializeDefaultSkills();
        
        // Initialize default achievements
        InitializeDefaultAchievements();

        m_initialized = true;
        LOG_INFO("Shared progression system initialized");
        return true;
    }

    void SharedProgression::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        LOG_INFO("Shutting down shared progression system...");
        
        // Save progression data
        SaveProgressionData();
        
        // Clear all data
        m_playerProgressions.clear();
        m_groupProgressions.clear();
        m_skills.clear();
        m_achievements.clear();
        m_playerAchievements.clear();
        
        m_initialized = false;
        LOG_INFO("Shared progression system shutdown complete");
    }

    void SharedProgression::AddPlayer(uint32_t playerId)
    {
        if (!m_initialized)
        {
            return;
        }

        PlayerProgressionData player;
        player.playerId = playerId;
        player.level = 1;
        player.experience = 0;
        player.experienceToNextLevel = CalculateExperienceRequired(2);
        player.skillPoints = 0;
        player.abilityPoints = 0;
        player.lastUpdate = std::chrono::high_resolution_clock::now();
        
        m_playerProgressions[playerId] = player;
        m_stats.totalPlayers++;

        LOG_INFO("Added player to progression system: " + std::to_string(playerId));
    }

    void SharedProgression::RemovePlayer(uint32_t playerId)
    {
        auto it = m_playerProgressions.find(playerId);
        if (it != m_playerProgressions.end())
        {
            m_playerProgressions.erase(it);
            m_stats.totalPlayers--;
            LOG_INFO("Removed player from progression system: " + std::to_string(playerId));
        }
    }

    PlayerProgressionData* SharedProgression::GetPlayerProgression(uint32_t playerId)
    {
        auto it = m_playerProgressions.find(playerId);
        if (it != m_playerProgressions.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    bool SharedProgression::AddExperience(uint32_t playerId, uint32_t amount, bool isGroupExperience)
    {
        PlayerProgressionData* player = GetPlayerProgression(playerId);
        if (!player)
        {
            return false;
        }

        // Apply experience multiplier
        uint32_t adjustedAmount = static_cast<uint32_t>(amount * m_experienceMultiplier);
        
        // Apply group bonus if applicable
        if (isGroupExperience && m_groupProgressionEnabled)
        {
            adjustedAmount = static_cast<uint32_t>(adjustedAmount * (1.0f + m_groupExperienceBonus));
        }

        player->experience += adjustedAmount;
        player->lastUpdate = std::chrono::high_resolution_clock::now();
        
        // Check for level up
        while (player->experience >= player->experienceToNextLevel && player->level < m_maxLevel)
        {
            LevelUpPlayer(playerId);
        }
        
        UpdatePlayerStats(playerId);
        
        LOG_DEBUG("Added " + std::to_string(adjustedAmount) + " experience to player " + std::to_string(playerId) + 
                 " (total: " + std::to_string(player->experience) + ")");
        return true;
    }

    bool SharedProgression::LevelUpPlayer(uint32_t playerId)
    {
        PlayerProgressionData* player = GetPlayerProgression(playerId);
        if (!player || player->level >= m_maxLevel)
        {
            return false;
        }

        uint32_t oldLevel = player->level;
        player->level++;
        player->skillPoints += m_skillPointsPerLevel;
        player->abilityPoints += 1;
        
        // Calculate experience for next level
        CalculateExperienceToNextLevel(*player);
        
        // Call callback
        if (m_playerLeveledUpCallback)
        {
            m_playerLeveledUpCallback(playerId, player->level);
        }
        
        LOG_INFO("Player " + std::to_string(playerId) + " leveled up to level " + std::to_string(player->level));
        return true;
    }

    bool SharedProgression::AddSkillPoint(uint32_t playerId, uint32_t amount)
    {
        PlayerProgressionData* player = GetPlayerProgression(playerId);
        if (!player)
        {
            return false;
        }

        player->skillPoints += amount;
        player->lastUpdate = std::chrono::high_resolution_clock::now();
        
        LOG_DEBUG("Added " + std::to_string(amount) + " skill points to player " + std::to_string(playerId));
        return true;
    }

    bool SharedProgression::SpendSkillPoint(uint32_t playerId, uint32_t skillId)
    {
        PlayerProgressionData* player = GetPlayerProgression(playerId);
        SkillData* skill = GetSkill(skillId);
        
        if (!player || !skill)
        {
            return false;
        }

        // Check if player has enough skill points
        if (player->skillPoints < 1)
        {
            return false;
        }

        // Check if skill can be upgraded
        if (skill->currentLevel >= skill->maxLevel)
        {
            return false;
        }

        // Check prerequisites
        if (!CheckSkillPrerequisites(playerId, skillId))
        {
            return false;
        }

        // Spend skill point
        player->skillPoints--;
        skill->currentLevel++;
        player->lastUpdate = std::chrono::high_resolution_clock::now();
        
        // Call callback
        if (m_skillUnlockedCallback)
        {
            m_skillUnlockedCallback(playerId, skillId);
        }
        
        LOG_DEBUG("Player " + std::to_string(playerId) + " upgraded skill " + skill->name + 
                 " to level " + std::to_string(skill->currentLevel));
        return true;
    }

    uint32_t SharedProgression::CreateGroup(const std::string& groupName)
    {
        if (!m_initialized)
        {
            return 0;
        }

        GroupProgressionData group;
        group.groupId = m_nextGroupId++;
        group.groupName = groupName;
        group.groupLevel = 1;
        group.sharedExperience = 0;
        group.createdTime = std::chrono::high_resolution_clock::now();
        group.isActive = true;

        m_groupProgressions[group.groupId] = group;
        m_stats.totalGroups++;

        LOG_INFO("Created progression group: " + groupName + " (ID: " + std::to_string(group.groupId) + ")");
        return group.groupId;
    }

    void SharedProgression::AddPlayerToGroup(uint32_t groupId, uint32_t playerId)
    {
        GroupProgressionData* group = GetGroupProgression(groupId);
        PlayerProgressionData* player = GetPlayerProgression(playerId);
        
        if (!group || !player)
        {
            return;
        }

        // Add player to group
        group->members.push_back(playerId);
        group->memberContributions[playerId] = 0;
        
        // Update group level
        CalculateGroupLevel(*group);
        
        LOG_DEBUG("Added player " + std::to_string(playerId) + " to group " + std::to_string(groupId));
    }

    void SharedProgression::RemovePlayerFromGroup(uint32_t groupId, uint32_t playerId)
    {
        GroupProgressionData* group = GetGroupProgression(groupId);
        if (!group)
        {
            return;
        }

        // Remove player from group
        auto it = std::find(group->members.begin(), group->members.end(), playerId);
        if (it != group->members.end())
        {
            group->members.erase(it);
        }
        
        // Remove contribution record
        group->memberContributions.erase(playerId);
        
        // Update group level
        CalculateGroupLevel(*group);
        
        LOG_DEBUG("Removed player " + std::to_string(playerId) + " from group " + std::to_string(groupId));
    }

    GroupProgressionData* SharedProgression::GetGroupProgression(uint32_t groupId)
    {
        auto it = m_groupProgressions.find(groupId);
        if (it != m_groupProgressions.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    bool SharedProgression::AddGroupExperience(uint32_t groupId, uint32_t amount)
    {
        GroupProgressionData* group = GetGroupProgression(groupId);
        if (!group || !group->isActive)
        {
            return false;
        }

        // Apply group bonus
        uint32_t adjustedAmount = static_cast<uint32_t>(amount * (1.0f + m_groupExperienceBonus));
        
        group->sharedExperience += adjustedAmount;
        
        // Distribute experience to members
        DistributeGroupExperience(groupId);
        
        LOG_DEBUG("Added " + std::to_string(adjustedAmount) + " experience to group " + std::to_string(groupId));
        return true;
    }

    bool SharedProgression::DistributeGroupExperience(uint32_t groupId)
    {
        GroupProgressionData* group = GetGroupProgression(groupId);
        if (!group || group->members.empty())
        {
            return false;
        }

        // Calculate experience per member
        uint32_t experiencePerMember = group->sharedExperience / group->members.size();
        
        // Distribute to all members
        for (uint32_t playerId : group->members)
        {
            AddExperience(playerId, experiencePerMember, true);
            group->memberContributions[playerId] += experiencePerMember;
        }
        
        // Reset shared experience
        group->sharedExperience = 0;
        
        LOG_DEBUG("Distributed " + std::to_string(experiencePerMember) + " experience to each member of group " + std::to_string(groupId));
        return true;
    }

    void SharedProgression::AddSkill(const SkillData& skill)
    {
        if (!m_initialized)
        {
            return;
        }

        SkillData skillCopy = skill;
        if (skillCopy.skillId == 0)
        {
            skillCopy.skillId = m_nextSkillId++;
        }

        m_skills[skillCopy.skillId] = skillCopy;
        
        LOG_INFO("Added skill: " + skillCopy.name + " (ID: " + std::to_string(skillCopy.skillId) + ")");
    }

    void SharedProgression::RemoveSkill(uint32_t skillId)
    {
        auto it = m_skills.find(skillId);
        if (it != m_skills.end())
        {
            m_skills.erase(it);
            LOG_INFO("Removed skill ID: " + std::to_string(skillId));
        }
    }

    SkillData* SharedProgression::GetSkill(uint32_t skillId)
    {
        auto it = m_skills.find(skillId);
        if (it != m_skills.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    std::vector<SkillData> SharedProgression::GetPlayerSkills(uint32_t playerId) const
    {
        std::vector<SkillData> playerSkills;
        
        for (const auto& pair : m_skills)
        {
            const SkillData& skill = pair.second;
            if (skill.isUnlocked)
            {
                playerSkills.push_back(skill);
            }
        }
        
        return playerSkills;
    }

    std::vector<SkillData> SharedProgression::GetSkillsByCategory(SkillCategory category) const
    {
        std::vector<SkillData> categorySkills;
        
        for (const auto& pair : m_skills)
        {
            if (pair.second.category == category)
            {
                categorySkills.push_back(pair.second);
            }
        }
        
        return categorySkills;
    }

    bool SharedProgression::UnlockSkill(uint32_t playerId, uint32_t skillId)
    {
        PlayerProgressionData* player = GetPlayerProgression(playerId);
        SkillData* skill = GetSkill(skillId);
        
        if (!player || !skill)
        {
            return false;
        }

        // Check if already unlocked
        if (skill->isUnlocked)
        {
            return false;
        }

        // Check prerequisites
        if (!CheckSkillPrerequisites(playerId, skillId))
        {
            return false;
        }

        // Unlock skill
        skill->isUnlocked = true;
        skill->isActive = true;
        player->lastUpdate = std::chrono::high_resolution_clock::now();
        
        // Call callback
        if (m_skillUnlockedCallback)
        {
            m_skillUnlockedCallback(playerId, skillId);
        }
        
        LOG_DEBUG("Player " + std::to_string(playerId) + " unlocked skill: " + skill->name);
        return true;
    }

    bool SharedProgression::UpgradeSkill(uint32_t playerId, uint32_t skillId)
    {
        return SpendSkillPoint(playerId, skillId);
    }

    void SharedProgression::AddAchievement(const AchievementData& achievement)
    {
        if (!m_initialized)
        {
            return;
        }

        AchievementData achievementCopy = achievement;
        if (achievementCopy.achievementId == 0)
        {
            achievementCopy.achievementId = m_nextAchievementId++;
        }

        m_achievements[achievementCopy.achievementId] = achievementCopy;
        
        LOG_INFO("Added achievement: " + achievementCopy.name + " (ID: " + std::to_string(achievementCopy.achievementId) + ")");
    }

    void SharedProgression::RemoveAchievement(uint32_t achievementId)
    {
        auto it = m_achievements.find(achievementId);
        if (it != m_achievements.end())
        {
            m_achievements.erase(it);
            LOG_INFO("Removed achievement ID: " + std::to_string(achievementId));
        }
    }

    AchievementData* SharedProgression::GetAchievement(uint32_t achievementId)
    {
        auto it = m_achievements.find(achievementId);
        if (it != m_achievements.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    std::vector<AchievementData> SharedProgression::GetPlayerAchievements(uint32_t playerId) const
    {
        std::vector<AchievementData> playerAchievements;
        
        auto it = m_playerAchievements.find(playerId);
        if (it != m_playerAchievements.end())
        {
            for (uint32_t achievementId : it->second)
            {
                auto achievementIt = m_achievements.find(achievementId);
                if (achievementIt != m_achievements.end())
                {
                    playerAchievements.push_back(achievementIt->second);
                }
            }
        }
        
        return playerAchievements;
    }

    bool SharedProgression::UnlockAchievement(uint32_t playerId, uint32_t achievementId)
    {
        PlayerProgressionData* player = GetPlayerProgression(playerId);
        AchievementData* achievement = GetAchievement(achievementId);
        
        if (!player || !achievement)
        {
            return false;
        }

        // Check if already unlocked
        if (achievement->isUnlocked)
        {
            return false;
        }

        // Check requirements
        if (!CheckAchievementRequirements(playerId, achievementId))
        {
            return false;
        }

        // Unlock achievement
        achievement->isUnlocked = true;
        achievement->unlockedTime = std::chrono::high_resolution_clock::now();
        
        // Add to player's achievements
        m_playerAchievements[playerId].push_back(achievementId);
        
        // Give rewards
        if (achievement->experienceReward > 0)
        {
            AddExperience(playerId, achievement->experienceReward);
        }
        
        if (achievement->skillPointReward > 0)
        {
            AddSkillPoint(playerId, achievement->skillPointReward);
        }
        
        // Call callback
        if (m_achievementUnlockedCallback)
        {
            m_achievementUnlockedCallback(playerId, achievementId);
        }
        
        LOG_INFO("Player " + std::to_string(playerId) + " unlocked achievement: " + achievement->name);
        return true;
    }

    bool SharedProgression::CheckAchievementRequirements(uint32_t playerId, uint32_t achievementId)
    {
        AchievementData* achievement = GetAchievement(achievementId);
        if (!achievement)
        {
            return false;
        }

        // Check each requirement
        for (uint32_t requirement : achievement->requirements)
        {
            // This would check specific requirements
            // For now, just return true
        }
        
        return true;
    }

    // Progression queries
    uint32_t SharedProgression::GetPlayerLevel(uint32_t playerId) const
    {
        const PlayerProgressionData* player = GetPlayerProgression(playerId);
        if (!player)
        {
            return 0;
        }
        return player->level;
    }

    uint32_t SharedProgression::GetPlayerExperience(uint32_t playerId) const
    {
        const PlayerProgressionData* player = GetPlayerProgression(playerId);
        if (!player)
        {
            return 0;
        }
        return player->experience;
    }

    uint32_t SharedProgression::GetPlayerSkillPoints(uint32_t playerId) const
    {
        const PlayerProgressionData* player = GetPlayerProgression(playerId);
        if (!player)
        {
            return 0;
        }
        return player->skillPoints;
    }

    uint32_t SharedProgression::GetGroupLevel(uint32_t groupId) const
    {
        const GroupProgressionData* group = GetGroupProgression(groupId);
        if (!group)
        {
            return 0;
        }
        return group->groupLevel;
    }

    uint32_t SharedProgression::GetGroupExperience(uint32_t groupId) const
    {
        const GroupProgressionData* group = GetGroupProgression(groupId);
        if (!group)
        {
            return 0;
        }
        return group->sharedExperience;
    }

    float SharedProgression::GetPlayerProgress(uint32_t playerId) const
    {
        const PlayerProgressionData* player = GetPlayerProgression(playerId);
        if (!player)
        {
            return 0.0f;
        }
        
        return static_cast<float>(player->experience) / static_cast<float>(player->experienceToNextLevel);
    }

    float SharedProgression::GetGroupProgress(uint32_t groupId) const
    {
        const GroupProgressionData* group = GetGroupProgression(groupId);
        if (!group)
        {
            return 0.0f;
        }
        
        // Calculate group progress based on average member level
        if (group->members.empty())
        {
            return 0.0f;
        }
        
        uint32_t totalLevel = 0;
        for (uint32_t playerId : group->members)
        {
            totalLevel += GetPlayerLevel(playerId);
        }
        
        return static_cast<float>(totalLevel) / static_cast<float>(group->members.size());
    }

    // Configuration methods
    void SharedProgression::SetExperienceMultiplier(float multiplier)
    {
        m_experienceMultiplier = std::max(0.1f, std::min(10.0f, multiplier));
    }

    void SharedProgression::SetGroupExperienceBonus(float bonus)
    {
        m_groupExperienceBonus = std::max(0.0f, std::min(2.0f, bonus));
    }

    void SharedProgression::SetMaxLevel(uint32_t maxLevel)
    {
        m_maxLevel = std::max(1u, std::min(1000u, maxLevel));
    }

    void SharedProgression::SetSkillPointPerLevel(uint32_t skillPoints)
    {
        m_skillPointsPerLevel = std::max(1u, std::min(10u, skillPoints));
    }

    void SharedProgression::EnableGroupProgression(bool enable)
    {
        m_groupProgressionEnabled = enable;
    }

    SharedProgression::ProgressionStats SharedProgression::GetStats() const
    {
        return m_stats;
    }

    void SharedProgression::ResetStats()
    {
        m_stats.Reset();
    }

    void SharedProgression::PrintStats() const
    {
        LOG_INFO("=== Shared Progression Statistics ===");
        LOG_INFO("Total players: " + std::to_string(m_stats.totalPlayers));
        LOG_INFO("Total groups: " + std::to_string(m_stats.totalGroups));
        LOG_INFO("Total experience: " + std::to_string(m_stats.totalExperience));
        LOG_INFO("Total skills unlocked: " + std::to_string(m_stats.totalSkillsUnlocked));
        LOG_INFO("Total achievements unlocked: " + std::to_string(m_stats.totalAchievementsUnlocked));
        LOG_INFO("Average level: " + std::to_string(m_stats.averageLevel));
        LOG_INFO("Average group size: " + std::to_string(m_stats.averageGroupSize));
        LOG_INFO("Highest level: " + std::to_string(m_stats.highestLevel));
        LOG_INFO("Most active group: " + std::to_string(m_stats.mostActiveGroup));
        LOG_INFO("====================================");
    }

    // Callback setters
    void SharedProgression::SetPlayerLeveledUpCallback(PlayerLeveledUpCallback callback)
    {
        m_playerLeveledUpCallback = callback;
    }

    void SharedProgression::SetSkillUnlockedCallback(SkillUnlockedCallback callback)
    {
        m_skillUnlockedCallback = callback;
    }

    void SharedProgression::SetAchievementUnlockedCallback(AchievementUnlockedCallback callback)
    {
        m_achievementUnlockedCallback = callback;
    }

    void SharedProgression::SetGroupLeveledUpCallback(GroupLeveledUpCallback callback)
    {
        m_groupLeveledUpCallback = callback;
    }

    // Private methods
    void SharedProgression::InitializeDefaultSkills()
    {
        // Create some default skills
        SkillData swordMastery = ProgressionUtils::CreateCombatSkill("Sword Mastery", "Increases sword damage", 5);
        AddSkill(swordMastery);
        
        SkillData igniSign = ProgressionUtils::CreateSignsSkill("Igni Mastery", "Increases Igni sign power", 3);
        AddSkill(igniSign);
        
        SkillData alchemy = ProgressionUtils::CreateAlchemySkill("Alchemy", "Increases potion effectiveness", 4);
        AddSkill(alchemy);
        
        SkillData vitality = ProgressionUtils::CreateGeneralSkill("Vitality", "Increases maximum health", 5);
        AddSkill(vitality);
    }

    void SharedProgression::InitializeDefaultAchievements()
    {
        // Create some default achievements
        AchievementData firstKill = ProgressionUtils::CreateCombatAchievement("First Blood", "Kill your first enemy", 100);
        AddAchievement(firstKill);
        
        AchievementData explorer = ProgressionUtils::CreateExplorationAchievement("Explorer", "Visit 10 different locations", 200);
        AddAchievement(explorer);
        
        AchievementData social = ProgressionUtils::CreateSocialAchievement("Team Player", "Complete 5 group quests", 300);
        AddAchievement(social);
    }

    void SharedProgression::CalculateExperienceToNextLevel(PlayerProgressionData& player)
    {
        player.experienceToNextLevel = CalculateExperienceRequired(player.level + 1) - player.experience;
    }

    void SharedProgression::CalculateGroupLevel(GroupProgressionData& group)
    {
        if (group.members.empty())
        {
            group.groupLevel = 1;
            return;
        }
        
        // Calculate average level of group members
        uint32_t totalLevel = 0;
        for (uint32_t playerId : group.members)
        {
            totalLevel += GetPlayerLevel(playerId);
        }
        
        group.groupLevel = totalLevel / group.members.size();
    }

    bool SharedProgression::CheckSkillPrerequisites(uint32_t playerId, uint32_t skillId) const
    {
        const SkillData* skill = GetSkill(skillId);
        if (!skill)
        {
            return false;
        }

        // Check if player meets level requirement
        const PlayerProgressionData* player = GetPlayerProgression(playerId);
        if (!player || player->level < skill->experienceRequired)
        {
            return false;
        }

        // Check prerequisite skills
        for (uint32_t prereqId : skill->prerequisites)
        {
            const SkillData* prereq = GetSkill(prereqId);
            if (!prereq || !prereq->isUnlocked)
            {
                return false;
            }
        }

        return true;
    }

    void SharedProgression::UpdatePlayerStats(uint32_t playerId)
    {
        // Update global statistics
        m_stats.totalExperience += GetPlayerExperience(playerId);
        m_stats.averageLevel = static_cast<float>(m_stats.totalExperience) / static_cast<float>(m_stats.totalPlayers);
        m_stats.highestLevel = std::max(m_stats.highestLevel, GetPlayerLevel(playerId));
    }

    void SharedProgression::UpdateGroupStats(uint32_t groupId)
    {
        const GroupProgressionData* group = GetGroupProgression(groupId);
        if (!group)
        {
            return;
        }

        m_stats.averageGroupSize = static_cast<float>(group->members.size());
        m_stats.mostActiveGroup = groupId;
    }

    void SharedProgression::CleanupInactiveGroups()
    {
        auto it = m_groupProgressions.begin();
        while (it != m_groupProgressions.end())
        {
            if (!it->second.isActive || it->second.members.empty())
            {
                it = m_groupProgressions.erase(it);
                m_stats.totalGroups--;
            }
            else
            {
                ++it;
            }
        }
    }

    uint32_t SharedProgression::CalculateExperienceRequired(uint32_t level) const
    {
        // Exponential experience curve
        return static_cast<uint32_t>(1000 * std::pow(1.2f, level - 1));
    }

    uint32_t SharedProgression::CalculateGroupExperienceBonus(uint32_t groupSize) const
    {
        // Bonus increases with group size
        return static_cast<uint32_t>(groupSize * 0.1f * 100); // 10% per member
    }

    uint32_t SharedProgression::CalculateSkillExperienceCost(uint32_t skillId, uint32_t currentLevel) const
    {
        const SkillData* skill = GetSkill(skillId);
        if (!skill)
        {
            return 0;
        }

        // Cost increases with level
        return 100 * (currentLevel + 1);
    }

    bool SharedProgression::IsPlayerValid(uint32_t playerId) const
    {
        return m_playerProgressions.find(playerId) != m_playerProgressions.end();
    }

    bool SharedProgression::IsGroupValid(uint32_t groupId) const
    {
        return m_groupProgressions.find(groupId) != m_groupProgressions.end();
    }

    bool SharedProgression::IsSkillValid(uint32_t skillId) const
    {
        return m_skills.find(skillId) != m_skills.end();
    }

    bool SharedProgression::IsAchievementValid(uint32_t achievementId) const
    {
        return m_achievements.find(achievementId) != m_achievements.end();
    }

    void SharedProgression::SaveProgressionData()
    {
        // This would save progression data to file
        // For now, just log
        LOG_DEBUG("Progression data saved");
    }

    // Progression utilities implementation
    namespace ProgressionUtils
    {
        SkillData CreateCombatSkill(const std::string& name, const std::string& description, uint32_t maxLevel)
        {
            SkillData skill;
            skill.name = name;
            skill.description = description;
            skill.category = SkillCategory::Combat;
            skill.maxLevel = maxLevel;
            skill.experienceRequired = 1;
            return skill;
        }

        SkillData CreateSignsSkill(const std::string& name, const std::string& description, uint32_t maxLevel)
        {
            SkillData skill;
            skill.name = name;
            skill.description = description;
            skill.category = SkillCategory::Signs;
            skill.maxLevel = maxLevel;
            skill.experienceRequired = 1;
            return skill;
        }

        SkillData CreateAlchemySkill(const std::string& name, const std::string& description, uint32_t maxLevel)
        {
            SkillData skill;
            skill.name = name;
            skill.description = description;
            skill.category = SkillCategory::Alchemy;
            skill.maxLevel = maxLevel;
            skill.experienceRequired = 1;
            return skill;
        }

        SkillData CreateGeneralSkill(const std::string& name, const std::string& description, uint32_t maxLevel)
        {
            SkillData skill;
            skill.name = name;
            skill.description = description;
            skill.category = SkillCategory::General;
            skill.maxLevel = maxLevel;
            skill.experienceRequired = 1;
            return skill;
        }

        AchievementData CreateCombatAchievement(const std::string& name, const std::string& description, uint32_t expReward)
        {
            AchievementData achievement;
            achievement.name = name;
            achievement.description = description;
            achievement.category = "Combat";
            achievement.experienceReward = expReward;
            return achievement;
        }

        AchievementData CreateExplorationAchievement(const std::string& name, const std::string& description, uint32_t expReward)
        {
            AchievementData achievement;
            achievement.name = name;
            achievement.description = description;
            achievement.category = "Exploration";
            achievement.experienceReward = expReward;
            return achievement;
        }

        AchievementData CreateSocialAchievement(const std::string& name, const std::string& description, uint32_t expReward)
        {
            AchievementData achievement;
            achievement.name = name;
            achievement.description = description;
            achievement.category = "Social";
            achievement.experienceReward = expReward;
            return achievement;
        }

        uint32_t CalculateLevelExperience(uint32_t level)
        {
            return static_cast<uint32_t>(1000 * std::pow(1.2f, level - 1));
        }

        uint32_t CalculateTotalExperience(uint32_t level)
        {
            uint32_t total = 0;
            for (uint32_t i = 1; i < level; ++i)
            {
                total += CalculateLevelExperience(i);
            }
            return total;
        }

        float CalculateProgressPercentage(uint32_t currentExp, uint32_t requiredExp)
        {
            if (requiredExp == 0)
            {
                return 1.0f;
            }
            return static_cast<float>(currentExp) / static_cast<float>(requiredExp);
        }

        float CalculateSkillPower(const SkillData& skill)
        {
            return static_cast<float>(skill.currentLevel) / static_cast<float>(skill.maxLevel);
        }

        std::vector<uint32_t> GetSkillDependencies(uint32_t skillId, const std::map<uint32_t, SkillData>& skills)
        {
            std::vector<uint32_t> dependencies;
            auto it = skills.find(skillId);
            if (it != skills.end())
            {
                dependencies = it->second.prerequisites;
            }
            return dependencies;
        }

        bool CanUpgradeSkill(const SkillData& skill, uint32_t playerLevel)
        {
            return skill.currentLevel < skill.maxLevel && playerLevel >= skill.experienceRequired;
        }

        float CalculateAchievementDifficulty(const AchievementData& achievement)
        {
            // Simple difficulty calculation based on rewards
            return static_cast<float>(achievement.experienceReward) / 1000.0f;
        }

        std::vector<std::string> GetAchievementCategories(const std::vector<AchievementData>& achievements)
        {
            std::vector<std::string> categories;
            for (const auto& achievement : achievements)
            {
                if (std::find(categories.begin(), categories.end(), achievement.category) == categories.end())
                {
                    categories.push_back(achievement.category);
                }
            }
            return categories;
        }

        bool ValidatePlayerProgression(const PlayerProgressionData& player)
        {
            return player.playerId != 0 && player.level > 0;
        }

        bool ValidateGroupProgression(const GroupProgressionData& group)
        {
            return group.groupId != 0 && !group.groupName.empty();
        }

        bool ValidateSkillData(const SkillData& skill)
        {
            return !skill.name.empty() && skill.maxLevel > 0;
        }

        bool ValidateAchievementData(const AchievementData& achievement)
        {
            return !achievement.name.empty() && !achievement.description.empty();
        }

        void PrintPlayerAnalysis(const PlayerProgressionData& player)
        {
            LOG_INFO("=== Player Progression Analysis ===");
            LOG_INFO("Player ID: " + std::to_string(player.playerId));
            LOG_INFO("Level: " + std::to_string(player.level));
            LOG_INFO("Experience: " + std::to_string(player.experience));
            LOG_INFO("Skill Points: " + std::to_string(player.skillPoints));
            LOG_INFO("Ability Points: " + std::to_string(player.abilityPoints));
            LOG_INFO("Skills: " + std::to_string(player.skills.size()));
            LOG_INFO("Achievements: " + std::to_string(player.unlockedAbilities.size()));
            LOG_INFO("=================================");
        }

        void PrintGroupAnalysis(const GroupProgressionData& group)
        {
            LOG_INFO("=== Group Progression Analysis ===");
            LOG_INFO("Group ID: " + std::to_string(group.groupId));
            LOG_INFO("Name: " + group.groupName);
            LOG_INFO("Level: " + std::to_string(group.groupLevel));
            LOG_INFO("Members: " + std::to_string(group.members.size()));
            LOG_INFO("Shared Experience: " + std::to_string(group.sharedExperience));
            LOG_INFO("Active: " + (group.isActive ? "Yes" : "No"));
            LOG_INFO("================================");
        }

        void PrintSkillAnalysis(const SkillData& skill)
        {
            LOG_INFO("=== Skill Analysis ===");
            LOG_INFO("Name: " + skill.name);
            LOG_INFO("Description: " + skill.description);
            LOG_INFO("Category: " + std::to_string(static_cast<int>(skill.category)));
            LOG_INFO("Level: " + std::to_string(skill.currentLevel) + "/" + std::to_string(skill.maxLevel));
            LOG_INFO("Unlocked: " + (skill.isUnlocked ? "Yes" : "No"));
            LOG_INFO("Active: " + (skill.isActive ? "Yes" : "No"));
            LOG_INFO("====================");
        }

        void PrintAchievementAnalysis(const AchievementData& achievement)
        {
            LOG_INFO("=== Achievement Analysis ===");
            LOG_INFO("Name: " + achievement.name);
            LOG_INFO("Description: " + achievement.description);
            LOG_INFO("Category: " + achievement.category);
            LOG_INFO("Experience Reward: " + std::to_string(achievement.experienceReward));
            LOG_INFO("Skill Point Reward: " + std::to_string(achievement.skillPointReward));
            LOG_INFO("Unlocked: " + (achievement.isUnlocked ? "Yes" : "No"));
            LOG_INFO("===========================");
        }
    }
}
