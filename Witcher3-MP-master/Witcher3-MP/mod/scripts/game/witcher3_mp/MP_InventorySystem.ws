// Witcher3-MP Inventory System
// Implements synchronized inventory system for multiplayer

using game;
using game::player;
using game::quests;
using game::ui;
using game::inventory;
using game::stats;
using game::effects;

// Item categories
enum class ItemCategory
{
    None = 0,
    Weapon = 1,
    Armor = 2,
    Consumable = 3,
    Ingredient = 4,
    Quest = 5,
    Misc = 6
}

// Item rarity
enum class ItemRarity
{
    Common = 1,
    Uncommon = 2,
    Rare = 3,
    Epic = 4,
    Legendary = 5
}

// Item data structure
class MP_ItemData
{
    public let id: CName;
    public let name: String;
    public let description: String;
    public let category: ItemCategory;
    public let rarity: ItemRarity;
    public let value: int32;
    public let weight: float;
    public let stackSize: int32;
    public let isEquippable: bool;
    public let isConsumable: bool;
    public let isTradeable: bool;
    public let effects: array<CName>;
    public let requirements: array<CName>;
    
    public func Initialize(itemId: CName, itemName: String, itemDescription: String, itemCategory: ItemCategory, itemRarity: ItemRarity, itemValue: int32, itemWeight: float)
    {
        this.id = itemId;
        this.name = itemName;
        this.description = itemDescription;
        this.category = itemCategory;
        this.rarity = itemRarity;
        this.value = itemValue;
        this.weight = itemWeight;
        this.stackSize = 1;
        this.isEquippable = false;
        this.isConsumable = false;
        this.isTradeable = true;
        this.effects = [];
        this.requirements = [];
    }
    
    public func SetStackSize(size: int32)
    {
        this.stackSize = size;
    }
    
    public func SetEquippable(equippable: bool)
    {
        this.isEquippable = equippable;
    }
    
    public func SetConsumable(consumable: bool)
    {
        this.isConsumable = consumable;
    }
    
    public func SetTradeable(tradeable: bool)
    {
        this.isTradeable = tradeable;
    }
    
    public func AddEffect(effect: CName)
    {
        ArrayPush(this.effects, effect);
    }
    
    public func AddRequirement(requirement: CName)
    {
        ArrayPush(this.requirements, requirement);
    }
}

// Inventory slot
class MP_InventorySlot
{
    public let item: ref<MP_ItemData>;
    public let quantity: int32;
    public let isEquipped: bool;
    public let slotIndex: int32;
    
    public func Initialize(slotIdx: int32)
    {
        this.item = null;
        this.quantity = 0;
        this.isEquipped = false;
        this.slotIndex = slotIdx;
    }
    
    public func IsEmpty() -> bool
    {
        return !IsDefined(this.item) || this.quantity <= 0;
    }
    
    public func CanAddItem(itemData: ref<MP_ItemData>, count: int32) -> bool
    {
        if this.IsEmpty()
        {
            return true;
        }
        
        if this.item.id == itemData.id
        {
            return this.quantity + count <= this.item.stackSize;
        }
        
        return false;
    }
    
    public func AddItem(itemData: ref<MP_ItemData>, count: int32) -> bool
    {
        if !this.CanAddItem(itemData, count)
        {
            return false;
        }
        
        if this.IsEmpty()
        {
            this.item = itemData;
            this.quantity = count;
        }
        else
        {
            this.quantity += count;
        }
        
        return true;
    }
    
    public func RemoveItem(count: int32) -> bool
    {
        if this.quantity < count
        {
            return false;
        }
        
        this.quantity -= count;
        
        if this.quantity <= 0
        {
            this.item = null;
            this.quantity = 0;
            this.isEquipped = false;
        }
        
        return true;
    }
}

// Main Inventory System class
class MP_InventorySystem
{
    private let m_items: array<ref<MP_ItemData>>;
    private let m_inventorySlots: array<ref<MP_InventorySlot>>;
    private let m_maxSlots: int32;
    private let m_maxWeight: float;
    private let m_currentWeight: float;
    private let m_isInitialized: bool;
    
    // Initialize the inventory system
    public func Initialize()
    {
        if this.m_isInitialized
        {
            return;
        }
        
        this.m_maxSlots = 100;
        this.m_maxWeight = 200.0;
        this.m_currentWeight = 0.0;
        
        // Initialize inventory slots
        this.InitializeInventorySlots();
        
        // Initialize items database
        this.InitializeItemsDatabase();
        
        this.m_isInitialized = true;
        LogChannel(n"Witcher3-MP", "Inventory system initialized");
    }
    
    // Initialize inventory slots
    private func InitializeInventorySlots()
    {
        let i: int32 = 0;
        while i < this.m_maxSlots
        {
            let slot: ref<MP_InventorySlot> = new MP_InventorySlot();
            slot.Initialize(i);
            ArrayPush(this.m_inventorySlots, slot);
            i += 1;
        }
        
        LogChannel(n"Witcher3-MP", s"Initialized \(this.m_maxSlots) inventory slots");
    }
    
    // Initialize items database
    private func InitializeItemsDatabase()
    {
        // Weapons
        this.InitializeWeapons();
        
        // Armor
        this.InitializeArmor();
        
        // Consumables
        this.InitializeConsumables();
        
        // Ingredients
        this.InitializeIngredients();
        
        // Quest items
        this.InitializeQuestItems();
        
        LogChannel(n"Witcher3-MP", s"Initialized \(ArraySize(this.m_items)) items");
    }
    
    // Initialize weapons
    private func InitializeWeapons()
    {
        // Steel Swords
        this.AddWeapon(n"steel_sword_basic", "Basic Steel Sword", "A basic steel sword", ItemRarity.Common, 50, 2.0);
        this.AddWeapon(n"steel_sword_enhanced", "Enhanced Steel Sword", "An enhanced steel sword", ItemRarity.Uncommon, 100, 2.2);
        this.AddWeapon(n"steel_sword_superior", "Superior Steel Sword", "A superior steel sword", ItemRarity.Rare, 200, 2.5);
        this.AddWeapon(n"steel_sword_master", "Master Steel Sword", "A master-crafted steel sword", ItemRarity.Epic, 400, 2.8);
        this.AddWeapon(n"steel_sword_legendary", "Legendary Steel Sword", "A legendary steel sword", ItemRarity.Legendary, 800, 3.0);
        
        // Silver Swords
        this.AddWeapon(n"silver_sword_basic", "Basic Silver Sword", "A basic silver sword", ItemRarity.Common, 75, 2.2);
        this.AddWeapon(n"silver_sword_enhanced", "Enhanced Silver Sword", "An enhanced silver sword", ItemRarity.Uncommon, 150, 2.5);
        this.AddWeapon(n"silver_sword_superior", "Superior Silver Sword", "A superior silver sword", ItemRarity.Rare, 300, 2.8);
        this.AddWeapon(n"silver_sword_master", "Master Silver Sword", "A master-crafted silver sword", ItemRarity.Epic, 600, 3.2);
        this.AddWeapon(n"silver_sword_legendary", "Legendary Silver Sword", "A legendary silver sword", ItemRarity.Legendary, 1200, 3.5);
        
        // Crossbows
        this.AddWeapon(n"crossbow_basic", "Basic Crossbow", "A basic crossbow", ItemRarity.Common, 60, 3.0);
        this.AddWeapon(n"crossbow_enhanced", "Enhanced Crossbow", "An enhanced crossbow", ItemRarity.Uncommon, 120, 3.2);
        this.AddWeapon(n"crossbow_superior", "Superior Crossbow", "A superior crossbow", ItemRarity.Rare, 240, 3.5);
        
        // Daggers
        this.AddWeapon(n"dagger_basic", "Basic Dagger", "A basic dagger", ItemRarity.Common, 25, 0.5);
        this.AddWeapon(n"dagger_enhanced", "Enhanced Dagger", "An enhanced dagger", ItemRarity.Uncommon, 50, 0.6);
        this.AddWeapon(n"dagger_superior", "Superior Dagger", "A superior dagger", ItemRarity.Rare, 100, 0.7);
    }
    
    // Initialize armor
    private func InitializeArmor()
    {
        // Light Armor
        this.AddArmor(n"light_armor_basic", "Basic Light Armor", "Basic light armor", ItemRarity.Common, 80, 4.0);
        this.AddArmor(n"light_armor_enhanced", "Enhanced Light Armor", "Enhanced light armor", ItemRarity.Uncommon, 160, 4.5);
        this.AddArmor(n"light_armor_superior", "Superior Light Armor", "Superior light armor", ItemRarity.Rare, 320, 5.0);
        
        // Medium Armor
        this.AddArmor(n"medium_armor_basic", "Basic Medium Armor", "Basic medium armor", ItemRarity.Common, 120, 6.0);
        this.AddArmor(n"medium_armor_enhanced", "Enhanced Medium Armor", "Enhanced medium armor", ItemRarity.Uncommon, 240, 6.5);
        this.AddArmor(n"medium_armor_superior", "Superior Medium Armor", "Superior medium armor", ItemRarity.Rare, 480, 7.0);
        
        // Heavy Armor
        this.AddArmor(n"heavy_armor_basic", "Basic Heavy Armor", "Basic heavy armor", ItemRarity.Common, 200, 8.0);
        this.AddArmor(n"heavy_armor_enhanced", "Enhanced Heavy Armor", "Enhanced heavy armor", ItemRarity.Uncommon, 400, 8.5);
        this.AddArmor(n"heavy_armor_superior", "Superior Heavy Armor", "Superior heavy armor", ItemRarity.Rare, 800, 9.0);
        
        // Witcher Armor
        this.AddArmor(n"witcher_armor_basic", "Basic Witcher Armor", "Basic witcher armor", ItemRarity.Uncommon, 300, 5.5);
        this.AddArmor(n"witcher_armor_enhanced", "Enhanced Witcher Armor", "Enhanced witcher armor", ItemRarity.Rare, 600, 6.0);
        this.AddArmor(n"witcher_armor_superior", "Superior Witcher Armor", "Superior witcher armor", ItemRarity.Epic, 1200, 6.5);
        this.AddArmor(n"witcher_armor_master", "Master Witcher Armor", "Master witcher armor", ItemRarity.Legendary, 2400, 7.0);
    }
    
    // Initialize consumables
    private func InitializeConsumables()
    {
        // Potions
        this.AddConsumable(n"potion_swallow", "Swallow", "Health regeneration potion", ItemRarity.Common, 25, 0.1, 5);
        this.AddConsumable(n"potion_thunderbolt", "Thunderbolt", "Attack power potion", ItemRarity.Common, 30, 0.1, 3);
        this.AddConsumable(n"potion_cat", "Cat", "Night vision potion", ItemRarity.Common, 20, 0.1, 2);
        this.AddConsumable(n"potion_white_honey", "White Honey", "Toxicity removal potion", ItemRarity.Common, 15, 0.1, 1);
        
        // Bombs
        this.AddConsumable(n"bomb_dancing_star", "Dancing Star", "Fire bomb", ItemRarity.Uncommon, 50, 0.5, 3);
        this.AddConsumable(n"bomb_grapeshot", "Grapeshot", "Fragmentation bomb", ItemRarity.Uncommon, 45, 0.4, 3);
        this.AddConsumable(n"bomb_northern_wind", "Northern Wind", "Freeze bomb", ItemRarity.Uncommon, 60, 0.6, 2);
        
        // Oils
        this.AddConsumable(n"oil_beast", "Beast Oil", "Effective against beasts", ItemRarity.Common, 20, 0.1, 10);
        this.AddConsumable(n"oil_draconid", "Draconid Oil", "Effective against draconids", ItemRarity.Common, 25, 0.1, 8);
        this.AddConsumable(n"oil_vampire", "Vampire Oil", "Effective against vampires", ItemRarity.Uncommon, 40, 0.1, 5);
        
        // Food
        this.AddConsumable(n"food_bread", "Bread", "Basic food", ItemRarity.Common, 5, 0.2, 20);
        this.AddConsumable(n"food_cheese", "Cheese", "Nutritious food", ItemRarity.Common, 8, 0.3, 15);
        this.AddConsumable(n"food_meat", "Meat", "Hearty food", ItemRarity.Common, 12, 0.5, 10);
    }
    
    // Initialize ingredients
    private func InitializeIngredients()
    {
        // Herbs
        this.AddIngredient(n"arnica", "Arnica", "Common herb", ItemRarity.Common, 8, 0.05, 50);
        this.AddIngredient(n"celandine", "Celandine", "Healing herb", ItemRarity.Common, 6, 0.05, 50);
        this.AddIngredient(n"honeysuckle", "Honeysuckle", "Sweet herb", ItemRarity.Common, 7, 0.05, 50);
        this.AddIngredient(n"ribleaf", "Ribleaf", "Common herb", ItemRarity.Common, 5, 0.05, 50);
        
        // Monster parts
        this.AddIngredient(n"drowner_brain", "Drowner Brain", "Monster part", ItemRarity.Uncommon, 20, 0.2, 20);
        this.AddIngredient(n"endrega_embryo", "Endrega Embryo", "Monster part", ItemRarity.Rare, 30, 0.3, 10);
        this.AddIngredient(n"griffin_feather", "Griffin Feather", "Rare monster part", ItemRarity.Epic, 100, 0.1, 5);
        
        // Alcohol
        this.AddIngredient(n"dwarven_spirit", "Dwarven Spirit", "Strong alcohol", ItemRarity.Common, 10, 0.1, 30);
        this.AddIngredient(n"white_gull", "White Gull", "Rare alcohol", ItemRarity.Rare, 50, 0.1, 10);
        this.AddIngredient(n"alcohest", "Alcohest", "Basic alcohol", ItemRarity.Common, 5, 0.1, 50);
    }
    
    // Initialize quest items
    private func InitializeQuestItems()
    {
        this.AddQuestItem(n"quest_letter", "Important Letter", "A letter with important information", ItemRarity.Common, 0, 0.1, 1);
        this.AddQuestItem(n"quest_key", "Ancient Key", "An ancient key", ItemRarity.Uncommon, 0, 0.2, 1);
        this.AddQuestItem(n"quest_artifact", "Mysterious Artifact", "A mysterious artifact", ItemRarity.Epic, 0, 1.0, 1);
    }
    
    // Add weapon
    private func AddWeapon(id: CName, name: String, description: String, rarity: ItemRarity, value: int32, weight: float)
    {
        let item: ref<MP_ItemData> = new MP_ItemData();
        item.Initialize(id, name, description, ItemCategory.Weapon, rarity, value, weight);
        item.SetEquippable(true);
        item.SetStackSize(1);
        ArrayPush(this.m_items, item);
    }
    
    // Add armor
    private func AddArmor(id: CName, name: String, description: String, rarity: ItemRarity, value: int32, weight: float)
    {
        let item: ref<MP_ItemData> = new MP_ItemData();
        item.Initialize(id, name, description, ItemCategory.Armor, rarity, value, weight);
        item.SetEquippable(true);
        item.SetStackSize(1);
        ArrayPush(this.m_items, item);
    }
    
    // Add consumable
    private func AddConsumable(id: CName, name: String, description: String, rarity: ItemRarity, value: int32, weight: float, stackSize: int32)
    {
        let item: ref<MP_ItemData> = new MP_ItemData();
        item.Initialize(id, name, description, ItemCategory.Consumable, rarity, value, weight);
        item.SetConsumable(true);
        item.SetStackSize(stackSize);
        ArrayPush(this.m_items, item);
    }
    
    // Add ingredient
    private func AddIngredient(id: CName, name: String, description: String, rarity: ItemRarity, value: int32, weight: float, stackSize: int32)
    {
        let item: ref<MP_ItemData> = new MP_ItemData();
        item.Initialize(id, name, description, ItemCategory.Ingredient, rarity, value, weight);
        item.SetStackSize(stackSize);
        ArrayPush(this.m_items, item);
    }
    
    // Add quest item
    private func AddQuestItem(id: CName, name: String, description: String, rarity: ItemRarity, value: int32, weight: float, stackSize: int32)
    {
        let item: ref<MP_ItemData> = new MP_ItemData();
        item.Initialize(id, name, description, ItemCategory.Quest, rarity, value, weight);
        item.SetStackSize(stackSize);
        item.SetTradeable(false);
        ArrayPush(this.m_items, item);
    }
    
    // Add item to inventory
    public func AddItem(playerRef: wref<Player>, itemId: CName, quantity: int32) -> bool
    {
        let itemData: ref<MP_ItemData> = this.GetItemData(itemId);
        if !IsDefined(itemData)
        {
            LogChannel(n"Witcher3-MP", s"Item not found: \(itemId)");
            return false;
        }
        
        // Check weight
        let totalWeight: float = itemData.weight * quantity;
        if this.m_currentWeight + totalWeight > this.m_maxWeight
        {
            LogChannel(n"Witcher3-MP", s"Inventory too heavy: \(this.m_currentWeight + totalWeight)/\(this.m_maxWeight)");
            return false;
        }
        
        // Try to add to existing stack first
        let i: int32 = 0;
        while i < ArraySize(this.m_inventorySlots)
        {
            let slot: ref<MP_InventorySlot> = this.m_inventorySlots[i];
            if IsDefined(slot) && !slot.IsEmpty() && slot.item.id == itemId
            {
                let canAdd: int32 = MinI(quantity, itemData.stackSize - slot.quantity);
                if canAdd > 0
                {
                    slot.quantity += canAdd;
                    this.m_currentWeight += itemData.weight * canAdd;
                    quantity -= canAdd;
                    
                    if quantity <= 0
                    {
                        this.NotifyInventoryChange(playerRef, itemId, canAdd, true);
                        return true;
                    }
                }
            }
            i += 1;
        }
        
        // Add to empty slots
        while quantity > 0
        {
            let emptySlot: ref<MP_InventorySlot> = this.FindEmptySlot();
            if !IsDefined(emptySlot)
            {
                LogChannel(n"Witcher3-MP", "No empty inventory slots");
                return false;
            }
            
            let canAdd: int32 = MinI(quantity, itemData.stackSize);
            emptySlot.AddItem(itemData, canAdd);
            this.m_currentWeight += itemData.weight * canAdd;
            quantity -= canAdd;
            
            this.NotifyInventoryChange(playerRef, itemId, canAdd, true);
        }
        
        LogChannel(n"Witcher3-MP", s"Added \(quantity) \(itemData.name) to inventory");
        return true;
    }
    
    // Remove item from inventory
    public func RemoveItem(playerRef: wref<Player>, itemId: CName, quantity: int32) -> bool
    {
        let itemData: ref<MP_ItemData> = this.GetItemData(itemId);
        if !IsDefined(itemData)
        {
            return false;
        }
        
        let remainingQuantity: int32 = quantity;
        let i: int32 = 0;
        while i < ArraySize(this.m_inventorySlots) && remainingQuantity > 0
        {
            let slot: ref<MP_InventorySlot> = this.m_inventorySlots[i];
            if IsDefined(slot) && !slot.IsEmpty() && slot.item.id == itemId
            {
                let canRemove: int32 = MinI(remainingQuantity, slot.quantity);
                slot.RemoveItem(canRemove);
                this.m_currentWeight -= itemData.weight * canRemove;
                remainingQuantity -= canRemove;
            }
            i += 1;
        }
        
        if remainingQuantity > 0
        {
            LogChannel(n"Witcher3-MP", s"Not enough items to remove: \(quantity - remainingQuantity)/\(quantity)");
            return false;
        }
        
        this.NotifyInventoryChange(playerRef, itemId, quantity, false);
        LogChannel(n"Witcher3-MP", s"Removed \(quantity) \(itemData.name) from inventory");
        return true;
    }
    
    // Find empty slot
    private func FindEmptySlot() -> ref<MP_InventorySlot>
    {
        let i: int32 = 0;
        while i < ArraySize(this.m_inventorySlots)
        {
            let slot: ref<MP_InventorySlot> = this.m_inventorySlots[i];
            if IsDefined(slot) && slot.IsEmpty()
            {
                return slot;
            }
            i += 1;
        }
        return null;
    }
    
    // Get item data
    private func GetItemData(itemId: CName) -> ref<MP_ItemData>
    {
        let i: int32 = 0;
        while i < ArraySize(this.m_items)
        {
            let item: ref<MP_ItemData> = this.m_items[i];
            if IsDefined(item) && item.id == itemId
            {
                return item;
            }
            i += 1;
        }
        return null;
    }
    
    // Get item count
    public func GetItemCount(itemId: CName) -> int32
    {
        let count: int32 = 0;
        let i: int32 = 0;
        while i < ArraySize(this.m_inventorySlots)
        {
            let slot: ref<MP_InventorySlot> = this.m_inventorySlots[i];
            if IsDefined(slot) && !slot.IsEmpty() && slot.item.id == itemId
            {
                count += slot.quantity;
            }
            i += 1;
        }
        return count;
    }
    
    // Equip item
    public func EquipItem(playerRef: wref<Player>, itemId: CName) -> bool
    {
        let itemData: ref<MP_ItemData> = this.GetItemData(itemId);
        if !IsDefined(itemData) || !itemData.isEquippable
        {
            LogChannel(n"Witcher3-MP", s"Cannot equip item: \(itemId)");
            return false;
        }
        
        // Find item in inventory
        let slot: ref<MP_InventorySlot> = this.FindItemSlot(itemId);
        if !IsDefined(slot) || slot.IsEmpty()
        {
            LogChannel(n"Witcher3-MP", s"Item not found in inventory: \(itemId)");
            return false;
        }
        
        // Unequip current item of same category
        this.UnequipItemByCategory(playerRef, itemData.category);
        
        // Equip new item
        slot.isEquipped = true;
        
        // Apply item effects
        this.ApplyItemEffects(playerRef, itemData);
        
        this.NotifyItemEquipped(playerRef, itemId, true);
        LogChannel(n"Witcher3-MP", s"Equipped \(itemData.name)");
        return true;
    }
    
    // Unequip item
    public func UnequipItem(playerRef: wref<Player>, itemId: CName) -> bool
    {
        let slot: ref<MP_InventorySlot> = this.FindItemSlot(itemId);
        if !IsDefined(slot) || slot.IsEmpty() || !slot.isEquipped
        {
            LogChannel(n"Witcher3-MP", s"Item not equipped: \(itemId)");
            return false;
        }
        
        slot.isEquipped = false;
        
        // Remove item effects
        let itemData: ref<MP_ItemData> = slot.item;
        this.RemoveItemEffects(playerRef, itemData);
        
        this.NotifyItemEquipped(playerRef, itemId, false);
        LogChannel(n"Witcher3-MP", s"Unequipped \(itemData.name)");
        return true;
    }
    
    // Find item slot
    private func FindItemSlot(itemId: CName) -> ref<MP_InventorySlot>
    {
        let i: int32 = 0;
        while i < ArraySize(this.m_inventorySlots)
        {
            let slot: ref<MP_InventorySlot> = this.m_inventorySlots[i];
            if IsDefined(slot) && !slot.IsEmpty() && slot.item.id == itemId
            {
                return slot;
            }
            i += 1;
        }
        return null;
    }
    
    // Unequip item by category
    private func UnequipItemByCategory(playerRef: wref<Player>, category: ItemCategory)
    {
        let i: int32 = 0;
        while i < ArraySize(this.m_inventorySlots)
        {
            let slot: ref<MP_InventorySlot> = this.m_inventorySlots[i];
            if IsDefined(slot) && !slot.IsEmpty() && slot.isEquipped && slot.item.category == category
            {
                slot.isEquipped = false;
                this.RemoveItemEffects(playerRef, slot.item);
                this.NotifyItemEquipped(playerRef, slot.item.id, false);
            }
            i += 1;
        }
    }
    
    // Apply item effects
    private func ApplyItemEffects(playerRef: wref<Player>, itemData: ref<MP_ItemData>)
    {
        let buffSystem: ref<BuffSystem> = GameInstance.GetBuffSystem();
        if !IsDefined(buffSystem)
        {
            return;
        }
        
        let i: int32 = 0;
        while i < ArraySize(itemData.effects)
        {
            let effect: CName = itemData.effects[i];
            buffSystem.AddBuff(playerRef, effect, -1.0); // Permanent buff
            i += 1;
        }
    }
    
    // Remove item effects
    private func RemoveItemEffects(playerRef: wref<Player>, itemData: ref<MP_ItemData>)
    {
        let buffSystem: ref<BuffSystem> = GameInstance.GetBuffSystem();
        if !IsDefined(buffSystem)
        {
            return;
        }
        
        let i: int32 = 0;
        while i < ArraySize(itemData.effects)
        {
            let effect: CName = itemData.effects[i];
            buffSystem.RemoveBuff(playerRef, effect);
            i += 1;
        }
    }
    
    // Notify inventory change
    private func NotifyInventoryChange(playerRef: wref<Player>, itemId: CName, quantity: int32, added: bool)
    {
        let serverManager: ref<MP_ServerManager> = GetMPServerManager();
        if IsDefined(serverManager)
        {
            serverManager.NotifyInventoryChange(playerRef, itemId, quantity, added);
        }
    }
    
    // Notify item equipped
    private func NotifyItemEquipped(playerRef: wref<Player>, itemId: CName, equipped: bool)
    {
        let serverManager: ref<MP_ServerManager> = GetMPServerManager();
        if IsDefined(serverManager)
        {
            serverManager.NotifyItemEquipped(playerRef, itemId, equipped);
        }
    }
    
    // Get inventory weight
    public func GetCurrentWeight() -> float
    {
        return this.m_currentWeight;
    }
    
    // Get max weight
    public func GetMaxWeight() -> float
    {
        return this.m_maxWeight;
    }
    
    // Get used slots
    public func GetUsedSlots() -> int32
    {
        let used: int32 = 0;
        let i: int32 = 0;
        while i < ArraySize(this.m_inventorySlots)
        {
            let slot: ref<MP_InventorySlot> = this.m_inventorySlots[i];
            if IsDefined(slot) && !slot.IsEmpty()
            {
                used += 1;
            }
            i += 1;
        }
        return used;
    }
    
    // Get max slots
    public func GetMaxSlots() -> int32
    {
        return this.m_maxSlots;
    }
    
    // Get all items
    public func GetItems() -> array<ref<MP_ItemData>>
    {
        return this.m_items;
    }
    
    // Get inventory slots
    public func GetInventorySlots() -> array<ref<MP_InventorySlot>>
    {
        return this.m_inventorySlots;
    }
    
    // Check if initialized
    public func IsInitialized() -> bool
    {
        return this.m_isInitialized;
    }
}
