#include "utils/Logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <algorithm>

// Static member initialization
Logger* Logger::s_instance = nullptr;

Logger::Logger()
    : m_enabledCategories(8, true) // Initialize all categories as enabled
{
    // Start the worker thread
    m_workerThread = std::thread(&Logger::LogWorkerThread, this);
    m_workerRunning = true;
    
    // Create logs directory if it doesn't exist
    std::filesystem::create_directories(m_logDirectory);
    
    LOG_INFO_CAT(LogCategory::SYSTEM, "Logger initialized with buffered logging enabled");
}

Logger::~Logger()
{
    m_running = false;
    m_condition.notify_all();
    
    if (m_workerThread.joinable())
    {
        m_workerThread.join();
    }
    
    // Flush any remaining logs
    FlushBuffer();
    
    if (m_logFileStream.is_open())
    {
        m_logFileStream.close();
    }
}

Logger& Logger::GetInstance()
{
    if (s_instance == nullptr)
    {
        s_instance = new Logger();
    }
    return *s_instance;
}

void Logger::DestroyInstance()
{
    if (s_instance != nullptr)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

void Logger::SetFileLogging(bool enable, const std::string& filename)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_fileLogging = enable;
    m_logFile = filename;
    
    if (m_fileLogging)
    {
        if (m_logFileStream.is_open())
        {
            m_logFileStream.close();
        }
        
        // Create directory if it doesn't exist
        std::filesystem::path filePath(filename);
        std::filesystem::create_directories(filePath.parent_path());
        
        m_logFileStream.open(filename, std::ios::app);
        if (m_logFileStream.is_open())
        {
            LOG_INFO_CAT(LogCategory::SYSTEM, "File logging enabled: " + filename);
        }
        else
        {
            LOG_ERROR_CAT(LogCategory::SYSTEM, "Failed to open log file: " + filename);
        }
    }
    else
    {
        if (m_logFileStream.is_open())
        {
            m_logFileStream.close();
        }
        LOG_INFO_CAT(LogCategory::SYSTEM, "File logging disabled");
    }
}

void Logger::SetConsoleLogging(bool enable)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_consoleLogging = enable;
    LOG_INFO_CAT(LogCategory::SYSTEM, enable ? "Console logging enabled" : "Console logging disabled");
}

void Logger::SetLogLevel(LogLevel level)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_level = level;
    LOG_INFO_CAT(LogCategory::SYSTEM, "Log level set to: " + GetLevelString(level));
}

void Logger::SetBufferedLogging(bool enable, size_t bufferSize)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_bufferedLogging = enable;
    m_bufferSize = bufferSize;
    
    if (!enable)
    {
        FlushBuffer();
    }
    
    LOG_INFO_CAT(LogCategory::SYSTEM, 
        "Buffered logging " + std::string(enable ? "enabled" : "disabled") + 
        " with buffer size: " + std::to_string(bufferSize));
}

void Logger::SetLogDirectory(const std::string& directory)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_logDirectory = directory;
    std::filesystem::create_directories(directory);
    LOG_INFO_CAT(LogCategory::SYSTEM, "Log directory set to: " + directory);
}

void Logger::Log(LogLevel level, LogCategory category, const std::string& message, const std::string& source)
{
    if (level < m_level || !m_enabledCategories[static_cast<int>(category)])
        return;

    std::string timestamp = GetCurrentTimeString();
    LogEntry entry(timestamp, level, category, message, source);
    
    if (m_bufferedLogging)
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        m_logQueue.push(entry);
        m_condition.notify_one();
    }
    else
    {
        WriteLog(entry);
    }
}

void Logger::Debug(LogCategory category, const std::string& message, const std::string& source)
{
    Log(LogLevel::DEBUG, category, message, source);
}

void Logger::Info(LogCategory category, const std::string& message, const std::string& source)
{
    Log(LogLevel::INFO, category, message, source);
}

void Logger::Warning(LogCategory category, const std::string& message, const std::string& source)
{
    Log(LogLevel::WARNING, category, message, source);
}

void Logger::Error(LogCategory category, const std::string& message, const std::string& source)
{
    Log(LogLevel::ERROR_LEVEL, category, message, source);
}

void Logger::Critical(LogCategory category, const std::string& message, const std::string& source)
{
    Log(LogLevel::CRITICAL, category, message, source);
}

void Logger::LogPlayerConnection(const std::string& playerId, const std::string& action)
{
    std::string message = "Player " + playerId + " " + action;
    Info(LogCategory::PLAYER, message, "PlayerConnection");
}

void Logger::LogQuestEvent(const std::string& questId, const std::string& event, const std::string& playerId)
{
    std::string message = "Quest [" + questId + "] " + event;
    if (!playerId.empty())
    {
        message += " (Player: " + playerId + ")";
    }
    Info(LogCategory::QUEST, message, "QuestSystem");
}

void Logger::LogInventorySync(const std::string& playerId, const std::string& itemId, const std::string& action)
{
    std::string message = "Inventory sync - Player: " + playerId + ", Item: " + itemId + ", Action: " + action;
    Info(LogCategory::INVENTORY, message, "InventorySystem");
}

void Logger::LogDialogEvent(const std::string& dialogId, const std::string& event, const std::string& playerId)
{
    std::string message = "Dialog [" + dialogId + "] " + event;
    if (!playerId.empty())
    {
        message += " (Player: " + playerId + ")";
    }
    Info(LogCategory::DIALOG, message, "DialogSystem");
}

void Logger::LogNetworkEvent(const std::string& event, const std::string& details)
{
    std::string message = "Network: " + event;
    if (!details.empty())
    {
        message += " - " + details;
    }
    Info(LogCategory::NETWORK, message, "NetworkSystem");
}

void Logger::LogCombatEvent(const std::string& event, const std::string& playerId, const std::string& details)
{
    std::string message = "Combat: " + event;
    if (!playerId.empty())
    {
        message += " (Player: " + playerId + ")";
    }
    if (!details.empty())
    {
        message += " - " + details;
    }
    Info(LogCategory::COMBAT, message, "CombatSystem");
}

void Logger::FlushLogs()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    FlushBuffer();
    LOG_INFO_CAT(LogCategory::SYSTEM, "Logs flushed to file");
}

void Logger::SetLogLevelFromString(const std::string& level)
{
    std::string lowerLevel = level;
    std::transform(lowerLevel.begin(), lowerLevel.end(), lowerLevel.begin(), ::tolower);
    
    if (lowerLevel == "debug")
        SetLogLevel(LogLevel::DEBUG);
    else if (lowerLevel == "info")
        SetLogLevel(LogLevel::INFO);
    else if (lowerLevel == "warning")
        SetLogLevel(LogLevel::WARNING);
    else if (lowerLevel == "error")
        SetLogLevel(LogLevel::ERROR_LEVEL);
    else if (lowerLevel == "critical")
        SetLogLevel(LogLevel::CRITICAL);
    else
        LOG_WARNING_CAT(LogCategory::SYSTEM, "Unknown log level: " + level);
}

void Logger::EnableCategory(LogCategory category, bool enable)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_enabledCategories[static_cast<int>(category)] = enable;
    LOG_INFO_CAT(LogCategory::SYSTEM, 
        "Category " + GetCategoryString(category) + " " + (enable ? "enabled" : "disabled"));
}

void Logger::DisableCategory(LogCategory category)
{
    EnableCategory(category, false);
}

void Logger::StartPerformanceLogging()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_performanceLogging = true;
    m_performanceStart = std::chrono::steady_clock::now();
    m_performanceData.clear();
    LOG_INFO_CAT(LogCategory::SYSTEM, "Performance logging started");
}

void Logger::StopPerformanceLogging()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_performanceLogging = false;
    
    // Log performance summary
    if (!m_performanceData.empty())
    {
        LOG_INFO_CAT(LogCategory::SYSTEM, "Performance logging stopped. Summary:");
        for (const auto& perf : m_performanceData)
        {
            LOG_INFO_CAT(LogCategory::SYSTEM, 
                "  " + perf.first + ": " + std::to_string(perf.second) + "ms");
        }
    }
}

void Logger::LogPerformance(const std::string& operation, double duration)
{
    if (m_performanceLogging)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_performanceData.push_back({operation, duration});
        LOG_DEBUG_CAT(LogCategory::SYSTEM, "Performance: " + operation + " took " + std::to_string(duration) + "ms");
    }
}

std::string Logger::GetLevelString(LogLevel level)
{
    switch (level)
    {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR_LEVEL: return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

std::string Logger::GetCategoryString(LogCategory category)
{
    switch (category)
    {
        case LogCategory::GENERAL: return "GENERAL";
        case LogCategory::NETWORK: return "NETWORK";
        case LogCategory::COMBAT: return "COMBAT";
        case LogCategory::QUEST: return "QUEST";
        case LogCategory::INVENTORY: return "INVENTORY";
        case LogCategory::DIALOG: return "DIALOG";
        case LogCategory::PLAYER: return "PLAYER";
        case LogCategory::SYSTEM: return "SYSTEM";
        default: return "UNKNOWN";
    }
}

std::string Logger::GetCurrentTimeString()
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    struct tm timeinfo;
    localtime_s(&timeinfo, &time_t);
    ss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

void Logger::WriteLog(const LogEntry& entry)
{
    std::string formattedEntry = FormatLogEntry(entry);
    
    if (m_consoleLogging)
    {
        std::cout << formattedEntry << std::endl;
    }
    
    if (m_fileLogging && m_logFileStream.is_open())
    {
        m_logFileStream << formattedEntry << std::endl;
        m_logFileStream.flush();
    }
}

void Logger::ProcessLogQueue()
{
    while (m_running)
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_condition.wait(lock, [this] { return !m_logQueue.empty() || !m_running; });
        
        if (!m_running) break;
        
        // Process all available logs
        while (!m_logQueue.empty())
        {
            LogEntry entry = m_logQueue.front();
            m_logQueue.pop();
            
            // Add to buffer
            m_logBuffer.push_back(entry);
            
            // Flush buffer if it's full
            if (m_logBuffer.size() >= m_bufferSize)
            {
                FlushBuffer();
            }
        }
        
        lock.unlock();
    }
}

void Logger::FlushBuffer()
{
    for (const auto& entry : m_logBuffer)
    {
        WriteLog(entry);
    }
    m_logBuffer.clear();
}

void Logger::RotateLogFile()
{
    if (m_logFileStream.is_open())
    {
        m_logFileStream.close();
    }
    
    // Create rotated filename with timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    struct tm timeinfo;
    localtime_s(&timeinfo, &time_t);
    ss << std::put_time(&timeinfo, "%Y%m%d_%H%M%S");
    
    std::filesystem::path originalPath(m_logFile);
    std::string rotatedFile = originalPath.stem().string() + "_" + ss.str() + originalPath.extension().string();
    std::filesystem::path rotatedPath = originalPath.parent_path() / rotatedFile;
    
    // Rename current file
    std::filesystem::rename(m_logFile, rotatedPath);
    
    // Open new file
    m_logFileStream.open(m_logFile, std::ios::app);
    LOG_INFO_CAT(LogCategory::SYSTEM, "Log file rotated: " + rotatedFile);
}

std::string Logger::FormatLogEntry(const LogEntry& entry)
{
    std::stringstream ss;
    ss << "[" << entry.timestamp << "] "
       << "[" << GetLevelString(entry.level) << "] "
       << "[" << GetCategoryString(entry.category) << "] ";
    
    if (!entry.source.empty())
    {
        ss << "[" << entry.source << "] ";
    }
    
    ss << entry.message;
    return ss.str();
}

void Logger::LogWorkerThread()
{
    m_workerRunning = true;
    ProcessLogQueue();
    m_workerRunning = false;
}

void Logger::NotifyWorker()
{
    m_condition.notify_one();
}