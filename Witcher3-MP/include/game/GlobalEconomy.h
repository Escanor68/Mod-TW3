#pragma once

#include "Common.h"
#include <vector>
#include <map>
#include <string>
#include <chrono>
#include <functional>

namespace Game
{
    // Currency types
    enum class CurrencyType
    {
        Gold = 0,
        Silver = 1,
        Copper = 2,
        Crowns = 3
    };

    // Item rarity
    enum class ItemRarity
    {
        Common = 0,
        Uncommon = 1,
        Rare = 2,
        Epic = 3,
        Legendary = 4,
        Artifact = 5
    };

    // Item types
    enum class ItemType
    {
        Weapon = 0,
        Armor = 1,
        Consumable = 2,
        Material = 3,
        Quest = 4,
        Misc = 5
    };

    // Item data
    struct ItemData
    {
        uint32_t itemId;
        std::string name;
        std::string description;
        ItemType type;
        ItemRarity rarity;
        uint32_t value;
        uint32_t weight;
        uint32_t stackSize;
        bool isTradeable;
        bool isSellable;
        std::map<CurrencyType, uint32_t> prices;
        
        ItemData() : itemId(0), type(ItemType::Misc), rarity(ItemRarity::Common),
                    value(0), weight(0), stackSize(1), isTradeable(true), isSellable(true) {}
    };

    // Player economy data
    struct PlayerEconomyData
    {
        uint32_t playerId;
        std::map<CurrencyType, uint32_t> currencies;
        std::map<uint32_t, uint32_t> inventory; // itemId -> quantity
        uint32_t totalWeight;
        uint32_t maxWeight;
        std::vector<uint32_t> transactionHistory;
        std::chrono::high_resolution_clock::time_point lastUpdate;
        
        PlayerEconomyData() : playerId(0), totalWeight(0), maxWeight(1000) {}
    };

    // NPC merchant data
    struct MerchantData
    {
        uint32_t merchantId;
        std::string name;
        std::string location;
        std::map<uint32_t, uint32_t> inventory; // itemId -> quantity
        std::map<uint32_t, uint32_t> buyPrices; // itemId -> price
        std::map<uint32_t, uint32_t> sellPrices; // itemId -> price
        uint32_t goldAmount;
        uint32_t maxGold;
        bool isActive;
        std::chrono::high_resolution_clock::time_point lastRestock;
        
        MerchantData() : merchantId(0), goldAmount(1000), maxGold(10000), isActive(true) {}
    };

    // Transaction data
    struct TransactionData
    {
        uint32_t transactionId;
        uint32_t playerId;
        uint32_t merchantId;
        TransactionType type;
        uint32_t itemId;
        uint32_t quantity;
        uint32_t price;
        CurrencyType currency;
        std::chrono::high_resolution_clock::time_point timestamp;
        bool isCompleted;
        
        TransactionData() : transactionId(0), playerId(0), merchantId(0), type(TransactionType::Buy),
                          itemId(0), quantity(0), price(0), currency(CurrencyType::Gold), isCompleted(false) {}
    };

    // Transaction types
    enum class TransactionType
    {
        Buy = 0,
        Sell = 1,
        Trade = 2,
        Gift = 3,
        Loot = 4
    };

    // Economy statistics
    struct EconomyStats
    {
        uint32_t totalTransactions = 0;
        uint32_t totalGoldCirculation = 0;
        uint32_t activeMerchants = 0;
        uint32_t totalItems = 0;
        float averageTransactionValue = 0.0f;
        float inflationRate = 0.0f;
        uint32_t mostTradedItem = 0;
        uint32_t richestPlayer = 0;
        
        void Reset()
        {
            totalTransactions = 0;
            totalGoldCirculation = 0;
            activeMerchants = 0;
            totalItems = 0;
            averageTransactionValue = 0.0f;
            inflationRate = 0.0f;
            mostTradedItem = 0;
            richestPlayer = 0;
        }
    };

    // Global economy system
    class GlobalEconomy
    {
    public:
        GlobalEconomy();
        ~GlobalEconomy();

        // Initialize system
        bool Initialize();
        void Shutdown();

        // Item management
        void AddItem(const ItemData& item);
        void RemoveItem(uint32_t itemId);
        ItemData* GetItem(uint32_t itemId);
        std::vector<ItemData> GetAllItems() const;
        std::vector<ItemData> GetItemsByType(ItemType type) const;
        std::vector<ItemData> GetItemsByRarity(ItemRarity rarity) const;

        // Player economy
        void AddPlayer(uint32_t playerId);
        void RemovePlayer(uint32_t playerId);
        PlayerEconomyData* GetPlayerEconomy(uint32_t playerId);
        bool AddCurrency(uint32_t playerId, CurrencyType currency, uint32_t amount);
        bool RemoveCurrency(uint32_t playerId, CurrencyType currency, uint32_t amount);
        uint32_t GetCurrency(uint32_t playerId, CurrencyType currency) const;
        bool AddItemToInventory(uint32_t playerId, uint32_t itemId, uint32_t quantity);
        bool RemoveItemFromInventory(uint32_t playerId, uint32_t itemId, uint32_t quantity);
        uint32_t GetItemQuantity(uint32_t playerId, uint32_t itemId) const;

        // Merchant management
        void AddMerchant(const MerchantData& merchant);
        void RemoveMerchant(uint32_t merchantId);
        MerchantData* GetMerchant(uint32_t merchantId);
        std::vector<MerchantData> GetAllMerchants() const;
        std::vector<MerchantData> GetMerchantsInLocation(const std::string& location) const;
        void RestockMerchant(uint32_t merchantId);
        void UpdateMerchantPrices(uint32_t merchantId);

        // Trading
        bool BuyItem(uint32_t playerId, uint32_t merchantId, uint32_t itemId, uint32_t quantity);
        bool SellItem(uint32_t playerId, uint32_t merchantId, uint32_t itemId, uint32_t quantity);
        bool TradeItems(uint32_t player1Id, uint32_t player2Id, const std::map<uint32_t, uint32_t>& items1, const std::map<uint32_t, uint32_t>& items2);
        bool GiftItem(uint32_t fromPlayerId, uint32_t toPlayerId, uint32_t itemId, uint32_t quantity);

        // Price calculation
        uint32_t CalculateBuyPrice(uint32_t itemId, uint32_t merchantId) const;
        uint32_t CalculateSellPrice(uint32_t itemId, uint32_t merchantId) const;
        uint32_t CalculateItemValue(uint32_t itemId) const;
        void UpdateItemPrices();
        void ApplyInflation(float rate);

        // Economy management
        void ProcessEconomyUpdate(float deltaTime);
        void RebalanceEconomy();
        void ResetEconomy();
        void SaveEconomyState();
        void LoadEconomyState();

        // Configuration
        void SetInflationRate(float rate);
        void SetMaxPlayerWeight(uint32_t maxWeight);
        void SetMerchantRestockInterval(float interval);
        void EnableTrading(bool enable);
        void EnableGifting(bool enable);

        // Statistics
        EconomyStats GetStats() const;
        void ResetStats();
        void PrintStats() const;

        // Callbacks
        using TransactionCompletedCallback = std::function<void(const TransactionData&)>;
        using PlayerWealthChangedCallback = std::function<void(uint32_t, uint32_t)>;
        using MerchantRestockedCallback = std::function<void(uint32_t)>;
        using EconomyCrashedCallback = std::function<void()>;
        
        void SetTransactionCompletedCallback(TransactionCompletedCallback callback);
        void SetPlayerWealthChangedCallback(PlayerWealthChangedCallback callback);
        void SetMerchantRestockedCallback(MerchantRestockedCallback callback);
        void SetEconomyCrashedCallback(EconomyCrashedCallback callback);

    private:
        // Internal methods
        bool ValidateTransaction(const TransactionData& transaction) const;
        bool ProcessTransaction(const TransactionData& transaction);
        void UpdatePlayerWealth(uint32_t playerId);
        void UpdateEconomyStats();
        void CheckEconomyHealth();
        void CleanupOldTransactions();
        
        // Price calculation helpers
        uint32_t CalculateBasePrice(uint32_t itemId) const;
        float GetRarityMultiplier(ItemRarity rarity) const;
        float GetTypeMultiplier(ItemType type) const;
        float GetMerchantMultiplier(uint32_t merchantId) const;
        
        // Validation
        bool CanPlayerAfford(uint32_t playerId, CurrencyType currency, uint32_t amount) const;
        bool CanPlayerCarry(uint32_t playerId, uint32_t itemId, uint32_t quantity) const;
        bool IsItemTradeable(uint32_t itemId) const;
        bool IsMerchantActive(uint32_t merchantId) const;

        // Member variables
        bool m_initialized;
        std::map<uint32_t, ItemData> m_items;
        std::map<uint32_t, PlayerEconomyData> m_playerEconomies;
        std::map<uint32_t, MerchantData> m_merchants;
        std::vector<TransactionData> m_transactions;
        
        // Configuration
        float m_inflationRate;
        uint32_t m_maxPlayerWeight;
        float m_merchantRestockInterval;
        bool m_tradingEnabled;
        bool m_giftingEnabled;
        
        // Statistics
        EconomyStats m_stats;
        
        // Callbacks
        TransactionCompletedCallback m_transactionCompletedCallback;
        PlayerWealthChangedCallback m_playerWealthChangedCallback;
        MerchantRestockedCallback m_merchantRestockedCallback;
        EconomyCrashedCallback m_economyCrashedCallback;
        
        // Timing
        std::chrono::high_resolution_clock::time_point m_lastUpdateTime;
        std::chrono::high_resolution_clock::time_point m_lastRestockTime;
        uint32_t m_nextItemId;
        uint32_t m_nextMerchantId;
        uint32_t m_nextTransactionId;
    };

    // Economy utilities
    namespace EconomyUtils
    {
        // Item creation
        ItemData CreateWeapon(const std::string& name, ItemRarity rarity, uint32_t value, uint32_t weight);
        ItemData CreateArmor(const std::string& name, ItemRarity rarity, uint32_t value, uint32_t weight);
        ItemData CreateConsumable(const std::string& name, ItemRarity rarity, uint32_t value, uint32_t weight);
        ItemData CreateMaterial(const std::string& name, ItemRarity rarity, uint32_t value, uint32_t weight);
        
        // Merchant creation
        MerchantData CreateMerchant(const std::string& name, const std::string& location, uint32_t goldAmount);
        MerchantData CreateWeaponMerchant(const std::string& name, const std::string& location);
        MerchantData CreateArmorMerchant(const std::string& name, const std::string& location);
        MerchantData CreateGeneralMerchant(const std::string& name, const std::string& location);
        
        // Price calculation
        uint32_t CalculateItemValue(const ItemData& item);
        float GetRarityMultiplier(ItemRarity rarity);
        float GetTypeMultiplier(ItemType type);
        
        // Economy analysis
        float CalculatePlayerWealth(const PlayerEconomyData& player);
        float CalculateMerchantValue(const MerchantData& merchant);
        std::vector<uint32_t> GetTopWealthyPlayers(const std::map<uint32_t, PlayerEconomyData>& players);
        
        // Validation
        bool ValidateItemData(const ItemData& item);
        bool ValidatePlayerEconomy(const PlayerEconomyData& player);
        bool ValidateMerchantData(const MerchantData& merchant);
        
        // Statistics
        void PrintItemAnalysis(const ItemData& item);
        void PrintPlayerEconomyAnalysis(const PlayerEconomyData& player);
        void PrintMerchantAnalysis(const MerchantData& merchant);
    }
}
