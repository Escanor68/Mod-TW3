// Witcher3-MP Alchemy System
// Implements The Witcher 3's alchemy system for multiplayer

using game;
using game::player;
using game::quests;
using game::ui;
using game::inventory;
using game::stats;
using game::effects;

// Alchemy item types
enum class AlchemyItemType
{
    None = 0,
    Potion = 1,
    Bomb = 2,
    Oil = 3,
    Decoction = 4,
    Ingredient = 5
}

// Potion types
enum class PotionType
{
    None = 0,
    Swallow = 1,        // Health regeneration
    Thunderbolt = 2,    // Attack power
    PetriPhilter = 3,   // Sign intensity
    Cat = 4,           // Night vision
    TawnyOwl = 5,      // Stamina regeneration
    WhiteHoney = 6,     // Remove toxicity
    GoldenOriel = 7,    // Immunity to poison
    MariborForest = 8,  // Adrenaline gain
    Blizzard = 9,       // Time slow on kill
    FullMoon = 10       // Health regeneration
}

// Bomb types
enum class BombType
{
    None = 0,
    DancingStar = 1,    // Fire bomb
    Grapeshot = 2,      // Fragmentation bomb
    Samum = 3,          // Flash bomb
    NorthernWind = 4,   // Freeze bomb
    DragonDream = 5,    // Gas bomb
    DevilPuffball = 6,  // Poison bomb
    MoonDust = 7        // Silver bomb
}

// Oil types
enum class OilType
{
    None = 0,
    BeastOil = 1,       // Against beasts
    CursedOil = 2,      // Against cursed ones
    DraconidOil = 3,    // Against draconids
    ElementaOil = 4,    // Against elementals
    HangedMansVenom = 5, // Against humans
    HybridOil = 6,      // Against hybrids
    InsectoidOil = 7,   // Against insectoids
    NecrophageOil = 8,  // Against necrophages
    OgroidOil = 9,      // Against ogroids
    RelictOil = 10,     // Against relics
    SpecterOil = 11,    // Against specters
    VampireOil = 12     // Against vampires
}

// Ingredient data
class AlchemyIngredient
{
    public let id: CName;
    public let name: String;
    public let description: String;
    public let rarity: int32;
    public let value: int32;
    public let weight: float;
    public let effects: array<CName>;
    
    public func Initialize(ingredientId: CName, ingredientName: String, ingredientDescription: String, ingredientRarity: int32, ingredientValue: int32, ingredientWeight: float)
    {
        this.id = ingredientId;
        this.name = ingredientName;
        this.description = ingredientDescription;
        this.rarity = ingredientRarity;
        this.value = ingredientValue;
        this.weight = ingredientWeight;
        this.effects = [];
    }
    
    public func AddEffect(effect: CName)
    {
        ArrayPush(this.effects, effect);
    }
}

// Recipe data
class AlchemyRecipe
{
    public let id: CName;
    public let name: String;
    public let description: String;
    public let itemType: AlchemyItemType;
    public let potionType: PotionType;
    public let bombType: BombType;
    public let oilType: OilType;
    public let ingredients: array<CName>;
    public let ingredientCounts: array<int32>;
    public let toxicity: float;
    public let duration: float;
    public let intensity: float;
    public let isKnown: bool;
    
    public func Initialize(recipeId: CName, recipeName: String, recipeDescription: String, recipeItemType: AlchemyItemType)
    {
        this.id = recipeId;
        this.name = recipeName;
        this.description = recipeDescription;
        this.itemType = recipeItemType;
        this.potionType = PotionType.None;
        this.bombType = BombType.None;
        this.oilType = OilType.None;
        this.ingredients = [];
        this.ingredientCounts = [];
        this.toxicity = 0.0;
        this.duration = 0.0;
        this.intensity = 0.0;
        this.isKnown = false;
    }
    
    public func AddIngredient(ingredient: CName, count: int32)
    {
        ArrayPush(this.ingredients, ingredient);
        ArrayPush(this.ingredientCounts, count);
    }
    
    public func SetPotionProperties(potion: PotionType, tox: float, dur: float, intens: float)
    {
        this.potionType = potion;
        this.toxicity = tox;
        this.duration = dur;
        this.intensity = intens;
    }
    
    public func SetBombProperties(bomb: BombType, tox: float, dur: float, intens: float)
    {
        this.bombType = bomb;
        this.toxicity = tox;
        this.duration = dur;
        this.intensity = intens;
    }
    
    public func SetOilProperties(oil: OilType, tox: float, dur: float, intens: float)
    {
        this.oilType = oil;
        this.toxicity = tox;
        this.duration = dur;
        this.intensity = intens;
    }
}

// Main Alchemy System class
class MP_AlchemySystem
{
    private let m_ingredients: array<ref<AlchemyIngredient>>;
    private let m_recipes: array<ref<AlchemyRecipe>>;
    private let m_playerToxicity: float;
    private let m_maxToxicity: float;
    private let m_isInitialized: bool;
    
    // Initialize the alchemy system
    public func Initialize()
    {
        if this.m_isInitialized
        {
            return;
        }
        
        this.m_playerToxicity = 0.0;
        this.m_maxToxicity = 100.0;
        
        // Initialize ingredients
        this.InitializeIngredients();
        
        // Initialize recipes
        this.InitializeRecipes();
        
        this.m_isInitialized = true;
        LogChannel(n"Witcher3-MP", "Alchemy system initialized");
    }
    
    // Initialize all ingredients
    private func InitializeIngredients()
    {
        // Common ingredients
        this.AddIngredient(n"dwarven_spirit", "Dwarven Spirit", "Strong alcohol used in alchemy", 1, 10, 0.1);
        this.AddIngredient(n"white_gull", "White Gull", "Rare alcohol for advanced alchemy", 3, 50, 0.1);
        this.AddIngredient(n"alcohest", "Alcohest", "Basic alcohol for alchemy", 1, 5, 0.1);
        
        // Herbs
        this.AddIngredient(n"arnica", "Arnica", "Common herb with healing properties", 1, 8, 0.05);
        this.AddIngredient(n"balisse_fruit", "Balisse Fruit", "Rare fruit with magical properties", 3, 25, 0.1);
        this.AddIngredient(n"beggartick_blossoms", "Beggartick Blossoms", "Flowers with toxic properties", 2, 15, 0.05);
        this.AddIngredient(n"bryonia", "Bryonia", "Poisonous plant", 2, 12, 0.05);
        this.AddIngredient(n"celandine", "Celandine", "Yellow flower with healing properties", 1, 6, 0.05);
        this.AddIngredient(n"drowner_brain", "Drowner Brain", "Monster part with magical properties", 2, 20, 0.2);
        this.AddIngredient(n"endrega_embryo", "Endrega Embryo", "Monster embryo with toxic properties", 3, 30, 0.3);
        this.AddIngredient(n"fool_parsley", "Fool's Parsley", "Poisonous herb", 1, 4, 0.05);
        this.AddIngredient(n"han", "Han", "Rare herb with magical properties", 3, 35, 0.05);
        this.AddIngredient(n"honeysuckle", "Honeysuckle", "Sweet flower with healing properties", 1, 7, 0.05);
        this.AddIngredient(n"mistletoe", "Mistletoe", "Sacred plant with magical properties", 2, 18, 0.05);
        this.AddIngredient(n"rennet", "Rennet", "Enzyme from animal stomach", 1, 3, 0.1);
        this.AddIngredient(n"ribleaf", "Ribleaf", "Common herb", 1, 5, 0.05);
        this.AddIngredient(n"white_myrtle_petals", "White Myrtle Petals", "Rare flower petals", 3, 40, 0.05);
        this.AddIngredient(n"wolfsbane", "Wolfsbane", "Poisonous plant", 2, 16, 0.05);
        
        LogChannel(n"Witcher3-MP", s"Initialized \(ArraySize(this.m_ingredients)) ingredients");
    }
    
    // Add ingredient
    private func AddIngredient(id: CName, name: String, description: String, rarity: int32, value: int32, weight: float)
    {
        let ingredient: ref<AlchemyIngredient> = new AlchemyIngredient();
        ingredient.Initialize(id, name, description, rarity, value, weight);
        ArrayPush(this.m_ingredients, ingredient);
    }
    
    // Initialize all recipes
    private func InitializeRecipes()
    {
        // Potion recipes
        this.InitializePotionRecipes();
        
        // Bomb recipes
        this.InitializeBombRecipes();
        
        // Oil recipes
        this.InitializeOilRecipes();
        
        LogChannel(n"Witcher3-MP", s"Initialized \(ArraySize(this.m_recipes)) recipes");
    }
    
    // Initialize potion recipes
    private func InitializePotionRecipes()
    {
        // Swallow (Health regeneration)
        let swallow: ref<AlchemyRecipe> = new AlchemyRecipe();
        swallow.Initialize(n"swallow_recipe", "Swallow", "Health regeneration potion", AlchemyItemType.Potion);
        swallow.AddIngredient(n"dwarven_spirit", 1);
        swallow.AddIngredient(n"arnica", 1);
        swallow.AddIngredient(n"drowner_brain", 1);
        swallow.SetPotionProperties(PotionType.Swallow, 20.0, 30.0, 1.0);
        ArrayPush(this.m_recipes, swallow);
        
        // Thunderbolt (Attack power)
        let thunderbolt: ref<AlchemyRecipe> = new AlchemyRecipe();
        thunderbolt.Initialize(n"thunderbolt_recipe", "Thunderbolt", "Increases attack power", AlchemyItemType.Potion);
        thunderbolt.AddIngredient(n"dwarven_spirit", 1);
        thunderbolt.AddIngredient(n"bryonia", 1);
        thunderbolt.AddIngredient(n"han", 1);
        thunderbolt.SetPotionProperties(PotionType.Thunderbolt, 30.0, 60.0, 1.0);
        ArrayPush(this.m_recipes, thunderbolt);
        
        // Cat (Night vision)
        let cat: ref<AlchemyRecipe> = new AlchemyRecipe();
        cat.Initialize(n"cat_recipe", "Cat", "Night vision potion", AlchemyItemType.Potion);
        cat.AddIngredient(n"dwarven_spirit", 1);
        cat.AddIngredient(n"celandine", 1);
        cat.AddIngredient(n"fool_parsley", 1);
        cat.SetPotionProperties(PotionType.Cat, 15.0, 300.0, 1.0);
        ArrayPush(this.m_recipes, cat);
        
        // White Honey (Remove toxicity)
        let whiteHoney: ref<AlchemyRecipe> = new AlchemyRecipe();
        whiteHoney.Initialize(n"white_honey_recipe", "White Honey", "Removes toxicity", AlchemyItemType.Potion);
        whiteHoney.AddIngredient(n"dwarven_spirit", 1);
        whiteHoney.AddIngredient(n"honeysuckle", 1);
        whiteHoney.AddIngredient(n"white_myrtle_petals", 1);
        whiteHoney.SetPotionProperties(PotionType.WhiteHoney, 0.0, 0.0, 1.0);
        ArrayPush(this.m_recipes, whiteHoney);
    }
    
    // Initialize bomb recipes
    private func InitializeBombRecipes()
    {
        // Dancing Star (Fire bomb)
        let dancingStar: ref<AlchemyRecipe> = new AlchemyRecipe();
        dancingStar.Initialize(n"dancing_star_recipe", "Dancing Star", "Fire bomb", AlchemyItemType.Bomb);
        dancingStar.AddIngredient(n"dwarven_spirit", 1);
        dancingStar.AddIngredient(n"arnica", 1);
        dancingStar.AddIngredient(n"beggartick_blossoms", 1);
        dancingStar.SetBombProperties(BombType.DancingStar, 0.0, 0.0, 1.0);
        ArrayPush(this.m_recipes, dancingStar);
        
        // Grapeshot (Fragmentation bomb)
        let grapeshot: ref<AlchemyRecipe> = new AlchemyRecipe();
        grapeshot.Initialize(n"grapeshot_recipe", "Grapeshot", "Fragmentation bomb", AlchemyItemType.Bomb);
        grapeshot.AddIngredient(n"dwarven_spirit", 1);
        grapeshot.AddIngredient(n"bryonia", 1);
        grapeshot.AddIngredient(n"rennet", 1);
        grapeshot.SetBombProperties(BombType.Grapeshot, 0.0, 0.0, 1.0);
        ArrayPush(this.m_recipes, grapeshot);
        
        // Northern Wind (Freeze bomb)
        let northernWind: ref<AlchemyRecipe> = new AlchemyRecipe();
        northernWind.Initialize(n"northern_wind_recipe", "Northern Wind", "Freeze bomb", AlchemyItemType.Bomb);
        northernWind.AddIngredient(n"dwarven_spirit", 1);
        northernWind.AddIngredient(n"balisse_fruit", 1);
        northernWind.AddIngredient(n"white_myrtle_petals", 1);
        northernWind.SetBombProperties(BombType.NorthernWind, 0.0, 0.0, 1.0);
        ArrayPush(this.m_recipes, northernWind);
    }
    
    // Initialize oil recipes
    private func InitializeOilRecipes()
    {
        // Beast Oil (Against beasts)
        let beastOil: ref<AlchemyRecipe> = new AlchemyRecipe();
        beastOil.Initialize(n"beast_oil_recipe", "Beast Oil", "Effective against beasts", AlchemyItemType.Oil);
        beastOil.AddIngredient(n"dwarven_spirit", 1);
        beastOil.AddIngredient(n"arnica", 1);
        beastOil.AddIngredient(n"ribleaf", 1);
        beastOil.SetOilProperties(OilType.BeastOil, 0.0, 300.0, 1.0);
        ArrayPush(this.m_recipes, beastOil);
        
        // Draconid Oil (Against draconids)
        let draconidOil: ref<AlchemyRecipe> = new AlchemyRecipe();
        draconidOil.Initialize(n"draconid_oil_recipe", "Draconid Oil", "Effective against draconids", AlchemyItemType.Oil);
        draconidOil.AddIngredient(n"dwarven_spirit", 1);
        draconidOil.AddIngredient(n"bryonia", 1);
        draconidOil.AddIngredient(n"mistletoe", 1);
        draconidOil.SetOilProperties(OilType.DraconidOil, 0.0, 300.0, 1.0);
        ArrayPush(this.m_recipes, draconidOil);
        
        // Vampire Oil (Against vampires)
        let vampireOil: ref<AlchemyRecipe> = new AlchemyRecipe();
        vampireOil.Initialize(n"vampire_oil_recipe", "Vampire Oil", "Effective against vampires", AlchemyItemType.Oil);
        vampireOil.AddIngredient(n"dwarven_spirit", 1);
        vampireOil.AddIngredient(n"han", 1);
        vampireOil.AddIngredient(n"wolfsbane", 1);
        vampireOil.SetOilProperties(OilType.VampireOil, 0.0, 300.0, 1.0);
        ArrayPush(this.m_recipes, vampireOil);
    }
    
    // Craft alchemy item
    public func CraftItem(playerRef: wref<Player>, recipeId: CName) -> bool
    {
        let recipe: ref<AlchemyRecipe> = this.GetRecipe(recipeId);
        if !IsDefined(recipe)
        {
            LogChannel(n"Witcher3-MP", s"Recipe not found: \(recipeId)");
            return false;
        }
        
        if !recipe.isKnown
        {
            LogChannel(n"Witcher3-MP", s"Recipe not known: \(recipeId)");
            return false;
        }
        
        // Check if player has required ingredients
        if !this.HasRequiredIngredients(playerRef, recipe)
        {
            LogChannel(n"Witcher3-MP", s"Missing ingredients for recipe: \(recipeId)");
            return false;
        }
        
        // Check toxicity
        if this.m_playerToxicity + recipe.toxicity > this.m_maxToxicity
        {
            LogChannel(n"Witcher3-MP", s"Too much toxicity to craft: \(this.m_playerToxicity + recipe.toxicity)/\(this.m_maxToxicity)");
            return false;
        }
        
        // Consume ingredients
        this.ConsumeIngredients(playerRef, recipe);
        
        // Add toxicity
        this.m_playerToxicity += recipe.toxicity;
        
        // Create the item
        this.CreateAlchemyItem(playerRef, recipe);
        
        LogChannel(n"Witcher3-MP", s"Player crafted \(recipe.name)");
        return true;
    }
    
    // Check if player has required ingredients
    private func HasRequiredIngredients(playerRef: wref<Player>, recipe: ref<AlchemyRecipe>) -> bool
    {
        let inventorySystem: ref<InventorySystem> = GameInstance.GetInventorySystem();
        if !IsDefined(inventorySystem)
        {
            return false;
        }
        
        let i: int32 = 0;
        while i < ArraySize(recipe.ingredients)
        {
            let ingredient: CName = recipe.ingredients[i];
            let requiredCount: int32 = recipe.ingredientCounts[i];
            let availableCount: int32 = inventorySystem.GetItemCount(playerRef, ingredient);
            
            if availableCount < requiredCount
            {
                return false;
            }
            i += 1;
        }
        
        return true;
    }
    
    // Consume ingredients
    private func ConsumeIngredients(playerRef: wref<Player>, recipe: ref<AlchemyRecipe>)
    {
        let inventorySystem: ref<InventorySystem> = GameInstance.GetInventorySystem();
        if !IsDefined(inventorySystem)
        {
            return;
        }
        
        let i: int32 = 0;
        while i < ArraySize(recipe.ingredients)
        {
            let ingredient: CName = recipe.ingredients[i];
            let count: int32 = recipe.ingredientCounts[i];
            inventorySystem.RemoveItem(playerRef, ingredient, count);
            i += 1;
        }
    }
    
    // Create alchemy item
    private func CreateAlchemyItem(playerRef: wref<Player>, recipe: ref<AlchemyRecipe>)
    {
        let inventorySystem: ref<InventorySystem> = GameInstance.GetInventorySystem();
        if !IsDefined(inventorySystem)
        {
            return;
        }
        
        // Create the appropriate item type
        switch recipe.itemType
        {
            case AlchemyItemType.Potion:
                this.CreatePotion(playerRef, recipe);
                break;
                
            case AlchemyItemType.Bomb:
                this.CreateBomb(playerRef, recipe);
                break;
                
            case AlchemyItemType.Oil:
                this.CreateOil(playerRef, recipe);
                break;
        }
    }
    
    // Create potion
    private func CreatePotion(playerRef: wref<Player>, recipe: ref<AlchemyRecipe>)
    {
        let potionId: CName = this.GetPotionId(recipe.potionType);
        let inventorySystem: ref<InventorySystem> = GameInstance.GetInventorySystem();
        
        if IsDefined(inventorySystem)
        {
            inventorySystem.AddItem(playerRef, potionId, 1);
        }
    }
    
    // Create bomb
    private func CreateBomb(playerRef: wref<Player>, recipe: ref<AlchemyRecipe>)
    {
        let bombId: CName = this.GetBombId(recipe.bombType);
        let inventorySystem: ref<InventorySystem> = GameInstance.GetInventorySystem();
        
        if IsDefined(inventorySystem)
        {
            inventorySystem.AddItem(playerRef, bombId, 1);
        }
    }
    
    // Create oil
    private func CreateOil(playerRef: wref<Player>, recipe: ref<AlchemyRecipe>)
    {
        let oilId: CName = this.GetOilId(recipe.oilType);
        let inventorySystem: ref<InventorySystem> = GameInstance.GetInventorySystem();
        
        if IsDefined(inventorySystem)
        {
            inventorySystem.AddItem(playerRef, oilId, 1);
        }
    }
    
    // Get potion ID
    private func GetPotionId(potionType: PotionType) -> CName
    {
        switch potionType
        {
            case PotionType.Swallow: return n"potion_swallow";
            case PotionType.Thunderbolt: return n"potion_thunderbolt";
            case PotionType.Cat: return n"potion_cat";
            case PotionType.WhiteHoney: return n"potion_white_honey";
            default: return n"potion_unknown";
        }
    }
    
    // Get bomb ID
    private func GetBombId(bombType: BombType) -> CName
    {
        switch bombType
        {
            case BombType.DancingStar: return n"bomb_dancing_star";
            case BombType.Grapeshot: return n"bomb_grapeshot";
            case BombType.NorthernWind: return n"bomb_northern_wind";
            default: return n"bomb_unknown";
        }
    }
    
    // Get oil ID
    private func GetOilId(oilType: OilType) -> CName
    {
        switch oilType
        {
            case OilType.BeastOil: return n"oil_beast";
            case OilType.DraconidOil: return n"oil_draconid";
            case OilType.VampireOil: return n"oil_vampire";
            default: return n"oil_unknown";
        }
    }
    
    // Get recipe
    private func GetRecipe(recipeId: CName) -> ref<AlchemyRecipe>
    {
        let i: int32 = 0;
        while i < ArraySize(this.m_recipes)
        {
            let recipe: ref<AlchemyRecipe> = this.m_recipes[i];
            if IsDefined(recipe) && recipe.id == recipeId
            {
                return recipe;
            }
            i += 1;
        }
        return null;
    }
    
    // Learn recipe
    public func LearnRecipe(recipeId: CName)
    {
        let recipe: ref<AlchemyRecipe> = this.GetRecipe(recipeId);
        if IsDefined(recipe)
        {
            recipe.isKnown = true;
            LogChannel(n"Witcher3-MP", s"Player learned recipe: \(recipe.name)");
        }
    }
    
    // Get player toxicity
    public func GetPlayerToxicity() -> float
    {
        return this.m_playerToxicity;
    }
    
    // Get max toxicity
    public func GetMaxToxicity() -> float
    {
        return this.m_maxToxicity;
    }
    
    // Reduce toxicity
    public func ReduceToxicity(amount: float)
    {
        this.m_playerToxicity = MaxF(0.0, this.m_playerToxicity - amount);
    }
    
    // Get all recipes
    public func GetRecipes() -> array<ref<AlchemyRecipe>>
    {
        return this.m_recipes;
    }
    
    // Get all ingredients
    public func GetIngredients() -> array<ref<AlchemyIngredient>>
    {
        return this.m_ingredients;
    }
    
    // Check if initialized
    public func IsInitialized() -> bool
    {
        return this.m_isInitialized;
    }
}
