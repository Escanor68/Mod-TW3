// Witcher3-MP Mod Initializer
// Main entry point for the multiplayer mod

using game;
using game::player;
using game::quests;
using game::ui;
using game::inventory;
using game::stats;
using game::effects;

// Global mod instance
private let g_mpMod: ref<MP_ModInitializer>;

// Main mod initializer class
class MP_ModInitializer
{
    private let m_isInitialized: bool;
    private let m_redkitIntegration: ref<MP_REDkitIntegration>;
    private let m_gameHooks: ref<MP_GameHooks>;
    private let m_config: ref<MP_Config>;
    
    // Initialize the mod
    public func Initialize()
    {
        if this.m_isInitialized
        {
            return;
        }
        
        LogChannel(n"Witcher3-MP", "Initializing Witcher3-MP mod...");
        
        // Load configuration
        this.LoadConfiguration();
        
        // Initialize REDkit integration
        this.m_redkitIntegration = new MP_REDkitIntegration();
        this.m_redkitIntegration.Initialize();
        
        // Initialize game hooks
        this.m_gameHooks = new MP_GameHooks();
        this.InitializeGameHooks();
        
        // Create multiplayer UI
        this.CreateMultiplayerUI();
        
        // Register mod with game systems
        this.RegisterWithGameSystems();
        
        this.m_isInitialized = true;
        
        LogChannel(n"Witcher3-MP", "Witcher3-MP mod initialized successfully");
    }
    
    // Load mod configuration
    private func LoadConfiguration()
    {
        this.m_config = new MP_Config();
        this.m_config.LoadFromFile("mod/config/mp_config.json");
        
        LogChannel(n"Witcher3-MP", "Configuration loaded");
    }
    
    // Initialize game hooks
    private func InitializeGameHooks()
    {
        // Register hooks with game systems
        this.RegisterPlayerHooks();
        this.RegisterCombatHooks();
        this.RegisterInventoryHooks();
        this.RegisterQuestHooks();
        this.RegisterWorldHooks();
        
        LogChannel(n"Witcher3-MP", "Game hooks initialized");
    }
    
    // Register player hooks
    private func RegisterPlayerHooks()
    {
        // Hook into player movement
        let playerSystem: ref<PlayerSystem> = GameInstance.GetPlayerSystem();
        if IsDefined(playerSystem)
        {
            // Register movement callback
            playerSystem.RegisterMovementCallback(this.OnPlayerMove);
            
            // Register health change callback
            playerSystem.RegisterHealthChangeCallback(this.OnPlayerHealthChanged);
        }
    }
    
    // Register combat hooks
    private func RegisterCombatHooks()
    {
        // Hook into combat system
        let combatSystem: ref<CombatSystem> = GameInstance.GetCombatSystem();
        if IsDefined(combatSystem)
        {
            // Register attack callback
            combatSystem.RegisterAttackCallback(this.OnPlayerAttack);
            
            // Register sign cast callback
            combatSystem.RegisterSignCastCallback(this.OnPlayerCastSign);
        }
    }
    
    // Register inventory hooks
    private func RegisterInventoryHooks()
    {
        // Hook into inventory system
        let inventorySystem: ref<InventorySystem> = GameInstance.GetInventorySystem();
        if IsDefined(inventorySystem)
        {
            // Register inventory change callback
            inventorySystem.RegisterInventoryChangeCallback(this.OnPlayerInventoryChanged);
        }
    }
    
    // Register quest hooks
    private func RegisterQuestHooks()
    {
        // Hook into quest system
        let questSystem: ref<QuestSystem> = GameInstance.GetQuestSystem();
        if IsDefined(questSystem)
        {
            // Register quest state change callback
            questSystem.RegisterQuestStateChangeCallback(this.OnQuestStateChanged);
        }
    }
    
    // Register world hooks
    private func RegisterWorldHooks()
    {
        // Hook into world systems
        let weatherSystem: ref<WeatherSystem> = GameInstance.GetWeatherSystem();
        if IsDefined(weatherSystem)
        {
            // Register weather change callback
            weatherSystem.RegisterWeatherChangeCallback(this.OnWeatherChanged);
        }
        
        let timeSystem: ref<TimeSystem> = GameInstance.GetTimeSystem();
        if IsDefined(timeSystem)
        {
            // Register time change callback
            timeSystem.RegisterTimeChangeCallback(this.OnTimeChanged);
        }
    }
    
    // Create multiplayer UI
    private func CreateMultiplayerUI()
    {
        if IsDefined(this.m_redkitIntegration)
        {
            let uiRoot: ref<inkWidget> = this.m_redkitIntegration.CreateMultiplayerUI();
            if IsDefined(uiRoot)
            {
                // Add UI to game
                let uiSystem: ref<UISystem> = GameInstance.GetUISystem();
                if IsDefined(uiSystem)
                {
                    uiSystem.AddWidget(uiRoot);
                }
            }
        }
    }
    
    // Register mod with game systems
    private func RegisterWithGameSystems()
    {
        // Register with mod system
        let modSystem: ref<ModSystem> = GameInstance.GetModSystem();
        if IsDefined(modSystem)
        {
            modSystem.RegisterMod(this);
        }
        
        // Register with save system
        let saveSystem: ref<SaveSystem> = GameInstance.GetSaveSystem();
        if IsDefined(saveSystem)
        {
            saveSystem.RegisterModData(this);
        }
    }
    
    // Player movement callback
    private func OnPlayerMove(playerRef: wref<Player>, newPosition: Vector4)
    {
        if IsDefined(this.m_gameHooks)
        {
            this.m_gameHooks.OnPlayerMove(playerRef, newPosition);
        }
    }
    
    // Player health change callback
    private func OnPlayerHealthChanged(playerRef: wref<Player>, oldHealth: float, newHealth: float)
    {
        if IsDefined(this.m_gameHooks)
        {
            this.m_gameHooks.OnPlayerHealthChanged(playerRef, oldHealth, newHealth);
        }
    }
    
    // Player attack callback
    private func OnPlayerAttack(playerRef: wref<Player>, target: wref<Entity>, damage: float)
    {
        if IsDefined(this.m_gameHooks)
        {
            this.m_gameHooks.OnPlayerAttack(playerRef, target, damage);
        }
    }
    
    // Player sign cast callback
    private func OnPlayerCastSign(playerRef: wref<Player>, signType: gamedataSignType, target: wref<Entity>)
    {
        if IsDefined(this.m_gameHooks)
        {
            this.m_gameHooks.OnPlayerCastSign(playerRef, signType, target);
        }
    }
    
    // Player inventory change callback
    private func OnPlayerInventoryChanged(playerRef: wref<Player>, item: wref<Item>, added: bool)
    {
        if IsDefined(this.m_gameHooks)
        {
            this.m_gameHooks.OnPlayerInventoryChanged(playerRef, item, added);
        }
    }
    
    // Quest state change callback
    private func OnQuestStateChanged(questRef: wref<Quest>, newState: questQuestState)
    {
        if IsDefined(this.m_gameHooks)
        {
            this.m_gameHooks.OnQuestStateChanged(questRef, newState);
        }
    }
    
    // Weather change callback
    private func OnWeatherChanged(weatherType: CName, intensity: float)
    {
        if IsDefined(this.m_gameHooks)
        {
            this.m_gameHooks.OnWeatherChanged(weatherType, intensity);
        }
    }
    
    // Time change callback
    private func OnTimeChanged(newTime: GameTime)
    {
        if IsDefined(this.m_gameHooks)
        {
            this.m_gameHooks.OnTimeChanged(newTime);
        }
    }
    
    // Start multiplayer server
    public func StartMultiplayerServer(port: int32) -> bool
    {
        if IsDefined(this.m_redkitIntegration)
        {
            let serverManager: ref<MP_ServerManager> = this.m_redkitIntegration.GetServerManager();
            if IsDefined(serverManager)
            {
                return serverManager.StartServer(port);
            }
        }
        return false;
    }
    
    // Stop multiplayer server
    public func StopMultiplayerServer()
    {
        if IsDefined(this.m_redkitIntegration)
        {
            let serverManager: ref<MP_ServerManager> = this.m_redkitIntegration.GetServerManager();
            if IsDefined(serverManager)
            {
                serverManager.StopServer();
            }
        }
    }
    
    // Get mod configuration
    public func GetConfig() -> ref<MP_Config>
    {
        return this.m_config;
    }
    
    // Check if mod is initialized
    public func IsInitialized() -> bool
    {
        return this.m_isInitialized;
    }
}

// Global initialization function
public func InitializeWitcher3MP()
{
    if !IsDefined(g_mpMod)
    {
        g_mpMod = new MP_ModInitializer();
        g_mpMod.Initialize();
    }
}

// Get mod instance
public func GetWitcher3MPMod() -> ref<MP_ModInitializer>
{
    return g_mpMod;
}
