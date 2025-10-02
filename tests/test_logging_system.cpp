#include <catch2/catch_test_macros.hpp>
#include "utils/Logger.h"
#include "utils/ConsoleCommands.h"
#include "integration/LoggerIntegration.h"
#include <iostream>
#include <fstream>
#include <filesystem>

TEST_CASE("Logger Basic Functionality", "[logger]")
{
    // Clean up any existing log files
    std::filesystem::remove("logs/mp_session.log");
    
    auto& logger = Logger::GetInstance();
    
    // Test basic logging
    logger.SetFileLogging(true, "logs/mp_session.log");
    logger.SetConsoleLogging(false); // Disable console for testing
    logger.SetLogLevel(LogLevel::DEBUG);
    
    logger.Debug(LogCategory::GENERAL, "Test debug message", "TestFunction");
    logger.Info(LogCategory::GENERAL, "Test info message", "TestFunction");
    logger.Warning(LogCategory::GENERAL, "Test warning message", "TestFunction");
    logger.Error(LogCategory::GENERAL, "Test error message", "TestFunction");
    logger.Critical(LogCategory::GENERAL, "Test critical message", "TestFunction");
    
    // Flush logs to ensure they're written
    logger.FlushLogs();
    
    // Verify log file was created and contains messages
    REQUIRE(std::filesystem::exists("logs/mp_session.log"));
    
    std::ifstream logFile("logs/mp_session.log");
    std::string content((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
    
    REQUIRE(content.find("Test debug message") != std::string::npos);
    REQUIRE(content.find("Test info message") != std::string::npos);
    REQUIRE(content.find("Test warning message") != std::string::npos);
    REQUIRE(content.find("Test error message") != std::string::npos);
    REQUIRE(content.find("Test critical message") != std::string::npos);
}

TEST_CASE("Logger Categories", "[logger]")
{
    auto& logger = Logger::GetInstance();
    
    // Test different categories
    logger.LogPlayerConnection("player123", "connected");
    logger.LogQuestEvent("quest_001", "started", "player123");
    logger.LogInventorySync("player123", "sword_001", "added");
    logger.LogDialogEvent("dialog_001", "started", "player123");
    logger.LogNetworkEvent("connection_established", "latency: 25ms");
    logger.LogCombatEvent("attack_performed", "player123", "damage: 50");
    
    logger.FlushLogs();
    
    // Verify category-specific messages were logged
    std::ifstream logFile("logs/mp_session.log");
    std::string content((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
    
    REQUIRE(content.find("[PLAYER]") != std::string::npos);
    REQUIRE(content.find("[QUEST]") != std::string::npos);
    REQUIRE(content.find("[INVENTORY]") != std::string::npos);
    REQUIRE(content.find("[DIALOG]") != std::string::npos);
    REQUIRE(content.find("[NETWORK]") != std::string::npos);
    REQUIRE(content.find("[COMBAT]") != std::string::npos);
}

TEST_CASE("Logger Buffering", "[logger]")
{
    auto& logger = Logger::GetInstance();
    
    // Enable buffered logging
    logger.SetBufferedLogging(true, 5); // Small buffer for testing
    
    // Log multiple messages
    for (int i = 0; i < 10; ++i)
    {
        logger.Info(LogCategory::GENERAL, "Buffered message " + std::to_string(i), "TestFunction");
    }
    
    // Flush buffer
    logger.FlushLogs();
    
    // Verify all messages were written
    std::ifstream logFile("logs/mp_session.log");
    std::string content((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
    
    for (int i = 0; i < 10; ++i)
    {
        REQUIRE(content.find("Buffered message " + std::to_string(i)) != std::string::npos);
    }
}

TEST_CASE("Logger Performance Monitoring", "[logger]")
{
    auto& logger = Logger::GetInstance();
    
    // Start performance logging
    logger.StartPerformanceLogging();
    
    // Log some performance data
    logger.LogPerformance("test_operation_1", 10.5);
    logger.LogPerformance("test_operation_2", 25.3);
    logger.LogPerformance("test_operation_3", 5.1);
    
    // Stop performance logging
    logger.StopPerformanceLogging();
    
    logger.FlushLogs();
    
    // Verify performance logs were written
    std::ifstream logFile("logs/mp_session.log");
    std::string content((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
    
    REQUIRE(content.find("test_operation_1") != std::string::npos);
    REQUIRE(content.find("test_operation_2") != std::string::npos);
    REQUIRE(content.find("test_operation_3") != std::string::npos);
}

TEST_CASE("Console Commands", "[console]")
{
    auto& cmdManager = ConsoleCommandManager::GetInstance();
    
    // Test command registration
    bool commandExecuted = false;
    cmdManager.RegisterCommand("test_cmd", "Test command", 
        [&commandExecuted](const std::vector<std::string>& args) {
            commandExecuted = true;
        });
    
    // Test command execution
    REQUIRE(cmdManager.ExecuteCommand("test_cmd"));
    REQUIRE(commandExecuted);
    
    // Test unknown command
    REQUIRE_FALSE(cmdManager.ExecuteCommand("unknown_cmd"));
    
    // Test command parsing
    auto tokens = cmdManager.ParseCommand("test_cmd arg1 arg2 \"quoted arg\"");
    REQUIRE(tokens.size() == 4);
    REQUIRE(tokens[0] == "test_cmd");
    REQUIRE(tokens[1] == "arg1");
    REQUIRE(tokens[2] == "arg2");
    REQUIRE(tokens[3] == "quoted arg");
}

TEST_CASE("Logger Integration", "[integration]")
{
    // Initialize the integration system
    LoggerIntegration::Initialize();
    
    // Test C-style functions
    InitializeNativeLogger();
    LogMessage(1, 0, "Test message from C", "TestFunction");
    FlushLogsNative();
    
    // Verify message was logged
    std::ifstream logFile("logs/mp_session.log");
    std::string content((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
    
    REQUIRE(content.find("Test message from C") != std::string::npos);
}

TEST_CASE("Logger Level Filtering", "[logger]")
{
    auto& logger = Logger::GetInstance();
    
    // Set log level to WARNING
    logger.SetLogLevel(LogLevel::WARNING);
    
    // Log messages at different levels
    logger.Debug(LogCategory::GENERAL, "This should not appear", "TestFunction");
    logger.Info(LogCategory::GENERAL, "This should not appear", "TestFunction");
    logger.Warning(LogCategory::GENERAL, "This should appear", "TestFunction");
    logger.Error(LogCategory::GENERAL, "This should appear", "TestFunction");
    logger.Critical(LogCategory::GENERAL, "This should appear", "TestFunction");
    
    logger.FlushLogs();
    
    // Verify only WARNING and above messages were logged
    std::ifstream logFile("logs/mp_session.log");
    std::string content((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
    
    REQUIRE(content.find("This should not appear") == std::string::npos);
    REQUIRE(content.find("This should appear") != std::string::npos);
}

TEST_CASE("Logger Category Filtering", "[logger]")
{
    auto& logger = Logger::GetInstance();
    
    // Disable COMBAT category
    logger.DisableCategory(LogCategory::COMBAT);
    
    // Log messages in different categories
    logger.Info(LogCategory::GENERAL, "General message", "TestFunction");
    logger.Info(LogCategory::COMBAT, "Combat message", "TestFunction");
    logger.Info(LogCategory::NETWORK, "Network message", "TestFunction");
    
    logger.FlushLogs();
    
    // Verify COMBAT messages were filtered out
    std::ifstream logFile("logs/mp_session.log");
    std::string content((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
    
    REQUIRE(content.find("General message") != std::string::npos);
    REQUIRE(content.find("Combat message") == std::string::npos);
    REQUIRE(content.find("Network message") != std::string::npos);
}

TEST_CASE("Logger Timestamp Format", "[logger]")
{
    auto& logger = Logger::GetInstance();
    
    logger.Info(LogCategory::GENERAL, "Timestamp test message", "TestFunction");
    logger.FlushLogs();
    
    // Verify timestamp format
    std::ifstream logFile("logs/mp_session.log");
    std::string content((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
    
    // Look for timestamp pattern: YYYY-MM-DD HH:MM:SS.mmm
    std::regex timestampRegex(R"(\[\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}\.\d{3}\])");
    REQUIRE(std::regex_search(content, timestampRegex));
}
