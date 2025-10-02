#pragma once

#include "Common.h"
#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <functional>

namespace Game
{
    // Skill categories
    enum class SkillCategory
    {
        Combat = 0,
        Signs = 1,
        Alchemy = 2,
        General = 3
    };

    // Skill data
    struct SkillData
    {
        uint32_t skillId;
        std::string name;
        std::string description;
        SkillCategory category;
        uint32_t maxLevel;
        uint32_t currentLevel;
        uint32_t experienceRequired;
        std::vector<uint32_t> prerequisites;
        bool isUnlocked;
        bool isActive;
        
        SkillData() : skillId(0), category(SkillCategory::General), maxLevel(5),
                     currentLevel(0), experienceRequired(100), isUnlocked(false), isActive(false) {}
    };

    // Player progression data
    struct PlayerProgressionData
    {
        uint32_t playerId;
        uint32_t level;
        uint32_t experience;
        uint32_t experienceToNextLevel;
        uint32_t skillPoints;
        uint32_t abilityPoints;
        std::map<uint32_t, SkillData> skills;
        std::vector<uint32_t> unlockedAbilities;
        std::vector<uint32_t> activeAbilities;
        std::chrono::high_resolution_clock::time_point lastUpdate;
        
        PlayerProgressionData() : playerId(0), level(1), experience(0), 
                                experienceToNextLevel(1000), skillPoints(0), abilityPoints(0) {}
    };

    // Group progression data
    struct GroupProgressionData
    {
        uint32_t groupId;
        std::string groupName;
        std::vector<uint32_t> members;
        uint32_t sharedExperience;
        uint32_t groupLevel;
        std::map<uint32_t, uint32_t> memberContributions;
        std::chrono::high_resolution_clock::time_point createdTime;
        bool isActive;
        
        GroupProgressionData() : groupId(0), sharedExperience(0), groupLevel(1), isActive(true) {}
    };

    // Achievement data
    struct AchievementData
    {
        uint32_t achievementId;
        std::string name;
        std::string description;
        std::string category;
        uint32_t experienceReward;
        uint32_t skillPointReward;
        std::vector<uint32_t> requirements;
        bool isUnlocked;
        std::chrono::high_resolution_clock::time_point unlockedTime;
        
        AchievementData() : achievementId(0), experienceReward(0), skillPointReward(0), isUnlocked(false) {}
    };

    // Progression statistics
    struct ProgressionStats
    {
        uint32_t totalPlayers = 0;
        uint32_t totalGroups = 0;
        uint32_t totalExperience = 0;
        uint32_t totalSkillsUnlocked = 0;
        uint32_t totalAchievementsUnlocked = 0;
        float averageLevel = 0.0f;
        float averageGroupSize = 0.0f;
        uint32_t highestLevel = 1;
        uint32_t mostActiveGroup = 0;
        
        void Reset()
        {
            totalPlayers = 0;
            totalGroups = 0;
            totalExperience = 0;
            totalSkillsUnlocked = 0;
            totalAchievementsUnlocked = 0;
            averageLevel = 0.0f;
            averageGroupSize = 0.0f;
            highestLevel = 1;
            mostActiveGroup = 0;
        }
    };

    // Shared progression system
    class SharedProgression
    {
    public:
        SharedProgression();
        ~SharedProgression();

        // Initialize system
        bool Initialize();
        void Shutdown();

        // Player progression
        void AddPlayer(uint32_t playerId);
        void RemovePlayer(uint32_t playerId);
        PlayerProgressionData* GetPlayerProgression(uint32_t playerId);
        bool AddExperience(uint32_t playerId, uint32_t amount, bool isGroupExperience = false);
        bool LevelUpPlayer(uint32_t playerId);
        bool AddSkillPoint(uint32_t playerId, uint32_t amount);
        bool SpendSkillPoint(uint32_t playerId, uint32_t skillId);

        // Group progression
        uint32_t CreateGroup(const std::string& groupName);
        void AddPlayerToGroup(uint32_t groupId, uint32_t playerId);
        void RemovePlayerFromGroup(uint32_t groupId, uint32_t playerId);
        GroupProgressionData* GetGroupProgression(uint32_t groupId);
        bool AddGroupExperience(uint32_t groupId, uint32_t amount);
        bool DistributeGroupExperience(uint32_t groupId);

        // Skill management
        void AddSkill(const SkillData& skill);
        void RemoveSkill(uint32_t skillId);
        SkillData* GetSkill(uint32_t skillId);
        std::vector<SkillData> GetPlayerSkills(uint32_t playerId) const;
        std::vector<SkillData> GetSkillsByCategory(SkillCategory category) const;
        bool UnlockSkill(uint32_t playerId, uint32_t skillId);
        bool UpgradeSkill(uint32_t playerId, uint32_t skillId);

        // Achievement system
        void AddAchievement(const AchievementData& achievement);
        void RemoveAchievement(uint32_t achievementId);
        AchievementData* GetAchievement(uint32_t achievementId);
        std::vector<AchievementData> GetPlayerAchievements(uint32_t playerId) const;
        bool UnlockAchievement(uint32_t playerId, uint32_t achievementId);
        bool CheckAchievementRequirements(uint32_t playerId, uint32_t achievementId);

        // Progression queries
        uint32_t GetPlayerLevel(uint32_t playerId) const;
        uint32_t GetPlayerExperience(uint32_t playerId) const;
        uint32_t GetPlayerSkillPoints(uint32_t playerId) const;
        uint32_t GetGroupLevel(uint32_t groupId) const;
        uint32_t GetGroupExperience(uint32_t groupId) const;
        float GetPlayerProgress(uint32_t playerId) const;
        float GetGroupProgress(uint32_t groupId) const;

        // Configuration
        void SetExperienceMultiplier(float multiplier);
        void SetGroupExperienceBonus(float bonus);
        void SetMaxLevel(uint32_t maxLevel);
        void SetSkillPointPerLevel(uint32_t skillPoints);
        void EnableGroupProgression(bool enable);

        // Statistics
        ProgressionStats GetStats() const;
        void ResetStats();
        void PrintStats() const;

        // Callbacks
        using PlayerLeveledUpCallback = std::function<void(uint32_t, uint32_t)>;
        using SkillUnlockedCallback = std::function<void(uint32_t, uint32_t)>;
        using AchievementUnlockedCallback = std::function<void(uint32_t, uint32_t)>;
        using GroupLeveledUpCallback = std::function<void(uint32_t, uint32_t)>;
        
        void SetPlayerLeveledUpCallback(PlayerLeveledUpCallback callback);
        void SetSkillUnlockedCallback(SkillUnlockedCallback callback);
        void SetAchievementUnlockedCallback(AchievementUnlockedCallback callback);
        void SetGroupLeveledUpCallback(GroupLeveledUpCallback callback);

    private:
        // Internal methods
        void CalculateExperienceToNextLevel(PlayerProgressionData& player);
        void CalculateGroupLevel(GroupProgressionData& group);
        bool CheckSkillPrerequisites(uint32_t playerId, uint32_t skillId) const;
        void UpdatePlayerStats(uint32_t playerId);
        void UpdateGroupStats(uint32_t groupId);
        void CleanupInactiveGroups();
        
        // Experience calculation
        uint32_t CalculateExperienceRequired(uint32_t level) const;
        uint32_t CalculateGroupExperienceBonus(uint32_t groupSize) const;
        uint32_t CalculateSkillExperienceCost(uint32_t skillId, uint32_t currentLevel) const;
        
        // Validation
        bool IsPlayerValid(uint32_t playerId) const;
        bool IsGroupValid(uint32_t groupId) const;
        bool IsSkillValid(uint32_t skillId) const;
        bool IsAchievementValid(uint32_t achievementId) const;

        // Member variables
        bool m_initialized;
        std::map<uint32_t, PlayerProgressionData> m_playerProgressions;
        std::map<uint32_t, GroupProgressionData> m_groupProgressions;
        std::map<uint32_t, SkillData> m_skills;
        std::map<uint32_t, AchievementData> m_achievements;
        std::map<uint32_t, std::vector<uint32_t>> m_playerAchievements;
        
        // Configuration
        float m_experienceMultiplier;
        float m_groupExperienceBonus;
        uint32_t m_maxLevel;
        uint32_t m_skillPointsPerLevel;
        bool m_groupProgressionEnabled;
        
        // Statistics
        ProgressionStats m_stats;
        
        // Callbacks
        PlayerLeveledUpCallback m_playerLeveledUpCallback;
        SkillUnlockedCallback m_skillUnlockedCallback;
        AchievementUnlockedCallback m_achievementUnlockedCallback;
        GroupLeveledUpCallback m_groupLeveledUpCallback;
        
        // Timing
        std::chrono::high_resolution_clock::time_point m_lastUpdateTime;
        uint32_t m_nextPlayerId;
        uint32_t m_nextGroupId;
        uint32_t m_nextSkillId;
        uint32_t m_nextAchievementId;
    };

    // Progression utilities
    namespace ProgressionUtils
    {
        // Skill creation
        SkillData CreateCombatSkill(const std::string& name, const std::string& description, uint32_t maxLevel);
        SkillData CreateSignsSkill(const std::string& name, const std::string& description, uint32_t maxLevel);
        SkillData CreateAlchemySkill(const std::string& name, const std::string& description, uint32_t maxLevel);
        SkillData CreateGeneralSkill(const std::string& name, const std::string& description, uint32_t maxLevel);
        
        // Achievement creation
        AchievementData CreateCombatAchievement(const std::string& name, const std::string& description, uint32_t expReward);
        AchievementData CreateExplorationAchievement(const std::string& name, const std::string& description, uint32_t expReward);
        AchievementData CreateSocialAchievement(const std::string& name, const std::string& description, uint32_t expReward);
        
        // Experience calculation
        uint32_t CalculateLevelExperience(uint32_t level);
        uint32_t CalculateTotalExperience(uint32_t level);
        float CalculateProgressPercentage(uint32_t currentExp, uint32_t requiredExp);
        
        // Skill analysis
        float CalculateSkillPower(const SkillData& skill);
        std::vector<uint32_t> GetSkillDependencies(uint32_t skillId, const std::map<uint32_t, SkillData>& skills);
        bool CanUpgradeSkill(const SkillData& skill, uint32_t playerLevel);
        
        // Achievement analysis
        float CalculateAchievementDifficulty(const AchievementData& achievement);
        std::vector<std::string> GetAchievementCategories(const std::vector<AchievementData>& achievements);
        
        // Validation
        bool ValidatePlayerProgression(const PlayerProgressionData& player);
        bool ValidateGroupProgression(const GroupProgressionData& group);
        bool ValidateSkillData(const SkillData& skill);
        bool ValidateAchievementData(const AchievementData& achievement);
        
        // Statistics
        void PrintPlayerAnalysis(const PlayerProgressionData& player);
        void PrintGroupAnalysis(const GroupProgressionData& group);
        void PrintSkillAnalysis(const SkillData& skill);
        void PrintAchievementAnalysis(const AchievementData& achievement);
    }
}
