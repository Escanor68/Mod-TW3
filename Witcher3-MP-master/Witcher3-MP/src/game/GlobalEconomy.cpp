#include "game/GlobalEconomy.h"
#include "utils/Logger.h"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace Game
{
    // GlobalEconomy implementation
    GlobalEconomy::GlobalEconomy()
        : m_initialized(false), m_inflationRate(0.01f), m_maxPlayerWeight(1000),
          m_merchantRestockInterval(3600.0f), m_tradingEnabled(true), m_giftingEnabled(true),
          m_nextItemId(1), m_nextMerchantId(1), m_nextTransactionId(1)
    {
        m_lastUpdateTime = std::chrono::high_resolution_clock::now();
        m_lastRestockTime = m_lastUpdateTime;
        
        LOG_INFO("Global economy system created");
    }

    GlobalEconomy::~GlobalEconomy()
    {
        Shutdown();
        LOG_INFO("Global economy system destroyed");
    }

    bool GlobalEconomy::Initialize()
    {
        if (m_initialized)
        {
            return true;
        }

        LOG_INFO("Initializing global economy system...");

        // Initialize default items
        InitializeDefaultItems();
        
        // Initialize default merchants
        InitializeDefaultMerchants();

        m_initialized = true;
        LOG_INFO("Global economy system initialized");
        return true;
    }

    void GlobalEconomy::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        LOG_INFO("Shutting down global economy system...");
        
        // Save economy state
        SaveEconomyState();
        
        // Clear all data
        m_items.clear();
        m_playerEconomies.clear();
        m_merchants.clear();
        m_transactions.clear();
        
        m_initialized = false;
        LOG_INFO("Global economy system shutdown complete");
    }

    void GlobalEconomy::AddItem(const ItemData& item)
    {
        if (!m_initialized)
        {
            return;
        }

        ItemData itemCopy = item;
        if (itemCopy.itemId == 0)
        {
            itemCopy.itemId = m_nextItemId++;
        }

        // Calculate default prices
        if (itemCopy.prices.empty())
        {
            itemCopy.prices[CurrencyType::Gold] = itemCopy.value;
            itemCopy.prices[CurrencyType::Silver] = itemCopy.value * 10;
            itemCopy.prices[CurrencyType::Copper] = itemCopy.value * 100;
        }

        m_items[itemCopy.itemId] = itemCopy;
        m_stats.totalItems++;

        LOG_INFO("Added item: " + itemCopy.name + " (ID: " + std::to_string(itemCopy.itemId) + 
                ", Value: " + std::to_string(itemCopy.value) + ")");
    }

    void GlobalEconomy::RemoveItem(uint32_t itemId)
    {
        auto it = m_items.find(itemId);
        if (it != m_items.end())
        {
            m_items.erase(it);
            m_stats.totalItems--;
            LOG_INFO("Removed item ID: " + std::to_string(itemId));
        }
    }

    ItemData* GlobalEconomy::GetItem(uint32_t itemId)
    {
        auto it = m_items.find(itemId);
        if (it != m_items.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    std::vector<ItemData> GlobalEconomy::GetAllItems() const
    {
        std::vector<ItemData> items;
        for (const auto& pair : m_items)
        {
            items.push_back(pair.second);
        }
        return items;
    }

    std::vector<ItemData> GlobalEconomy::GetItemsByType(ItemType type) const
    {
        std::vector<ItemData> items;
        for (const auto& pair : m_items)
        {
            if (pair.second.type == type)
            {
                items.push_back(pair.second);
            }
        }
        return items;
    }

    std::vector<ItemData> GlobalEconomy::GetItemsByRarity(ItemRarity rarity) const
    {
        std::vector<ItemData> items;
        for (const auto& pair : m_items)
        {
            if (pair.second.rarity == rarity)
            {
                items.push_back(pair.second);
            }
        }
        return items;
    }

    void GlobalEconomy::AddPlayer(uint32_t playerId)
    {
        if (!m_initialized)
        {
            return;
        }

        PlayerEconomyData player;
        player.playerId = playerId;
        player.currencies[CurrencyType::Gold] = 100; // Starting gold
        player.lastUpdate = std::chrono::high_resolution_clock::now();
        
        m_playerEconomies[playerId] = player;
        
        LOG_INFO("Added player to economy: " + std::to_string(playerId));
    }

    void GlobalEconomy::RemovePlayer(uint32_t playerId)
    {
        auto it = m_playerEconomies.find(playerId);
        if (it != m_playerEconomies.end())
        {
            m_playerEconomies.erase(it);
            LOG_INFO("Removed player from economy: " + std::to_string(playerId));
        }
    }

    PlayerEconomyData* GlobalEconomy::GetPlayerEconomy(uint32_t playerId)
    {
        auto it = m_playerEconomies.find(playerId);
        if (it != m_playerEconomies.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    bool GlobalEconomy::AddCurrency(uint32_t playerId, CurrencyType currency, uint32_t amount)
    {
        PlayerEconomyData* player = GetPlayerEconomy(playerId);
        if (!player)
        {
            return false;
        }

        player->currencies[currency] += amount;
        player->lastUpdate = std::chrono::high_resolution_clock::now();
        
        UpdatePlayerWealth(playerId);
        
        LOG_DEBUG("Added " + std::to_string(amount) + " " + std::to_string(static_cast<int>(currency)) + 
                 " to player " + std::to_string(playerId));
        return true;
    }

    bool GlobalEconomy::RemoveCurrency(uint32_t playerId, CurrencyType currency, uint32_t amount)
    {
        PlayerEconomyData* player = GetPlayerEconomy(playerId);
        if (!player)
        {
            return false;
        }

        if (!CanPlayerAfford(playerId, currency, amount))
        {
            return false;
        }

        player->currencies[currency] -= amount;
        player->lastUpdate = std::chrono::high_resolution_clock::now();
        
        UpdatePlayerWealth(playerId);
        
        LOG_DEBUG("Removed " + std::to_string(amount) + " " + std::to_string(static_cast<int>(currency)) + 
                 " from player " + std::to_string(playerId));
        return true;
    }

    uint32_t GlobalEconomy::GetCurrency(uint32_t playerId, CurrencyType currency) const
    {
        const PlayerEconomyData* player = GetPlayerEconomy(playerId);
        if (!player)
        {
            return 0;
        }

        auto it = player->currencies.find(currency);
        if (it != player->currencies.end())
        {
            return it->second;
        }
        return 0;
    }

    bool GlobalEconomy::AddItemToInventory(uint32_t playerId, uint32_t itemId, uint32_t quantity)
    {
        PlayerEconomyData* player = GetPlayerEconomy(playerId);
        ItemData* item = GetItem(itemId);
        
        if (!player || !item)
        {
            return false;
        }

        // Check weight limit
        uint32_t additionalWeight = item->weight * quantity;
        if (player->totalWeight + additionalWeight > player->maxWeight)
        {
            return false;
        }

        player->inventory[itemId] += quantity;
        player->totalWeight += additionalWeight;
        player->lastUpdate = std::chrono::high_resolution_clock::now();
        
        LOG_DEBUG("Added " + std::to_string(quantity) + " " + item->name + 
                 " to player " + std::to_string(playerId) + " inventory");
        return true;
    }

    bool GlobalEconomy::RemoveItemFromInventory(uint32_t playerId, uint32_t itemId, uint32_t quantity)
    {
        PlayerEconomyData* player = GetPlayerEconomy(playerId);
        ItemData* item = GetItem(itemId);
        
        if (!player || !item)
        {
            return false;
        }

        auto it = player->inventory.find(itemId);
        if (it == player->inventory.end() || it->second < quantity)
        {
            return false;
        }

        it->second -= quantity;
        if (it->second == 0)
        {
            player->inventory.erase(it);
        }

        player->totalWeight -= item->weight * quantity;
        player->lastUpdate = std::chrono::high_resolution_clock::now();
        
        LOG_DEBUG("Removed " + std::to_string(quantity) + " " + item->name + 
                 " from player " + std::to_string(playerId) + " inventory");
        return true;
    }

    uint32_t GlobalEconomy::GetItemQuantity(uint32_t playerId, uint32_t itemId) const
    {
        const PlayerEconomyData* player = GetPlayerEconomy(playerId);
        if (!player)
        {
            return 0;
        }

        auto it = player->inventory.find(itemId);
        if (it != player->inventory.end())
        {
            return it->second;
        }
        return 0;
    }

    void GlobalEconomy::AddMerchant(const MerchantData& merchant)
    {
        if (!m_initialized)
        {
            return;
        }

        MerchantData merchantCopy = merchant;
        if (merchantCopy.merchantId == 0)
        {
            merchantCopy.merchantId = m_nextMerchantId++;
        }

        m_merchants[merchantCopy.merchantId] = merchantCopy;
        m_stats.activeMerchants++;

        LOG_INFO("Added merchant: " + merchantCopy.name + " (ID: " + std::to_string(merchantCopy.merchantId) + 
                ", Location: " + merchantCopy.location + ")");
    }

    void GlobalEconomy::RemoveMerchant(uint32_t merchantId)
    {
        auto it = m_merchants.find(merchantId);
        if (it != m_merchants.end())
        {
            m_merchants.erase(it);
            m_stats.activeMerchants--;
            LOG_INFO("Removed merchant ID: " + std::to_string(merchantId));
        }
    }

    MerchantData* GlobalEconomy::GetMerchant(uint32_t merchantId)
    {
        auto it = m_merchants.find(merchantId);
        if (it != m_merchants.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    std::vector<MerchantData> GlobalEconomy::GetAllMerchants() const
    {
        std::vector<MerchantData> merchants;
        for (const auto& pair : m_merchants)
        {
            merchants.push_back(pair.second);
        }
        return merchants;
    }

    std::vector<MerchantData> GlobalEconomy::GetMerchantsInLocation(const std::string& location) const
    {
        std::vector<MerchantData> merchants;
        for (const auto& pair : m_merchants)
        {
            if (pair.second.location == location)
            {
                merchants.push_back(pair.second);
            }
        }
        return merchants;
    }

    void GlobalEconomy::RestockMerchant(uint32_t merchantId)
    {
        MerchantData* merchant = GetMerchant(merchantId);
        if (!merchant || !merchant->isActive)
        {
            return;
        }

        // Restock inventory with random items
        for (const auto& pair : m_items)
        {
            const ItemData& item = pair.second;
            if (item.isTradeable && merchant->inventory.find(item.itemId) == merchant->inventory.end())
            {
                // Random chance to add item to merchant inventory
                if (rand() % 100 < 30) // 30% chance
                {
                    merchant->inventory[item.itemId] = rand() % 5 + 1; // 1-5 quantity
                }
            }
        }

        merchant->lastRestock = std::chrono::high_resolution_clock::now();
        
        if (m_merchantRestockedCallback)
        {
            m_merchantRestockedCallback(merchantId);
        }

        LOG_DEBUG("Restocked merchant " + std::to_string(merchantId));
    }

    void GlobalEconomy::UpdateMerchantPrices(uint32_t merchantId)
    {
        MerchantData* merchant = GetMerchant(merchantId);
        if (!merchant)
        {
            return;
        }

        // Update buy and sell prices based on current economy
        for (const auto& pair : merchant->inventory)
        {
            uint32_t itemId = pair.first;
            ItemData* item = GetItem(itemId);
            if (item)
            {
                uint32_t buyPrice = CalculateBuyPrice(itemId, merchantId);
                uint32_t sellPrice = CalculateSellPrice(itemId, merchantId);
                
                merchant->buyPrices[itemId] = buyPrice;
                merchant->sellPrices[itemId] = sellPrice;
            }
        }

        LOG_DEBUG("Updated prices for merchant " + std::to_string(merchantId));
    }

    bool GlobalEconomy::BuyItem(uint32_t playerId, uint32_t merchantId, uint32_t itemId, uint32_t quantity)
    {
        if (!m_tradingEnabled)
        {
            return false;
        }

        PlayerEconomyData* player = GetPlayerEconomy(playerId);
        MerchantData* merchant = GetMerchant(merchantId);
        ItemData* item = GetItem(itemId);
        
        if (!player || !merchant || !item || !merchant->isActive)
        {
            return false;
        }

        // Check if merchant has the item
        auto merchantIt = merchant->inventory.find(itemId);
        if (merchantIt == merchant->inventory.end() || merchantIt->second < quantity)
        {
            return false;
        }

        // Calculate price
        uint32_t totalPrice = CalculateBuyPrice(itemId, merchantId) * quantity;
        
        // Check if player can afford it
        if (!CanPlayerAfford(playerId, CurrencyType::Gold, totalPrice))
        {
            return false;
        }

        // Check if player can carry it
        if (!CanPlayerCarry(playerId, itemId, quantity))
        {
            return false;
        }

        // Process transaction
        TransactionData transaction;
        transaction.transactionId = m_nextTransactionId++;
        transaction.playerId = playerId;
        transaction.merchantId = merchantId;
        transaction.type = TransactionType::Buy;
        transaction.itemId = itemId;
        transaction.quantity = quantity;
        transaction.price = totalPrice;
        transaction.currency = CurrencyType::Gold;
        transaction.timestamp = std::chrono::high_resolution_clock::now();
        transaction.isCompleted = false;

        if (ProcessTransaction(transaction))
        {
            // Remove currency from player
            RemoveCurrency(playerId, CurrencyType::Gold, totalPrice);
            
            // Add item to player inventory
            AddItemToInventory(playerId, itemId, quantity);
            
            // Remove item from merchant inventory
            merchantIt->second -= quantity;
            if (merchantIt->second == 0)
            {
                merchant->inventory.erase(merchantIt);
            }
            
            // Add gold to merchant
            merchant->goldAmount += totalPrice;
            
            transaction.isCompleted = true;
            m_transactions.push_back(transaction);
            m_stats.totalTransactions++;
            
            if (m_transactionCompletedCallback)
            {
                m_transactionCompletedCallback(transaction);
            }

            LOG_INFO("Player " + std::to_string(playerId) + " bought " + std::to_string(quantity) + 
                    " " + item->name + " from merchant " + std::to_string(merchantId) + 
                    " for " + std::to_string(totalPrice) + " gold");
            return true;
        }

        return false;
    }

    bool GlobalEconomy::SellItem(uint32_t playerId, uint32_t merchantId, uint32_t itemId, uint32_t quantity)
    {
        if (!m_tradingEnabled)
        {
            return false;
        }

        PlayerEconomyData* player = GetPlayerEconomy(playerId);
        MerchantData* merchant = GetMerchant(merchantId);
        ItemData* item = GetItem(itemId);
        
        if (!player || !merchant || !item || !merchant->isActive)
        {
            return false;
        }

        // Check if player has the item
        if (GetItemQuantity(playerId, itemId) < quantity)
        {
            return false;
        }

        // Check if item is sellable
        if (!item->isSellable)
        {
            return false;
        }

        // Calculate price
        uint32_t totalPrice = CalculateSellPrice(itemId, merchantId) * quantity;
        
        // Check if merchant can afford it
        if (merchant->goldAmount < totalPrice)
        {
            return false;
        }

        // Process transaction
        TransactionData transaction;
        transaction.transactionId = m_nextTransactionId++;
        transaction.playerId = playerId;
        transaction.merchantId = merchantId;
        transaction.type = TransactionType::Sell;
        transaction.itemId = itemId;
        transaction.quantity = quantity;
        transaction.price = totalPrice;
        transaction.currency = CurrencyType::Gold;
        transaction.timestamp = std::chrono::high_resolution_clock::now();
        transaction.isCompleted = false;

        if (ProcessTransaction(transaction))
        {
            // Remove item from player inventory
            RemoveItemFromInventory(playerId, itemId, quantity);
            
            // Add currency to player
            AddCurrency(playerId, CurrencyType::Gold, totalPrice);
            
            // Add item to merchant inventory
            merchant->inventory[itemId] += quantity;
            
            // Remove gold from merchant
            merchant->goldAmount -= totalPrice;
            
            transaction.isCompleted = true;
            m_transactions.push_back(transaction);
            m_stats.totalTransactions++;
            
            if (m_transactionCompletedCallback)
            {
                m_transactionCompletedCallback(transaction);
            }

            LOG_INFO("Player " + std::to_string(playerId) + " sold " + std::to_string(quantity) + 
                    " " + item->name + " to merchant " + std::to_string(merchantId) + 
                    " for " + std::to_string(totalPrice) + " gold");
            return true;
        }

        return false;
    }

    bool GlobalEconomy::TradeItems(uint32_t player1Id, uint32_t player2Id, 
                                  const std::map<uint32_t, uint32_t>& items1, 
                                  const std::map<uint32_t, uint32_t>& items2)
    {
        if (!m_tradingEnabled)
        {
            return false;
        }

        PlayerEconomyData* player1 = GetPlayerEconomy(player1Id);
        PlayerEconomyData* player2 = GetPlayerEconomy(player2Id);
        
        if (!player1 || !player2)
        {
            return false;
        }

        // Validate trade
        for (const auto& pair : items1)
        {
            if (GetItemQuantity(player1Id, pair.first) < pair.second)
            {
                return false;
            }
        }
        
        for (const auto& pair : items2)
        {
            if (GetItemQuantity(player2Id, pair.first) < pair.second)
            {
                return false;
            }
        }

        // Process trade
        for (const auto& pair : items1)
        {
            RemoveItemFromInventory(player1Id, pair.first, pair.second);
            AddItemToInventory(player2Id, pair.first, pair.second);
        }
        
        for (const auto& pair : items2)
        {
            RemoveItemFromInventory(player2Id, pair.first, pair.second);
            AddItemToInventory(player1Id, pair.first, pair.second);
        }

        LOG_INFO("Players " + std::to_string(player1Id) + " and " + std::to_string(player2Id) + " traded items");
        return true;
    }

    bool GlobalEconomy::GiftItem(uint32_t fromPlayerId, uint32_t toPlayerId, uint32_t itemId, uint32_t quantity)
    {
        if (!m_giftingEnabled)
        {
            return false;
        }

        PlayerEconomyData* fromPlayer = GetPlayerEconomy(fromPlayerId);
        PlayerEconomyData* toPlayer = GetPlayerEconomy(toPlayerId);
        
        if (!fromPlayer || !toPlayer)
        {
            return false;
        }

        // Check if from player has the item
        if (GetItemQuantity(fromPlayerId, itemId) < quantity)
        {
            return false;
        }

        // Check if to player can carry it
        if (!CanPlayerCarry(toPlayerId, itemId, quantity))
        {
            return false;
        }

        // Process gift
        RemoveItemFromInventory(fromPlayerId, itemId, quantity);
        AddItemToInventory(toPlayerId, itemId, quantity);

        LOG_INFO("Player " + std::to_string(fromPlayerId) + " gifted " + std::to_string(quantity) + 
                " " + std::to_string(itemId) + " to player " + std::to_string(toPlayerId));
        return true;
    }

    uint32_t GlobalEconomy::CalculateBuyPrice(uint32_t itemId, uint32_t merchantId) const
    {
        const ItemData* item = GetItem(itemId);
        const MerchantData* merchant = GetMerchant(merchantId);
        
        if (!item || !merchant)
        {
            return 0;
        }

        // Check if merchant has specific price
        auto it = merchant->buyPrices.find(itemId);
        if (it != merchant->buyPrices.end())
        {
            return it->second;
        }

        // Calculate base price
        uint32_t basePrice = CalculateBasePrice(itemId);
        
        // Apply merchant multiplier
        float merchantMultiplier = GetMerchantMultiplier(merchantId);
        
        return static_cast<uint32_t>(basePrice * merchantMultiplier);
    }

    uint32_t GlobalEconomy::CalculateSellPrice(uint32_t itemId, uint32_t merchantId) const
    {
        const ItemData* item = GetItem(itemId);
        const MerchantData* merchant = GetMerchant(merchantId);
        
        if (!item || !merchant)
        {
            return 0;
        }

        // Check if merchant has specific price
        auto it = merchant->sellPrices.find(itemId);
        if (it != merchant->sellPrices.end())
        {
            return it->second;
        }

        // Calculate base price (usually 50% of buy price)
        uint32_t basePrice = CalculateBasePrice(itemId);
        
        // Apply merchant multiplier and sell discount
        float merchantMultiplier = GetMerchantMultiplier(merchantId);
        float sellDiscount = 0.5f; // 50% of buy price
        
        return static_cast<uint32_t>(basePrice * merchantMultiplier * sellDiscount);
    }

    uint32_t GlobalEconomy::CalculateItemValue(uint32_t itemId) const
    {
        const ItemData* item = GetItem(itemId);
        if (!item)
        {
            return 0;
        }

        return item->value;
    }

    void GlobalEconomy::UpdateItemPrices()
    {
        // Update all item prices based on inflation
        for (auto& pair : m_items)
        {
            ItemData& item = pair.second;
            item.value = static_cast<uint32_t>(item.value * (1.0f + m_inflationRate));
            
            // Update currency prices
            for (auto& pricePair : item.prices)
            {
                pricePair.second = static_cast<uint32_t>(pricePair.second * (1.0f + m_inflationRate));
            }
        }

        // Update merchant prices
        for (auto& pair : m_merchants)
        {
            UpdateMerchantPrices(pair.first);
        }

        LOG_DEBUG("Updated all item prices with inflation rate: " + std::to_string(m_inflationRate));
    }

    void GlobalEconomy::ApplyInflation(float rate)
    {
        m_inflationRate = rate;
        UpdateItemPrices();
        
        LOG_INFO("Applied inflation rate: " + std::to_string(rate * 100.0f) + "%");
    }

    void GlobalEconomy::ProcessEconomyUpdate(float deltaTime)
    {
        if (!m_initialized)
        {
            return;
        }

        auto now = std::chrono::high_resolution_clock::now();
        
        // Check if it's time to restock merchants
        float timeSinceLastRestock = std::chrono::duration<float>(now - m_lastRestockTime).count();
        if (timeSinceLastRestock >= m_merchantRestockInterval)
        {
            for (auto& pair : m_merchants)
            {
                RestockMerchant(pair.first);
            }
            m_lastRestockTime = now;
        }
        
        // Update economy statistics
        UpdateEconomyStats();
        
        // Check economy health
        CheckEconomyHealth();
        
        // Cleanup old transactions
        CleanupOldTransactions();
        
        m_lastUpdateTime = now;
    }

    void GlobalEconomy::RebalanceEconomy()
    {
        // Reset all prices to base values
        for (auto& pair : m_items)
        {
            ItemData& item = pair.second;
            item.value = CalculateItemValue(item.itemId);
        }
        
        // Update merchant prices
        for (auto& pair : m_merchants)
        {
            UpdateMerchantPrices(pair.first);
        }
        
        LOG_INFO("Economy rebalanced");
    }

    void GlobalEconomy::ResetEconomy()
    {
        // Reset all player currencies to default
        for (auto& pair : m_playerEconomies)
        {
            PlayerEconomyData& player = pair.second;
            player.currencies[CurrencyType::Gold] = 100;
            player.currencies[CurrencyType::Silver] = 0;
            player.currencies[CurrencyType::Copper] = 0;
            player.inventory.clear();
            player.totalWeight = 0;
        }
        
        // Reset all merchant inventories
        for (auto& pair : m_merchants)
        {
            MerchantData& merchant = pair.second;
            merchant.inventory.clear();
            merchant.goldAmount = 1000;
        }
        
        // Clear transaction history
        m_transactions.clear();
        
        LOG_INFO("Economy reset");
    }

    void GlobalEconomy::SaveEconomyState()
    {
        // This would save economy state to file
        // For now, just log
        LOG_DEBUG("Economy state saved");
    }

    void GlobalEconomy::LoadEconomyState()
    {
        // This would load economy state from file
        // For now, just log
        LOG_DEBUG("Economy state loaded");
    }

    // Configuration methods
    void GlobalEconomy::SetInflationRate(float rate)
    {
        m_inflationRate = std::max(0.0f, std::min(1.0f, rate));
    }

    void GlobalEconomy::SetMaxPlayerWeight(uint32_t maxWeight)
    {
        m_maxPlayerWeight = maxWeight;
    }

    void GlobalEconomy::SetMerchantRestockInterval(float interval)
    {
        m_merchantRestockInterval = std::max(60.0f, interval);
    }

    void GlobalEconomy::EnableTrading(bool enable)
    {
        m_tradingEnabled = enable;
    }

    void GlobalEconomy::EnableGifting(bool enable)
    {
        m_giftingEnabled = enable;
    }

    GlobalEconomy::EconomyStats GlobalEconomy::GetStats() const
    {
        return m_stats;
    }

    void GlobalEconomy::ResetStats()
    {
        m_stats.Reset();
    }

    void GlobalEconomy::PrintStats() const
    {
        LOG_INFO("=== Global Economy Statistics ===");
        LOG_INFO("Total transactions: " + std::to_string(m_stats.totalTransactions));
        LOG_INFO("Total gold circulation: " + std::to_string(m_stats.totalGoldCirculation));
        LOG_INFO("Active merchants: " + std::to_string(m_stats.activeMerchants));
        LOG_INFO("Total items: " + std::to_string(m_stats.totalItems));
        LOG_INFO("Average transaction value: " + std::to_string(m_stats.averageTransactionValue));
        LOG_INFO("Inflation rate: " + std::to_string(m_stats.inflationRate * 100.0f) + "%");
        LOG_INFO("Most traded item: " + std::to_string(m_stats.mostTradedItem));
        LOG_INFO("Richest player: " + std::to_string(m_stats.richestPlayer));
        LOG_INFO("=================================");
    }

    // Callback setters
    void GlobalEconomy::SetTransactionCompletedCallback(TransactionCompletedCallback callback)
    {
        m_transactionCompletedCallback = callback;
    }

    void GlobalEconomy::SetPlayerWealthChangedCallback(PlayerWealthChangedCallback callback)
    {
        m_playerWealthChangedCallback = callback;
    }

    void GlobalEconomy::SetMerchantRestockedCallback(MerchantRestockedCallback callback)
    {
        m_merchantRestockedCallback = callback;
    }

    void GlobalEconomy::SetEconomyCrashedCallback(EconomyCrashedCallback callback)
    {
        m_economyCrashedCallback = callback;
    }

    // Private methods
    void GlobalEconomy::InitializeDefaultItems()
    {
        // Create some default items
        ItemData sword = EconomyUtils::CreateWeapon("Iron Sword", ItemRarity::Common, 50, 10);
        AddItem(sword);
        
        ItemData armor = EconomyUtils::CreateArmor("Leather Armor", ItemRarity::Common, 30, 15);
        AddItem(armor);
        
        ItemData potion = EconomyUtils::CreateConsumable("Health Potion", ItemRarity::Common, 10, 1);
        AddItem(potion);
        
        ItemData ore = EconomyUtils::CreateMaterial("Iron Ore", ItemRarity::Common, 5, 2);
        AddItem(ore);
    }

    void GlobalEconomy::InitializeDefaultMerchants()
    {
        // Create some default merchants
        MerchantData weaponMerchant = EconomyUtils::CreateWeaponMerchant("Blacksmith", "Novigrad");
        AddMerchant(weaponMerchant);
        
        MerchantData armorMerchant = EconomyUtils::CreateArmorMerchant("Armorer", "Oxenfurt");
        AddMerchant(armorMerchant);
        
        MerchantData generalMerchant = EconomyUtils::CreateGeneralMerchant("Trader", "Velen");
        AddMerchant(generalMerchant);
    }

    bool GlobalEconomy::ValidateTransaction(const TransactionData& transaction) const
    {
        return transaction.playerId != 0 && transaction.merchantId != 0 && 
               transaction.itemId != 0 && transaction.quantity > 0;
    }

    bool GlobalEconomy::ProcessTransaction(const TransactionData& transaction)
    {
        if (!ValidateTransaction(transaction))
        {
            return false;
        }

        // Transaction processing logic would go here
        return true;
    }

    void GlobalEconomy::UpdatePlayerWealth(uint32_t playerId)
    {
        PlayerEconomyData* player = GetPlayerEconomy(playerId);
        if (!player)
        {
            return;
        }

        // Calculate total wealth
        uint32_t totalWealth = 0;
        for (const auto& pair : player->currencies)
        {
            totalWealth += pair.second;
        }
        
        // Add item values
        for (const auto& pair : player->inventory)
        {
            const ItemData* item = GetItem(pair.first);
            if (item)
            {
                totalWealth += item->value * pair.second;
            }
        }

        if (m_playerWealthChangedCallback)
        {
            m_playerWealthChangedCallback(playerId, totalWealth);
        }
    }

    void GlobalEconomy::UpdateEconomyStats()
    {
        // Update total gold circulation
        m_stats.totalGoldCirculation = 0;
        for (const auto& pair : m_playerEconomies)
        {
            m_stats.totalGoldCirculation += GetCurrency(pair.first, CurrencyType::Gold);
        }
        
        for (const auto& pair : m_merchants)
        {
            m_stats.totalGoldCirculation += pair.second.goldAmount;
        }
    }

    void GlobalEconomy::CheckEconomyHealth()
    {
        // Check for economy crashes or imbalances
        if (m_stats.totalGoldCirculation == 0)
        {
            if (m_economyCrashedCallback)
            {
                m_economyCrashedCallback();
            }
        }
    }

    void GlobalEconomy::CleanupOldTransactions()
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto it = m_transactions.begin();
        
        while (it != m_transactions.end())
        {
            float age = std::chrono::duration<float>(now - it->timestamp).count();
            if (age > 86400.0f) // 24 hours
            {
                it = m_transactions.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    uint32_t GlobalEconomy::CalculateBasePrice(uint32_t itemId) const
    {
        const ItemData* item = GetItem(itemId);
        if (!item)
        {
            return 0;
        }

        return item->value;
    }

    float GlobalEconomy::GetRarityMultiplier(ItemRarity rarity) const
    {
        switch (rarity)
        {
            case ItemRarity::Common: return 1.0f;
            case ItemRarity::Uncommon: return 1.5f;
            case ItemRarity::Rare: return 2.0f;
            case ItemRarity::Epic: return 3.0f;
            case ItemRarity::Legendary: return 5.0f;
            case ItemRarity::Artifact: return 10.0f;
            default: return 1.0f;
        }
    }

    float GlobalEconomy::GetTypeMultiplier(ItemType type) const
    {
        switch (type)
        {
            case ItemType::Weapon: return 1.2f;
            case ItemType::Armor: return 1.1f;
            case ItemType::Consumable: return 0.8f;
            case ItemType::Material: return 0.9f;
            case ItemType::Quest: return 0.5f;
            case ItemType::Misc: return 1.0f;
            default: return 1.0f;
        }
    }

    float GlobalEconomy::GetMerchantMultiplier(uint32_t merchantId) const
    {
        // Different merchants have different price multipliers
        return 1.0f; // Default multiplier
    }

    bool GlobalEconomy::CanPlayerAfford(uint32_t playerId, CurrencyType currency, uint32_t amount) const
    {
        return GetCurrency(playerId, currency) >= amount;
    }

    bool GlobalEconomy::CanPlayerCarry(uint32_t playerId, uint32_t itemId, uint32_t quantity) const
    {
        const PlayerEconomyData* player = GetPlayerEconomy(playerId);
        const ItemData* item = GetItem(itemId);
        
        if (!player || !item)
        {
            return false;
        }

        uint32_t additionalWeight = item->weight * quantity;
        return player->totalWeight + additionalWeight <= player->maxWeight;
    }

    bool GlobalEconomy::IsItemTradeable(uint32_t itemId) const
    {
        const ItemData* item = GetItem(itemId);
        return item && item->isTradeable;
    }

    bool GlobalEconomy::IsMerchantActive(uint32_t merchantId) const
    {
        const MerchantData* merchant = GetMerchant(merchantId);
        return merchant && merchant->isActive;
    }

    // Economy utilities implementation
    namespace EconomyUtils
    {
        ItemData CreateWeapon(const std::string& name, ItemRarity rarity, uint32_t value, uint32_t weight)
        {
            ItemData item;
            item.name = name;
            item.type = ItemType::Weapon;
            item.rarity = rarity;
            item.value = value;
            item.weight = weight;
            item.isTradeable = true;
            item.isSellable = true;
            return item;
        }

        ItemData CreateArmor(const std::string& name, ItemRarity rarity, uint32_t value, uint32_t weight)
        {
            ItemData item;
            item.name = name;
            item.type = ItemType::Armor;
            item.rarity = rarity;
            item.value = value;
            item.weight = weight;
            item.isTradeable = true;
            item.isSellable = true;
            return item;
        }

        ItemData CreateConsumable(const std::string& name, ItemRarity rarity, uint32_t value, uint32_t weight)
        {
            ItemData item;
            item.name = name;
            item.type = ItemType::Consumable;
            item.rarity = rarity;
            item.value = value;
            item.weight = weight;
            item.stackSize = 10;
            item.isTradeable = true;
            item.isSellable = true;
            return item;
        }

        ItemData CreateMaterial(const std::string& name, ItemRarity rarity, uint32_t value, uint32_t weight)
        {
            ItemData item;
            item.name = name;
            item.type = ItemType::Material;
            item.rarity = rarity;
            item.value = value;
            item.weight = weight;
            item.stackSize = 100;
            item.isTradeable = true;
            item.isSellable = true;
            return item;
        }

        MerchantData CreateMerchant(const std::string& name, const std::string& location, uint32_t goldAmount)
        {
            MerchantData merchant;
            merchant.name = name;
            merchant.location = location;
            merchant.goldAmount = goldAmount;
            merchant.maxGold = goldAmount * 10;
            merchant.isActive = true;
            return merchant;
        }

        MerchantData CreateWeaponMerchant(const std::string& name, const std::string& location)
        {
            MerchantData merchant = CreateMerchant(name, location, 2000);
            return merchant;
        }

        MerchantData CreateArmorMerchant(const std::string& name, const std::string& location)
        {
            MerchantData merchant = CreateMerchant(name, location, 1500);
            return merchant;
        }

        MerchantData CreateGeneralMerchant(const std::string& name, const std::string& location)
        {
            MerchantData merchant = CreateMerchant(name, location, 1000);
            return merchant;
        }

        uint32_t CalculateItemValue(const ItemData& item)
        {
            float rarityMultiplier = GetRarityMultiplier(item.rarity);
            float typeMultiplier = GetTypeMultiplier(item.type);
            return static_cast<uint32_t>(item.value * rarityMultiplier * typeMultiplier);
        }

        float GetRarityMultiplier(ItemRarity rarity)
        {
            switch (rarity)
            {
                case ItemRarity::Common: return 1.0f;
                case ItemRarity::Uncommon: return 1.5f;
                case ItemRarity::Rare: return 2.0f;
                case ItemRarity::Epic: return 3.0f;
                case ItemRarity::Legendary: return 5.0f;
                case ItemRarity::Artifact: return 10.0f;
                default: return 1.0f;
            }
        }

        float GetTypeMultiplier(ItemType type)
        {
            switch (type)
            {
                case ItemType::Weapon: return 1.2f;
                case ItemType::Armor: return 1.1f;
                case ItemType::Consumable: return 0.8f;
                case ItemType::Material: return 0.9f;
                case ItemType::Quest: return 0.5f;
                case ItemType::Misc: return 1.0f;
                default: return 1.0f;
            }
        }

        float CalculatePlayerWealth(const PlayerEconomyData& player)
        {
            float wealth = 0.0f;
            
            // Add currency values
            for (const auto& pair : player.currencies)
            {
                wealth += static_cast<float>(pair.second);
            }
            
            return wealth;
        }

        float CalculateMerchantValue(const MerchantData& merchant)
        {
            float value = static_cast<float>(merchant.goldAmount);
            
            // Add inventory values
            for (const auto& pair : merchant.inventory)
            {
                // This would need access to item data to calculate value
                value += static_cast<float>(pair.second * 10); // Placeholder
            }
            
            return value;
        }

        std::vector<uint32_t> GetTopWealthyPlayers(const std::map<uint32_t, PlayerEconomyData>& players)
        {
            std::vector<std::pair<uint32_t, float>> playerWealths;
            
            for (const auto& pair : players)
            {
                float wealth = CalculatePlayerWealth(pair.second);
                playerWealths.push_back({pair.first, wealth});
            }
            
            // Sort by wealth (descending)
            std::sort(playerWealths.begin(), playerWealths.end(), 
                     [](const std::pair<uint32_t, float>& a, const std::pair<uint32_t, float>& b) {
                         return a.second > b.second;
                     });
            
            std::vector<uint32_t> topPlayers;
            for (const auto& pair : playerWealths)
            {
                topPlayers.push_back(pair.first);
            }
            
            return topPlayers;
        }

        bool ValidateItemData(const ItemData& item)
        {
            return !item.name.empty() && item.value > 0 && item.weight >= 0;
        }

        bool ValidatePlayerEconomy(const PlayerEconomyData& player)
        {
            return player.playerId != 0 && player.maxWeight > 0;
        }

        bool ValidateMerchantData(const MerchantData& merchant)
        {
            return !merchant.name.empty() && !merchant.location.empty() && merchant.maxGold > 0;
        }

        void PrintItemAnalysis(const ItemData& item)
        {
            LOG_INFO("=== Item Analysis ===");
            LOG_INFO("Name: " + item.name);
            LOG_INFO("Type: " + std::to_string(static_cast<int>(item.type)));
            LOG_INFO("Rarity: " + std::to_string(static_cast<int>(item.rarity)));
            LOG_INFO("Value: " + std::to_string(item.value));
            LOG_INFO("Weight: " + std::to_string(item.weight));
            LOG_INFO("Tradeable: " + (item.isTradeable ? "Yes" : "No"));
            LOG_INFO("Sellable: " + (item.isSellable ? "Yes" : "No"));
            LOG_INFO("===================");
        }

        void PrintPlayerEconomyAnalysis(const PlayerEconomyData& player)
        {
            LOG_INFO("=== Player Economy Analysis ===");
            LOG_INFO("Player ID: " + std::to_string(player.playerId));
            LOG_INFO("Total Weight: " + std::to_string(player.totalWeight) + "/" + std::to_string(player.maxWeight));
            LOG_INFO("Inventory Items: " + std::to_string(player.inventory.size()));
            LOG_INFO("Wealth: " + std::to_string(CalculatePlayerWealth(player)));
            LOG_INFO("=============================");
        }

        void PrintMerchantAnalysis(const MerchantData& merchant)
        {
            LOG_INFO("=== Merchant Analysis ===");
            LOG_INFO("Name: " + merchant.name);
            LOG_INFO("Location: " + merchant.location);
            LOG_INFO("Gold: " + std::to_string(merchant.goldAmount) + "/" + std::to_string(merchant.maxGold));
            LOG_INFO("Inventory Items: " + std::to_string(merchant.inventory.size()));
            LOG_INFO("Active: " + (merchant.isActive ? "Yes" : "No"));
            LOG_INFO("=======================");
        }
    }
}
