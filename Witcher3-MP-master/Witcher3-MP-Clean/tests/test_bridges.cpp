#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include "integration/TW3ModInterface.h"
#include "integration/REDkitBridge.h"
#include "integration/AssetLoader.h"
#include "integration/WitcherScriptBridge.h"
#include "optimization/NetworkOptimizer.h"
#include "optimization/DataCompression.h"
#include <vector>
#include <string>

using namespace TW3Integration;
using namespace Optimization;

// Test TW3ModInterface with REDkit integration
TEST_CASE("TW3ModInterface - REDkit Integration", "[integration]")
{
    auto& interface = TW3ModInterface::GetInstance();
    
    SECTION("Initialize with REDkit bridge")
    {
        REQUIRE(interface.Initialize() == true);
        REQUIRE(interface.IsInitialized() == true);
    }
    
    SECTION("Load asset from REDkit")
    {
        std::vector<uint8_t> data;
        REQUIRE(interface.LoadAsset("sword_steel", data) == true);
        REQUIRE(data.size() > 0);
    }
    
    SECTION("Serialize game state with REDkit")
    {
        std::string gameState = interface.SerializeGameState();
        REQUIRE(gameState.find("players") != std::string::npos);
    }
    
    SECTION("Send message to REDkit")
    {
        interface.SendToMod("test_message");
        // This should not throw an exception
        REQUIRE(true);
    }
    
    SECTION("Process message from REDkit")
    {
        interface.ProcessModMessage("{\"type\":\"test\",\"data\":\"test_data\"}");
        // This should not throw an exception
        REQUIRE(true);
    }
}

// Test REDkitBridge functionality
TEST_CASE("REDkitBridge - Basic Functionality", "[redkit]")
{
    auto& bridge = REDkitBridge::GetInstance();
    
    SECTION("Initialize bridge")
    {
        REQUIRE(bridge.Initialize() == true);
        REQUIRE(bridge.IsInitialized() == true);
    }
    
    SECTION("Create multiplayer quest")
    {
        REQUIRE(bridge.CreateMultiplayerQuest("test_quest", "Test quest description") == true);
    }
    
    SECTION("Create multiplayer UI")
    {
        REQUIRE(bridge.CreateMultiplayerUI() == true);
    }
    
    SECTION("Create multiplayer NPC")
    {
        Vector4F position = {100.0f, 200.0f, 300.0f, 1.0f};
        REQUIRE(bridge.CreateMultiplayerNPC("human", position) == true);
    }
    
    SECTION("Create multiplayer item")
    {
        Vector4F position = {150.0f, 250.0f, 350.0f, 1.0f};
        REQUIRE(bridge.CreateMultiplayerItem("sword_steel", position) == true);
    }
    
    SECTION("Export game data")
    {
        std::string gameData = bridge.ExportGameData();
        REQUIRE(gameData.empty() == false);
    }
    
    SECTION("Import game data")
    {
        std::string jsonData = "{\"players\":[],\"npcs\":[],\"items\":[]}";
        REQUIRE(bridge.ImportGameData(jsonData) == true);
    }
}

// Test REDkitBridge asset loading
TEST_CASE("REDkitBridge - Asset Loading", "[redkit][assets]")
{
    auto& bridge = REDkitBridge::GetInstance();
    
    SECTION("Load weapon data")
    {
        std::vector<uint8_t> data;
        REQUIRE(bridge.LoadWeaponData("sword_steel", data) == true);
        REQUIRE(data.size() > 0);
    }
    
    SECTION("Load armor data")
    {
        std::vector<uint8_t> data;
        REQUIRE(bridge.LoadArmorData("armor_light", data) == true);
        REQUIRE(data.size() > 0);
    }
    
    SECTION("Load item data")
    {
        std::vector<uint8_t> data;
        REQUIRE(bridge.LoadItemData("potion", data) == true);
        REQUIRE(data.size() > 0);
    }
    
    SECTION("Load NPC data")
    {
        std::vector<uint8_t> data;
        REQUIRE(bridge.LoadNPCData("human", data) == true);
        REQUIRE(data.size() > 0);
    }
    
    SECTION("Load quest data")
    {
        std::vector<uint8_t> data;
        REQUIRE(bridge.LoadQuestData("main_quest", data) == true);
        REQUIRE(data.size() > 0);
    }
}

// Test REDkitBridge combat system
TEST_CASE("REDkitBridge - Combat System", "[redkit][combat]")
{
    auto& bridge = REDkitBridge::GetInstance();
    
    SECTION("Execute attack")
    {
        Vector4F targetPosition = {100.0f, 200.0f, 300.0f, 1.0f};
        REQUIRE(bridge.ExecuteAttack(1, 1, 1, targetPosition) == true);
    }
    
    SECTION("Execute defense")
    {
        REQUIRE(bridge.ExecuteDefense(1, 1, 1) == true);
    }
    
    SECTION("Play attack animation")
    {
        REQUIRE(bridge.PlayAttackAnimation(1, 1) == true);
    }
    
    SECTION("Play defense animation")
    {
        REQUIRE(bridge.PlayDefenseAnimation(1, 1) == true);
    }
}

// Test REDkitBridge signs system
TEST_CASE("REDkitBridge - Signs System", "[redkit][signs]")
{
    auto& bridge = REDkitBridge::GetInstance();
    
    SECTION("Cast sign")
    {
        Vector4F targetPosition = {100.0f, 200.0f, 300.0f, 1.0f};
        REQUIRE(bridge.CastSign(1, 1, 1, targetPosition) == true);
    }
    
    SECTION("Check if can cast sign")
    {
        REQUIRE(bridge.CanCastSign(1, 1, 1) == true);
    }
    
    SECTION("Get sign cooldown")
    {
        float cooldown = bridge.GetSignCooldown(1, 1);
        REQUIRE(cooldown >= 0.0f);
    }
}

// Test REDkitBridge alchemy system
TEST_CASE("REDkitBridge - Alchemy System", "[redkit][alchemy]")
{
    auto& bridge = REDkitBridge::GetInstance();
    
    SECTION("Create potion")
    {
        REQUIRE(bridge.CreatePotion(1, "health_potion", 5) == true);
    }
    
    SECTION("Create bomb")
    {
        REQUIRE(bridge.CreateBomb(1, "dimeritium_bomb", 3) == true);
    }
    
    SECTION("Create oil")
    {
        REQUIRE(bridge.CreateOil(1, "beast_oil", 2) == true);
    }
    
    SECTION("Use alchemy item")
    {
        REQUIRE(bridge.UseAlchemyItem(1, 1) == true);
    }
}

// Test REDkitBridge inventory system
TEST_CASE("REDkitBridge - Inventory System", "[redkit][inventory]")
{
    auto& bridge = REDkitBridge::GetInstance();
    
    SECTION("Add item to inventory")
    {
        REQUIRE(bridge.AddItemToInventory(1, 1, 5) == true);
    }
    
    SECTION("Remove item from inventory")
    {
        REQUIRE(bridge.RemoveItemFromInventory(1, 1, 2) == true);
    }
    
    SECTION("Check if has item")
    {
        REQUIRE(bridge.HasItemInInventory(1, 1) == true);
    }
    
    SECTION("Get item quantity")
    {
        uint32_t quantity = bridge.GetItemQuantity(1, 1);
        REQUIRE(quantity >= 0);
    }
    
    SECTION("Get inventory weight")
    {
        float weight = bridge.GetInventoryWeight(1);
        REQUIRE(weight >= 0.0f);
    }
}

// Test AssetLoader functionality
TEST_CASE("AssetLoader - Basic Functionality", "[assets]")
{
    AssetLoader loader;
    
    SECTION("Initialize loader")
    {
        REQUIRE(loader.Initialize() == true);
    }
    
    SECTION("Load weapon data from REDkit")
    {
        AssetData assetData;
        REQUIRE(loader.LoadWeaponData("sword_steel", assetData) == true);
        REQUIRE(assetData.name == "sword_steel");
        REQUIRE(assetData.type == AssetType::Weapon);
        REQUIRE(assetData.data.size() > 0);
    }
    
    SECTION("Load armor data from REDkit")
    {
        AssetData assetData;
        REQUIRE(loader.LoadArmorData("armor_light", assetData) == true);
        REQUIRE(assetData.name == "armor_light");
        REQUIRE(assetData.type == AssetType::Armor);
        REQUIRE(assetData.data.size() > 0);
    }
    
    SECTION("Load item data from REDkit")
    {
        AssetData assetData;
        REQUIRE(loader.LoadItemData("potion", assetData) == true);
        REQUIRE(assetData.name == "potion");
        REQUIRE(assetData.type == AssetType::Item);
        REQUIRE(assetData.data.size() > 0);
    }
    
    SECTION("Load NPC data from REDkit")
    {
        AssetData assetData;
        REQUIRE(loader.LoadNPCData("human", assetData) == true);
        REQUIRE(assetData.name == "human");
        REQUIRE(assetData.type == AssetType::NPC);
        REQUIRE(assetData.data.size() > 0);
    }
    
    SECTION("Load quest data from REDkit")
    {
        AssetData assetData;
        REQUIRE(loader.LoadQuestData("main_quest", assetData) == true);
        REQUIRE(assetData.name == "main_quest");
        REQUIRE(assetData.type == AssetType::Quest);
        REQUIRE(assetData.data.size() > 0);
    }
}

// Test AssetLoader bundle functionality
TEST_CASE("AssetLoader - Bundle Functionality", "[assets][bundles]")
{
    AssetLoader loader;
    REQUIRE(loader.Initialize() == true);
    
    SECTION("Load bundle")
    {
        REQUIRE(loader.LoadBundle("test.bundle") == true);
        REQUIRE(loader.IsBundleLoaded("test.bundle") == true);
    }
    
    SECTION("Unload bundle")
    {
        loader.LoadBundle("test.bundle");
        REQUIRE(loader.UnloadBundle("test.bundle") == true);
        REQUIRE(loader.IsBundleLoaded("test.bundle") == false);
    }
    
    SECTION("Get loaded bundles")
    {
        loader.LoadBundle("test1.bundle");
        loader.LoadBundle("test2.bundle");
        auto bundles = loader.GetLoadedBundles();
        REQUIRE(bundles.size() >= 2);
    }
}

// Test AssetLoader compression
TEST_CASE("AssetLoader - Compression", "[assets][compression]")
{
    AssetLoader loader;
    REQUIRE(loader.Initialize() == true);
    
    SECTION("Compress asset")
    {
        AssetData assetData;
        assetData.name = "test_asset";
        assetData.type = AssetType::Item;
        assetData.data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        assetData.isCompressed = false;
        
        REQUIRE(loader.CompressAsset(assetData) == true);
        REQUIRE(assetData.isCompressed == true);
        REQUIRE(assetData.originalSize > 0);
        REQUIRE(assetData.compressedSize > 0);
    }
    
    SECTION("Decompress asset")
    {
        AssetData assetData;
        assetData.name = "test_asset";
        assetData.type = AssetType::Item;
        assetData.data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        assetData.isCompressed = true;
        
        REQUIRE(loader.DecompressAsset(assetData) == true);
        REQUIRE(assetData.isCompressed == false);
    }
    
    SECTION("Optimize asset")
    {
        AssetData assetData;
        assetData.name = "test_asset";
        assetData.type = AssetType::Item;
        assetData.data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        assetData.isCompressed = false;
        
        REQUIRE(loader.OptimizeAsset(assetData) == true);
    }
}

// Test AssetLoader statistics
TEST_CASE("AssetLoader - Statistics", "[assets][statistics]")
{
    AssetLoader loader;
    REQUIRE(loader.Initialize() == true);
    
    SECTION("Get total loaded assets")
    {
        AssetData assetData;
        loader.LoadWeaponData("sword_steel", assetData);
        REQUIRE(loader.GetTotalLoadedAssets() > 0);
    }
    
    SECTION("Get total loaded size")
    {
        AssetData assetData;
        loader.LoadWeaponData("sword_steel", assetData);
        REQUIRE(loader.GetTotalLoadedSize() > 0);
    }
    
    SECTION("Get compression ratio")
    {
        AssetData assetData;
        loader.LoadWeaponData("sword_steel", assetData);
        float ratio = loader.GetCompressionRatio();
        REQUIRE(ratio >= 0.0f);
        REQUIRE(ratio <= 1.0f);
    }
    
    SECTION("Get asset statistics")
    {
        AssetData assetData;
        loader.LoadWeaponData("sword_steel", assetData);
        std::string stats = loader.GetAssetStatistics();
        REQUIRE(stats.find("Asset Statistics") != std::string::npos);
    }
}

// Test NetworkOptimizer with real compression
TEST_CASE("NetworkOptimizer - Real Compression", "[network][compression]")
{
    NetworkOptimizer optimizer;
    
    SECTION("Compress message with real compression")
    {
        Networking::message<MessageType> original;
        original.body = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        original.header.size = original.body.size();
        
        auto compressed = optimizer.CompressMessage(original);
        REQUIRE(compressed.body.size() > 0);
        REQUIRE(compressed.header.size == compressed.body.size());
    }
    
    SECTION("Decompress message with real compression")
    {
        Networking::message<MessageType> original;
        original.body = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        original.header.size = original.body.size();
        
        auto compressed = optimizer.CompressMessage(original);
        auto decompressed = optimizer.DecompressMessage(compressed);
        
        REQUIRE(decompressed.body.size() == original.body.size());
        REQUIRE(decompressed.body == original.body);
    }
    
    SECTION("Compression efficiency")
    {
        // Create a larger message for better compression
        std::vector<uint8_t> largeData(1000, 42); // 1000 bytes of the same value
        
        Networking::message<MessageType> original;
        original.body = largeData;
        original.header.size = original.body.size();
        
        auto compressed = optimizer.CompressMessage(original);
        
        // With LZ4 compression, we should see some reduction
        REQUIRE(compressed.body.size() <= original.body.size());
    }
}

// Test DataCompression integration
TEST_CASE("DataCompression - Integration", "[compression]")
{
    auto& compression = DataCompression::GetInstance();
    
    SECTION("Initialize compression")
    {
        REQUIRE(compression.Initialize() == true);
    }
    
    SECTION("Compress and decompress data")
    {
        std::vector<uint8_t> original = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        
        auto compressed = compression.Compress(original, CompressionAlgorithm::LZ4, CompressionLevel::Balanced);
        REQUIRE(compressed.size() > 0);
        
        auto decompressed = compression.Decompress(compressed, CompressionAlgorithm::LZ4);
        REQUIRE(decompressed.size() == original.size());
        REQUIRE(decompressed == original);
    }
    
    SECTION("Different compression algorithms")
    {
        std::vector<uint8_t> original = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        
        auto zlibCompressed = compression.Compress(original, CompressionAlgorithm::ZLIB, CompressionLevel::Balanced);
        auto lz4Compressed = compression.Compress(original, CompressionAlgorithm::LZ4, CompressionLevel::Balanced);
        
        REQUIRE(zlibCompressed.size() > 0);
        REQUIRE(lz4Compressed.size() > 0);
        
        // Both should decompress to the same original data
        auto zlibDecompressed = compression.Decompress(zlibCompressed, CompressionAlgorithm::ZLIB);
        auto lz4Decompressed = compression.Decompress(lz4Compressed, CompressionAlgorithm::LZ4);
        
        REQUIRE(zlibDecompressed == original);
        REQUIRE(lz4Decompressed == original);
    }
}

// Test error handling
TEST_CASE("Error Handling", "[error]")
{
    SECTION("TW3ModInterface error handling")
    {
        auto& interface = TW3ModInterface::GetInstance();
        
        // Test with invalid data
        std::vector<uint8_t> data;
        // This should handle errors gracefully
        interface.LoadAsset("nonexistent_asset", data);
    }
    
    SECTION("REDkitBridge error handling")
    {
        auto& bridge = REDkitBridge::GetInstance();
        
        // Test with invalid parameters
        Vector4F position = {0.0f, 0.0f, 0.0f, 1.0f};
        bridge.CreateMultiplayerNPC("invalid_npc_type", position);
        // This should handle errors gracefully
    }
    
    SECTION("AssetLoader error handling")
    {
        AssetLoader loader;
        
        // Test without initialization
        AssetData assetData;
        loader.LoadWeaponData("sword_steel", assetData);
        // This should handle errors gracefully
    }
}

// Test integration between components
TEST_CASE("Component Integration", "[integration]")
{
    SECTION("TW3ModInterface with REDkitBridge")
    {
        auto& interface = TW3ModInterface::GetInstance();
        auto& bridge = REDkitBridge::GetInstance();
        
        REQUIRE(interface.Initialize() == true);
        REQUIRE(bridge.IsInitialized() == true);
        
        // Test that they work together
        std::string gameState = interface.SerializeGameState();
        REQUIRE(gameState.empty() == false);
    }
    
    SECTION("AssetLoader with REDkitBridge")
    {
        AssetLoader loader;
        auto& bridge = REDkitBridge::GetInstance();
        
        REQUIRE(loader.Initialize() == true);
        REQUIRE(bridge.IsInitialized() == true);
        
        // Test that they work together
        AssetData assetData;
        REQUIRE(loader.LoadWeaponData("sword_steel", assetData) == true);
    }
    
    SECTION("NetworkOptimizer with DataCompression")
    {
        NetworkOptimizer optimizer;
        auto& compression = DataCompression::GetInstance();
        
        REQUIRE(compression.Initialize() == true);
        
        // Test that they work together
        Networking::message<MessageType> original;
        original.body = {1, 2, 3, 4, 5};
        original.header.size = original.body.size();
        
        auto compressed = optimizer.CompressMessage(original);
        REQUIRE(compressed.body.size() > 0);
    }
}

// Performance tests
TEST_CASE("Performance Tests", "[performance]")
{
    SECTION("Asset loading performance")
    {
        AssetLoader loader;
        REQUIRE(loader.Initialize() == true);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 100; ++i)
        {
            AssetData assetData;
            loader.LoadWeaponData("sword_steel", assetData);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Should complete within reasonable time (adjust as needed)
        REQUIRE(duration.count() < 5000); // 5 seconds
    }
    
    SECTION("Compression performance")
    {
        auto& compression = DataCompression::GetInstance();
        REQUIRE(compression.Initialize() == true);
        
        std::vector<uint8_t> largeData(10000, 42);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 100; ++i)
        {
            auto compressed = compression.Compress(largeData, CompressionAlgorithm::LZ4, CompressionLevel::Balanced);
            auto decompressed = compression.Decompress(compressed, CompressionAlgorithm::LZ4);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Should complete within reasonable time
        REQUIRE(duration.count() < 10000); // 10 seconds
    }
}

int main(int argc, char* argv[])
{
    return Catch::Session().run(argc, argv);
}
