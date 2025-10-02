#pragma once

#include "Common.h"
#include "game/CooperativeQuests.h"
#include "game/SyncedMonsterAI.h"
#include "game/GlobalEconomy.h"
#include "game/SharedProgression.h"
#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <functional>

namespace Game
{
    // Save data types
    enum class SaveDataType
    {
        Player = 0,
        Quest = 1,
        Economy = 2,
        Progression = 3,
        Monster = 4,
        Group = 5,
        World = 6
    };

    // Save data structure
    struct SaveData
    {
        uint32_t saveId;
        std::string saveName;
        SaveDataType type;
        std::vector<uint8_t> data;
        std::chrono::high_resolution_clock::time_point timestamp;
        uint32_t version;
        bool isCompressed;
        bool isEncrypted;
        uint32_t checksum;
        
        SaveData() : saveId(0), version(1), isCompressed(false), isEncrypted(false), checksum(0) {}
    };

    // Player save data
    struct PlayerSaveData
    {
        uint32_t playerId;
        std::string playerName;
        Vector4F position;
        float rotation;
        uint32_t level;
        uint32_t experience;
        std::map<uint32_t, uint32_t> inventory;
        std::map<CurrencyType, uint32_t> currencies;
        std::vector<uint32_t> activeQuests;
        std::vector<uint32_t> completedQuests;
        std::vector<uint32_t> unlockedSkills;
        std::vector<uint32_t> achievements;
        std::chrono::high_resolution_clock::time_point lastSave;
        
        PlayerSaveData() : playerId(0), level(1), experience(0), rotation(0.0f) {}
    };

    // World save data
    struct WorldSaveData
    {
        uint32_t worldId;
        std::string worldName;
        std::map<uint32_t, MonsterAIData> monsters;
        std::map<uint32_t, MerchantData> merchants;
        std::vector<QuestData> quests;
        std::map<std::string, bool> worldStates;
        std::chrono::high_resolution_clock::time_point lastUpdate;
        
        WorldSaveData() : worldId(0) {}
    };

    // Save metadata
    struct SaveMetadata
    {
        uint32_t saveId;
        std::string saveName;
        std::string description;
        uint32_t version;
        std::chrono::high_resolution_clock::time_point createdTime;
        std::chrono::high_resolution_clock::time_point lastModified;
        uint32_t fileSize;
        std::string checksum;
        bool isCorrupted;
        bool isBackup;
        
        SaveMetadata() : saveId(0), version(1), fileSize(0), isCorrupted(false), isBackup(false) {}
    };

    // Save statistics
    struct SaveStats
    {
        uint32_t totalSaves = 0;
        uint32_t corruptedSaves = 0;
        uint32_t totalBackups = 0;
        uint32_t totalSize = 0;
        float averageSaveTime = 0.0f;
        float compressionRatio = 0.0f;
        uint32_t lastSaveId = 0;
        
        void Reset()
        {
            totalSaves = 0;
            corruptedSaves = 0;
            totalBackups = 0;
            totalSize = 0;
            averageSaveTime = 0.0f;
            compressionRatio = 0.0f;
            lastSaveId = 0;
        }
    };

    // Shared save system
    class SharedSaveSystem
    {
    public:
        SharedSaveSystem();
        ~SharedSaveSystem();

        // Initialize system
        bool Initialize(const std::string& saveDirectory);
        void Shutdown();

        // Save operations
        bool SavePlayerData(uint32_t playerId, const PlayerSaveData& playerData);
        bool SaveWorldData(uint32_t worldId, const WorldSaveData& worldData);
        bool SaveQuestData(const std::vector<QuestData>& quests);
        bool SaveEconomyData(const std::map<uint32_t, PlayerEconomyData>& playerEconomies);
        bool SaveProgressionData(const std::map<uint32_t, PlayerProgressionData>& playerProgressions);
        bool SaveMonsterData(const std::map<uint32_t, MonsterAIData>& monsters);
        bool SaveGroupData(const std::map<uint32_t, QuestGroup>& groups);

        // Load operations
        bool LoadPlayerData(uint32_t playerId, PlayerSaveData& playerData);
        bool LoadWorldData(uint32_t worldId, WorldSaveData& worldData);
        bool LoadQuestData(std::vector<QuestData>& quests);
        bool LoadEconomyData(std::map<uint32_t, PlayerEconomyData>& playerEconomies);
        bool LoadProgressionData(std::map<uint32_t, PlayerProgressionData>& playerProgressions);
        bool LoadMonsterData(std::map<uint32_t, MonsterAIData>& monsters);
        bool LoadGroupData(std::map<uint32_t, QuestGroup>& groups);

        // Save management
        bool CreateSave(const std::string& saveName, const std::string& description = "");
        bool LoadSave(uint32_t saveId);
        bool DeleteSave(uint32_t saveId);
        bool RenameSave(uint32_t saveId, const std::string& newName);
        std::vector<SaveMetadata> GetAvailableSaves() const;
        SaveMetadata GetSaveMetadata(uint32_t saveId) const;

        // Backup operations
        bool CreateBackup(uint32_t saveId);
        bool RestoreFromBackup(uint32_t saveId, uint32_t backupId);
        std::vector<SaveMetadata> GetBackups(uint32_t saveId) const;
        bool DeleteBackup(uint32_t saveId, uint32_t backupId);

        // Data validation
        bool ValidateSaveData(uint32_t saveId);
        bool RepairSaveData(uint32_t saveId);
        bool IsSaveCorrupted(uint32_t saveId) const;
        std::vector<uint32_t> GetCorruptedSaves() const;

        // Compression and encryption
        bool CompressSaveData(uint32_t saveId);
        bool DecompressSaveData(uint32_t saveId);
        bool EncryptSaveData(uint32_t saveId, const std::string& password);
        bool DecryptSaveData(uint32_t saveId, const std::string& password);

        // Import/Export
        bool ExportSave(uint32_t saveId, const std::string& filePath);
        bool ImportSave(const std::string& filePath);
        bool ExportPlayerData(uint32_t playerId, const std::string& filePath);
        bool ImportPlayerData(uint32_t playerId, const std::string& filePath);

        // Configuration
        void SetSaveDirectory(const std::string& directory);
        void SetCompressionEnabled(bool enabled);
        void SetEncryptionEnabled(bool enabled);
        void SetMaxBackups(uint32_t maxBackups);
        void SetAutoSaveInterval(float interval);

        // Statistics
        SaveStats GetStats() const;
        void ResetStats();
        void PrintStats() const;

        // Callbacks
        using SaveCompletedCallback = std::function<void(uint32_t, bool)>;
        using LoadCompletedCallback = std::function<void(uint32_t, bool)>;
        using SaveCorruptedCallback = std::function<void(uint32_t)>;
        using BackupCreatedCallback = std::function<void(uint32_t, uint32_t)>;
        
        void SetSaveCompletedCallback(SaveCompletedCallback callback);
        void SetLoadCompletedCallback(LoadCompletedCallback callback);
        void SetSaveCorruptedCallback(SaveCorruptedCallback callback);
        void SetBackupCreatedCallback(BackupCreatedCallback callback);

    private:
        // Internal methods
        bool SaveDataToFile(const SaveData& saveData, const std::string& filePath);
        bool LoadDataFromFile(SaveData& saveData, const std::string& filePath);
        std::string GetSaveFilePath(uint32_t saveId) const;
        std::string GetBackupFilePath(uint32_t saveId, uint32_t backupId) const;
        std::string GetMetadataFilePath() const;
        
        // Data serialization
        std::vector<uint8_t> SerializePlayerData(const PlayerSaveData& playerData);
        std::vector<uint8_t> SerializeWorldData(const WorldSaveData& worldData);
        std::vector<uint8_t> SerializeQuestData(const std::vector<QuestData>& quests);
        std::vector<uint8_t> SerializeEconomyData(const std::map<uint32_t, PlayerEconomyData>& playerEconomies);
        std::vector<uint8_t> SerializeProgressionData(const std::map<uint32_t, PlayerProgressionData>& playerProgressions);
        std::vector<uint8_t> SerializeMonsterData(const std::map<uint32_t, MonsterAIData>& monsters);
        std::vector<uint8_t> SerializeGroupData(const std::map<uint32_t, QuestGroup>& groups);
        
        // Data deserialization
        bool DeserializePlayerData(const std::vector<uint8_t>& data, PlayerSaveData& playerData);
        bool DeserializeWorldData(const std::vector<uint8_t>& data, WorldSaveData& worldData);
        bool DeserializeQuestData(const std::vector<uint8_t>& data, std::vector<QuestData>& quests);
        bool DeserializeEconomyData(const std::vector<uint8_t>& data, std::map<uint32_t, PlayerEconomyData>& playerEconomies);
        bool DeserializeProgressionData(const std::vector<uint8_t>& data, std::map<uint32_t, PlayerProgressionData>& playerProgressions);
        bool DeserializeMonsterData(const std::vector<uint8_t>& data, std::map<uint32_t, MonsterAIData>& monsters);
        bool DeserializeGroupData(const std::vector<uint8_t>& data, std::map<uint32_t, QuestGroup>& groups);
        
        // Validation and repair
        uint32_t CalculateChecksum(const std::vector<uint8_t>& data) const;
        bool VerifyChecksum(const SaveData& saveData) const;
        bool RepairCorruptedData(uint32_t saveId);
        
        // File operations
        bool CreateDirectory(const std::string& path);
        bool FileExists(const std::string& filePath) const;
        bool DeleteFile(const std::string& filePath);
        std::vector<std::string> GetFilesInDirectory(const std::string& directory) const;
        
        // Metadata management
        void LoadMetadata();
        void SaveMetadata();
        void UpdateSaveMetadata(uint32_t saveId, const SaveMetadata& metadata);

        // Member variables
        bool m_initialized;
        std::string m_saveDirectory;
        std::map<uint32_t, SaveMetadata> m_saveMetadata;
        std::map<uint32_t, SaveData> m_cachedSaves;
        
        // Configuration
        bool m_compressionEnabled;
        bool m_encryptionEnabled;
        uint32_t m_maxBackups;
        float m_autoSaveInterval;
        
        // Statistics
        SaveStats m_stats;
        
        // Callbacks
        SaveCompletedCallback m_saveCompletedCallback;
        LoadCompletedCallback m_loadCompletedCallback;
        SaveCorruptedCallback m_saveCorruptedCallback;
        BackupCreatedCallback m_backupCreatedCallback;
        
        // Timing
        std::chrono::high_resolution_clock::time_point m_lastAutoSave;
        uint32_t m_nextSaveId;
    };

    // Save utilities
    namespace SaveUtils
    {
        // Save creation helpers
        SaveData CreatePlayerSave(uint32_t playerId, const PlayerSaveData& playerData);
        SaveData CreateWorldSave(uint32_t worldId, const WorldSaveData& worldData);
        SaveData CreateQuestSave(const std::vector<QuestData>& quests);
        
        // Data validation
        bool ValidatePlayerSaveData(const PlayerSaveData& playerData);
        bool ValidateWorldSaveData(const WorldSaveData& worldData);
        bool ValidateSaveMetadata(const SaveMetadata& metadata);
        
        // File operations
        bool CreateSaveDirectory(const std::string& path);
        bool CleanupOldSaves(const std::string& directory, uint32_t maxAge);
        bool CompressFile(const std::string& inputPath, const std::string& outputPath);
        bool DecompressFile(const std::string& inputPath, const std::string& outputPath);
        
        // Data conversion
        std::string DataToBase64(const std::vector<uint8_t>& data);
        std::vector<uint8_t> Base64ToData(const std::string& base64);
        std::string DataToHex(const std::vector<uint8_t>& data);
        std::vector<uint8_t> HexToData(const std::string& hex);
        
        // Statistics
        void PrintSaveAnalysis(const SaveData& saveData);
        void PrintSaveMetadataAnalysis(const SaveMetadata& metadata);
    }
}
