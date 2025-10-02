#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include "integration/WitcherScriptBridge.h"
#include "integration/REDkitBridge.h"
#include "integration/AssetLoader.h"
#include <vector>
#include <string>

using namespace TW3Integration;

// Test WitcherScriptBridge functionality
TEST_CASE("WitcherScriptBridge - Basic Functionality", "[witcherscript][integration]")
{
    auto& bridge = WitcherScriptBridge::GetInstance();
    
    SECTION("Initialization")
    {
        REQUIRE(bridge.Initialize());
        REQUIRE(bridge.IsInitialized());
        REQUIRE(bridge.IsWitcherScriptEngineReady());
    }
    
    SECTION("Script Management")
    {
        REQUIRE(bridge.LoadWitcherScript("MP_BridgeSupport.ws"));
        REQUIRE(bridge.IsWitcherScriptLoaded("MP_BridgeSupport.ws"));
        REQUIRE(bridge.GetLoadedScriptsCount() > 0);
        
        auto scripts = bridge.GetLoadedScripts();
        REQUIRE(!scripts.empty());
        
        REQUIRE(bridge.UnloadWitcherScript("MP_BridgeSupport.ws"));
        REQUIRE_FALSE(bridge.IsWitcherScriptLoaded("MP_BridgeSupport.ws"));
    }
    
    SECTION("Function Calls")
    {
        auto result = bridge.CallWitcherScriptFunction("LoadAsset", "sword_steel");
        REQUIRE(result.success);
        REQUIRE(!result.data.empty());
        
        auto classResult = bridge.CallWitcherScriptFunction("MP_BridgeSupport", "LoadAsset", "armor_light");
        REQUIRE(classResult.success);
        REQUIRE(!classResult.data.empty());
        
        std::string simpleResult = bridge.GetWitcherScriptResult("LoadAsset", "potion_health");
        REQUIRE(!simpleResult.empty());
    }
    
    SECTION("Asset Management")
    {
        std::vector<uint8_t> weaponData;
        REQUIRE(bridge.LoadAssetFromWitcherScript("weapons/sword_steel", weaponData));
        REQUIRE(!weaponData.empty());
        
        std::vector<uint8_t> armorData;
        REQUIRE(bridge.LoadAssetFromWitcherScript("armor/armor_light", armorData));
        REQUIRE(!armorData.empty());
        
        REQUIRE(bridge.AssetExistsInWitcherScript("weapons/sword_steel"));
        REQUIRE_FALSE(bridge.AssetExistsInWitcherScript("nonexistent_asset"));
        
        auto assets = bridge.ListAssetsInWitcherScript("weapons");
        REQUIRE(!assets.empty());
    }
    
    SECTION("Game System Integration")
    {
        REQUIRE(bridge.ExecuteCombatAction(1, "ExecuteAttack", "1|1|{\"x\":10.0,\"y\":0.0,\"z\":5.0,\"w\":1.0}"));
        REQUIRE(bridge.CastSign(1, "CastSign", "1|1|{\"x\":5.0,\"y\":0.0,\"z\":10.0,\"w\":1.0}"));
        REQUIRE(bridge.UseAlchemyItem(1, "potion", "1|1"));
        REQUIRE(bridge.ManageInventory(1, "add", "1|1"));
    }
    
    SECTION("Quest System")
    {
        REQUIRE(bridge.CreateQuest("test_quest", "Test quest description|main"));
        REQUIRE(bridge.UpdateQuest("test_quest", "Test quest update"));
        REQUIRE(bridge.CompleteQuest("test_quest"));
        REQUIRE(bridge.GetQuestStatus("test_quest") == "active");
    }
    
    SECTION("NPC System")
    {
        REQUIRE(bridge.CreateNPC("test_npc", "human|{\"x\":15.0,\"y\":0.0,\"z\":20.0,\"w\":1.0}"));
        REQUIRE(bridge.UpdateNPC("test_npc", "Test NPC update"));
        REQUIRE(bridge.RemoveNPC("test_npc"));
        REQUIRE(!bridge.GetNPCData("test_npc").empty());
    }
    
    SECTION("World System")
    {
        REQUIRE(bridge.UpdateWorldState("Test world state"));
        REQUIRE(bridge.SetWeather("sunny"));
        REQUIRE(bridge.SetTimeOfDay(12.0f));
        REQUIRE(!bridge.GetWorldState().empty());
    }
    
    SECTION("Data Conversion")
    {
        Vector4F testVector = {1.0f, 2.0f, 3.0f, 4.0f};
        std::string vectorStr = bridge.Vector4FToWitcherScript(testVector);
        Vector4F convertedVector = bridge.WitcherScriptToVector4F(vectorStr);
        
        REQUIRE(convertedVector.x == Approx(testVector.x));
        REQUIRE(convertedVector.y == Approx(testVector.y));
        REQUIRE(convertedVector.z == Approx(testVector.z));
        REQUIRE(convertedVector.w == Approx(testVector.w));
    }
    
    SECTION("Event System")
    {
        bool eventReceived = false;
        bridge.RegisterCppCallback("TestEvent", [&eventReceived](const std::string& eventName, const std::string& eventData) {
            eventReceived = true;
        });
        
        bridge.ProcessWitcherScriptEvent("TestEvent", "test data");
        REQUIRE(eventReceived);
        
        bridge.UnregisterCppCallback("TestEvent");
    }
    
    SECTION("Error Handling")
    {
        // Test error handling
        std::vector<uint8_t> emptyData;
        REQUIRE_FALSE(bridge.LoadAssetFromWitcherScript("nonexistent_asset", emptyData));
        REQUIRE(bridge.HasError());
        REQUIRE(!bridge.GetLastError().empty());
        
        bridge.ClearError();
        REQUIRE_FALSE(bridge.HasError());
    }
    
    SECTION("Statistics")
    {
        REQUIRE(bridge.GetLoadedScriptsCount() >= 0);
        REQUIRE(bridge.GetRegisteredCallbacksCount() >= 0);
        REQUIRE(!bridge.GetBridgeStatistics().empty());
        REQUIRE(!bridge.GetWitcherScriptVersion().empty());
    }
    
    bridge.Shutdown();
}

// Test REDkitBridge with WitcherScript integration
TEST_CASE("REDkitBridge - WitcherScript Integration", "[redkit][witcherscript][integration]")
{
    auto& bridge = REDkitBridge::GetInstance();
    
    SECTION("Initialization with WitcherScript")
    {
        REQUIRE(bridge.Initialize());
        REQUIRE(bridge.IsInitialized());
        REQUIRE(bridge.IsREDkitAvailable());
    }
    
    SECTION("Asset Loading via WitcherScript")
    {
        std::vector<uint8_t> weaponData;
        REQUIRE(bridge.LoadWeaponData("sword_steel", weaponData));
        REQUIRE(!weaponData.empty());
        
        std::vector<uint8_t> armorData;
        REQUIRE(bridge.LoadArmorData("armor_light", armorData));
        REQUIRE(!armorData.empty());
        
        std::vector<uint8_t> itemData;
        REQUIRE(bridge.LoadItemData("potion_health", itemData));
        REQUIRE(!itemData.empty());
    }
    
    SECTION("Combat System via WitcherScript")
    {
        Vector4F targetPos = {10.0f, 0.0f, 5.0f, 1.0f};
        REQUIRE(bridge.ExecuteAttack(1, 1, 1, targetPos));
        REQUIRE(bridge.ExecuteDefense(1, 1, 1));
        REQUIRE(bridge.PlayAttackAnimation(1, 1));
        REQUIRE(bridge.PlayDefenseAnimation(1, 1));
    }
    
    SECTION("Signs System via WitcherScript")
    {
        Vector4F targetPos = {5.0f, 0.0f, 10.0f, 1.0f};
        REQUIRE(bridge.CastSign(1, 1, 1, targetPos));
        REQUIRE(bridge.CanCastSign(1, 1, 1));
        REQUIRE(bridge.GetSignCooldown(1, 1) >= 0.0f);
    }
    
    SECTION("Alchemy System via WitcherScript")
    {
        REQUIRE(bridge.UsePotion(1, "potion_health", 1));
        REQUIRE(bridge.UseBomb(1, "bomb_grapeshot", 1));
        REQUIRE(bridge.UseOil(1, "oil_beast", 1));
        REQUIRE(bridge.GetToxicity(1) >= 0.0f);
    }
    
    SECTION("Inventory System via WitcherScript")
    {
        REQUIRE(bridge.AddItem(1, 1, 1));
        REQUIRE(bridge.RemoveItem(1, 1, 1));
        REQUIRE(bridge.HasItem(1, 1));
        REQUIRE(bridge.GetItemCount(1, 1) >= 0);
        REQUIRE(bridge.GetInventoryWeight(1) >= 0.0f);
    }
    
    SECTION("Quest System via WitcherScript")
    {
        REQUIRE(bridge.CreateQuest("test_quest", "Test quest description"));
        REQUIRE(bridge.UpdateQuest("test_quest", "Test quest update"));
        REQUIRE(bridge.CompleteQuest("test_quest"));
        REQUIRE(bridge.GetQuestStatus("test_quest") == "completed");
    }
    
    SECTION("NPC System via WitcherScript")
    {
        Vector4F npcPos = {15.0f, 0.0f, 20.0f, 1.0f};
        REQUIRE(bridge.CreateNPC("test_npc", "human", npcPos));
        REQUIRE(bridge.UpdateNPC("test_npc", "Test NPC update"));
        REQUIRE(bridge.RemoveNPC("test_npc"));
    }
    
    SECTION("World System via WitcherScript")
    {
        REQUIRE(bridge.UpdateWorldState("Test world state"));
        REQUIRE(bridge.SetWeather("sunny"));
        REQUIRE(bridge.SetTimeOfDay(12.0f));
        REQUIRE(!bridge.GetWorldState().empty());
    }
    
    bridge.Shutdown();
}

// Test AssetLoader with WitcherScript integration
TEST_CASE("AssetLoader - WitcherScript Integration", "[assetloader][witcherscript][integration]")
{
    auto& loader = AssetLoader::GetInstance();
    
    SECTION("Initialization")
    {
        REQUIRE(loader.Initialize());
        REQUIRE(loader.IsInitialized());
    }
    
    SECTION("Load Weapon Data via WitcherScript")
    {
        AssetData weaponData;
        REQUIRE(loader.LoadWeaponData("sword_steel", weaponData));
        REQUIRE(weaponData.type == AssetType::Weapon);
        REQUIRE(weaponData.name == "sword_steel");
        REQUIRE(!weaponData.data.empty());
        REQUIRE(weaponData.originalSize > 0);
        REQUIRE(weaponData.checksum > 0);
    }
    
    SECTION("Load Armor Data via WitcherScript")
    {
        AssetData armorData;
        REQUIRE(loader.LoadArmorData("armor_light", armorData));
        REQUIRE(armorData.type == AssetType::Armor);
        REQUIRE(armorData.name == "armor_light");
        REQUIRE(!armorData.data.empty());
        REQUIRE(armorData.originalSize > 0);
        REQUIRE(armorData.checksum > 0);
    }
    
    SECTION("Load Item Data via WitcherScript")
    {
        AssetData itemData;
        REQUIRE(loader.LoadItemData("potion_health", itemData));
        REQUIRE(itemData.type == AssetType::Item);
        REQUIRE(itemData.name == "potion_health");
        REQUIRE(!itemData.data.empty());
        REQUIRE(itemData.originalSize > 0);
        REQUIRE(itemData.checksum > 0);
    }
    
    SECTION("Load NPC Data via WitcherScript")
    {
        AssetData npcData;
        REQUIRE(loader.LoadNPCData("human", npcData));
        REQUIRE(npcData.type == AssetType::NPC);
        REQUIRE(npcData.name == "human");
        REQUIRE(!npcData.data.empty());
        REQUIRE(npcData.originalSize > 0);
        REQUIRE(npcData.checksum > 0);
    }
    
    SECTION("Load Quest Data via WitcherScript")
    {
        AssetData questData;
        REQUIRE(loader.LoadQuestData("main_quest", questData));
        REQUIRE(questData.type == AssetType::Quest);
        REQUIRE(questData.name == "main_quest");
        REQUIRE(!questData.data.empty());
        REQUIRE(questData.originalSize > 0);
        REQUIRE(questData.checksum > 0);
    }
    
    SECTION("Asset Caching")
    {
        AssetData weaponData;
        REQUIRE(loader.LoadWeaponData("sword_steel", weaponData));
        
        // Load again - should come from cache
        AssetData cachedWeaponData;
        REQUIRE(loader.LoadWeaponData("sword_steel", cachedWeaponData));
        REQUIRE(cachedWeaponData.checksum == weaponData.checksum);
    }
    
    SECTION("Asset Validation")
    {
        AssetData weaponData;
        REQUIRE(loader.LoadWeaponData("sword_steel", weaponData));
        REQUIRE(loader.ValidateAsset(weaponData));
        
        // Corrupt the data
        weaponData.data[0] = 0xFF;
        REQUIRE_FALSE(loader.ValidateAsset(weaponData));
    }
    
    SECTION("Statistics")
    {
        REQUIRE(loader.GetLoadedAssetsCount() >= 0);
        REQUIRE(loader.GetCacheSize() >= 0);
        REQUIRE(!loader.GetLoaderStatistics().empty());
    }
    
    loader.Shutdown();
}

// Test integration between all components
TEST_CASE("Full Integration Test", "[integration][witcherscript]")
{
    auto& witcherScriptBridge = WitcherScriptBridge::GetInstance();
    auto& redkitBridge = REDkitBridge::GetInstance();
    auto& assetLoader = AssetLoader::GetInstance();
    
    SECTION("Initialize All Components")
    {
        REQUIRE(witcherScriptBridge.Initialize());
        REQUIRE(redkitBridge.Initialize());
        REQUIRE(assetLoader.Initialize());
    }
    
    SECTION("End-to-End Asset Loading")
    {
        // Load asset through WitcherScript
        std::vector<uint8_t> witcherScriptData;
        REQUIRE(witcherScriptBridge.LoadAssetFromWitcherScript("weapons/sword_steel", witcherScriptData));
        
        // Load same asset through REDkitBridge (which uses WitcherScript)
        std::vector<uint8_t> redkitData;
        REQUIRE(redkitBridge.LoadWeaponData("sword_steel", redkitData));
        
        // Load same asset through AssetLoader (which uses WitcherScript)
        AssetData assetData;
        REQUIRE(assetLoader.LoadWeaponData("sword_steel", assetData));
        
        // All should return the same data
        REQUIRE(witcherScriptData.size() == redkitData.size());
        REQUIRE(witcherScriptData.size() == assetData.data.size());
    }
    
    SECTION("End-to-End Game System")
    {
        // Test combat action through all layers
        Vector4F targetPos = {10.0f, 0.0f, 5.0f, 1.0f};
        
        // Direct WitcherScript call
        REQUIRE(witcherScriptBridge.ExecuteCombatAction(1, "ExecuteAttack", "1|1|{\"x\":10.0,\"y\":0.0,\"z\":5.0,\"w\":1.0}"));
        
        // REDkitBridge call (which uses WitcherScript)
        REQUIRE(redkitBridge.ExecuteAttack(1, 1, 1, targetPos));
    }
    
    SECTION("Event Propagation")
    {
        bool eventReceived = false;
        witcherScriptBridge.RegisterCppCallback("TestEvent", [&eventReceived](const std::string& eventName, const std::string& eventData) {
            eventReceived = true;
        });
        
        // Trigger event through WitcherScript
        witcherScriptBridge.ProcessWitcherScriptEvent("TestEvent", "test data");
        REQUIRE(eventReceived);
    }
    
    SECTION("Cleanup")
    {
        witcherScriptBridge.Shutdown();
        redkitBridge.Shutdown();
        assetLoader.Shutdown();
    }
}
