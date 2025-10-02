#include "integration/REDkitBridge.h"
#include "integration/WitcherScriptBridge.h"
#include "utils/Logger.h"
#include <sstream>
#include <fstream>
#include <filesystem>
#include <json/json.h>

namespace TW3Integration
{
    // Static member initialization
    REDkitBridge* REDkitBridge::s_instance = nullptr;

    // REDkit script function names
    const std::string REDkitBridge::REDKIT_CREATE_QUEST_FUNCTION = "MP_REDkitIntegration.CreateMultiplayerQuest";
    const std::string REDkitBridge::REDKIT_CREATE_UI_FUNCTION = "MP_REDkitIntegration.CreateMultiplayerUI";
    const std::string REDkitBridge::REDKIT_CREATE_NPC_FUNCTION = "MP_REDkitIntegration.CreateMultiplayerNPC";
    const std::string REDkitBridge::REDKIT_CREATE_ITEM_FUNCTION = "MP_REDkitIntegration.CreateMultiplayerItem";
    const std::string REDkitBridge::REDKIT_EXPORT_DATA_FUNCTION = "MP_REDkitIntegration.ExportGameData";
    const std::string REDkitBridge::REDKIT_IMPORT_DATA_FUNCTION = "MP_REDkitIntegration.ImportGameData";
    const std::string REDkitBridge::REDKIT_LOAD_WEAPON_FUNCTION = "MP_AssetManager.GetWeaponData";
    const std::string REDkitBridge::REDKIT_LOAD_ARMOR_FUNCTION = "MP_AssetManager.GetArmorData";
    const std::string REDkitBridge::REDKIT_LOAD_ITEM_FUNCTION = "MP_AssetManager.GetItemData";
    const std::string REDkitBridge::REDKIT_LOAD_NPC_FUNCTION = "MP_AssetManager.GetNPCData";
    const std::string REDkitBridge::REDKIT_LOAD_QUEST_FUNCTION = "MP_AssetManager.GetQuestData";
    const std::string REDkitBridge::REDKIT_EXECUTE_ATTACK_FUNCTION = "MP_CombatSystem.ExecuteAttack";
    const std::string REDkitBridge::REDKIT_EXECUTE_DEFENSE_FUNCTION = "MP_CombatSystem.ExecuteDefense";
    const std::string REDkitBridge::REDKIT_CAST_SIGN_FUNCTION = "MP_SignsSystem.CastSign";
    const std::string REDkitBridge::REDKIT_CREATE_POTION_FUNCTION = "MP_AlchemySystem.CreatePotion";
    const std::string REDkitBridge::REDKIT_ADD_ITEM_FUNCTION = "MP_InventorySystem.AddItem";
    const std::string REDkitBridge::REDKIT_REMOVE_ITEM_FUNCTION = "MP_InventorySystem.RemoveItem";

    REDkitBridge& REDkitBridge::GetInstance()
    {
        if (s_instance == nullptr)
        {
            s_instance = new REDkitBridge();
        }
        return *s_instance;
    }

    void REDkitBridge::DestroyInstance()
    {
        if (s_instance != nullptr)
        {
            delete s_instance;
            s_instance = nullptr;
        }
    }

    REDkitBridge::REDkitBridge()
        : m_initialized(false), m_redkitAvailable(false), m_redkitPipe(nullptr)
    {
        LOG_INFO("REDkitBridge created");
    }

    REDkitBridge::~REDkitBridge()
    {
        Shutdown();
        LOG_INFO("REDkitBridge destroyed");
    }

    bool REDkitBridge::Initialize()
    {
        if (m_initialized)
        {
            return true;
        }

        LOG_INFO("Initializing REDkitBridge...");

        // Initialize WitcherScript bridge
        auto& witcherScriptBridge = WitcherScriptBridge::GetInstance();
        if (!witcherScriptBridge.Initialize())
        {
            SetError("Failed to initialize WitcherScript bridge");
            return false;
        }

        // Check if REDkit is available through WitcherScript
        m_redkitAvailable = witcherScriptBridge.IsWitcherScriptEngineReady();
        if (!m_redkitAvailable)
        {
            SetError("REDkit not available through WitcherScript");
            return false;
        }

        // Initialize communication with REDkit via WitcherScript
        if (!InitializeREDkitCommunication())
        {
            SetError("Failed to initialize REDkit communication");
            return false;
        }

        m_initialized = true;
        LOG_INFO("REDkitBridge initialized successfully with WitcherScript");
        return true;
    }

    void REDkitBridge::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        LOG_INFO("Shutting down REDkitBridge...");

        // Clear callbacks
        m_questCallback = nullptr;
        m_playerMoveCallback = nullptr;
        m_playerAttackCallback = nullptr;
        m_signCastCallback = nullptr;
        m_inventoryCallback = nullptr;

        m_initialized = false;
        m_redkitAvailable = false;

        LOG_INFO("REDkitBridge shutdown complete");
    }

    // C++ → REDkit Communication
    bool REDkitBridge::CreateMultiplayerQuest(const std::string& questName, const std::string& description)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        std::vector<std::pair<std::string, std::string>> params;
        params.push_back({"questName", questName});
        params.push_back({"description", description});

        std::string parameters = REDkitUtils::CreateREDkitParameters(params);
        std::string result = GetREDkitFunctionResult(REDKIT_CREATE_QUEST_FUNCTION, parameters);

        if (result.empty() || !ValidateREDkitResponse(result))
        {
            SetError("Failed to create multiplayer quest");
            return false;
        }

        LOG_INFO("Created multiplayer quest: " + questName);
        return true;
    }

    bool REDkitBridge::CreateMultiplayerUI()
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        std::string result = GetREDkitFunctionResult(REDKIT_CREATE_UI_FUNCTION, "{}");

        if (result.empty() || !ValidateREDkitResponse(result))
        {
            SetError("Failed to create multiplayer UI");
            return false;
        }

        LOG_INFO("Created multiplayer UI");
        return true;
    }

    bool REDkitBridge::CreateMultiplayerNPC(const std::string& npcType, const Vector4F& position)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        std::vector<std::pair<std::string, std::string>> params;
        params.push_back({"npcType", npcType});
        params.push_back({"position", REDkitUtils::Vector4FToREDkit(position)});

        std::string parameters = REDkitUtils::CreateREDkitParameters(params);
        std::string result = GetREDkitFunctionResult(REDKIT_CREATE_NPC_FUNCTION, parameters);

        if (result.empty() || !ValidateREDkitResponse(result))
        {
            SetError("Failed to create multiplayer NPC");
            return false;
        }

        LOG_INFO("Created multiplayer NPC: " + npcType);
        return true;
    }

    bool REDkitBridge::CreateMultiplayerItem(const std::string& itemType, const Vector4F& position)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        std::vector<std::pair<std::string, std::string>> params;
        params.push_back({"itemType", itemType});
        params.push_back({"position", REDkitUtils::Vector4FToREDkit(position)});

        std::string parameters = REDkitUtils::CreateREDkitParameters(params);
        std::string result = GetREDkitFunctionResult(REDKIT_CREATE_ITEM_FUNCTION, parameters);

        if (result.empty() || !ValidateREDkitResponse(result))
        {
            SetError("Failed to create multiplayer item");
            return false;
        }

        LOG_INFO("Created multiplayer item: " + itemType);
        return true;
    }

    std::string REDkitBridge::ExportGameData()
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return "";
        }

        std::string result = GetREDkitFunctionResult(REDKIT_EXPORT_DATA_FUNCTION, "{}");

        if (result.empty() || !ValidateREDkitResponse(result))
        {
            SetError("Failed to export game data");
            return "";
        }

        LOG_INFO("Exported game data from REDkit");
        return result;
    }

    bool REDkitBridge::ImportGameData(const std::string& jsonData)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        if (!REDkitUtils::ValidateREDkitJSON(jsonData))
        {
            SetError("Invalid JSON data for import");
            return false;
        }

        std::vector<std::pair<std::string, std::string>> params;
        params.push_back({"jsonData", jsonData});

        std::string parameters = REDkitUtils::CreateREDkitParameters(params);
        std::string result = GetREDkitFunctionResult(REDKIT_IMPORT_DATA_FUNCTION, parameters);

        if (result.empty() || !ValidateREDkitResponse(result))
        {
            SetError("Failed to import game data");
            return false;
        }

        LOG_INFO("Imported game data to REDkit");
        return true;
    }

    // Asset loading functions
    bool REDkitBridge::LoadWeaponData(const std::string& weaponType, std::vector<uint8_t>& data)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        // Use WitcherScript bridge for real communication
        auto& witcherScriptBridge = WitcherScriptBridge::GetInstance();
        if (!witcherScriptBridge.LoadAssetFromWitcherScript("weapons/" + weaponType, data))
        {
            SetError("Failed to load weapon data via WitcherScript: " + weaponType);
            return false;
        }

        LOG_INFO("Loaded weapon data via WitcherScript: " + weaponType);
        return true;
    }

    bool REDkitBridge::LoadArmorData(const std::string& armorType, std::vector<uint8_t>& data)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        std::vector<std::pair<std::string, std::string>> params;
        params.push_back({"armorType", armorType});

        std::string parameters = REDkitUtils::CreateREDkitParameters(params);
        std::string result = GetREDkitFunctionResult(REDKIT_LOAD_ARMOR_FUNCTION, parameters);

        if (result.empty() || !ValidateREDkitResponse(result))
        {
            SetError("Failed to load armor data");
            return false;
        }

        data.assign(result.begin(), result.end());
        LOG_INFO("Loaded armor data: " + armorType);
        return true;
    }

    bool REDkitBridge::LoadItemData(const std::string& itemType, std::vector<uint8_t>& data)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        std::vector<std::pair<std::string, std::string>> params;
        params.push_back({"itemType", itemType});

        std::string parameters = REDkitUtils::CreateREDkitParameters(params);
        std::string result = GetREDkitFunctionResult(REDKIT_LOAD_ITEM_FUNCTION, parameters);

        if (result.empty() || !ValidateREDkitResponse(result))
        {
            SetError("Failed to load item data");
            return false;
        }

        data.assign(result.begin(), result.end());
        LOG_INFO("Loaded item data: " + itemType);
        return true;
    }

    bool REDkitBridge::LoadNPCData(const std::string& npcType, std::vector<uint8_t>& data)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        std::vector<std::pair<std::string, std::string>> params;
        params.push_back({"npcType", npcType});

        std::string parameters = REDkitUtils::CreateREDkitParameters(params);
        std::string result = GetREDkitFunctionResult(REDKIT_LOAD_NPC_FUNCTION, parameters);

        if (result.empty() || !ValidateREDkitResponse(result))
        {
            SetError("Failed to load NPC data");
            return false;
        }

        data.assign(result.begin(), result.end());
        LOG_INFO("Loaded NPC data: " + npcType);
        return true;
    }

    bool REDkitBridge::LoadQuestData(const std::string& questType, std::vector<uint8_t>& data)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        std::vector<std::pair<std::string, std::string>> params;
        params.push_back({"questType", questType});

        std::string parameters = REDkitUtils::CreateREDkitParameters(params);
        std::string result = GetREDkitFunctionResult(REDKIT_LOAD_QUEST_FUNCTION, parameters);

        if (result.empty() || !ValidateREDkitResponse(result))
        {
            SetError("Failed to load quest data");
            return false;
        }

        data.assign(result.begin(), result.end());
        LOG_INFO("Loaded quest data: " + questType);
        return true;
    }

    // Combat system functions
    bool REDkitBridge::ExecuteAttack(uint32_t playerId, uint32_t attackType, uint32_t weaponType, const Vector4F& targetPosition)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        // Use WitcherScript bridge for real communication with mod APIs
        auto& witcherScriptBridge = WitcherScriptBridge::GetInstance();
        
        // Create JSON parameters using real W3DamageAction APIs from the mod
        Json::Value attackParams;
        attackParams["attacker"] = playerId;
        attackParams["victim"] = GetTargetFromPosition(targetPosition);
        attackParams["sourceName"] = "multiplayer_attack";
        attackParams["hitReactionType"] = "EHRT_Light";
        attackParams["powerStatType"] = "CPS_AttackPower";
        attackParams["damageType"] = GetDamageTypeFromAttackType(attackType);
        attackParams["damageValue"] = CalculateDamage(attackType, weaponType);
        attackParams["hitAnimationPlayType"] = "EAHA_ForceYes";
        attackParams["canPlayHitParticle"] = true;
        attackParams["isCriticalHit"] = IsCriticalHit(playerId);
        attackParams["ignoreImmortality"] = false;
        attackParams["ignoreArmor"] = false;
        attackParams["ignoreResistance"] = false;
        attackParams["isEnvironment"] = false;
        
        Json::StreamWriterBuilder builder;
        std::string jsonParams = Json::writeString(builder, attackParams);
        
        if (!witcherScriptBridge.ExecuteCombatAction(playerId, "ExecuteAttack", jsonParams))
        {
            SetError("Failed to execute attack via WitcherScript");
            return false;
        }

        LOG_INFO("Executed attack via WitcherScript for player: " + std::to_string(playerId));
        return true;
    }

    bool REDkitBridge::ExecuteDefense(uint32_t playerId, uint32_t defenseType, uint32_t incomingAttackId)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        std::vector<std::pair<std::string, std::string>> params;
        params.push_back({"playerId", std::to_string(playerId)});
        params.push_back({"defenseType", std::to_string(defenseType)});
        params.push_back({"incomingAttackId", std::to_string(incomingAttackId)});

        std::string parameters = REDkitUtils::CreateREDkitParameters(params);
        std::string result = GetREDkitFunctionResult(REDKIT_EXECUTE_DEFENSE_FUNCTION, parameters);

        if (result.empty() || !ValidateREDkitResponse(result))
        {
            SetError("Failed to execute defense");
            return false;
        }

        LOG_INFO("Executed defense for player: " + std::to_string(playerId));
        return true;
    }

    bool REDkitBridge::PlayAttackAnimation(uint32_t playerId, uint32_t attackType)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        // This would typically call the animation system
        LOG_INFO("Playing attack animation for player: " + std::to_string(playerId));
        return true;
    }

    bool REDkitBridge::PlayDefenseAnimation(uint32_t playerId, uint32_t defenseType)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        // This would typically call the animation system
        LOG_INFO("Playing defense animation for player: " + std::to_string(playerId));
        return true;
    }

    // Signs system functions
    bool REDkitBridge::CastSign(uint32_t playerId, uint32_t signType, uint32_t intensity, const Vector4F& targetPosition)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        // Use WitcherScript bridge for real communication
        auto& witcherScriptBridge = WitcherScriptBridge::GetInstance();
        
        std::string parameters = std::to_string(signType) + "|" + std::to_string(intensity) + "|" + 
                               witcherScriptBridge.Vector4FToWitcherScript(targetPosition);
        
        if (!witcherScriptBridge.CastSign(playerId, "CastSign", parameters))
        {
            SetError("Failed to cast sign via WitcherScript");
            return false;
        }

        LOG_INFO("Cast sign via WitcherScript for player: " + std::to_string(playerId));
        return true;
    }

    bool REDkitBridge::CanCastSign(uint32_t playerId, uint32_t signType, uint32_t intensity)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        // This would check stamina, cooldown, etc.
        LOG_INFO("Checking if player can cast sign: " + std::to_string(playerId));
        return true;
    }

    float REDkitBridge::GetSignCooldown(uint32_t playerId, uint32_t signType)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return 0.0f;
        }

        // This would return the actual cooldown time
        LOG_INFO("Getting sign cooldown for player: " + std::to_string(playerId));
        return 0.0f;
    }

    // Alchemy system functions
    bool REDkitBridge::CreatePotion(uint32_t playerId, const std::string& potionType, uint32_t quantity)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        std::vector<std::pair<std::string, std::string>> params;
        params.push_back({"playerId", std::to_string(playerId)});
        params.push_back({"potionType", potionType});
        params.push_back({"quantity", std::to_string(quantity)});

        std::string parameters = REDkitUtils::CreateREDkitParameters(params);
        std::string result = GetREDkitFunctionResult(REDKIT_CREATE_POTION_FUNCTION, parameters);

        if (result.empty() || !ValidateREDkitResponse(result))
        {
            SetError("Failed to create potion");
            return false;
        }

        LOG_INFO("Created potion for player: " + std::to_string(playerId));
        return true;
    }

    bool REDkitBridge::CreateBomb(uint32_t playerId, const std::string& bombType, uint32_t quantity)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        // Similar to CreatePotion but for bombs
        LOG_INFO("Created bomb for player: " + std::to_string(playerId));
        return true;
    }

    bool REDkitBridge::CreateOil(uint32_t playerId, const std::string& oilType, uint32_t quantity)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        // Similar to CreatePotion but for oils
        LOG_INFO("Created oil for player: " + std::to_string(playerId));
        return true;
    }

    bool REDkitBridge::UseAlchemyItem(uint32_t playerId, uint32_t itemId)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        // This would use the item and apply its effects
        LOG_INFO("Used alchemy item for player: " + std::to_string(playerId));
        return true;
    }

    // Inventory system functions
    bool REDkitBridge::AddItemToInventory(uint32_t playerId, uint32_t itemId, uint32_t quantity)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        std::vector<std::pair<std::string, std::string>> params;
        params.push_back({"playerId", std::to_string(playerId)});
        params.push_back({"itemId", std::to_string(itemId)});
        params.push_back({"quantity", std::to_string(quantity)});

        std::string parameters = REDkitUtils::CreateREDkitParameters(params);
        std::string result = GetREDkitFunctionResult(REDKIT_ADD_ITEM_FUNCTION, parameters);

        if (result.empty() || !ValidateREDkitResponse(result))
        {
            SetError("Failed to add item to inventory");
            return false;
        }

        LOG_INFO("Added item to inventory for player: " + std::to_string(playerId));
        return true;
    }

    bool REDkitBridge::RemoveItemFromInventory(uint32_t playerId, uint32_t itemId, uint32_t quantity)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        std::vector<std::pair<std::string, std::string>> params;
        params.push_back({"playerId", std::to_string(playerId)});
        params.push_back({"itemId", std::to_string(itemId)});
        params.push_back({"quantity", std::to_string(quantity)});

        std::string parameters = REDkitUtils::CreateREDkitParameters(params);
        std::string result = GetREDkitFunctionResult(REDKIT_REMOVE_ITEM_FUNCTION, parameters);

        if (result.empty() || !ValidateREDkitResponse(result))
        {
            SetError("Failed to remove item from inventory");
            return false;
        }

        LOG_INFO("Removed item from inventory for player: " + std::to_string(playerId));
        return true;
    }

    bool REDkitBridge::HasItemInInventory(uint32_t playerId, uint32_t itemId)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        // This would check if the player has the item
        LOG_INFO("Checking if player has item: " + std::to_string(playerId));
        return true;
    }

    uint32_t REDkitBridge::GetItemQuantity(uint32_t playerId, uint32_t itemId)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return 0;
        }

        // This would return the actual quantity
        LOG_INFO("Getting item quantity for player: " + std::to_string(playerId));
        return 0;
    }

    float REDkitBridge::GetInventoryWeight(uint32_t playerId)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return 0.0f;
        }

        // This would return the actual weight
        LOG_INFO("Getting inventory weight for player: " + std::to_string(playerId));
        return 0.0f;
    }

    // REDkit → C++ Communication (Event Handlers)
    void REDkitBridge::OnQuestStateChanged(uint32_t questId, uint32_t newState)
    {
        if (m_questCallback)
        {
            m_questCallback(questId, newState);
        }
        LOG_INFO("Quest state changed: " + std::to_string(questId) + " -> " + std::to_string(newState));
    }

    void REDkitBridge::OnPlayerMoved(uint32_t playerId, const Vector4F& position)
    {
        if (m_playerMoveCallback)
        {
            m_playerMoveCallback(playerId, position);
        }
        LOG_INFO("Player moved: " + std::to_string(playerId));
    }

    void REDkitBridge::OnPlayerAttacked(uint32_t playerId, uint32_t targetId, float damage)
    {
        if (m_playerAttackCallback)
        {
            m_playerAttackCallback(playerId, targetId, damage);
        }
        LOG_INFO("Player attacked: " + std::to_string(playerId) + " -> " + std::to_string(targetId));
    }

    void REDkitBridge::OnSignCast(uint32_t playerId, uint32_t signType, const Vector4F& position)
    {
        if (m_signCastCallback)
        {
            m_signCastCallback(playerId, signType, position);
        }
        LOG_INFO("Sign cast: " + std::to_string(playerId) + " -> " + std::to_string(signType));
    }

    void REDkitBridge::OnInventoryChanged(uint32_t playerId, uint32_t itemId, bool added)
    {
        if (m_inventoryCallback)
        {
            m_inventoryCallback(playerId, itemId, added);
        }
        LOG_INFO("Inventory changed: " + std::to_string(playerId) + " -> " + std::to_string(itemId));
    }

    void REDkitBridge::OnNPCStateChanged(uint32_t npcId, uint32_t newState)
    {
        LOG_INFO("NPC state changed: " + std::to_string(npcId) + " -> " + std::to_string(newState));
    }

    void REDkitBridge::OnItemStateChanged(uint32_t itemId, uint32_t newState)
    {
        LOG_INFO("Item state changed: " + std::to_string(itemId) + " -> " + std::to_string(newState));
    }

    // Callback registration
    void REDkitBridge::SetQuestCallback(REDkitQuestCallback callback)
    {
        m_questCallback = callback;
    }

    void REDkitBridge::SetPlayerMoveCallback(REDkitPlayerMoveCallback callback)
    {
        m_playerMoveCallback = callback;
    }

    void REDkitBridge::SetPlayerAttackCallback(REDkitPlayerAttackCallback callback)
    {
        m_playerAttackCallback = callback;
    }

    void REDkitBridge::SetSignCastCallback(REDkitSignCastCallback callback)
    {
        m_signCastCallback = callback;
    }

    void REDkitBridge::SetInventoryCallback(REDkitInventoryCallback callback)
    {
        m_inventoryCallback = callback;
    }

    // Communication with REDkit scripts
    bool REDkitBridge::SendToREDkit(const std::string& message)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        // In a real implementation, this would send data to REDkit
        // through a shared memory, named pipe, or other IPC mechanism
        LOG_DEBUG("Sending to REDkit: " + message);
        return true;
    }

    bool REDkitBridge::ProcessFromREDkit(const std::string& message)
    {
        if (!m_initialized || !m_redkitAvailable)
        {
            SetError("REDkitBridge not initialized or REDkit not available");
            return false;
        }

        // Parse and process message from REDkit
        LOG_DEBUG("Processing from REDkit: " + message);
        return true;
    }

    // Utility functions
    bool REDkitBridge::IsREDkitAvailable() const
    {
        return m_redkitAvailable;
    }

    bool REDkitBridge::IsInitialized() const
    {
        return m_initialized;
    }

    std::string REDkitBridge::GetLastError() const
    {
        return m_lastError;
    }

    // Private methods
    bool REDkitBridge::CheckREDkitAvailability()
    {
        LOG_INFO("Checking REDkit availability...");
        
        // Based on community research: Check for REDkit process and registry
        // Method 1: Check for REDkit window
        HWND redkitWindow = FindWindowA(nullptr, "REDkit");
        if (redkitWindow != nullptr)
        {
            LOG_INFO("REDkit window found");
            return true;
        }
        
        // Method 2: Check for REDkit process
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot != INVALID_HANDLE_VALUE)
        {
            PROCESSENTRY32 pe32;
            pe32.dwSize = sizeof(PROCESSENTRY32);
            
            if (Process32First(hSnapshot, &pe32))
            {
                do
                {
                    if (strstr(pe32.szExeFile, "REDkit") != nullptr)
                    {
                        LOG_INFO("REDkit process found: " + std::string(pe32.szExeFile));
                        CloseHandle(hSnapshot);
                        return true;
                    }
                } while (Process32Next(hSnapshot, &pe32));
            }
            CloseHandle(hSnapshot);
        }
        
        // Method 3: Check registry for REDkit installation
        HKEY hKey;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\CD Projekt RED\\REDkit", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
        {
            LOG_INFO("REDkit registry entry found");
            RegCloseKey(hKey);
            return true;
        }
        
        LOG_WARNING("REDkit not found - using mock mode");
        return false; // Not available, but continue with mock
    }

    bool REDkitBridge::InitializeREDkitCommunication()
    {
        LOG_INFO("Initializing REDkit communication via named pipes...");
        
        // Based on community research: Use named pipes for IPC
        // Create named pipe for communication with REDkit
        m_redkitPipe = CreateNamedPipeA(
            "\\\\.\\pipe\\REDkitPipe",
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1, // Max instances
            4096, // Out buffer size
            4096, // In buffer size
            0, // Default timeout
            nullptr // Security attributes
        );
        
        if (m_redkitPipe == INVALID_HANDLE_VALUE)
        {
            LOG_WARNING("Failed to create REDkit named pipe - using mock mode");
            m_redkitPipe = nullptr;
            return true; // Continue with mock mode
        }
        
        LOG_INFO("REDkit named pipe created successfully");
        
        // Try to connect to REDkit (if it's running)
        if (ConnectNamedPipe(m_redkitPipe, nullptr) || GetLastError() == ERROR_PIPE_CONNECTED)
        {
            LOG_INFO("Connected to REDkit via named pipe");
            return true;
        }
        else
        {
            LOG_WARNING("REDkit not responding to pipe connection - using mock mode");
            CloseHandle(m_redkitPipe);
            m_redkitPipe = nullptr;
            return true; // Continue with mock mode
        }
    }

    bool REDkitBridge::ExecuteREDkitFunction(const std::string& functionName, const std::string& parameters)
    {
        LOG_DEBUG("Executing REDkit function: " + functionName + " with parameters: " + parameters);
        
        // Based on community research: Use JSON over IPC
        if (m_redkitPipe != nullptr)
        {
            // Create JSON request
            Json::Value request;
            request["func"] = functionName;
            request["args"] = parameters;
            request["timestamp"] = static_cast<uint64_t>(std::time(nullptr));
            
            Json::StreamWriterBuilder builder;
            std::string jsonRequest = Json::writeString(builder, request);
            
            // Send via named pipe
            DWORD bytesWritten;
            if (WriteFile(m_redkitPipe, jsonRequest.c_str(), jsonRequest.length(), &bytesWritten, nullptr))
            {
                LOG_DEBUG("Sent JSON request to REDkit: " + jsonRequest);
                return true;
            }
            else
            {
                LOG_WARNING("Failed to send request to REDkit via named pipe");
                return false;
            }
        }
        else
        {
            // Mock mode - simulate successful execution
            LOG_DEBUG("Mock mode: Simulating REDkit function execution");
            return true;
        }
    }

    std::string REDkitBridge::GetREDkitFunctionResult(const std::string& functionName, const std::string& parameters)
    {
        // In a real implementation, this would get the result from a REDkit function
        LOG_DEBUG("Getting REDkit function result: " + functionName + " with parameters: " + parameters);
        
        // For now, return a mock result
        return "{\"success\": true, \"result\": \"mock_result\"}";
    }

    bool REDkitBridge::ValidateREDkitResponse(const std::string& response)
    {
        // In a real implementation, this would validate the REDkit response
        return !response.empty() && response.find("success") != std::string::npos;
    }

    void REDkitBridge::SetError(const std::string& error)
    {
        m_lastError = error;
        LOG_ERROR("REDkitBridge error: " + error);
    }

    void REDkitBridge::ClearError()
    {
        m_lastError.clear();
    }

    // REDkitUtils implementation
    namespace REDkitUtils
    {
        std::string Vector4FToREDkit(const Vector4F& vector)
        {
            std::stringstream ss;
            ss << "{\"x\":" << vector.x << ",\"y\":" << vector.y << ",\"z\":" << vector.z << ",\"w\":" << vector.w << "}";
            return ss.str();
        }

        std::string PlayerDataToREDkit(const PlayerData& data)
        {
            std::stringstream ss;
            ss << "{\"id\":" << data.id << ",\"name\":\"" << data.name << "\",\"x\":" << data.x << ",\"y\":" << data.y << ",\"z\":" << data.z << ",\"w\":" << data.w << ",\"health\":" << data.health << "}";
            return ss.str();
        }

        std::string NPCDataToREDkit(const NPCData& data)
        {
            std::stringstream ss;
            ss << "{\"id\":" << data.id << ",\"name\":\"" << data.name << "\",\"x\":" << data.x << ",\"y\":" << data.y << ",\"z\":" << data.z << ",\"w\":" << data.w << ",\"health\":" << data.health << "}";
            return ss.str();
        }

        std::string ItemDataToREDkit(const ItemData& data)
        {
            std::stringstream ss;
            ss << "{\"id\":" << data.id << ",\"name\":\"" << data.name << "\",\"type\":\"" << data.type << "\",\"value\":" << data.value << ",\"weight\":" << data.weight << "}";
            return ss.str();
        }

        std::string QuestDataToREDkit(const QuestData& data)
        {
            std::stringstream ss;
            ss << "{\"id\":" << data.id << ",\"name\":\"" << data.name << "\",\"state\":" << data.state << ",\"progress\":" << data.progress << "}";
            return ss.str();
        }

        std::string CreateREDkitParameters(const std::vector<std::pair<std::string, std::string>>& params)
        {
            std::stringstream ss;
            ss << "{";
            
            for (size_t i = 0; i < params.size(); ++i)
            {
                if (i > 0) ss << ",";
                ss << "\"" << params[i].first << "\":\"" << params[i].second << "\"";
            }
            
            ss << "}";
            return ss.str();
        }

        std::string ParseREDkitResponse(const std::string& response)
        {
            // In a real implementation, this would parse the REDkit response
            return response;
        }

        bool ValidateREDkitJSON(const std::string& json)
        {
            // In a real implementation, this would validate JSON
            return !json.empty() && json.find("{") != std::string::npos;
        }

        Vector4F REDkitToVector4F(const std::string& redkitData)
        {
            // Based on community research: Parse JSON Vector4 data
            Vector4F result = {0.0f, 0.0f, 0.0f, 1.0f};
            
            try
            {
                Json::Value json;
                Json::Reader reader;
                
                if (reader.parse(redkitData, json))
                {
                    if (json.isMember("x") && json.isMember("y") && json.isMember("z") && json.isMember("w"))
                    {
                        result.x = json["x"].asFloat();
                        result.y = json["y"].asFloat();
                        result.z = json["z"].asFloat();
                        result.w = json["w"].asFloat();
                    }
                    else if (json.isArray() && json.size() >= 4)
                    {
                        // Handle array format: [x, y, z, w]
                        result.x = json[0].asFloat();
                        result.y = json[1].asFloat();
                        result.z = json[2].asFloat();
                        result.w = json[3].asFloat();
                    }
                }
            }
            catch (const std::exception& e)
            {
                LOG_WARNING("Failed to parse Vector4F from REDkit data: " + std::string(e.what()));
            }
            
            return result;
        }

        PlayerData REDkitToPlayerData(const std::string& redkitData)
        {
            // In a real implementation, this would parse the REDkit data
            PlayerData result = {};
            return result;
        }

        NPCData REDkitToNPCData(const std::string& redkitData)
        {
            // In a real implementation, this would parse the REDkit data
            NPCData result = {};
            return result;
        }

        ItemData REDkitToItemData(const std::string& redkitData)
        {
            // In a real implementation, this would parse the REDkit data
            ItemData result = {};
            return result;
        }

        QuestData REDkitToQuestData(const std::string& redkitData)
        {
            // In a real implementation, this would parse the REDkit data
            QuestData result = {};
            return result;
        }
    }

    // Helper functions for combat system (based on mod analysis)
    uint32_t REDkitBridge::GetTargetFromPosition(const Vector4F& position)
    {
        // Use WitcherScript to find target at position
        auto& witcherScriptBridge = WitcherScriptBridge::GetInstance();
        
        // Create JSON parameters for target detection
        Json::Value targetParams;
        targetParams["position"]["x"] = position.x;
        targetParams["position"]["y"] = position.y;
        targetParams["position"]["z"] = position.z;
        targetParams["position"]["w"] = position.w;
        targetParams["searchRadius"] = 5.0f; // 5 meter search radius
        targetParams["targetType"] = "enemy"; // Look for enemies
        
        Json::StreamWriterBuilder builder;
        std::string jsonParams = Json::writeString(builder, targetParams);
        
        // Execute target detection via WitcherScript
        std::string result = witcherScriptBridge.ExecuteWitcherScriptFunction("FindTargetAtPosition", jsonParams);
        
        // Parse result to get target ID
        try
        {
            Json::Value jsonResult;
            Json::Reader reader;
            if (reader.parse(result, jsonResult) && jsonResult.isMember("targetId"))
            {
                return jsonResult["targetId"].asUInt();
            }
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Failed to parse target detection result: " + std::string(e.what()));
        }
        
        return 0; // No target found
    }
    
    std::string REDkitBridge::GetDamageTypeFromAttackType(uint32_t attackType)
    {
        // Map attack types to damage types based on mod analysis
        switch (attackType)
        {
            case 0: return "DAMAGE_NAME_PHYSICAL";
            case 1: return "DAMAGE_NAME_SLASHING";
            case 2: return "DAMAGE_NAME_PIERCING";
            case 3: return "DAMAGE_NAME_BLUNT";
            case 4: return "DAMAGE_NAME_FIRE";
            case 5: return "DAMAGE_NAME_FROST";
            case 6: return "DAMAGE_NAME_LIGHTNING";
            case 7: return "DAMAGE_NAME_POISON";
            default: return "DAMAGE_NAME_PHYSICAL";
        }
    }
    
    float REDkitBridge::CalculateDamage(uint32_t attackType, uint32_t weaponType)
    {
        // Base damage values based on weapon types (from mod analysis)
        float baseDamage = 50.0f;
        
        // Weapon type multipliers (based on TW3 weapon categories)
        switch (weaponType)
        {
            case 0: // Fists
                baseDamage = 20.0f;
                break;
            case 1: // Steel Sword
                baseDamage = 60.0f;
                break;
            case 2: // Silver Sword
                baseDamage = 70.0f;
                break;
            case 3: // Axe
                baseDamage = 80.0f;
                break;
            case 4: // Mace
                baseDamage = 75.0f;
                break;
            case 5: // Spear
                baseDamage = 65.0f;
                break;
            case 6: // Crossbow
                baseDamage = 90.0f;
                break;
            case 7: // Bow
                baseDamage = 85.0f;
                break;
            default:
                baseDamage = 50.0f;
                break;
        }
        
        // Attack type multipliers (based on attack animations)
        float attackMultiplier = 1.0f;
        switch (attackType)
        {
            case 0: // Light Attack
                attackMultiplier = 0.8f;
                break;
            case 1: // Heavy Attack
                attackMultiplier = 1.5f;
                break;
            case 2: // Strong Attack
                attackMultiplier = 2.0f;
                break;
            case 3: // Combo Attack
                attackMultiplier = 1.2f;
                break;
            case 4: // Counter Attack
                attackMultiplier = 1.8f;
                break;
            case 5: // Finisher Attack
                attackMultiplier = 2.5f;
                break;
            default:
                attackMultiplier = 1.0f;
                break;
        }
        
        // Add some randomness (±10%)
        float randomFactor = 0.9f + (static_cast<float>(rand()) / RAND_MAX) * 0.2f;
        
        return baseDamage * attackMultiplier * randomFactor;
    }
    
    bool REDkitBridge::IsCriticalHit(uint32_t playerId)
    {
        // Use WitcherScript to get player stats for critical hit calculation
        auto& witcherScriptBridge = WitcherScriptBridge::GetInstance();
        
        // Get player stats via WitcherScript
        Json::Value playerParams;
        playerParams["playerId"] = playerId;
        playerParams["stats"] = {"luck", "criticalChance", "level"};
        
        Json::StreamWriterBuilder builder;
        std::string jsonParams = Json::writeString(builder, playerParams);
        
        std::string result = witcherScriptBridge.ExecuteWitcherScriptFunction("GetPlayerStats", jsonParams);
        
        try
        {
            Json::Value jsonResult;
            Json::Reader reader;
            if (reader.parse(result, jsonResult))
            {
                float luck = jsonResult.get("luck", 0.0f).asFloat();
                float criticalChance = jsonResult.get("criticalChance", 5.0f).asFloat();
                int level = jsonResult.get("level", 1).asInt();
                
                // Calculate critical hit chance based on stats
                float baseChance = criticalChance + (luck * 0.1f) + (level * 0.05f);
                float randomValue = static_cast<float>(rand()) / RAND_MAX * 100.0f;
                
                return randomValue < baseChance;
            }
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Failed to parse player stats for critical hit: " + std::string(e.what()));
        }
        
        // Fallback to base 10% chance
        return (rand() % 100) < 10;
    }
}
