#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>
#include <map>

// Test inventory system
TEST_CASE("Inventory Management", "[inventory]")
{
    SECTION("Item Addition")
    {
        // Test adding items to inventory
        std::map<std::string, int> inventory;
        inventory["sword"] = 1;
        inventory["potion"] = 3;
        
        REQUIRE(inventory["sword"] == 1);
        REQUIRE(inventory["potion"] == 3);
    }
    
    SECTION("Item Removal")
    {
        // Test removing items from inventory
        std::map<std::string, int> inventory;
        inventory["sword"] = 1;
        inventory["potion"] = 3;
        
        inventory["potion"]--;
        REQUIRE(inventory["potion"] == 2);
        
        inventory.erase("sword");
        REQUIRE(inventory.find("sword") == inventory.end());
    }
    
    SECTION("Weight Calculation")
    {
        // Test inventory weight calculation
        std::map<std::string, std::pair<int, float>> items; // item -> (quantity, weight)
        items["sword"] = {1, 2.5f};
        items["potion"] = {3, 0.5f};
        
        float totalWeight = 0.0f;
        for (const auto& item : items)
        {
            totalWeight += item.second.first * item.second.second;
        }
        
        REQUIRE(totalWeight == 4.0f); // 1*2.5 + 3*0.5 = 4.0
    }
}

TEST_CASE("Item Properties", "[items]")
{
    SECTION("Item Rarity")
    {
        enum class Rarity { Common, Uncommon, Rare, Epic, Legendary };
        
        Rarity swordRarity = Rarity::Uncommon;
        REQUIRE(static_cast<int>(swordRarity) >= 0);
    }
    
    SECTION("Item Durability")
    {
        struct Item {
            std::string name;
            int durability;
            int maxDurability;
        };
        
        Item sword{"Iron Sword", 100, 100};
        REQUIRE(sword.durability == sword.maxDurability);
    }
}
