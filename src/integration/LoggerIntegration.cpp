#include "integration/LoggerIntegration.h"
#include "utils/Logger.h"
#include "utils/ConsoleCommands.h"
#include <iostream>

// Initialize the logging system
void LoggerIntegration::Initialize()
{
    auto& logger = Logger::GetInstance();
    
    // Configure logging from mod config
    logger.SetFileLogging(true, "logs/mp_session.log");
    logger.SetConsoleLogging(true);
    logger.SetBufferedLogging(true, 1000);
    logger.SetLogDirectory("logs");
    
    // Initialize console commands
    InitializeModConsoleCommands();
    
    LOG_INFO_CAT(LogCategory::SYSTEM, "Logger integration initialized");
}

// WitcherScript integration functions
extern "C" {
    
    // Initialize native logger
    void InitializeNativeLogger()
    {
        LoggerIntegration::Initialize();
    }
    
    // Log message from WitcherScript
    void LogMessage(int level, int category, const char* message, const char* source)
    {
        LogLevel logLevel = static_cast<LogLevel>(level);
        LogCategory logCategory = static_cast<LogCategory>(category);
        
        Logger::GetInstance().Log(logLevel, logCategory, std::string(message), std::string(source));
    }
    
    // Flush logs
    void FlushLogsNative()
    {
        Logger::GetInstance().FlushLogs();
    }
    
    // Set log level
    void SetLogLevelNative(int level)
    {
        Logger::GetInstance().SetLogLevel(static_cast<LogLevel>(level));
    }
    
    // Enable/disable category
    void EnableCategoryNative(int category, bool enable)
    {
        Logger::GetInstance().EnableCategory(static_cast<LogCategory>(category), enable);
    }
    
    // Performance logging
    void StartPerformanceLoggingNative()
    {
        Logger::GetInstance().StartPerformanceLogging();
    }
    
    void StopPerformanceLoggingNative()
    {
        Logger::GetInstance().StopPerformanceLogging();
    }
    
    void LogPerformanceNative(const char* operation, float duration)
    {
        Logger::GetInstance().LogPerformance(std::string(operation), duration);
    }
}

// Console command processing
void LoggerIntegration::ProcessConsoleCommand(const std::string& command, const std::vector<std::string>& args)
{
    ConsoleCommandManager::GetInstance().ExecuteCommand(command, args);
}

// Initialize mod console commands
void LoggerIntegration::InitializeModConsoleCommands()
{
    // This will be implemented in ModConsoleCommands.cpp
    // For now, we'll just log that it's being called
    LOG_INFO_CAT(LogCategory::SYSTEM, "Initializing mod console commands");
}

// Performance monitoring
void LoggerIntegration::StartPerformanceMonitoring()
{
    Logger::GetInstance().StartPerformanceLogging();
    LOG_INFO_CAT(LogCategory::SYSTEM, "Performance monitoring started");
}

void LoggerIntegration::StopPerformanceMonitoring()
{
    Logger::GetInstance().StopPerformanceLogging();
    LOG_INFO_CAT(LogCategory::SYSTEM, "Performance monitoring stopped");
}

// Log rotation
void LoggerIntegration::RotateLogs()
{
    // This would implement log rotation logic
    LOG_INFO_CAT(LogCategory::SYSTEM, "Log rotation requested");
}

// Get logging statistics
LoggingStats LoggerIntegration::GetLoggingStats()
{
    LoggingStats stats;
    // This would collect actual statistics from the logger
    stats.totalLogs = 0;
    stats.errorsLogged = 0;
    stats.warningsLogged = 0;
    stats.performanceLogs = 0;
    
    return stats;
}
