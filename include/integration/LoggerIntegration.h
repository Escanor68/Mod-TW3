#pragma once

#include <string>
#include <vector>

// Logging statistics structure
struct LoggingStats
{
    int totalLogs;
    int errorsLogged;
    int warningsLogged;
    int performanceLogs;
};

class LoggerIntegration
{
public:
    // Initialize the logging system
    static void Initialize();
    
    // Console command processing
    static void ProcessConsoleCommand(const std::string& command, const std::vector<std::string>& args);
    
    // Performance monitoring
    static void StartPerformanceMonitoring();
    static void StopPerformanceMonitoring();
    
    // Log management
    static void RotateLogs();
    static LoggingStats GetLoggingStats();

private:
    // Initialize mod console commands
    static void InitializeModConsoleCommands();
};

// C-style functions for WitcherScript integration
extern "C" {
    // Initialize native logger
    void InitializeNativeLogger();
    
    // Log message from WitcherScript
    void LogMessage(int level, int category, const char* message, const char* source);
    
    // Flush logs
    void FlushLogsNative();
    
    // Set log level
    void SetLogLevelNative(int level);
    
    // Enable/disable category
    void EnableCategoryNative(int category, bool enable);
    
    // Performance logging
    void StartPerformanceLoggingNative();
    void StopPerformanceLoggingNative();
    void LogPerformanceNative(const char* operation, float duration);
}
