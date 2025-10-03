// Witcher3-MP Game Mode Manager
// Manages different multiplayer game modes (Story/Exploration)
// Compatible with The Witcher 3: Wild Hunt Next-Gen (v4.0+)

using game;
using game::player;
using game::quests;
using game::ui;
using game::inventory;
using game::stats;
using game::effects;

// Game mode enumeration
enum class GameMode
{
    Story = 0,
    Exploration = 1
}

// Quest type enumeration
enum class QuestType
{
    MainQuest = 0,
    SideQuest = 1,
    WitcherContract = 2,
    TreasureHunt = 3,
    DynamicEvent = 4
}

// Game mode configuration
class MP_GameModeConfig
{
    public let mode: GameMode;
    public let enableMainQuests: bool;
    public let enableCutscenes: bool;
    public let enableDialogs: bool;
    public let enableWitcherContracts: bool;
    public let enableTreasureHunts: bool;
    public let enableQuestionMarks: bool;
    public let enableDynamicEvents: bool;
    public let unlockAllAreas: bool;
    public let syncInventory: bool;
    public let syncContracts: bool;
    public let saveExternalProgress: bool;
    public let hostControlsDecisions: bool;
    public let syncXpRewards: bool;
    public let syncQuestProgress: bool;
    
    public func Initialize()
    {
        this.mode = GameMode.Story;
        this.enableMainQuests = true;
        this.enableCutscenes = true;
        this.enableDialogs = true;
        this.enableWitcherContracts = true;
        this.enableTreasureHunts = true;
        this.enableQuestionMarks = true;
        this.enableDynamicEvents = true;
        this.unlockAllAreas = false;
        this.syncInventory = true;
        this.syncContracts = true;
        this.saveExternalProgress = false;
        this.hostControlsDecisions = true;
        this.syncXpRewards = true;
        this.syncQuestProgress = true;
    }
    
    public func SetStoryMode()
    {
        this.mode = GameMode.Story;
        this.enableMainQuests = true;
        this.enableCutscenes = true;
        this.enableDialogs = true;
        this.enableWitcherContracts = true;
        this.enableTreasureHunts = true;
        this.enableQuestionMarks = true;
        this.enableDynamicEvents = true;
        this.unlockAllAreas = false;
        this.syncInventory = true;
        this.syncContracts = true;
        this.saveExternalProgress = false;
        this.hostControlsDecisions = true;
        this.syncXpRewards = true;
        this.syncQuestProgress = true;
    }
    
    public func SetExplorationMode()
    {
        this.mode = GameMode.Exploration;
        this.enableMainQuests = false;
        this.enableCutscenes = false;
        this.enableDialogs = false;
        this.enableWitcherContracts = true;
        this.enableTreasureHunts = true;
        this.enableQuestionMarks = true;
        this.enableDynamicEvents = true;
        this.unlockAllAreas = true;
        this.syncInventory = true;
        this.syncContracts = true;
        this.saveExternalProgress = true;
        this.hostControlsDecisions = false;
        this.syncXpRewards = true;
        this.syncQuestProgress = false;
    }
}

// Main Game Mode Manager class
class MP_GameModeManager
{
    private let m_config: ref<MP_GameModeConfig>;
    private let m_currentMode: GameMode;
    private let m_isInitialized: bool;
    private let m_questManager: ref<MP_QuestManager>;
    private let m_areaManager: ref<MP_AreaManager>;
    private let m_contractManager: ref<MP_ContractManager>;
    private let m_inventorySystem: ref<MP_InventorySystem>;
    private let m_logger: ref<MP_Logger>;
    
    // Initialize game mode manager
    public func Initialize()
    {
        if this.m_isInitialized
        {
            return;
        }
        
        // Initialize configuration
        this.m_config = new MP_GameModeConfig();
        this.m_config.Initialize();
        
        // Initialize subsystems
        this.m_questManager = new MP_QuestManager();
        this.m_questManager.Initialize();
        
        this.m_areaManager = new MP_AreaManager();
        this.m_areaManager.Initialize();
        
        this.m_contractManager = new MP_ContractManager();
        this.m_contractManager.Initialize();
        
        this.m_inventorySystem = new MP_InventorySystem();
        this.m_inventorySystem.Initialize();
        
        this.m_logger = new MP_Logger();
        this.m_logger.Initialize();
        
        // Load configuration from file
        this.LoadConfiguration();
        
        this.m_isInitialized = true;
        this.m_logger.LogInfo("Game Mode Manager initialized");
    }
    
    // Load configuration from mp_config.json
    private func LoadConfiguration()
    {
        let configManager: ref<MP_Config> = GetMPConfig();
        if IsDefined(configManager)
        {
            let gameMode: String = configManager.GetStringValue("gameMode.mode", "story");
            
            if gameMode == "exploration"
            {
                this.SetGameMode(GameMode.Exploration);
            }
            else
            {
                this.SetGameMode(GameMode.Story);
            }
            
            this.m_logger.LogInfo(s"Loaded game mode configuration: \(gameMode)");
        }
    }
    
    // Set game mode
    public func SetGameMode(mode: GameMode)
    {
        this.m_currentMode = mode;
        
        if mode == GameMode.Story
        {
            this.m_config.SetStoryMode();
            this.m_logger.LogInfo("Switched to Story Mode");
        }
        else if mode == GameMode.Exploration
        {
            this.m_config.SetExplorationMode();
            this.m_logger.LogInfo("Switched to Exploration Mode");
        }
        
        // Apply mode-specific settings
        this.ApplyModeSettings();
        
        // Log mode change
        this.LogModeChange();
    }
    
    // Apply mode-specific settings
    private func ApplyModeSettings()
    {
        if this.m_currentMode == GameMode.Story
        {
            this.EnableStoryModeFeatures();
        }
        else if this.m_currentMode == GameMode.Exploration
        {
            this.EnableExplorationModeFeatures();
        }
    }
    
    // Enable story mode features
    private func EnableStoryModeFeatures()
    {
        // Enable main quests
        if this.m_config.enableMainQuests
        {
            this.m_questManager.EnableMainQuests();
        }
        
        // Enable cutscenes
        if this.m_config.enableCutscenes
        {
            this.EnableCutscenes();
        }
        
        // Enable dialogs
        if this.m_config.enableDialogs
        {
            this.EnableDialogs();
        }
        
        // Enable quest synchronization
        if this.m_config.syncQuestProgress
        {
            this.m_questManager.EnableQuestSynchronization();
        }
        
        this.m_logger.LogInfo("Story mode features enabled");
    }
    
    // Enable exploration mode features
    private func EnableExplorationModeFeatures()
    {
        // Disable main quests
        if !this.m_config.enableMainQuests
        {
            this.m_questManager.DisableMainQuests();
        }
        
        // Disable cutscenes
        if !this.m_config.enableCutscenes
        {
            this.DisableCutscenes();
        }
        
        // Disable dialogs
        if !this.m_config.enableDialogs
        {
            this.DisableDialogs();
        }
        
        // Unlock all areas
        if this.m_config.unlockAllAreas
        {
            this.m_areaManager.UnlockAllAreas();
        }
        
        // Enable witcher contracts
        if this.m_config.enableWitcherContracts
        {
            this.m_contractManager.EnableContracts();
        }
        
        // Enable treasure hunts
        if this.m_config.enableTreasureHunts
        {
            this.EnableTreasureHunts();
        }
        
        // Enable question marks
        if this.m_config.enableQuestionMarks
        {
            this.EnableQuestionMarks();
        }
        
        // Enable dynamic events
        if this.m_config.enableDynamicEvents
        {
            this.EnableDynamicEvents();
        }
        
        // Enable external progress saving
        if this.m_config.saveExternalProgress
        {
            this.EnableExternalProgressSaving();
        }
        
        this.m_logger.LogInfo("Exploration mode features enabled");
    }
    
    // Enable cutscenes
    private func EnableCutscenes()
    {
        // Implementation would enable cutscene system
        this.m_logger.LogInfo("Cutscenes enabled");
    }
    
    // Disable cutscenes
    private func DisableCutscenes()
    {
        // Implementation would disable cutscene system
        this.m_logger.LogInfo("Cutscenes disabled");
    }
    
    // Enable dialogs
    private func EnableDialogs()
    {
        // Implementation would enable dialog system
        this.m_logger.LogInfo("Dialogs enabled");
    }
    
    // Disable dialogs
    private func DisableDialogs()
    {
        // Implementation would disable dialog system
        this.m_logger.LogInfo("Dialogs disabled");
    }
    
    // Enable treasure hunts
    private func EnableTreasureHunts()
    {
        // Implementation would enable treasure hunt system
        this.m_logger.LogInfo("Treasure hunts enabled");
    }
    
    // Enable question marks
    private func EnableQuestionMarks()
    {
        // Implementation would enable question mark system
        this.m_logger.LogInfo("Question marks enabled");
    }
    
    // Enable dynamic events
    private func EnableDynamicEvents()
    {
        // Implementation would enable dynamic event system
        this.m_logger.LogInfo("Dynamic events enabled");
    }
    
    // Enable external progress saving
    private func EnableExternalProgressSaving()
    {
        // Implementation would enable external save system
        this.m_logger.LogInfo("External progress saving enabled");
    }
    
    // Log mode change
    private func LogModeChange()
    {
        let modeName: String = this.m_currentMode == GameMode.Story ? "Story" : "Exploration";
        this.m_logger.LogInfo(s"Game mode changed to: \(modeName)");
        
        // Log to external file
        this.LogToFile(s"Game mode: \(modeName) - Timestamp: \(GameInstance.GetGameTime())");
    }
    
    // Log to external file
    private func LogToFile(message: String)
    {
        // Implementation would write to logs/mp_session.log
        this.m_logger.LogToFile("mp_session.log", message);
    }
    
    // Check if quest should be enabled
    public func ShouldEnableQuest(questType: QuestType) -> bool
    {
        switch questType
        {
            case QuestType.MainQuest:
                return this.m_config.enableMainQuests;
            case QuestType.SideQuest:
                return true; // Always enable side quests
            case QuestType.WitcherContract:
                return this.m_config.enableWitcherContracts;
            case QuestType.TreasureHunt:
                return this.m_config.enableTreasureHunts;
            case QuestType.DynamicEvent:
                return this.m_config.enableDynamicEvents;
            default:
                return true;
        }
    }
    
    // Check if area should be unlocked
    public func ShouldUnlockArea(areaId: String) -> bool
    {
        if this.m_config.unlockAllAreas
        {
            return true;
        }
        
        // In story mode, areas unlock based on quest progress
        return this.m_currentMode == GameMode.Story;
    }
    
    // Check if cutscene should play
    public func ShouldPlayCutscene(cutsceneId: String) -> bool
    {
        return this.m_config.enableCutscenes;
    }
    
    // Check if dialog should be shown
    public func ShouldShowDialog(dialogId: String) -> bool
    {
        return this.m_config.enableDialogs;
    }
    
    // Check if inventory should sync
    public func ShouldSyncInventory() -> bool
    {
        return this.m_config.syncInventory;
    }
    
    // Check if contracts should sync
    public func ShouldSyncContracts() -> bool
    {
        return this.m_config.syncContracts;
    }
    
    // Check if XP should sync
    public func ShouldSyncXP() -> bool
    {
        return this.m_config.syncXpRewards;
    }
    
    // Check if quest progress should sync
    public func ShouldSyncQuestProgress() -> bool
    {
        return this.m_config.syncQuestProgress;
    }
    
    // Check if host controls decisions
    public func DoesHostControlDecisions() -> bool
    {
        return this.m_config.hostControlsDecisions;
    }
    
    // Get current game mode
    public func GetCurrentMode() -> GameMode
    {
        return this.m_currentMode;
    }
    
    // Get mode name
    public func GetModeName() -> String
    {
        return this.m_currentMode == GameMode.Story ? "Story" : "Exploration";
    }
    
    // Get configuration
    public func GetConfig() -> ref<MP_GameModeConfig>
    {
        return this.m_config;
    }
    
    // Check if initialized
    public func IsInitialized() -> bool
    {
        return this.m_isInitialized;
    }
}

// Quest Manager for handling quests based on game mode
class MP_QuestManager
{
    private let m_isInitialized: bool;
    private let m_mainQuestsEnabled: bool;
    private let m_questSyncEnabled: bool;
    private let m_activeQuests: array<wref<Quest>>;
    private let m_disabledQuests: array<CName>;
    
    public func Initialize()
    {
        this.m_isInitialized = true;
        this.m_mainQuestsEnabled = true;
        this.m_questSyncEnabled = true;
        this.m_activeQuests = [];
        this.m_disabledQuests = [];
        
        // Add main quest IDs to disabled list for exploration mode
        this.m_disabledQuests = [
            n"q001_white_orchard",
            n"q002_kaer_morhen",
            n"q003_velen",
            n"q004_novigrad",
            n"q005_skellige",
            n"q006_blood_and_wine"
        ];
    }
    
    public func EnableMainQuests()
    {
        this.m_mainQuestsEnabled = true;
        this.m_disabledQuests = [];
    }
    
    public func DisableMainQuests()
    {
        this.m_mainQuestsEnabled = false;
        // Main quests are already in disabled list
    }
    
    public func EnableQuestSynchronization()
    {
        this.m_questSyncEnabled = true;
    }
    
    public func DisableQuestSynchronization()
    {
        this.m_questSyncEnabled = false;
    }
    
    public func IsQuestEnabled(questId: CName) -> bool
    {
        if !this.m_mainQuestsEnabled
        {
            let i: int32 = 0;
            while i < ArraySize(this.m_disabledQuests)
            {
                if this.m_disabledQuests[i] == questId
                {
                    return false;
                }
                i += 1;
            }
        }
        
        return true;
    }
    
    public func AddQuest(quest: wref<Quest>)
    {
        if this.IsQuestEnabled(quest.GetName())
        {
            ArrayPush(this.m_activeQuests, quest);
        }
    }
    
    public func RemoveQuest(quest: wref<Quest>)
    {
        let i: int32 = 0;
        while i < ArraySize(this.m_activeQuests)
        {
            if this.m_activeQuests[i] == quest
            {
                ArrayErase(this.m_activeQuests, i);
                break;
            }
            i += 1;
        }
    }
    
    public func GetActiveQuests() -> array<wref<Quest>>
    {
        return this.m_activeQuests;
    }
}

// Area Manager for handling area unlocking
class MP_AreaManager
{
    private let m_isInitialized: bool;
    private let m_unlockedAreas: array<String>;
    private let m_allAreasUnlocked: bool;
    
    public func Initialize()
    {
        this.m_isInitialized = true;
        this.m_unlockedAreas = [];
        this.m_allAreasUnlocked = false;
    }
    
    public func UnlockAllAreas()
    {
        this.m_allAreasUnlocked = true;
        this.m_unlockedAreas = [
            "white_orchard",
            "velen",
            "novigrad",
            "skellige",
            "kaer_morhen",
            "toussaint"
        ];
    }
    
    public func LockAllAreas()
    {
        this.m_allAreasUnlocked = false;
        this.m_unlockedAreas = [];
    }
    
    public func UnlockArea(areaId: String)
    {
        if !this.IsAreaUnlocked(areaId)
        {
            ArrayPush(this.m_unlockedAreas, areaId);
        }
    }
    
    public func IsAreaUnlocked(areaId: String) -> bool
    {
        if this.m_allAreasUnlocked
        {
            return true;
        }
        
        let i: int32 = 0;
        while i < ArraySize(this.m_unlockedAreas)
        {
            if this.m_unlockedAreas[i] == areaId
            {
                return true;
            }
            i += 1;
        }
        
        return false;
    }
}

// Contract Manager for handling witcher contracts
class MP_ContractManager
{
    private let m_isInitialized: bool;
    private let m_contractsEnabled: bool;
    private let m_activeContracts: array<wref<Quest>>;
    
    public func Initialize()
    {
        this.m_isInitialized = true;
        this.m_contractsEnabled = true;
        this.m_activeContracts = [];
    }
    
    public func EnableContracts()
    {
        this.m_contractsEnabled = true;
    }
    
    public func DisableContracts()
    {
        this.m_contractsEnabled = false;
    }
    
    public func AddContract(contract: wref<Quest>)
    {
        if this.m_contractsEnabled
        {
            ArrayPush(this.m_activeContracts, contract);
        }
    }
    
    public func RemoveContract(contract: wref<Quest>)
    {
        let i: int32 = 0;
        while i < ArraySize(this.m_activeContracts)
        {
            if this.m_activeContracts[i] == contract
            {
                ArrayErase(this.m_activeContracts, i);
                break;
            }
            i += 1;
        }
    }
    
    public func GetActiveContracts() -> array<wref<Quest>>
    {
        return this.m_activeContracts;
    }
}

// Global game mode manager instance
var ref<MP_GameModeManager> g_gameModeManager;

// Global functions
public func InitializeGameModeManager()
{
    g_gameModeManager = new MP_GameModeManager();
    g_gameModeManager.Initialize();
}

public func SetGameMode(mode: GameMode)
{
    if IsDefined(g_gameModeManager)
    {
        g_gameModeManager.SetGameMode(mode);
    }
}

public func GetGameModeManager() -> ref<MP_GameModeManager>
{
    return g_gameModeManager;
}

public func IsMainQuestEnabled(questId: CName) -> bool
{
    if IsDefined(g_gameModeManager)
    {
        return g_gameModeManager.ShouldEnableQuest(QuestType.MainQuest);
    }
    return true;
}

public func IsAreaUnlocked(areaId: String) -> bool
{
    if IsDefined(g_gameModeManager)
    {
        return g_gameModeManager.ShouldUnlockArea(areaId);
    }
    return false;
}

public func ShouldPlayCutscene(cutsceneId: String) -> bool
{
    if IsDefined(g_gameModeManager)
    {
        return g_gameModeManager.ShouldPlayCutscene(cutsceneId);
    }
    return true;
}

public func ShouldShowDialog(dialogId: String) -> bool
{
    if IsDefined(g_gameModeManager)
    {
        return g_gameModeManager.ShouldShowDialog(dialogId);
    }
    return true;
}
