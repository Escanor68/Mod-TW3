#pragma once

#include <iostream>
#include <fstream>
#include <mutex>
#include <string>

class Logger
{
public:
    // Singleton pattern
    static Logger& GetInstance();
    static void DestroyInstance();
    
    // Configuration
    void SetFileLogging(bool enable, const std::string& filename = "witcher3mp.log");
    void SetConsoleLogging(bool enable);
    void SetLogLevel(int level);
    
    // Logging methods
    void Log(int level, const std::string& message);
    void Debug(const std::string& message);
    void Info(const std::string& message);
    void Warning(const std::string& message);
    void Error(const std::string& message);
    void Critical(const std::string& message);

private:
    Logger() = default;
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string GetLevelString(int level);
    std::string GetCurrentTimeString();
    void WriteLog(int level, const std::string& message);

    int m_level = 1; // INFO level
    bool m_fileLogging = false;
    bool m_consoleLogging = true;
    std::ofstream m_logFile;
    std::mutex m_mutex;
    static Logger* s_instance;
};

// Convenience macros
#define LOG_DEBUG(msg) Logger::GetInstance().Debug(msg)
#define LOG_INFO(msg) Logger::GetInstance().Info(msg)
#define LOG_WARNING(msg) Logger::GetInstance().Warning(msg)
#define LOG_ERROR(msg) Logger::GetInstance().Error(msg)
#define LOG_CRITICAL(msg) Logger::GetInstance().Critical(msg)