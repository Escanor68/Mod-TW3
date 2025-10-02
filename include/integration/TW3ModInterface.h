#pragma once

#include "Common.h"
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>

namespace TW3Integration
{
    // Forward declarations
    struct PlayerData;
    struct NPCData;
    struct ItemData;
    struct QuestData;
    struct WorldData;

    // Callback types for game events
    using PlayerMoveCallback = std::function<void(uint32_t playerId, float x, float y, float z, float w)>;
    using PlayerHealthCallback = std::function<void(uint32_t playerId, float health)>;
    using PlayerAttackCallback = std::function<void(uint32_t playerId, uint32_t targetId, float damage)>;
    using PlayerSignCastCallback = std::function<void(uint32_t playerId, uint32_t signType, uint32_t targetId)>;
    using InventoryChangeCallback = std::function<void(uint32_t playerId, uint32_t itemId, bool added)>;
    using QuestStateCallback = std::function<void(uint32_t questId, uint32_t newState)>;
    using WeatherChangeCallback = std::function<void(uint32_t weatherType, float intensity)>;
    using TimeChangeCallback = std::function<void(uint32_t hours, uint32_t minutes)>;

    // Data structures for game synchronization
    struct PlayerData
    {
        uint32_t id;
        float x, y, z, w;  // Position
        float health;
        float maxHealth;
        uint32_t characterId;
        std::string name;
        bool isConnected;
        
        PlayerData() : id(0), x(0), y(0), z(0), w(0), health(100), maxHealth(100), 
                      characterId(0), isConnected(false) {}
    };

    struct NPCData
    {
        uint32_t id;
        uint32_t resourceId;
        float x, y, z, w;  // Position
        float health;
        float maxHealth;
        std::string name;
        bool isAlive;
        
        NPCData() : id(0), resourceId(0), x(0), y(0), z(0), w(0), 
                   health(100), maxHealth(100), isAlive(true) {}
    };

    struct ItemData
    {
        uint32_t id;
        uint32_t itemType;
        float x, y, z, w;  // Position
        std::string name;
        float value;
        float weight;
        bool isActive;
        
        ItemData() : id(0), itemType(0), x(0), y(0), z(0), w(0), 
                    value(0), weight(0), isActive(true) {}
    };

    struct QuestData
    {
        uint32_t id;
        std::string name;
        std::string description;
        uint32_t state;
        bool isActive;
        
        QuestData() : id(0), state(0), isActive(false) {}
    };

    struct WorldData
    {
        uint32_t weatherType;
        float weatherIntensity;
        uint32_t timeHours;
        uint32_t timeMinutes;
        bool isDay;
        
        WorldData() : weatherType(0), weatherIntensity(0), timeHours(12), 
                     timeMinutes(0), isDay(true) {}
    };

    // Main interface class for TW3 mod integration
    class TW3ModInterface
    {
    public:
        static TW3ModInterface& GetInstance();
        static void DestroyInstance();

        // Initialization
        bool Initialize();
        void Shutdown();

        // Game data management
        void RegisterPlayer(uint32_t playerId, const PlayerData& data);
        void UnregisterPlayer(uint32_t playerId);
        void UpdatePlayerData(uint32_t playerId, const PlayerData& data);
        PlayerData GetPlayerData(uint32_t playerId) const;

        void RegisterNPC(uint32_t npcId, const NPCData& data);
        void UnregisterNPC(uint32_t npcId);
        void UpdateNPCData(uint32_t npcId, const NPCData& data);
        NPCData GetNPCData(uint32_t npcId) const;

        void RegisterItem(uint32_t itemId, const ItemData& data);
        void UnregisterItem(uint32_t itemId);
        void UpdateItemData(uint32_t itemId, const ItemData& data);
        ItemData GetItemData(uint32_t itemId) const;

        void RegisterQuest(uint32_t questId, const QuestData& data);
        void UnregisterQuest(uint32_t questId);
        void UpdateQuestData(uint32_t questId, const QuestData& data);
        QuestData GetQuestData(uint32_t questId) const;

        void UpdateWorldData(const WorldData& data);
        WorldData GetWorldData() const;

        // Event callbacks
        void SetPlayerMoveCallback(PlayerMoveCallback callback);
        void SetPlayerHealthCallback(PlayerHealthCallback callback);
        void SetPlayerAttackCallback(PlayerAttackCallback callback);
        void SetPlayerSignCastCallback(PlayerSignCastCallback callback);
        void SetInventoryChangeCallback(InventoryChangeCallback callback);
        void SetQuestStateCallback(QuestStateCallback callback);
        void SetWeatherChangeCallback(WeatherChangeCallback callback);
        void SetTimeChangeCallback(TimeChangeCallback callback);

        // Communication with mod
        void SendToMod(const std::string& message);
        void ProcessModMessage(const std::string& message);

        // Game state synchronization
        std::string SerializeGameState() const;
        void DeserializeGameState(const std::string& jsonData);

        // Asset management
        std::string GetAssetPath(const std::string& assetName) const;
        bool LoadAsset(const std::string& assetName, std::vector<uint8_t>& data) const;
        bool SaveAsset(const std::string& assetName, const std::vector<uint8_t>& data) const;

        // Utility functions
        bool IsModConnected() const;
        void SetModConnected(bool connected);
        std::vector<uint32_t> GetConnectedPlayers() const;
        size_t GetPlayerCount() const;

    private:
        TW3ModInterface();
        ~TW3ModInterface();
        TW3ModInterface(const TW3ModInterface&) = delete;
        TW3ModInterface& operator=(const TW3ModInterface&) = delete;

        // Internal data
        std::unordered_map<uint32_t, PlayerData> m_players;
        std::unordered_map<uint32_t, NPCData> m_npcs;
        std::unordered_map<uint32_t, ItemData> m_items;
        std::unordered_map<uint32_t, QuestData> m_quests;
        WorldData m_worldData;

        // Callbacks
        PlayerMoveCallback m_playerMoveCallback;
        PlayerHealthCallback m_playerHealthCallback;
        PlayerAttackCallback m_playerAttackCallback;
        PlayerSignCastCallback m_playerSignCastCallback;
        InventoryChangeCallback m_inventoryChangeCallback;
        QuestStateCallback m_questStateCallback;
        WeatherChangeCallback m_weatherChangeCallback;
        TimeChangeCallback m_timeChangeCallback;

        // State
        bool m_initialized;
        bool m_modConnected;
        static TW3ModInterface* s_instance;
    };

    // Helper functions for JSON serialization
    std::string PlayerDataToJSON(const PlayerData& data);
    std::string NPCDataToJSON(const NPCData& data);
    std::string ItemDataToJSON(const ItemData& data);
    std::string QuestDataToJSON(const QuestData& data);
    std::string WorldDataToJSON(const WorldData& data);

    PlayerData JSONToPlayerData(const std::string& json);
    NPCData JSONToNPCData(const std::string& json);
    ItemData JSONToItemData(const std::string& json);
    QuestData JSONToQuestData(const std::string& json);
    WorldData JSONToWorldData(const std::string& json);
}
