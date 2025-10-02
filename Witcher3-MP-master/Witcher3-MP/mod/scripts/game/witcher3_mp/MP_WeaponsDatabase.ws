// Witcher3-MP Weapons Database
// Complete database of The Witcher 3 weapons for multiplayer

using game;
using game::player;
using game::quests;
using game::ui;
using game::inventory;
using game::stats;
using game::effects;

// Weapon categories
enum class WeaponCategory
{
    None = 0,
    SteelSword = 1,
    SilverSword = 2,
    Crossbow = 3,
    Dagger = 4,
    Axe = 5,
    Mace = 6,
    Spear = 7,
    Bow = 8
}

// Weapon quality levels
enum class WeaponQuality
{
    None = 0,
    Basic = 1,
    Enhanced = 2,
    Superior = 3,
    Master = 4,
    Legendary = 5
}

// Weapon stats
class WeaponStats
{
    public let damage: float;
    public let armorPenetration: float;
    public let criticalChance: float;
    public let criticalMultiplier: float;
    public let attackSpeed: float;
    public let range: float;
    public let durability: float;
    public let maxDurability: float;
    public let weight: float;
    public let value: int32;
    
    public func Initialize(baseDamage: float, armorPen: float, critChance: float, critMult: float, speed: float, weaponRange: float, dur: float, weaponWeight: float, weaponValue: int32)
    {
        this.damage = baseDamage;
        this.armorPenetration = armorPen;
        this.criticalChance = critChance;
        this.criticalMultiplier = critMult;
        this.attackSpeed = speed;
        this.range = weaponRange;
        this.durability = dur;
        this.maxDurability = dur;
        this.weight = weaponWeight;
        this.value = weaponValue;
    }
}

// Weapon data
class WeaponData
{
    public let id: CName;
    public let name: String;
    public let description: String;
    public let category: WeaponCategory;
    public let quality: WeaponQuality;
    public let stats: ref<WeaponStats>;
    public let requirements: array<CName>;
    public let effects: array<CName>;
    public let isWitcherGear: bool;
    public let witcherSchool: CName;
    public let isUnique: bool;
    public let isCraftable: bool;
    public let craftingMaterials: array<CName>;
    public let craftingCounts: array<int32>;
    
    public func Initialize(weaponId: CName, weaponName: String, weaponDescription: String, weaponCategory: WeaponCategory, weaponQuality: WeaponQuality)
    {
        this.id = weaponId;
        this.name = weaponName;
        this.description = weaponDescription;
        this.category = weaponCategory;
        this.quality = weaponQuality;
        this.stats = new WeaponStats();
        this.requirements = [];
        this.effects = [];
        this.isWitcherGear = false;
        this.witcherSchool = n"";
        this.isUnique = false;
        this.isCraftable = true;
        this.craftingMaterials = [];
        this.craftingCounts = [];
    }
    
    public func SetStats(weaponStats: ref<WeaponStats>)
    {
        this.stats = weaponStats;
    }
    
    public func AddRequirement(requirement: CName)
    {
        ArrayPush(this.requirements, requirement);
    }
    
    public func AddEffect(effect: CName)
    {
        ArrayPush(this.effects, effect);
    }
    
    public func SetWitcherGear(school: CName)
    {
        this.isWitcherGear = true;
        this.witcherSchool = school;
    }
    
    public func SetUnique(unique: bool)
    {
        this.isUnique = unique;
    }
    
    public func AddCraftingMaterial(material: CName, count: int32)
    {
        ArrayPush(this.craftingMaterials, material);
        ArrayPush(this.craftingCounts, count);
    }
}

// Main Weapons Database class
class MP_WeaponsDatabase
{
    private let m_weapons: array<ref<WeaponData>>;
    private let m_isInitialized: bool;
    
    // Initialize the weapons database
    public func Initialize()
    {
        if this.m_isInitialized
        {
            return;
        }
        
        // Initialize all weapon categories
        this.InitializeSteelSwords();
        this.InitializeSilverSwords();
        this.InitializeCrossbows();
        this.InitializeDaggers();
        this.InitializeAxes();
        this.InitializeMaces();
        this.InitializeSpears();
        this.InitializeWitcherGear();
        this.InitializeUniqueWeapons();
        
        this.m_isInitialized = true;
        LogChannel(n"Witcher3-MP", s"Weapons database initialized with \(ArraySize(this.m_weapons)) weapons");
    }
    
    // Initialize steel swords
    private func InitializeSteelSwords()
    {
        // Basic Steel Swords
        this.AddSteelSword(n"steel_sword_basic", "Basic Steel Sword", "A basic steel sword", WeaponQuality.Basic, 25.0, 0.1, 0.05, 1.5, 2.0, 100.0, 2.0, 50);
        this.AddSteelSword(n"steel_sword_enhanced", "Enhanced Steel Sword", "An enhanced steel sword", WeaponQuality.Enhanced, 35.0, 0.15, 0.08, 1.8, 2.2, 120.0, 2.2, 100);
        this.AddSteelSword(n"steel_sword_superior", "Superior Steel Sword", "A superior steel sword", WeaponQuality.Superior, 50.0, 0.2, 0.12, 2.0, 2.5, 150.0, 2.5, 200);
        this.AddSteelSword(n"steel_sword_master", "Master Steel Sword", "A master-crafted steel sword", WeaponQuality.Master, 70.0, 0.25, 0.15, 2.2, 2.8, 180.0, 2.8, 400);
        this.AddSteelSword(n"steel_sword_legendary", "Legendary Steel Sword", "A legendary steel sword", WeaponQuality.Legendary, 100.0, 0.3, 0.2, 2.5, 3.0, 200.0, 3.0, 800);
        
        // Special Steel Swords
        this.AddSteelSword(n"steel_sword_ironside", "Ironside", "A sturdy steel sword", WeaponQuality.Enhanced, 40.0, 0.2, 0.06, 1.6, 2.3, 140.0, 2.5, 150);
        this.AddSteelSword(n"steel_sword_bloodsword", "Bloodsword", "A sword that thirsts for blood", WeaponQuality.Superior, 55.0, 0.18, 0.1, 1.9, 2.4, 160.0, 2.6, 250);
        this.AddSteelSword(n"steel_sword_flame", "Flame", "A sword that burns with inner fire", WeaponQuality.Master, 65.0, 0.22, 0.12, 2.1, 2.7, 170.0, 2.7, 350);
    }
    
    // Initialize silver swords
    private func InitializeSilverSwords()
    {
        // Basic Silver Swords
        this.AddSilverSword(n"silver_sword_basic", "Basic Silver Sword", "A basic silver sword", WeaponQuality.Basic, 30.0, 0.15, 0.06, 1.6, 2.2, 100.0, 2.2, 75);
        this.AddSilverSword(n"silver_sword_enhanced", "Enhanced Silver Sword", "An enhanced silver sword", WeaponQuality.Enhanced, 45.0, 0.2, 0.1, 1.9, 2.5, 120.0, 2.5, 150);
        this.AddSilverSword(n"silver_sword_superior", "Superior Silver Sword", "A superior silver sword", WeaponQuality.Superior, 65.0, 0.25, 0.15, 2.2, 2.8, 150.0, 2.8, 300);
        this.AddSilverSword(n"silver_sword_master", "Master Silver Sword", "A master-crafted silver sword", WeaponQuality.Master, 90.0, 0.3, 0.18, 2.5, 3.2, 180.0, 3.2, 600);
        this.AddSilverSword(n"silver_sword_legendary", "Legendary Silver Sword", "A legendary silver sword", WeaponQuality.Legendary, 130.0, 0.35, 0.25, 2.8, 3.5, 200.0, 3.5, 1200);
        
        // Special Silver Swords
        this.AddSilverSword(n"silver_sword_moonblade", "Moonblade", "A sword that glows in moonlight", WeaponQuality.Enhanced, 50.0, 0.22, 0.08, 1.8, 2.6, 130.0, 2.6, 180);
        this.AddSilverSword(n"silver_sword_storm", "Storm", "A sword that crackles with energy", WeaponQuality.Superior, 70.0, 0.28, 0.12, 2.0, 2.9, 160.0, 2.9, 320);
        this.AddSilverSword(n"silver_sword_lightbringer", "Lightbringer", "A sword that banishes darkness", WeaponQuality.Master, 85.0, 0.32, 0.15, 2.3, 3.0, 175.0, 3.0, 500);
    }
    
    // Initialize crossbows
    private func InitializeCrossbows()
    {
        this.AddCrossbow(n"crossbow_basic", "Basic Crossbow", "A basic crossbow", WeaponQuality.Basic, 20.0, 0.05, 0.1, 1.2, 8.0, 80.0, 3.0, 60);
        this.AddCrossbow(n"crossbow_enhanced", "Enhanced Crossbow", "An enhanced crossbow", WeaponQuality.Enhanced, 30.0, 0.08, 0.12, 1.4, 10.0, 100.0, 3.2, 120);
        this.AddCrossbow(n"crossbow_superior", "Superior Crossbow", "A superior crossbow", WeaponQuality.Superior, 45.0, 0.12, 0.15, 1.6, 12.0, 120.0, 3.5, 240);
        this.AddCrossbow(n"crossbow_master", "Master Crossbow", "A master-crafted crossbow", WeaponQuality.Master, 65.0, 0.15, 0.18, 1.8, 15.0, 140.0, 3.8, 480);
        this.AddCrossbow(n"crossbow_legendary", "Legendary Crossbow", "A legendary crossbow", WeaponQuality.Legendary, 90.0, 0.2, 0.25, 2.0, 18.0, 160.0, 4.0, 960);
    }
    
    // Initialize daggers
    private func InitializeDaggers()
    {
        this.AddDagger(n"dagger_basic", "Basic Dagger", "A basic dagger", WeaponQuality.Basic, 15.0, 0.02, 0.15, 1.8, 1.5, 60.0, 0.5, 25);
        this.AddDagger(n"dagger_enhanced", "Enhanced Dagger", "An enhanced dagger", WeaponQuality.Enhanced, 22.0, 0.03, 0.18, 2.0, 1.7, 80.0, 0.6, 50);
        this.AddDagger(n"dagger_superior", "Superior Dagger", "A superior dagger", WeaponQuality.Superior, 32.0, 0.05, 0.22, 2.2, 2.0, 100.0, 0.7, 100);
        this.AddDagger(n"dagger_master", "Master Dagger", "A master-crafted dagger", WeaponQuality.Master, 45.0, 0.08, 0.25, 2.5, 2.2, 120.0, 0.8, 200);
        this.AddDagger(n"dagger_legendary", "Legendary Dagger", "A legendary dagger", WeaponQuality.Legendary, 65.0, 0.1, 0.3, 2.8, 2.5, 140.0, 0.9, 400);
    }
    
    // Initialize axes
    private func InitializeAxes()
    {
        this.AddAxe(n"axe_basic", "Basic Axe", "A basic axe", WeaponQuality.Basic, 35.0, 0.2, 0.08, 1.3, 2.5, 120.0, 3.5, 80);
        this.AddAxe(n"axe_enhanced", "Enhanced Axe", "An enhanced axe", WeaponQuality.Enhanced, 50.0, 0.25, 0.1, 1.5, 2.8, 140.0, 3.8, 160);
        this.AddAxe(n"axe_superior", "Superior Axe", "A superior axe", WeaponQuality.Superior, 70.0, 0.3, 0.12, 1.7, 3.0, 160.0, 4.0, 320);
        this.AddAxe(n"axe_master", "Master Axe", "A master-crafted axe", WeaponQuality.Master, 95.0, 0.35, 0.15, 1.9, 3.2, 180.0, 4.2, 640);
        this.AddAxe(n"axe_legendary", "Legendary Axe", "A legendary axe", WeaponQuality.Legendary, 130.0, 0.4, 0.2, 2.1, 3.5, 200.0, 4.5, 1280);
    }
    
    // Initialize maces
    private func InitializeMaces()
    {
        this.AddMace(n"mace_basic", "Basic Mace", "A basic mace", WeaponQuality.Basic, 40.0, 0.25, 0.06, 1.2, 2.3, 130.0, 4.0, 90);
        this.AddMace(n"mace_enhanced", "Enhanced Mace", "An enhanced mace", WeaponQuality.Enhanced, 55.0, 0.3, 0.08, 1.4, 2.6, 150.0, 4.3, 180);
        this.AddMace(n"mace_superior", "Superior Mace", "A superior mace", WeaponQuality.Superior, 75.0, 0.35, 0.1, 1.6, 2.9, 170.0, 4.6, 360);
        this.AddMace(n"mace_master", "Master Mace", "A master-crafted mace", WeaponQuality.Master, 100.0, 0.4, 0.12, 1.8, 3.1, 190.0, 4.9, 720);
        this.AddMace(n"mace_legendary", "Legendary Mace", "A legendary mace", WeaponQuality.Legendary, 135.0, 0.45, 0.15, 2.0, 3.4, 210.0, 5.2, 1440);
    }
    
    // Initialize spears
    private func InitializeSpears()
    {
        this.AddSpear(n"spear_basic", "Basic Spear", "A basic spear", WeaponQuality.Basic, 30.0, 0.1, 0.1, 1.4, 4.0, 100.0, 3.0, 70);
        this.AddSpear(n"spear_enhanced", "Enhanced Spear", "An enhanced spear", WeaponQuality.Enhanced, 42.0, 0.12, 0.12, 1.6, 4.5, 120.0, 3.2, 140);
        this.AddSpear(n"spear_superior", "Superior Spear", "A superior spear", WeaponQuality.Superior, 58.0, 0.15, 0.15, 1.8, 5.0, 140.0, 3.5, 280);
        this.AddSpear(n"spear_master", "Master Spear", "A master-crafted spear", WeaponQuality.Master, 78.0, 0.18, 0.18, 2.0, 5.5, 160.0, 3.8, 560);
        this.AddSpear(n"spear_legendary", "Legendary Spear", "A legendary spear", WeaponQuality.Legendary, 105.0, 0.22, 0.22, 2.2, 6.0, 180.0, 4.0, 1120);
    }
    
    // Initialize Witcher gear
    private func InitializeWitcherGear()
    {
        // Wolf School Gear
        this.AddWitcherSteelSword(n"wolf_steel_sword_basic", "Wolf School Steel Sword", "Basic Wolf School steel sword", WeaponQuality.Basic, 30.0, 0.12, 0.08, 1.6, 2.3, 110.0, 2.3, 100, n"wolf");
        this.AddWitcherSteelSword(n"wolf_steel_sword_enhanced", "Enhanced Wolf School Steel Sword", "Enhanced Wolf School steel sword", WeaponQuality.Enhanced, 45.0, 0.18, 0.12, 1.9, 2.6, 130.0, 2.6, 200, n"wolf");
        this.AddWitcherSteelSword(n"wolf_steel_sword_superior", "Superior Wolf School Steel Sword", "Superior Wolf School steel sword", WeaponQuality.Superior, 65.0, 0.25, 0.15, 2.2, 2.9, 160.0, 2.9, 400, n"wolf");
        this.AddWitcherSteelSword(n"wolf_steel_sword_master", "Master Wolf School Steel Sword", "Master Wolf School steel sword", WeaponQuality.Master, 90.0, 0.32, 0.18, 2.5, 3.2, 190.0, 3.2, 800, n"wolf");
        
        this.AddWitcherSilverSword(n"wolf_silver_sword_basic", "Wolf School Silver Sword", "Basic Wolf School silver sword", WeaponQuality.Basic, 35.0, 0.18, 0.1, 1.8, 2.5, 110.0, 2.5, 150, n"wolf");
        this.AddWitcherSilverSword(n"wolf_silver_sword_enhanced", "Enhanced Wolf School Silver Sword", "Enhanced Wolf School silver sword", WeaponQuality.Enhanced, 52.0, 0.25, 0.12, 2.1, 2.8, 130.0, 2.8, 300, n"wolf");
        this.AddWitcherSilverSword(n"wolf_silver_sword_superior", "Superior Wolf School Silver Sword", "Superior Wolf School silver sword", WeaponQuality.Superior, 75.0, 0.32, 0.15, 2.4, 3.1, 160.0, 3.1, 600, n"wolf");
        this.AddWitcherSilverSword(n"wolf_silver_sword_master", "Master Wolf School Silver Sword", "Master Wolf School silver sword", WeaponQuality.Master, 105.0, 0.4, 0.18, 2.7, 3.4, 190.0, 3.4, 1200, n"wolf");
        
        // Cat School Gear
        this.AddWitcherSteelSword(n"cat_steel_sword_basic", "Cat School Steel Sword", "Basic Cat School steel sword", WeaponQuality.Basic, 28.0, 0.1, 0.1, 1.8, 2.2, 100.0, 2.0, 90, n"cat");
        this.AddWitcherSteelSword(n"cat_steel_sword_enhanced", "Enhanced Cat School Steel Sword", "Enhanced Cat School steel sword", WeaponQuality.Enhanced, 42.0, 0.15, 0.12, 2.1, 2.5, 120.0, 2.3, 180, n"cat");
        this.AddWitcherSteelSword(n"cat_steel_sword_superior", "Superior Cat School Steel Sword", "Superior Cat School steel sword", WeaponQuality.Superior, 60.0, 0.2, 0.15, 2.4, 2.8, 140.0, 2.6, 360, n"cat");
        this.AddWitcherSteelSword(n"cat_steel_sword_master", "Master Cat School Steel Sword", "Master Cat School steel sword", WeaponQuality.Master, 85.0, 0.25, 0.18, 2.7, 3.1, 160.0, 2.9, 720, n"cat");
        
        this.AddWitcherSilverSword(n"cat_silver_sword_basic", "Cat School Silver Sword", "Basic Cat School silver sword", WeaponQuality.Basic, 32.0, 0.15, 0.12, 2.0, 2.4, 100.0, 2.2, 135, n"cat");
        this.AddWitcherSilverSword(n"cat_silver_sword_enhanced", "Enhanced Cat School Silver Sword", "Enhanced Cat School silver sword", WeaponQuality.Enhanced, 48.0, 0.2, 0.15, 2.3, 2.7, 120.0, 2.5, 270, n"cat");
        this.AddWitcherSilverSword(n"cat_silver_sword_superior", "Superior Cat School Silver Sword", "Superior Cat School silver sword", WeaponQuality.Superior, 68.0, 0.25, 0.18, 2.6, 3.0, 140.0, 2.8, 540, n"cat");
        this.AddWitcherSilverSword(n"cat_silver_sword_master", "Master Cat School Silver Sword", "Master Cat School silver sword", WeaponQuality.Master, 95.0, 0.3, 0.22, 2.9, 3.3, 160.0, 3.1, 1080, n"cat");
        
        // Bear School Gear
        this.AddWitcherSteelSword(n"bear_steel_sword_basic", "Bear School Steel Sword", "Basic Bear School steel sword", WeaponQuality.Basic, 35.0, 0.15, 0.06, 1.4, 2.5, 130.0, 2.8, 120, n"bear");
        this.AddWitcherSteelSword(n"bear_steel_sword_enhanced", "Enhanced Bear School Steel Sword", "Enhanced Bear School steel sword", WeaponQuality.Enhanced, 50.0, 0.2, 0.08, 1.6, 2.8, 150.0, 3.1, 240, n"bear");
        this.AddWitcherSteelSword(n"bear_steel_sword_superior", "Superior Bear School Steel Sword", "Superior Bear School steel sword", WeaponQuality.Superior, 70.0, 0.25, 0.1, 1.8, 3.1, 170.0, 3.4, 480, n"bear");
        this.AddWitcherSteelSword(n"bear_steel_sword_master", "Master Bear School Steel Sword", "Master Bear School steel sword", WeaponQuality.Master, 95.0, 0.3, 0.12, 2.0, 3.4, 190.0, 3.7, 960, n"bear");
        
        this.AddWitcherSilverSword(n"bear_silver_sword_basic", "Bear School Silver Sword", "Basic Bear School silver sword", WeaponQuality.Basic, 40.0, 0.2, 0.08, 1.6, 2.8, 130.0, 3.0, 180, n"bear");
        this.AddWitcherSilverSword(n"bear_silver_sword_enhanced", "Enhanced Bear School Silver Sword", "Enhanced Bear School silver sword", WeaponQuality.Enhanced, 58.0, 0.25, 0.1, 1.8, 3.1, 150.0, 3.3, 360, n"bear");
        this.AddWitcherSilverSword(n"bear_silver_sword_superior", "Superior Bear School Silver Sword", "Superior Bear School silver sword", WeaponQuality.Superior, 80.0, 0.3, 0.12, 2.0, 3.4, 170.0, 3.6, 720, n"bear");
        this.AddWitcherSilverSword(n"bear_silver_sword_master", "Master Bear School Silver Sword", "Master Bear School silver sword", WeaponQuality.Master, 110.0, 0.35, 0.15, 2.2, 3.7, 190.0, 3.9, 1440, n"bear");
    }
    
    // Initialize unique weapons
    private func InitializeUniqueWeapons()
    {
        // Aerondight - The Lady of the Lake's sword
        this.AddUniqueSilverSword(n"aerondight", "Aerondight", "The Lady of the Lake's sword, grows stronger with each kill", WeaponQuality.Legendary, 100.0, 0.3, 0.2, 2.0, 3.0, 200.0, 3.0, 2000);
        
        // Iris - Olgierd's sword
        this.AddUniqueSteelSword(n"iris", "Iris", "Olgierd's cursed sword", WeaponQuality.Legendary, 80.0, 0.25, 0.15, 1.8, 2.8, 180.0, 2.8, 1500);
        
        // Winter's Blade - From the Wild Hunt
        this.AddUniqueSilverSword(n"winters_blade", "Winter's Blade", "A blade from the Wild Hunt", WeaponQuality.Legendary, 90.0, 0.28, 0.18, 1.9, 2.9, 190.0, 2.9, 1800);
        
        // Toussaint Knight's Steel Sword
        this.AddUniqueSteelSword(n"toussaint_steel_sword", "Toussaint Knight's Steel Sword", "A knight's sword from Toussaint", WeaponQuality.Master, 70.0, 0.22, 0.12, 1.7, 2.7, 160.0, 2.7, 800);
        
        // Toussaint Knight's Silver Sword
        this.AddUniqueSilverSword(n"toussaint_silver_sword", "Toussaint Knight's Silver Sword", "A knight's silver sword from Toussaint", WeaponQuality.Master, 85.0, 0.28, 0.15, 1.9, 3.0, 170.0, 3.0, 1000);
    }
    
    // Add steel sword
    private func AddSteelSword(id: CName, name: String, description: String, quality: WeaponQuality, damage: float, armorPen: float, critChance: float, critMult: float, speed: float, range: float, weight: float, value: int32)
    {
        let weapon: ref<WeaponData> = new WeaponData();
        weapon.Initialize(id, name, description, WeaponCategory.SteelSword, quality);
        
        let stats: ref<WeaponStats> = new WeaponStats();
        stats.Initialize(damage, armorPen, critChance, critMult, speed, range, 100.0, weight, value);
        weapon.SetStats(stats);
        
        ArrayPush(this.m_weapons, weapon);
    }
    
    // Add silver sword
    private func AddSilverSword(id: CName, name: String, description: String, quality: WeaponQuality, damage: float, armorPen: float, critChance: float, critMult: float, speed: float, range: float, weight: float, value: int32)
    {
        let weapon: ref<WeaponData> = new WeaponData();
        weapon.Initialize(id, name, description, WeaponCategory.SilverSword, quality);
        
        let stats: ref<WeaponStats> = new WeaponStats();
        stats.Initialize(damage, armorPen, critChance, critMult, speed, range, 100.0, weight, value);
        weapon.SetStats(stats);
        
        // Silver swords are more effective against monsters
        weapon.AddEffect(n"monster_damage_bonus");
        
        ArrayPush(this.m_weapons, weapon);
    }
    
    // Add crossbow
    private func AddCrossbow(id: CName, name: String, description: String, quality: WeaponQuality, damage: float, armorPen: float, critChance: float, critMult: float, speed: float, range: float, weight: float, value: int32)
    {
        let weapon: ref<WeaponData> = new WeaponData();
        weapon.Initialize(id, name, description, WeaponCategory.Crossbow, quality);
        
        let stats: ref<WeaponStats> = new WeaponStats();
        stats.Initialize(damage, armorPen, critChance, critMult, speed, range, 100.0, weight, value);
        weapon.SetStats(stats);
        
        // Crossbows have piercing effect
        weapon.AddEffect(n"piercing_shot");
        
        ArrayPush(this.m_weapons, weapon);
    }
    
    // Add dagger
    private func AddDagger(id: CName, name: String, description: String, quality: WeaponQuality, damage: float, armorPen: float, critChance: float, critMult: float, speed: float, range: float, weight: float, value: int32)
    {
        let weapon: ref<WeaponData> = new WeaponData();
        weapon.Initialize(id, name, description, WeaponCategory.Dagger, quality);
        
        let stats: ref<WeaponStats> = new WeaponStats();
        stats.Initialize(damage, armorPen, critChance, critMult, speed, range, 100.0, weight, value);
        weapon.SetStats(stats);
        
        // Daggers have bleeding effect
        weapon.AddEffect(n"bleeding_wound");
        
        ArrayPush(this.m_weapons, weapon);
    }
    
    // Add axe
    private func AddAxe(id: CName, name: String, description: String, quality: WeaponQuality, damage: float, armorPen: float, critChance: float, critMult: float, speed: float, range: float, weight: float, value: int32)
    {
        let weapon: ref<WeaponData> = new WeaponData();
        weapon.Initialize(id, name, description, WeaponCategory.Axe, quality);
        
        let stats: ref<WeaponStats> = new WeaponStats();
        stats.Initialize(damage, armorPen, critChance, critMult, speed, range, 100.0, weight, value);
        weapon.SetStats(stats);
        
        ArrayPush(this.m_weapons, weapon);
    }
    
    // Add mace
    private func AddMace(id: CName, name: String, description: String, quality: WeaponQuality, damage: float, armorPen: float, critChance: float, critMult: float, speed: float, range: float, weight: float, value: int32)
    {
        let weapon: ref<WeaponData> = new WeaponData();
        weapon.Initialize(id, name, description, WeaponCategory.Mace, quality);
        
        let stats: ref<WeaponStats> = new WeaponStats();
        stats.Initialize(damage, armorPen, critChance, critMult, speed, range, 100.0, weight, value);
        weapon.SetStats(stats);
        
        ArrayPush(this.m_weapons, weapon);
    }
    
    // Add spear
    private func AddSpear(id: CName, name: String, description: String, quality: WeaponQuality, damage: float, armorPen: float, critChance: float, critMult: float, speed: float, range: float, weight: float, value: int32)
    {
        let weapon: ref<WeaponData> = new WeaponData();
        weapon.Initialize(id, name, description, WeaponCategory.Spear, quality);
        
        let stats: ref<WeaponStats> = new WeaponStats();
        stats.Initialize(damage, armorPen, critChance, critMult, speed, range, 100.0, weight, value);
        weapon.SetStats(stats);
        
        ArrayPush(this.m_weapons, weapon);
    }
    
    // Add Witcher steel sword
    private func AddWitcherSteelSword(id: CName, name: String, description: String, quality: WeaponQuality, damage: float, armorPen: float, critChance: float, critMult: float, speed: float, range: float, weight: float, value: int32, school: CName)
    {
        let weapon: ref<WeaponData> = new WeaponData();
        weapon.Initialize(id, name, description, WeaponCategory.SteelSword, quality);
        
        let stats: ref<WeaponStats> = new WeaponStats();
        stats.Initialize(damage, armorPen, critChance, critMult, speed, range, 100.0, weight, value);
        weapon.SetStats(stats);
        
        weapon.SetWitcherGear(school);
        weapon.AddEffect(n"witcher_gear_bonus");
        
        ArrayPush(this.m_weapons, weapon);
    }
    
    // Add Witcher silver sword
    private func AddWitcherSilverSword(id: CName, name: String, description: String, quality: WeaponQuality, damage: float, armorPen: float, critChance: float, critMult: float, speed: float, range: float, weight: float, value: int32, school: CName)
    {
        let weapon: ref<WeaponData> = new WeaponData();
        weapon.Initialize(id, name, description, WeaponCategory.SilverSword, quality);
        
        let stats: ref<WeaponStats> = new WeaponStats();
        stats.Initialize(damage, armorPen, critChance, critMult, speed, range, 100.0, weight, value);
        weapon.SetStats(stats);
        
        weapon.SetWitcherGear(school);
        weapon.AddEffect(n"witcher_gear_bonus");
        weapon.AddEffect(n"monster_damage_bonus");
        
        ArrayPush(this.m_weapons, weapon);
    }
    
    // Add unique steel sword
    private func AddUniqueSteelSword(id: CName, name: String, description: String, quality: WeaponQuality, damage: float, armorPen: float, critChance: float, critMult: float, speed: float, range: float, weight: float, value: int32)
    {
        let weapon: ref<WeaponData> = new WeaponData();
        weapon.Initialize(id, name, description, WeaponCategory.SteelSword, quality);
        
        let stats: ref<WeaponStats> = new WeaponStats();
        stats.Initialize(damage, armorPen, critChance, critMult, speed, range, 100.0, weight, value);
        weapon.SetStats(stats);
        
        weapon.SetUnique(true);
        weapon.AddEffect(n"unique_weapon_bonus");
        
        ArrayPush(this.m_weapons, weapon);
    }
    
    // Add unique silver sword
    private func AddUniqueSilverSword(id: CName, name: String, description: String, quality: WeaponQuality, damage: float, armorPen: float, critChance: float, critMult: float, speed: float, range: float, weight: float, value: int32)
    {
        let weapon: ref<WeaponData> = new WeaponData();
        weapon.Initialize(id, name, description, WeaponCategory.SilverSword, quality);
        
        let stats: ref<WeaponStats> = new WeaponStats();
        stats.Initialize(damage, armorPen, critChance, critMult, speed, range, 100.0, weight, value);
        weapon.SetStats(stats);
        
        weapon.SetUnique(true);
        weapon.AddEffect(n"unique_weapon_bonus");
        weapon.AddEffect(n"monster_damage_bonus");
        
        ArrayPush(this.m_weapons, weapon);
    }
    
    // Get weapon by ID
    public func GetWeapon(weaponId: CName) -> ref<WeaponData>
    {
        let i: int32 = 0;
        while i < ArraySize(this.m_weapons)
        {
            let weapon: ref<WeaponData> = this.m_weapons[i];
            if IsDefined(weapon) && weapon.id == weaponId
            {
                return weapon;
            }
            i += 1;
        }
        return null;
    }
    
    // Get weapons by category
    public func GetWeaponsByCategory(category: WeaponCategory) -> array<ref<WeaponData>>
    {
        let weapons: array<ref<WeaponData>> = [];
        let i: int32 = 0;
        while i < ArraySize(this.m_weapons)
        {
            let weapon: ref<WeaponData> = this.m_weapons[i];
            if IsDefined(weapon) && weapon.category == category
            {
                ArrayPush(weapons, weapon);
            }
            i += 1;
        }
        return weapons;
    }
    
    // Get weapons by quality
    public func GetWeaponsByQuality(quality: WeaponQuality) -> array<ref<WeaponData>>
    {
        let weapons: array<ref<WeaponData>> = [];
        let i: int32 = 0;
        while i < ArraySize(this.m_weapons)
        {
            let weapon: ref<WeaponData> = this.m_weapons[i];
            if IsDefined(weapon) && weapon.quality == quality
            {
                ArrayPush(weapons, weapon);
            }
            i += 1;
        }
        return weapons;
    }
    
    // Get Witcher gear by school
    public func GetWitcherGearBySchool(school: CName) -> array<ref<WeaponData>>
    {
        let weapons: array<ref<WeaponData>> = [];
        let i: int32 = 0;
        while i < ArraySize(this.m_weapons)
        {
            let weapon: ref<WeaponData> = this.m_weapons[i];
            if IsDefined(weapon) && weapon.isWitcherGear && weapon.witcherSchool == school
            {
                ArrayPush(weapons, weapon);
            }
            i += 1;
        }
        return weapons;
    }
    
    // Get unique weapons
    public func GetUniqueWeapons() -> array<ref<WeaponData>>
    {
        let weapons: array<ref<WeaponData>> = [];
        let i: int32 = 0;
        while i < ArraySize(this.m_weapons)
        {
            let weapon: ref<WeaponData> = this.m_weapons[i];
            if IsDefined(weapon) && weapon.isUnique
            {
                ArrayPush(weapons, weapon);
            }
            i += 1;
        }
        return weapons;
    }
    
    // Get all weapons
    public func GetAllWeapons() -> array<ref<WeaponData>>
    {
        return this.m_weapons;
    }
    
    // Check if initialized
    public func IsInitialized() -> bool
    {
        return this.m_isInitialized;
    }
}
