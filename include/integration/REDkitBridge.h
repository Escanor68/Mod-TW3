#pragma once
#include "Common.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace TW3Integration
{
    // Forward declarations
    struct Vector4F;
    struct PlayerData;
    struct NPCData;
    struct ItemData;
    struct QuestData;

    // Callback types for REDkit events
    using REDkitQuestCallback = std::function<void(uint32_t questId, uint32_t newState)>;
    using REDkitPlayerMoveCallback = std::function<void(uint32_t playerId, const Vector4F& position)>;
    using REDkitPlayerAttackCallback = std::function<void(uint32_t playerId, uint32_t targetId, float damage)>;
    using REDkitSignCastCallback = std::function<void(uint32_t playerId, uint32_t signType, const Vector4F& position)>;
    using REDkitInventoryCallback = std::function<void(uint32_t playerId, uint32_t itemId, bool added)>;

    // Bridge class for communication between C++ and REDkit scripts
    class REDkitBridge
    {
    public:
        // Singleton pattern
        static REDkitBridge& GetInstance();
        static void DestroyInstance();

        // Constructor/Destructor
        REDkitBridge();
        ~REDkitBridge();

        // Initialization
        bool Initialize();
        void Shutdown();

        // C++ → REDkit Communication
        // Bridge to MP_REDkitIntegration.ws
        bool CreateMultiplayerQuest(const std::string& questName, const std::string& description);
        bool CreateMultiplayerUI();
        bool CreateMultiplayerNPC(const std::string& npcType, const Vector4F& position);
        bool CreateMultiplayerItem(const std::string& itemType, const Vector4F& position);
        std::string ExportGameData();
        bool ImportGameData(const std::string& jsonData);

        // Bridge to MP_AssetManager.ws
        bool LoadWeaponData(const std::string& weaponType, std::vector<uint8_t>& data);
        bool LoadArmorData(const std::string& armorType, std::vector<uint8_t>& data);
        bool LoadItemData(const std::string& itemType, std::vector<uint8_t>& data);
        bool LoadNPCData(const std::string& npcType, std::vector<uint8_t>& data);
        bool LoadQuestData(const std::string& questType, std::vector<uint8_t>& data);

        // Bridge to MP_CombatSystem.ws
        bool ExecuteAttack(uint32_t playerId, uint32_t attackType, uint32_t weaponType, const Vector4F& targetPosition);
        bool ExecuteDefense(uint32_t playerId, uint32_t defenseType, uint32_t incomingAttackId);
        bool PlayAttackAnimation(uint32_t playerId, uint32_t attackType);
        bool PlayDefenseAnimation(uint32_t playerId, uint32_t defenseType);

        // Bridge to MP_SignsSystem.ws
        bool CastSign(uint32_t playerId, uint32_t signType, uint32_t intensity, const Vector4F& targetPosition);
        bool CanCastSign(uint32_t playerId, uint32_t signType, uint32_t intensity);
        float GetSignCooldown(uint32_t playerId, uint32_t signType);

        // Bridge to MP_AlchemySystem.ws
        bool CreatePotion(uint32_t playerId, const std::string& potionType, uint32_t quantity);
        bool CreateBomb(uint32_t playerId, const std::string& bombType, uint32_t quantity);
        bool CreateOil(uint32_t playerId, const std::string& oilType, uint32_t quantity);
        bool UseAlchemyItem(uint32_t playerId, uint32_t itemId);

        // Bridge to MP_InventorySystem.ws
        bool AddItemToInventory(uint32_t playerId, uint32_t itemId, uint32_t quantity);
        bool RemoveItemFromInventory(uint32_t playerId, uint32_t itemId, uint32_t quantity);
        bool HasItemInInventory(uint32_t playerId, uint32_t itemId);
        uint32_t GetItemQuantity(uint32_t playerId, uint32_t itemId);
        float GetInventoryWeight(uint32_t playerId);

        // REDkit → C++ Communication (Event Handlers)
        void OnQuestStateChanged(uint32_t questId, uint32_t newState);
        void OnPlayerMoved(uint32_t playerId, const Vector4F& position);
        void OnPlayerAttacked(uint32_t playerId, uint32_t targetId, float damage);
        void OnSignCast(uint32_t playerId, uint32_t signType, const Vector4F& position);
        void OnInventoryChanged(uint32_t playerId, uint32_t itemId, bool added);
        void OnNPCStateChanged(uint32_t npcId, uint32_t newState);
        void OnItemStateChanged(uint32_t itemId, uint32_t newState);

        // Callback registration
        void SetQuestCallback(REDkitQuestCallback callback);
        void SetPlayerMoveCallback(REDkitPlayerMoveCallback callback);
        void SetPlayerAttackCallback(REDkitPlayerAttackCallback callback);
        void SetSignCastCallback(REDkitSignCastCallback callback);
        void SetInventoryCallback(REDkitInventoryCallback callback);

        // Communication with REDkit scripts
        bool SendToREDkit(const std::string& message);
        bool ProcessFromREDkit(const std::string& message);

        // Utility functions
        bool IsREDkitAvailable() const;
        bool IsInitialized() const;
        std::string GetLastError() const;

    private:
        // Singleton instance
        static REDkitBridge* s_instance;

        // State
        bool m_initialized;
        bool m_redkitAvailable;
        std::string m_lastError;
        void* m_redkitPipe; // Named pipe handle for REDkit communication

        // Callbacks
        REDkitQuestCallback m_questCallback;
        REDkitPlayerMoveCallback m_playerMoveCallback;
        REDkitPlayerAttackCallback m_playerAttackCallback;
        REDkitSignCastCallback m_signCastCallback;
        REDkitInventoryCallback m_inventoryCallback;

        // Internal communication methods
        bool ExecuteREDkitFunction(const std::string& functionName, const std::string& parameters);
        std::string GetREDkitFunctionResult(const std::string& functionName, const std::string& parameters);
        bool ValidateREDkitResponse(const std::string& response);

        // Error handling
        void SetError(const std::string& error);
        void ClearError();

        // REDkit script function names
        static const std::string REDKIT_CREATE_QUEST_FUNCTION;
        static const std::string REDKIT_CREATE_UI_FUNCTION;
        static const std::string REDKIT_CREATE_NPC_FUNCTION;
        static const std::string REDKIT_CREATE_ITEM_FUNCTION;
        static const std::string REDKIT_EXPORT_DATA_FUNCTION;
        static const std::string REDKIT_IMPORT_DATA_FUNCTION;
        static const std::string REDKIT_LOAD_WEAPON_FUNCTION;
        static const std::string REDKIT_LOAD_ARMOR_FUNCTION;
        static const std::string REDKIT_LOAD_ITEM_FUNCTION;
        static const std::string REDKIT_LOAD_NPC_FUNCTION;
        static const std::string REDKIT_LOAD_QUEST_FUNCTION;
        static const std::string REDKIT_EXECUTE_ATTACK_FUNCTION;
        static const std::string REDKIT_EXECUTE_DEFENSE_FUNCTION;
        static const std::string REDKIT_CAST_SIGN_FUNCTION;
        static const std::string REDKIT_CREATE_POTION_FUNCTION;
        static const std::string REDKIT_ADD_ITEM_FUNCTION;
        static const std::string REDKIT_REMOVE_ITEM_FUNCTION;
    };

    // Utility functions for data conversion
    namespace REDkitUtils
    {
        // Convert C++ data to REDkit format
        std::string Vector4FToREDkit(const Vector4F& vector);
        std::string PlayerDataToREDkit(const PlayerData& data);
        std::string NPCDataToREDkit(const NPCData& data);
        std::string ItemDataToREDkit(const ItemData& data);
        std::string QuestDataToREDkit(const QuestData& data);

        // Convert REDkit data to C++ format
        Vector4F REDkitToVector4F(const std::string& redkitData);
        PlayerData REDkitToPlayerData(const std::string& redkitData);
        NPCData REDkitToNPCData(const std::string& redkitData);
        ItemData REDkitToItemData(const std::string& redkitData);
        QuestData REDkitToQuestData(const std::string& redkitData);

        // JSON utilities for REDkit communication
        std::string CreateREDkitParameters(const std::vector<std::pair<std::string, std::string>>& params);
        std::string ParseREDkitResponse(const std::string& response);
        bool ValidateREDkitJSON(const std::string& json);
    }

    // Helper functions for combat system (based on mod analysis)
    uint32_t GetTargetFromPosition(const Vector4F& position);
    std::string GetDamageTypeFromAttackType(uint32_t attackType);
    float CalculateDamage(uint32_t attackType, uint32_t weaponType);
    bool IsCriticalHit(uint32_t playerId);
}
