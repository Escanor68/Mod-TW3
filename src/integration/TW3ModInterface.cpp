#include "integration/TW3ModInterface.h"
#include "integration/REDkitBridge.h"
#include "networking/LANDiscovery.h"
#include "utils/Logger.h"
#include <sstream>
#include <fstream>
#include <filesystem>

namespace TW3Integration
{
    // Static member initialization
    TW3ModInterface* TW3ModInterface::s_instance = nullptr;

    TW3ModInterface& TW3ModInterface::GetInstance()
    {
        if (s_instance == nullptr)
        {
            s_instance = new TW3ModInterface();
        }
        return *s_instance;
    }

    void TW3ModInterface::DestroyInstance()
    {
        if (s_instance != nullptr)
        {
            delete s_instance;
            s_instance = nullptr;
        }
    }

    TW3ModInterface::TW3ModInterface()
        : m_initialized(false), m_modConnected(false)
        , m_lanDiscovery(nullptr), m_ipConnection(nullptr)
    {
        LOG_INFO("TW3ModInterface created");
    }

    TW3ModInterface::~TW3ModInterface()
    {
        Shutdown();
        LOG_INFO("TW3ModInterface destroyed");
    }

    bool TW3ModInterface::Initialize()
    {
        if (m_initialized)
        {
            return true;
        }

        LOG_INFO("Initializing TW3ModInterface...");

        // Initialize REDkit bridge
        auto& redkitBridge = REDkitBridge::GetInstance();
        if (!redkitBridge.Initialize())
        {
            LOG_ERROR("Failed to initialize REDkit bridge");
            return false;
        }

        // Set up callbacks for REDkit events
        redkitBridge.SetQuestCallback([this](uint32_t questId, uint32_t newState) {
            if (m_questStateCallback)
            {
                m_questStateCallback(questId, newState);
            }
        });

        redkitBridge.SetPlayerMoveCallback([this](uint32_t playerId, const Vector4F& position) {
            if (m_playerMoveCallback)
            {
                m_playerMoveCallback(playerId, position.x, position.y, position.z, position.w);
            }
        });

        redkitBridge.SetPlayerAttackCallback([this](uint32_t playerId, uint32_t targetId, float damage) {
            if (m_playerAttackCallback)
            {
                m_playerAttackCallback(playerId, targetId, damage);
            }
        });

        redkitBridge.SetSignCastCallback([this](uint32_t playerId, uint32_t signType, const Vector4F& position) {
            if (m_playerSignCastCallback)
            {
                m_playerSignCastCallback(playerId, signType, position.x, position.y, position.z, position.w);
            }
        });

        redkitBridge.SetInventoryCallback([this](uint32_t playerId, uint32_t itemId, bool added) {
            if (m_inventoryChangeCallback)
            {
                m_inventoryChangeCallback(playerId, itemId, added);
            }
        });

        // Initialize data structures
        m_players.clear();
        m_npcs.clear();
        m_items.clear();
        m_quests.clear();

        // Initialize world data
        m_worldData = WorldData();

        m_initialized = true;
        LOG_INFO("TW3ModInterface initialized successfully");
        return true;
    }

    void TW3ModInterface::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        LOG_INFO("Shutting down TW3ModInterface...");

        // Clear all data
        m_players.clear();
        m_npcs.clear();
        m_items.clear();
        m_quests.clear();

        // Clear callbacks
        m_playerMoveCallback = nullptr;
        m_playerHealthCallback = nullptr;
        m_playerAttackCallback = nullptr;
        m_playerSignCastCallback = nullptr;
        m_inventoryChangeCallback = nullptr;
        m_questStateCallback = nullptr;
        m_weatherChangeCallback = nullptr;
        m_timeChangeCallback = nullptr;

        m_initialized = false;
        m_modConnected = false;

        LOG_INFO("TW3ModInterface shutdown complete");
    }

    // Player management
    void TW3ModInterface::RegisterPlayer(uint32_t playerId, const PlayerData& data)
    {
        m_players[playerId] = data;
        LOG_INFO("Player registered: ID=" + std::to_string(playerId) + ", Name=" + data.name);
    }

    void TW3ModInterface::UnregisterPlayer(uint32_t playerId)
    {
        auto it = m_players.find(playerId);
        if (it != m_players.end())
        {
            LOG_INFO("Player unregistered: ID=" + std::to_string(playerId));
            m_players.erase(it);
        }
    }

    void TW3ModInterface::UpdatePlayerData(uint32_t playerId, const PlayerData& data)
    {
        auto it = m_players.find(playerId);
        if (it != m_players.end())
        {
            it->second = data;
            
            // Trigger callbacks
            if (m_playerMoveCallback)
            {
                m_playerMoveCallback(playerId, data.x, data.y, data.z, data.w);
            }
            if (m_playerHealthCallback)
            {
                m_playerHealthCallback(playerId, data.health);
            }
        }
    }

    PlayerData TW3ModInterface::GetPlayerData(uint32_t playerId) const
    {
        auto it = m_players.find(playerId);
        if (it != m_players.end())
        {
            return it->second;
        }
        return PlayerData();
    }

    // NPC management
    void TW3ModInterface::RegisterNPC(uint32_t npcId, const NPCData& data)
    {
        m_npcs[npcId] = data;
        LOG_INFO("NPC registered: ID=" + std::to_string(npcId) + ", Name=" + data.name);
    }

    void TW3ModInterface::UnregisterNPC(uint32_t npcId)
    {
        auto it = m_npcs.find(npcId);
        if (it != m_npcs.end())
        {
            LOG_INFO("NPC unregistered: ID=" + std::to_string(npcId));
            m_npcs.erase(it);
        }
    }

    void TW3ModInterface::UpdateNPCData(uint32_t npcId, const NPCData& data)
    {
        auto it = m_npcs.find(npcId);
        if (it != m_npcs.end())
        {
            it->second = data;
        }
    }

    NPCData TW3ModInterface::GetNPCData(uint32_t npcId) const
    {
        auto it = m_npcs.find(npcId);
        if (it != m_npcs.end())
        {
            return it->second;
        }
        return NPCData();
    }

    // Item management
    void TW3ModInterface::RegisterItem(uint32_t itemId, const ItemData& data)
    {
        m_items[itemId] = data;
        LOG_INFO("Item registered: ID=" + std::to_string(itemId) + ", Name=" + data.name);
    }

    void TW3ModInterface::UnregisterItem(uint32_t itemId)
    {
        auto it = m_items.find(itemId);
        if (it != m_items.end())
        {
            LOG_INFO("Item unregistered: ID=" + std::to_string(itemId));
            m_items.erase(it);
        }
    }

    void TW3ModInterface::UpdateItemData(uint32_t itemId, const ItemData& data)
    {
        auto it = m_items.find(itemId);
        if (it != m_items.end())
        {
            it->second = data;
        }
    }

    ItemData TW3ModInterface::GetItemData(uint32_t itemId) const
    {
        auto it = m_items.find(itemId);
        if (it != m_items.end())
        {
            return it->second;
        }
        return ItemData();
    }

    // Quest management
    void TW3ModInterface::RegisterQuest(uint32_t questId, const QuestData& data)
    {
        m_quests[questId] = data;
        LOG_INFO("Quest registered: ID=" + std::to_string(questId) + ", Name=" + data.name);
    }

    void TW3ModInterface::UnregisterQuest(uint32_t questId)
    {
        auto it = m_quests.find(questId);
        if (it != m_quests.end())
        {
            LOG_INFO("Quest unregistered: ID=" + std::to_string(questId));
            m_quests.erase(it);
        }
    }

    void TW3ModInterface::UpdateQuestData(uint32_t questId, const QuestData& data)
    {
        auto it = m_quests.find(questId);
        if (it != m_quests.end())
        {
            it->second = data;
            
            // Trigger callback
            if (m_questStateCallback)
            {
                m_questStateCallback(questId, data.state);
            }
        }
    }

    QuestData TW3ModInterface::GetQuestData(uint32_t questId) const
    {
        auto it = m_quests.find(questId);
        if (it != m_quests.end())
        {
            return it->second;
        }
        return QuestData();
    }

    // World data management
    void TW3ModInterface::UpdateWorldData(const WorldData& data)
    {
        m_worldData = data;
        
        // Trigger callbacks
        if (m_weatherChangeCallback)
        {
            m_weatherChangeCallback(data.weatherType, data.weatherIntensity);
        }
        if (m_timeChangeCallback)
        {
            m_timeChangeCallback(data.timeHours, data.timeMinutes);
        }
    }

    WorldData TW3ModInterface::GetWorldData() const
    {
        return m_worldData;
    }

    // Callback setters
    void TW3ModInterface::SetPlayerMoveCallback(PlayerMoveCallback callback)
    {
        m_playerMoveCallback = callback;
    }

    void TW3ModInterface::SetPlayerHealthCallback(PlayerHealthCallback callback)
    {
        m_playerHealthCallback = callback;
    }

    void TW3ModInterface::SetPlayerAttackCallback(PlayerAttackCallback callback)
    {
        m_playerAttackCallback = callback;
    }

    void TW3ModInterface::SetPlayerSignCastCallback(PlayerSignCastCallback callback)
    {
        m_playerSignCastCallback = callback;
    }

    void TW3ModInterface::SetInventoryChangeCallback(InventoryChangeCallback callback)
    {
        m_inventoryChangeCallback = callback;
    }

    void TW3ModInterface::SetQuestStateCallback(QuestStateCallback callback)
    {
        m_questStateCallback = callback;
    }

    void TW3ModInterface::SetWeatherChangeCallback(WeatherChangeCallback callback)
    {
        m_weatherChangeCallback = callback;
    }

    void TW3ModInterface::SetTimeChangeCallback(TimeChangeCallback callback)
    {
        m_timeChangeCallback = callback;
    }

    // Communication with mod
    void TW3ModInterface::SendToMod(const std::string& message)
    {
        if (m_modConnected)
        {
            LOG_DEBUG("Sending to mod: " + message);
            auto& redkitBridge = REDkitBridge::GetInstance();
            redkitBridge.SendToREDkit(message);
        }
    }

    void TW3ModInterface::ProcessModMessage(const std::string& message)
    {
        LOG_DEBUG("Processing mod message: " + message);
        auto& redkitBridge = REDkitBridge::GetInstance();
        redkitBridge.ProcessFromREDkit(message);
    }

    // Game state synchronization
    std::string TW3ModInterface::SerializeGameState() const
    {
        // Use REDkit bridge to get game state from scripts
        auto& redkitBridge = REDkitBridge::GetInstance();
        if (redkitBridge.IsInitialized())
        {
            std::string gameState = redkitBridge.ExportGameData();
            if (!gameState.empty())
            {
                return gameState;
            }
        }

        // Fallback to local serialization
        std::stringstream ss;
        ss << "{";
        ss << "\"players\":[";
        
        bool first = true;
        for (const auto& pair : m_players)
        {
            if (!first) ss << ",";
            ss << PlayerDataToJSON(pair.second);
            first = false;
        }
        
        ss << "],\"npcs\":[";
        first = true;
        for (const auto& pair : m_npcs)
        {
            if (!first) ss << ",";
            ss << NPCDataToJSON(pair.second);
            first = false;
        }
        
        ss << "],\"items\":[";
        first = true;
        for (const auto& pair : m_items)
        {
            if (!first) ss << ",";
            ss << ItemDataToJSON(pair.second);
            first = false;
        }
        
        ss << "],\"world\":" << WorldDataToJSON(m_worldData);
        ss << "}";
        
        return ss.str();
    }

    void TW3ModInterface::DeserializeGameState(const std::string& jsonData)
    {
        LOG_DEBUG("Deserializing game state from JSON");
        auto& redkitBridge = REDkitBridge::GetInstance();
        if (redkitBridge.IsInitialized())
        {
            redkitBridge.ImportGameData(jsonData);
        }
    }

    // Asset management
    std::string TW3ModInterface::GetAssetPath(const std::string& assetName) const
    {
        // Return path to asset in mod directory
        return "mod/assets/" + assetName;
    }

    bool TW3ModInterface::LoadAsset(const std::string& assetName, std::vector<uint8_t>& data) const
    {
        // Try to load from REDkit first
        auto& redkitBridge = REDkitBridge::GetInstance();
        if (redkitBridge.IsInitialized())
        {
            // Try different asset types
            if (redkitBridge.LoadWeaponData(assetName, data) ||
                redkitBridge.LoadArmorData(assetName, data) ||
                redkitBridge.LoadItemData(assetName, data) ||
                redkitBridge.LoadNPCData(assetName, data) ||
                redkitBridge.LoadQuestData(assetName, data))
            {
                LOG_INFO("Loaded asset from REDkit: " + assetName + " (" + std::to_string(data.size()) + " bytes)");
                return true;
            }
        }

        // Fallback to file system
        std::string path = GetAssetPath(assetName);
        std::ifstream file(path, std::ios::binary);
        
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open asset: " + path);
            return false;
        }
        
        data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        file.close();
        
        LOG_DEBUG("Loaded asset from file system: " + assetName + " (" + std::to_string(data.size()) + " bytes)");
        return true;
    }

    bool TW3ModInterface::SaveAsset(const std::string& assetName, const std::vector<uint8_t>& data) const
    {
        std::string path = GetAssetPath(assetName);
        std::ofstream file(path, std::ios::binary);
        
        if (!file.is_open())
        {
            LOG_ERROR("Failed to create asset: " + path);
            return false;
        }
        
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        file.close();
        
        LOG_DEBUG("Saved asset: " + assetName + " (" + std::to_string(data.size()) + " bytes)");
        return true;
    }

    // Utility functions
    bool TW3ModInterface::IsModConnected() const
    {
        return m_modConnected;
    }

    void TW3ModInterface::SetModConnected(bool connected)
    {
        m_modConnected = connected;
        LOG_INFO("Mod connection status: " + std::string(connected ? "Connected" : "Disconnected"));
    }

    std::vector<uint32_t> TW3ModInterface::GetConnectedPlayers() const
    {
        std::vector<uint32_t> players;
        for (const auto& pair : m_players)
        {
            if (pair.second.isConnected)
            {
                players.push_back(pair.first);
            }
        }
        return players;
    }

    size_t TW3ModInterface::GetPlayerCount() const
    {
        return m_players.size();
    }

    // JSON serialization helpers
    std::string PlayerDataToJSON(const PlayerData& data)
    {
        std::stringstream ss;
        ss << "{\"id\":" << data.id;
        ss << ",\"x\":" << data.x;
        ss << ",\"y\":" << data.y;
        ss << ",\"z\":" << data.z;
        ss << ",\"w\":" << data.w;
        ss << ",\"health\":" << data.health;
        ss << ",\"maxHealth\":" << data.maxHealth;
        ss << ",\"characterId\":" << data.characterId;
        ss << ",\"name\":\"" << data.name << "\"";
        ss << ",\"isConnected\":" << (data.isConnected ? "true" : "false");
        ss << "}";
        return ss.str();
    }

    std::string NPCDataToJSON(const NPCData& data)
    {
        std::stringstream ss;
        ss << "{\"id\":" << data.id;
        ss << ",\"resourceId\":" << data.resourceId;
        ss << ",\"x\":" << data.x;
        ss << ",\"y\":" << data.y;
        ss << ",\"z\":" << data.z;
        ss << ",\"w\":" << data.w;
        ss << ",\"health\":" << data.health;
        ss << ",\"maxHealth\":" << data.maxHealth;
        ss << ",\"name\":\"" << data.name << "\"";
        ss << ",\"isAlive\":" << (data.isAlive ? "true" : "false");
        ss << "}";
        return ss.str();
    }

    std::string ItemDataToJSON(const ItemData& data)
    {
        std::stringstream ss;
        ss << "{\"id\":" << data.id;
        ss << ",\"itemType\":" << data.itemType;
        ss << ",\"x\":" << data.x;
        ss << ",\"y\":" << data.y;
        ss << ",\"z\":" << data.z;
        ss << ",\"w\":" << data.w;
        ss << ",\"name\":\"" << data.name << "\"";
        ss << ",\"value\":" << data.value;
        ss << ",\"weight\":" << data.weight;
        ss << ",\"isActive\":" << (data.isActive ? "true" : "false");
        ss << "}";
        return ss.str();
    }

    std::string QuestDataToJSON(const QuestData& data)
    {
        std::stringstream ss;
        ss << "{\"id\":" << data.id;
        ss << ",\"name\":\"" << data.name << "\"";
        ss << ",\"description\":\"" << data.description << "\"";
        ss << ",\"state\":" << data.state;
        ss << ",\"isActive\":" << (data.isActive ? "true" : "false");
        ss << "}";
        return ss.str();
    }

    std::string WorldDataToJSON(const WorldData& data)
    {
        std::stringstream ss;
        ss << "{\"weatherType\":" << data.weatherType;
        ss << ",\"weatherIntensity\":" << data.weatherIntensity;
        ss << ",\"timeHours\":" << data.timeHours;
        ss << ",\"timeMinutes\":" << data.timeMinutes;
        ss << ",\"isDay\":" << (data.isDay ? "true" : "false");
        ss << "}";
        return ss.str();
    }

    // JSON deserialization helpers (placeholder implementations)
    PlayerData JSONToPlayerData(const std::string& json)
    {
        PlayerData data;
        // In a real implementation, this would parse JSON
        return data;
    }

    NPCData JSONToNPCData(const std::string& json)
    {
        NPCData data;
        // In a real implementation, this would parse JSON
        return data;
    }

    ItemData JSONToItemData(const std::string& json)
    {
        ItemData data;
        // In a real implementation, this would parse JSON
        return data;
    }

    QuestData JSONToQuestData(const std::string& json)
    {
        QuestData data;
        // In a real implementation, this would parse JSON
        return data;
    }

    WorldData JSONToWorldData(const std::string& json)
    {
        WorldData data;
        // In a real implementation, this would parse JSON
        return data;
    }

    // LAN Discovery functions
    void TW3ModInterface::InitializeLANDiscovery()
    {
        if (!m_lanDiscovery)
        {
            m_lanDiscovery = new TW3Networking::LANDiscovery();
            if (m_lanDiscovery->Initialize())
            {
                // Set up callbacks
                m_lanDiscovery->SetServerDiscoveredCallback([this](const TW3Networking::ServerInfo& server) {
                    LOG_INFO("Server discovered: " + server.name + " at " + server.ip + ":" + std::to_string(server.port));
                });
                
                m_lanDiscovery->SetDiscoveryFinishedCallback([this]() {
                    LOG_INFO("LAN discovery finished, found " + std::to_string(m_lanDiscovery->GetServerCount()) + " servers");
                });
                
                LOG_INFO("LAN Discovery system initialized");
            }
            else
            {
                LOG_ERROR("Failed to initialize LAN Discovery system");
                delete m_lanDiscovery;
                m_lanDiscovery = nullptr;
            }
        }
    }

    void TW3ModInterface::StartLANDiscovery()
    {
        if (m_lanDiscovery)
        {
            m_lanDiscovery->StartDiscovery();
        }
        else
        {
            LOG_WARNING("LAN Discovery not initialized");
        }
    }

    void TW3ModInterface::StopLANDiscovery()
    {
        if (m_lanDiscovery)
        {
            m_lanDiscovery->StopDiscovery();
        }
    }

    std::vector<TW3Networking::ServerInfo> TW3ModInterface::GetDiscoveredServers() const
    {
        if (m_lanDiscovery)
        {
            return m_lanDiscovery->GetDiscoveredServers();
        }
        return std::vector<TW3Networking::ServerInfo>();
    }

    // IP Connection functions
    void TW3ModInterface::InitializeIPConnection()
    {
        if (!m_ipConnection)
        {
            m_ipConnection = new TW3Networking::IPConnection();
            if (m_ipConnection->Initialize())
            {
                // Set up callbacks
                m_ipConnection->SetConnectionCallback([this](bool success, const std::string& message) {
                    if (success)
                    {
                        LOG_INFO("Connected to server: " + message);
                        SetModConnected(true);
                    }
                    else
                    {
                        LOG_ERROR("Failed to connect to server: " + message);
                        SetModConnected(false);
                    }
                });
                
                m_ipConnection->SetDataReceivedCallback([this](const std::string& data) {
                    ProcessModMessage(data);
                });
                
                LOG_INFO("IP Connection system initialized");
            }
            else
            {
                LOG_ERROR("Failed to initialize IP Connection system");
                delete m_ipConnection;
                m_ipConnection = nullptr;
            }
        }
    }

    bool TW3ModInterface::ConnectToServer(const std::string& ip, uint16_t port, const std::string& username)
    {
        if (!m_ipConnection)
        {
            InitializeIPConnection();
        }
        
        if (m_ipConnection)
        {
            return m_ipConnection->ConnectToServer(ip, port, username);
        }
        
        return false;
    }

    void TW3ModInterface::DisconnectFromServer()
    {
        if (m_ipConnection)
        {
            m_ipConnection->Disconnect();
            SetModConnected(false);
        }
    }

    bool TW3ModInterface::IsConnectedToServer() const
    {
        return m_ipConnection && m_ipConnection->IsConnected();
    }

    void TW3ModInterface::SendToServer(const std::string& message)
    {
        if (m_ipConnection && m_ipConnection->IsConnected())
        {
            m_ipConnection->SendData(message);
        }
        else
        {
            LOG_WARNING("Not connected to server, cannot send message");
        }
    }
}
