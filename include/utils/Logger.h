#pragma once

#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <memory>

// Log levels
enum class LogLevel : int
{
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4
};

// Log categories for different systems
enum class LogCategory : int
{
    GENERAL = 0,
    NETWORK = 1,
    COMBAT = 2,
    QUEST = 3,
    INVENTORY = 4,
    DIALOG = 5,
    PLAYER = 6,
    SYSTEM = 7
};

// Log entry structure
struct LogEntry
{
    std::string timestamp;
    LogLevel level;
    LogCategory category;
    std::string message;
    std::string source;
    
    LogEntry(const std::string& ts, LogLevel lvl, LogCategory cat, 
             const std::string& msg, const std::string& src = "")
        : timestamp(ts), level(lvl), category(cat), message(msg), source(src) {}
};

class Logger
{
public:
    // Singleton pattern
    static Logger& GetInstance();
    static void DestroyInstance();
    
    // Configuration
    void SetFileLogging(bool enable, const std::string& filename = "logs/mp_session.log");
    void SetConsoleLogging(bool enable);
    void SetLogLevel(LogLevel level);
    void SetBufferedLogging(bool enable, size_t bufferSize = 1000);
    void SetLogDirectory(const std::string& directory);
    
    // Logging methods
    void Log(LogLevel level, LogCategory category, const std::string& message, const std::string& source = "");
    void Debug(LogCategory category, const std::string& message, const std::string& source = "");
    void Info(LogCategory category, const std::string& message, const std::string& source = "");
    void Warning(LogCategory category, const std::string& message, const std::string& source = "");
    void Error(LogCategory category, const std::string& message, const std::string& source = "");
    void Critical(LogCategory category, const std::string& message, const std::string& source = "");
    
    // Specialized logging for mod events
    void LogPlayerConnection(const std::string& playerId, const std::string& action);
    void LogQuestEvent(const std::string& questId, const std::string& event, const std::string& playerId = "");
    void LogInventorySync(const std::string& playerId, const std::string& itemId, const std::string& action);
    void LogDialogEvent(const std::string& dialogId, const std::string& event, const std::string& playerId = "");
    void LogNetworkEvent(const std::string& event, const std::string& details = "");
    void LogCombatEvent(const std::string& event, const std::string& playerId = "", const std::string& details = "");
    
    // Console commands
    void FlushLogs();
    void SetLogLevelFromString(const std::string& level);
    void EnableCategory(LogCategory category, bool enable);
    void DisableCategory(LogCategory category);
    
    // Performance monitoring
    void StartPerformanceLogging();
    void StopPerformanceLogging();
    void LogPerformance(const std::string& operation, double duration);

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string GetLevelString(LogLevel level);
    std::string GetCategoryString(LogCategory category);
    std::string GetCurrentTimeString();
    void WriteLog(const LogEntry& entry);
    void ProcessLogQueue();
    void FlushBuffer();
    void RotateLogFile();
    std::string FormatLogEntry(const LogEntry& entry);
    
    // Threading
    void LogWorkerThread();
    void NotifyWorker();
    
    // Configuration
    LogLevel m_level = LogLevel::INFO;
    bool m_fileLogging = false;
    bool m_consoleLogging = true;
    bool m_bufferedLogging = true;
    bool m_performanceLogging = false;
    size_t m_bufferSize = 1000;
    std::string m_logDirectory = "logs";
    std::string m_logFile = "logs/mp_session.log";
    
    // Threading and buffering
    std::ofstream m_logFileStream;
    std::mutex m_mutex;
    std::mutex m_queueMutex;
    std::condition_variable m_condition;
    std::queue<LogEntry> m_logQueue;
    std::vector<LogEntry> m_logBuffer;
    std::thread m_workerThread;
    std::atomic<bool> m_running{true};
    std::atomic<bool> m_workerRunning{false};
    
    // Category filtering
    std::vector<bool> m_enabledCategories;
    
    // Performance tracking
    std::chrono::high_resolution_clock::time_point m_performanceStart;
    std::vector<std::pair<std::string, double>> m_performanceData;
    
    static Logger* s_instance;
};

// Convenience macros with categories
#define LOG_DEBUG_CAT(cat, msg) Logger::GetInstance().Debug(cat, msg, __FUNCTION__)
#define LOG_INFO_CAT(cat, msg) Logger::GetInstance().Info(cat, msg, __FUNCTION__)
#define LOG_WARNING_CAT(cat, msg) Logger::GetInstance().Warning(cat, msg, __FUNCTION__)
#define LOG_ERROR_CAT(cat, msg) Logger::GetInstance().Error(cat, msg, __FUNCTION__)
#define LOG_CRITICAL_CAT(cat, msg) Logger::GetInstance().Critical(cat, msg, __FUNCTION__)

// Legacy macros for backward compatibility
#define LOG_DEBUG(msg) Logger::GetInstance().Debug(LogCategory::GENERAL, msg, __FUNCTION__)
#define LOG_INFO(msg) Logger::GetInstance().Info(LogCategory::GENERAL, msg, __FUNCTION__)
#define LOG_WARNING(msg) Logger::GetInstance().Warning(LogCategory::GENERAL, msg, __FUNCTION__)
#define LOG_ERROR(msg) Logger::GetInstance().Error(LogCategory::GENERAL, msg, __FUNCTION__)
#define LOG_CRITICAL(msg) Logger::GetInstance().Critical(LogCategory::GENERAL, msg, __FUNCTION__)

// Specialized logging macros
#define LOG_PLAYER_CONNECTION(playerId, action) Logger::GetInstance().LogPlayerConnection(playerId, action)
#define LOG_QUEST_EVENT(questId, event, playerId) Logger::GetInstance().LogQuestEvent(questId, event, playerId)
#define LOG_INVENTORY_SYNC(playerId, itemId, action) Logger::GetInstance().LogInventorySync(playerId, itemId, action)
#define LOG_DIALOG_EVENT(dialogId, event, playerId) Logger::GetInstance().LogDialogEvent(dialogId, event, playerId)
#define LOG_NETWORK_EVENT(event, details) Logger::GetInstance().LogNetworkEvent(event, details)
#define LOG_COMBAT_EVENT(event, playerId, details) Logger::GetInstance().LogCombatEvent(event, playerId, details)