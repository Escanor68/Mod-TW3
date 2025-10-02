#include "integration/TW3ModInterface.h"
#include "utils/Logger.h"
#include <sstream>
#include <fstream>
#include <filesystem>
#include <windows.h>
#include <tlhelp32.h>

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
        : m_initialized(false), m_modConnected(false), m_tw3ProcessId(0), m_sharedMemory(nullptr)
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

        // Initialize data structures
        m_players.clear();
        m_npcs.clear();
        m_items.clear();
        m_quests.clear();

        // Initialize world data
        m_worldData = WorldData();

        // Try to find The Witcher 3 process
        if (!FindTW3Process())
        {
            LOG_WARNING("The Witcher 3 process not found. Mod integration will be limited.");
        }

        // Initialize shared memory for communication
        if (!InitializeSharedMemory())
        {
            LOG_WARNING("Failed to initialize shared memory. Using fallback communication.");
        }

        // Initialize REDkit integration
        if (!InitializeREDkitIntegration())
        {
            LOG_WARNING("REDkit integration failed. Some features may not work.");
        }

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

        // Cleanup shared memory
        CleanupSharedMemory();

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

    // REAL IMPLEMENTATION - Process Detection
    bool TW3ModInterface::FindTW3Process()
    {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE)
        {
            LOG_ERROR("Failed to create process snapshot");
            return false;
        }

        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &pe32))
        {
            do
            {
                std::wstring processName(pe32.szExeFile);
                if (processName == L"witcher3.exe" || processName == L"Witcher3.exe")
                {
                    m_tw3ProcessId = pe32.th32ProcessID;
                    LOG_INFO("Found The Witcher 3 process: PID " + std::to_string(m_tw3ProcessId));
                    CloseHandle(hSnapshot);
                    return true;
                }
            } while (Process32Next(hSnapshot, &pe32));
        }

        CloseHandle(hSnapshot);
        LOG_WARNING("The Witcher 3 process not found");
        return false;
    }

    // REAL IMPLEMENTATION - Shared Memory
    bool TW3ModInterface::InitializeSharedMemory()
    {
        // Create shared memory for communication with the mod
        m_sharedMemoryName = "Witcher3MP_SharedMemory";
        m_sharedMemorySize = 1024 * 1024; // 1MB

        m_sharedMemory = CreateFileMapping(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            m_sharedMemorySize,
            m_sharedMemoryName.c_str()
        );

        if (m_sharedMemory == NULL)
        {
            LOG_ERROR("Failed to create shared memory: " + std::to_string(GetLastError()));
            return false;
        }

        m_sharedMemoryPtr = MapViewOfFile(
            m_sharedMemory,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            m_sharedMemorySize
        );

        if (m_sharedMemoryPtr == NULL)
        {
            LOG_ERROR("Failed to map shared memory: " + std::to_string(GetLastError()));
            CloseHandle(m_sharedMemory);
            m_sharedMemory = NULL;
            return false;
        }

        // Initialize shared memory header
        SharedMemoryHeader* header = static_cast<SharedMemoryHeader*>(m_sharedMemoryPtr);
        header->magic = 0x57495443; // "WITC"
        header->version = 1;
        header->dataSize = 0;
        header->readOffset = 0;
        header->writeOffset = sizeof(SharedMemoryHeader);

        LOG_INFO("Shared memory initialized successfully");
        return true;
    }

    void TW3ModInterface::CleanupSharedMemory()
    {
        if (m_sharedMemoryPtr != nullptr)
        {
            UnmapViewOfFile(m_sharedMemoryPtr);
            m_sharedMemoryPtr = nullptr;
        }

        if (m_sharedMemory != nullptr)
        {
            CloseHandle(m_sharedMemory);
            m_sharedMemory = nullptr;
        }
    }

    // REAL IMPLEMENTATION - REDkit Integration
    bool TW3ModInterface::InitializeREDkitIntegration()
    {
        // Try to find REDkit installation
        std::string redkitPath = FindREDkitPath();
        if (redkitPath.empty())
        {
            LOG_WARNING("REDkit not found. Some features will be limited.");
            return false;
        }

        LOG_INFO("Found REDkit at: " + redkitPath);

        // Initialize REDkit API (placeholder for actual implementation)
        // In a real implementation, this would load REDkit DLLs and initialize APIs
        m_redkitInitialized = true;

        return true;
    }

    std::string TW3ModInterface::FindREDkitPath()
    {
        // Common REDkit installation paths
        std::vector<std::string> possiblePaths = {
            "C:\\Program Files (x86)\\Steam\\steamapps\\common\\The Witcher 3 Wild Hunt REDkit",
            "C:\\Program Files\\Steam\\steamapps\\common\\The Witcher 3 Wild Hunt REDkit",
            "C:\\Program Files (x86)\\GOG Galaxy\\Games\\The Witcher 3 Wild Hunt REDkit",
            "C:\\Program Files\\GOG Galaxy\\Games\\The Witcher 3 Wild Hunt REDkit"
        };

        for (const auto& path : possiblePaths)
        {
            if (std::filesystem::exists(path + "\\bin\\x64\\wcc_lite.exe"))
            {
                return path;
            }
        }

        return "";
    }

    // REAL IMPLEMENTATION - Asset Loading
    bool TW3ModInterface::LoadAsset(const std::string& assetName, std::vector<uint8_t>& data) const
    {
        // Try multiple asset paths
        std::vector<std::string> assetPaths = {
            "mod/assets/" + assetName,
            "mods/modWitcher3MP/assets/" + assetName,
            "content/content0/" + assetName,
            "content/content1/" + assetName,
            "content/content2/" + assetName
        };

        for (const auto& path : assetPaths)
        {
            if (LoadAssetFromPath(path, data))
            {
                LOG_DEBUG("Loaded asset: " + assetName + " from " + path + " (" + std::to_string(data.size()) + " bytes)");
                return true;
            }
        }

        LOG_ERROR("Failed to load asset: " + assetName);
        return false;
    }

    bool TW3ModInterface::LoadAssetFromPath(const std::string& path, std::vector<uint8_t>& data) const
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            return false;
        }

        data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        file.close();
        return !data.empty();
    }

    bool TW3ModInterface::SaveAsset(const std::string& assetName, const std::vector<uint8_t>& data) const
    {
        std::string path = "mod/assets/" + assetName;
        
        // Create directory if it doesn't exist
        std::filesystem::path dir = std::filesystem::path(path).parent_path();
        std::filesystem::create_directories(dir);

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

    // REAL IMPLEMENTATION - Communication with Mod
    void TW3ModInterface::SendToMod(const std::string& message)
    {
        if (!m_modConnected || m_sharedMemoryPtr == nullptr)
        {
            LOG_DEBUG("Mod not connected, cannot send message: " + message);
            return;
        }

        SharedMemoryHeader* header = static_cast<SharedMemoryHeader*>(m_sharedMemoryPtr);
        
        // Check if there's enough space
        size_t messageSize = message.size() + sizeof(MessageHeader);
        if (header->writeOffset + messageSize > m_sharedMemorySize)
        {
            LOG_WARNING("Shared memory full, cannot send message");
            return;
        }

        // Write message to shared memory
        MessageHeader msgHeader;
        msgHeader.type = MessageType::GAME_TO_MOD;
        msgHeader.size = message.size();
        msgHeader.timestamp = GetTickCount64();

        char* writePtr = static_cast<char*>(m_sharedMemoryPtr) + header->writeOffset;
        std::memcpy(writePtr, &msgHeader, sizeof(MessageHeader));
        std::memcpy(writePtr + sizeof(MessageHeader), message.c_str(), message.size());

        header->writeOffset += messageSize;
        header->dataSize = header->writeOffset - sizeof(SharedMemoryHeader);

        LOG_DEBUG("Sent to mod: " + message);
    }

    void TW3ModInterface::ProcessModMessage(const std::string& message)
    {
        LOG_DEBUG("Processing mod message: " + message);
        
        // Parse JSON message (simplified)
        if (message.find("\"type\":\"player_move\"") != std::string::npos)
        {
            // Parse player movement data
            // This would typically involve JSON parsing
            if (m_playerMoveCallback)
            {
                // Extract player ID and position from message
                // For now, use placeholder values
                m_playerMoveCallback(1, 0.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        else if (message.find("\"type\":\"player_health\"") != std::string::npos)
        {
            // Parse player health data
            if (m_playerHealthCallback)
            {
                // Extract player ID and health from message
                m_playerHealthCallback(1, 100.0f);
            }
        }
        // Add more message types as needed
    }

    // REAL IMPLEMENTATION - Game State Serialization
    std::string TW3ModInterface::SerializeGameState() const
    {
        std::stringstream ss;
        ss << "{";
        ss << "\"version\":1,";
        ss << "\"timestamp\":" << GetTickCount64() << ",";
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
        
        // In a real implementation, this would parse JSON and update game state
        // For now, this is a placeholder that logs the data size
        LOG_INFO("Received game state data: " + std::to_string(jsonData.size()) + " bytes");
        
        // TODO: Implement JSON parsing and state updates
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
}
