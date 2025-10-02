#pragma once
#include "Common.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>

namespace TW3Integration
{
    // Forward declarations
    struct Vector4F;
    struct PlayerData;
    struct NPCData;
    struct ItemData;
    struct QuestData;

    // WitcherScript function result
    struct WitcherScriptResult
    {
        bool success;
        std::string data;
        std::string error;
        uint32_t timestamp;
    };

    // WitcherScript event callback
    using WitcherScriptEventCallback = std::function<void(const std::string& eventName, const std::string& eventData)>;

    // Main WitcherScript Bridge class
    class WitcherScriptBridge
    {
    public:
        // Singleton pattern
        static WitcherScriptBridge& GetInstance();
        static void DestroyInstance();

        // Constructor/Destructor
        WitcherScriptBridge();
        ~WitcherScriptBridge();

        // Initialization
        bool Initialize();
        void Shutdown();

        // WitcherScript Engine Management
        bool InitializeWitcherScriptEngine();
        bool LoadWitcherScript(const std::string& scriptPath);
        bool UnloadWitcherScript(const std::string& scriptName);
        bool IsWitcherScriptLoaded(const std::string& scriptName) const;

        // C++ → WitcherScript Communication
        WitcherScriptResult CallWitcherScriptFunction(const std::string& functionName, const std::string& parameters);
        WitcherScriptResult CallWitcherScriptFunction(const std::string& className, const std::string& functionName, const std::string& parameters);
        std::string GetWitcherScriptResult(const std::string& functionName, const std::string& parameters);
        bool ExecuteWitcherScriptCode(const std::string& code);

        // WitcherScript → C++ Communication (Event System)
        void RegisterCppCallback(const std::string& eventName, WitcherScriptEventCallback callback);
        void UnregisterCppCallback(const std::string& eventName);
        void ProcessWitcherScriptEvent(const std::string& eventName, const std::string& eventData);
        void NotifyWitcherScriptEvent(const std::string& eventName, const std::string& eventData);

        // Asset Management through WitcherScript
        bool LoadAssetFromWitcherScript(const std::string& assetName, std::vector<uint8_t>& data);
        bool SaveAssetToWitcherScript(const std::string& assetName, const std::vector<uint8_t>& data);
        bool AssetExistsInWitcherScript(const std::string& assetName) const;
        std::vector<std::string> ListAssetsInWitcherScript(const std::string& category) const;

        // Game System Integration
        bool ExecuteCombatAction(uint32_t playerId, const std::string& action, const std::string& parameters);
        bool CastSign(uint32_t playerId, const std::string& signType, const std::string& parameters);
        bool UseAlchemyItem(uint32_t playerId, const std::string& itemType, const std::string& parameters);
        bool ManageInventory(uint32_t playerId, const std::string& action, const std::string& parameters);

        // Quest System Integration
        bool CreateQuest(const std::string& questName, const std::string& questData);
        bool UpdateQuest(const std::string& questName, const std::string& questData);
        bool CompleteQuest(const std::string& questName);
        std::string GetQuestStatus(const std::string& questName) const;

        // NPC System Integration
        bool CreateNPC(const std::string& npcName, const std::string& npcData);
        bool UpdateNPC(const std::string& npcName, const std::string& npcData);
        bool RemoveNPC(const std::string& npcName);
        std::string GetNPCData(const std::string& npcName) const;

        // World System Integration
        bool UpdateWorldState(const std::string& worldData);
        bool SetWeather(const std::string& weatherType);
        bool SetTimeOfDay(float hour);
        std::string GetWorldState() const;

        // Data Conversion Utilities
        std::string Vector4FToWitcherScript(const Vector4F& vector) const;
        Vector4F WitcherScriptToVector4F(const std::string& data) const;
        std::string PlayerDataToWitcherScript(const PlayerData& data) const;
        PlayerData WitcherScriptToPlayerData(const std::string& data) const;
        std::string NPCDataToWitcherScript(const NPCData& data) const;
        NPCData WitcherScriptToNPCData(const std::string& data) const;
        std::string ItemDataToWitcherScript(const ItemData& data) const;
        ItemData WitcherScriptToItemData(const std::string& data) const;
        std::string QuestDataToWitcherScript(const QuestData& data) const;
        QuestData WitcherScriptToQuestData(const std::string& data) const;

        // Error Handling
        std::string GetLastError() const;
        void ClearError();
        bool HasError() const;

        // Statistics and Monitoring
        size_t GetLoadedScriptsCount() const;
        std::vector<std::string> GetLoadedScripts() const;
        size_t GetRegisteredCallbacksCount() const;
        std::string GetBridgeStatistics() const;

        // Utility Functions
        bool IsInitialized() const;
        bool IsWitcherScriptEngineReady() const;
        std::string GetWitcherScriptVersion() const;

    private:
        // Singleton instance
        static WitcherScriptBridge* s_instance;

        // State
        bool m_initialized;
        bool m_witcherScriptEngineReady;
        std::string m_lastError;
        std::string m_witcherScriptVersion;

        // WitcherScript Engine
        void* m_witcherScriptEngine; // Pointer to WitcherScript engine instance
        std::unordered_map<std::string, void*> m_loadedScripts; // Loaded script instances

        // Event System
        std::unordered_map<std::string, WitcherScriptEventCallback> m_eventCallbacks;

        // Internal Methods
        bool InitializeWitcherScriptEngineInternal();
        bool LoadWitcherScriptInternal(const std::string& scriptPath, void*& scriptInstance);
        void UnloadWitcherScriptInternal(const std::string& scriptName);
        WitcherScriptResult ExecuteWitcherScriptFunctionInternal(const std::string& functionName, const std::string& parameters);
        void ProcessWitcherScriptEventInternal(const std::string& eventName, const std::string& eventData);

        // Error Handling
        void SetError(const std::string& error);
        void ClearError();

        // WitcherScript Engine Functions (improved with community research)
        bool CreateWitcherScriptEngine();
        void DestroyWitcherScriptEngine();
        void* LoadScriptFromFile(const std::string& filePath);
        void UnloadScript(void* scriptInstance);
        WitcherScriptResult CallScriptFunction(void* scriptInstance, const std::string& functionName, const std::string& parameters);
        bool RegisterCppFunction(const std::string& functionName, void* functionPtr);
        bool TriggerScriptEvent(const std::string& eventName, const std::string& eventData);

        // Lua callback functions for WitcherScript communication
        static int LuaOnPlayerMoved(lua_State* L);
        static int LuaOnCombatAction(lua_State* L);
        static int LuaOnSignCast(lua_State* L);
        static int LuaOnInventoryChanged(lua_State* L);
        static int LuaOnQuestStateChanged(lua_State* L);
        static int LuaPanicHandler(lua_State* L);

        // Constants
        static const std::string WITCHERSCRIPT_VERSION;
        static const std::string DEFAULT_SCRIPT_PATH;
        static const size_t MAX_LOADED_SCRIPTS;
        static const size_t MAX_EVENT_CALLBACKS;
    };

    // Utility namespace for WitcherScript data conversion
    namespace WitcherScriptUtils
    {
        // JSON utilities for WitcherScript communication
        std::string CreateWitcherScriptParameters(const std::vector<std::pair<std::string, std::string>>& params);
        std::string ParseWitcherScriptResponse(const std::string& response);
        bool ValidateWitcherScriptJSON(const std::string& json);

        // Data serialization for WitcherScript
        std::string SerializePlayerData(const PlayerData& data);
        PlayerData DeserializePlayerData(const std::string& data);
        std::string SerializeNPCData(const NPCData& data);
        NPCData DeserializeNPCData(const std::string& data);
        std::string SerializeItemData(const ItemData& data);
        ItemData DeserializeItemData(const std::string& data);
        std::string SerializeQuestData(const QuestData& data);
        QuestData DeserializeQuestData(const std::string& data);

        // Vector utilities
        std::string SerializeVector4F(const Vector4F& vector);
        Vector4F DeserializeVector4F(const std::string& data);

        // Event data utilities
        std::string CreateEventData(const std::string& eventType, const std::string& eventData);
        std::string ParseEventData(const std::string& eventData, std::string& eventType);
    }
}
