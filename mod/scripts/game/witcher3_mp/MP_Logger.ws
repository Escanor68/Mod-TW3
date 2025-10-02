// Witcher3-MP Logger System
// WitcherScript interface for the C++ logging system

class MP_Logger
{
    private static var s_instance: MP_Logger;
    private var m_initialized: bool;
    private var m_logLevel: int;
    private var m_fileLogging: bool;
    private var m_consoleLogging: bool;
    private var m_performanceLogging: bool;
    
    // Log levels
    private const DEBUG_LEVEL: int = 0;
    private const INFO_LEVEL: int = 1;
    private const WARNING_LEVEL: int = 2;
    private const ERROR_LEVEL: int = 3;
    private const CRITICAL_LEVEL: int = 4;
    
    // Log categories
    private const GENERAL_CATEGORY: int = 0;
    private const NETWORK_CATEGORY: int = 1;
    private const COMBAT_CATEGORY: int = 2;
    private const QUEST_CATEGORY: int = 3;
    private const INVENTORY_CATEGORY: int = 4;
    private const DIALOG_CATEGORY: int = 5;
    private const PLAYER_CATEGORY: int = 6;
    private const SYSTEM_CATEGORY: int = 7;
    
    public static function GetInstance(): MP_Logger
    {
        if (s_instance == null)
        {
            s_instance = new MP_Logger();
        }
        return s_instance;
    }
    
    private function Initialize()
    {
        if (m_initialized)
            return;
            
        m_logLevel = INFO_LEVEL;
        m_fileLogging = true;
        m_consoleLogging = true;
        m_performanceLogging = false;
        
        // Initialize C++ logger
        InitializeNativeLogger();
        
        m_initialized = true;
        LogInfo("MP_Logger initialized");
    }
    
    // Native function calls to C++ logger
    private native function InitializeNativeLogger();
    private native function LogMessage(level: int, category: int, message: String, source: String);
    private native function FlushLogsNative();
    private native function SetLogLevelNative(level: int);
    private native function EnableCategoryNative(category: int, enable: bool);
    private native function StartPerformanceLoggingNative();
    private native function StopPerformanceLoggingNative();
    private native function LogPerformanceNative(operation: String, duration: float);
    
    // Public logging methods
    public function LogDebug(category: int, message: String, source: String = "")
    {
        LogMessage(DEBUG_LEVEL, category, message, source);
    }
    
    public function LogInfo(category: int, message: String, source: String = "")
    {
        LogMessage(INFO_LEVEL, category, message, source);
    }
    
    public function LogWarning(category: int, message: String, source: String = "")
    {
        LogMessage(WARNING_LEVEL, category, message, source);
    }
    
    public function LogError(category: int, message: String, source: String = "")
    {
        LogMessage(ERROR_LEVEL, category, message, source);
    }
    
    public function LogCritical(category: int, message: String, source: String = "")
    {
        LogMessage(CRITICAL_LEVEL, category, message, source);
    }
    
    // Convenience methods for different categories
    public function LogDebug(message: String, source: String = "")
    {
        LogDebug(GENERAL_CATEGORY, message, source);
    }
    
    public function LogInfo(message: String, source: String = "")
    {
        LogInfo(GENERAL_CATEGORY, message, source);
    }
    
    public function LogWarning(message: String, source: String = "")
    {
        LogWarning(GENERAL_CATEGORY, message, source);
    }
    
    public function LogError(message: String, source: String = "")
    {
        LogError(GENERAL_CATEGORY, message, source);
    }
    
    public function LogCritical(message: String, source: String = "")
    {
        LogCritical(GENERAL_CATEGORY, message, source);
    }
    
    // Specialized logging methods
    public function LogPlayerConnection(playerId: String, action: String)
    {
        var message: String = "Player " + playerId + " " + action;
        LogInfo(PLAYER_CATEGORY, message, "PlayerConnection");
    }
    
    public function LogQuestEvent(questId: String, event: String, playerId: String = "")
    {
        var message: String = "Quest [" + questId + "] " + event;
        if (playerId != "")
        {
            message += " (Player: " + playerId + ")";
        }
        LogInfo(QUEST_CATEGORY, message, "QuestSystem");
    }
    
    public function LogInventorySync(playerId: String, itemId: String, action: String)
    {
        var message: String = "Inventory sync - Player: " + playerId + ", Item: " + itemId + ", Action: " + action;
        LogInfo(INVENTORY_CATEGORY, message, "InventorySystem");
    }
    
    public function LogDialogEvent(dialogId: String, event: String, playerId: String = "")
    {
        var message: String = "Dialog [" + dialogId + "] " + event;
        if (playerId != "")
        {
            message += " (Player: " + playerId + ")";
        }
        LogInfo(DIALOG_CATEGORY, message, "DialogSystem");
    }
    
    public function LogNetworkEvent(event: String, details: String = "")
    {
        var message: String = "Network: " + event;
        if (details != "")
        {
            message += " - " + details;
        }
        LogInfo(NETWORK_CATEGORY, message, "NetworkSystem");
    }
    
    public function LogCombatEvent(event: String, playerId: String = "", details: String = "")
    {
        var message: String = "Combat: " + event;
        if (playerId != "")
        {
            message += " (Player: " + playerId + ")";
        }
        if (details != "")
        {
            message += " - " + details;
        }
        LogInfo(COMBAT_CATEGORY, message, "CombatSystem");
    }
    
    // Configuration methods
    public function FlushLogs()
    {
        FlushLogsNative();
    }
    
    public function SetLogLevel(level: String)
    {
        var levelInt: int = INFO_LEVEL;
        
        if (level == "debug")
            levelInt = DEBUG_LEVEL;
        else if (level == "info")
            levelInt = INFO_LEVEL;
        else if (level == "warning")
            levelInt = WARNING_LEVEL;
        else if (level == "error")
            levelInt = ERROR_LEVEL;
        else if (level == "critical")
            levelInt = CRITICAL_LEVEL;
        else
        {
            LogWarning("Unknown log level: " + level);
            return;
        }
        
        m_logLevel = levelInt;
        SetLogLevelNative(levelInt);
    }
    
    public function EnableCategory(category: String, enable: bool)
    {
        var categoryInt: int = GENERAL_CATEGORY;
        
        if (category == "general")
            categoryInt = GENERAL_CATEGORY;
        else if (category == "network")
            categoryInt = NETWORK_CATEGORY;
        else if (category == "combat")
            categoryInt = COMBAT_CATEGORY;
        else if (category == "quest")
            categoryInt = QUEST_CATEGORY;
        else if (category == "inventory")
            categoryInt = INVENTORY_CATEGORY;
        else if (category == "dialog")
            categoryInt = DIALOG_CATEGORY;
        else if (category == "player")
            categoryInt = PLAYER_CATEGORY;
        else if (category == "system")
            categoryInt = SYSTEM_CATEGORY;
        else
        {
            LogWarning("Unknown category: " + category);
            return;
        }
        
        EnableCategoryNative(categoryInt, enable);
    }
    
    public function DisableCategory(category: String)
    {
        EnableCategory(category, false);
    }
    
    public function TogglePerformanceLogging()
    {
        m_performanceLogging = !m_performanceLogging;
        
        if (m_performanceLogging)
        {
            StartPerformanceLoggingNative();
        }
        else
        {
            StopPerformanceLoggingNative();
        }
    }
    
    public function LogPerformance(operation: String, duration: float)
    {
        if (m_performanceLogging)
        {
            LogPerformanceNative(operation, duration);
        }
    }
    
    public function ShowStatus()
    {
        LogInfo("=== Logging Status ===");
        LogInfo("File logging: " + (m_fileLogging ? "Enabled" : "Disabled"));
        LogInfo("Console logging: " + (m_consoleLogging ? "Enabled" : "Disabled"));
        LogInfo("Performance logging: " + (m_performanceLogging ? "Enabled" : "Disabled"));
        LogInfo("Log level: " + GetLogLevelString());
    }
    
    private function GetLogLevelString(): String
    {
        switch (m_logLevel)
        {
            case DEBUG_LEVEL: return "DEBUG";
            case INFO_LEVEL: return "INFO";
            case WARNING_LEVEL: return "WARNING";
            case ERROR_LEVEL: return "ERROR";
            case CRITICAL_LEVEL: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }
    
    // Debug toggles
    public function ToggleInventoryDebug()
    {
        // Toggle inventory debug logging
        LogInfo("Inventory debug logging toggled");
    }
    
    public function ToggleNetworkDebug()
    {
        // Toggle network debug logging
        LogInfo("Network debug logging toggled");
    }
    
    // Public interface
    public function Initialize()
    {
        Initialize();
    }
}
