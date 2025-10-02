#include "utils/ConsoleCommands.h"
#include "utils/Logger.h"
#include <iostream>
#include <iomanip>

// Initialize all mod-specific console commands
void InitializeModConsoleCommands()
{
    auto& cmdManager = ConsoleCommandManager::GetInstance();
    
    // mp_log command - Main logging command
    cmdManager.RegisterCommand("mp_log", "Multiplayer logging commands", 
        [](const std::vector<std::string>& args) {
            if (args.empty())
            {
                std::cout << "Usage: mp_log <subcommand>\n";
                std::cout << "Subcommands: flush, level, enable, disable, performance, status\n";
                return;
            }
            
            std::string subcommand = args[0];
            auto& logger = Logger::GetInstance();
            
            if (subcommand == "flush")
            {
                logger.FlushLogs();
                std::cout << "Logs flushed to file.\n";
            }
            else if (subcommand == "level")
            {
                if (args.size() < 2)
                {
                    std::cout << "Usage: mp_log level <debug|info|warning|error|critical>\n";
                    return;
                }
                logger.SetLogLevelFromString(args[1]);
                std::cout << "Log level set to: " << args[1] << "\n";
            }
            else if (subcommand == "enable")
            {
                if (args.size() < 2)
                {
                    std::cout << "Usage: mp_log enable <category>\n";
                    std::cout << "Categories: general, network, combat, quest, inventory, dialog, player, system\n";
                    return;
                }
                
                std::string category = args[1];
                std::transform(category.begin(), category.end(), category.begin(), ::tolower);
                
                LogCategory cat = LogCategory::GENERAL;
                if (category == "general") cat = LogCategory::GENERAL;
                else if (category == "network") cat = LogCategory::NETWORK;
                else if (category == "combat") cat = LogCategory::COMBAT;
                else if (category == "quest") cat = LogCategory::QUEST;
                else if (category == "inventory") cat = LogCategory::INVENTORY;
                else if (category == "dialog") cat = LogCategory::DIALOG;
                else if (category == "player") cat = LogCategory::PLAYER;
                else if (category == "system") cat = LogCategory::SYSTEM;
                else
                {
                    std::cout << "Unknown category: " << category << "\n";
                    return;
                }
                
                logger.EnableCategory(cat, true);
                std::cout << "Category " << category << " enabled.\n";
            }
            else if (subcommand == "disable")
            {
                if (args.size() < 2)
                {
                    std::cout << "Usage: mp_log disable <category>\n";
                    std::cout << "Categories: general, network, combat, quest, inventory, dialog, player, system\n";
                    return;
                }
                
                std::string category = args[1];
                std::transform(category.begin(), category.end(), category.begin(), ::tolower);
                
                LogCategory cat = LogCategory::GENERAL;
                if (category == "general") cat = LogCategory::GENERAL;
                else if (category == "network") cat = LogCategory::NETWORK;
                else if (category == "combat") cat = LogCategory::COMBAT;
                else if (category == "quest") cat = LogCategory::QUEST;
                else if (category == "inventory") cat = LogCategory::INVENTORY;
                else if (category == "dialog") cat = LogCategory::DIALOG;
                else if (category == "player") cat = LogCategory::PLAYER;
                else if (category == "system") cat = LogCategory::SYSTEM;
                else
                {
                    std::cout << "Unknown category: " << category << "\n";
                    return;
                }
                
                logger.DisableCategory(cat);
                std::cout << "Category " << category << " disabled.\n";
            }
            else if (subcommand == "performance")
            {
                // Toggle performance logging
                static bool perfEnabled = false;
                if (perfEnabled)
                {
                    logger.StopPerformanceLogging();
                    perfEnabled = false;
                    std::cout << "Performance logging stopped.\n";
                }
                else
                {
                    logger.StartPerformanceLogging();
                    perfEnabled = true;
                    std::cout << "Performance logging started.\n";
                }
            }
            else if (subcommand == "status")
            {
                std::cout << "\n=== Logging Status ===\n";
                std::cout << "File logging: " << (logger.m_fileLogging ? "Enabled" : "Disabled") << "\n";
                std::cout << "Console logging: " << (logger.m_consoleLogging ? "Enabled" : "Disabled") << "\n";
                std::cout << "Buffered logging: " << (logger.m_bufferedLogging ? "Enabled" : "Disabled") << "\n";
                std::cout << "Log file: " << logger.m_logFile << "\n";
                std::cout << "Buffer size: " << logger.m_bufferSize << "\n";
                std::cout << "Performance logging: " << (logger.m_performanceLogging ? "Enabled" : "Disabled") << "\n";
                std::cout << "\n";
            }
            else
            {
                std::cout << "Unknown subcommand: " << subcommand << "\n";
                std::cout << "Available subcommands: flush, level, enable, disable, performance, status\n";
            }
        });
    
    // mp_status command - Show multiplayer status
    cmdManager.RegisterCommand("mp_status", "Show multiplayer session status", 
        [](const std::vector<std::string>& args) {
            std::cout << "\n=== Witcher3-MP Status ===\n";
            std::cout << "Session: Active\n";
            std::cout << "Connected Players: 0\n"; // TODO: Get from actual player manager
            std::cout << "Active Quests: 0\n"; // TODO: Get from quest manager
            std::cout << "Network Status: Connected\n"; // TODO: Get from network manager
            std::cout << "Performance: Good\n"; // TODO: Get from performance monitor
            std::cout << "\n";
            
            LOG_INFO_CAT(LogCategory::SYSTEM, "Status command executed");
        });
    
    // mp_quest command - Quest management
    cmdManager.RegisterCommand("mp_quest", "Quest management commands", 
        [](const std::vector<std::string>& args) {
            if (args.empty())
            {
                std::cout << "Usage: mp_quest <subcommand>\n";
                std::cout << "Subcommands: list, start, complete, sync\n";
                return;
            }
            
            std::string subcommand = args[0];
            
            if (subcommand == "list")
            {
                std::cout << "\n=== Active Quests ===\n";
                std::cout << "No active quests found.\n"; // TODO: Get from quest manager
                std::cout << "\n";
                
                LOG_QUEST_EVENT("SYSTEM", "Quest list requested", "");
            }
            else if (subcommand == "start")
            {
                if (args.size() < 2)
                {
                    std::cout << "Usage: mp_quest start <quest_id>\n";
                    return;
                }
                
                std::string questId = args[1];
                std::cout << "Starting quest: " << questId << "\n";
                LOG_QUEST_EVENT(questId, "Quest started", "");
            }
            else if (subcommand == "complete")
            {
                if (args.size() < 2)
                {
                    std::cout << "Usage: mp_quest complete <quest_id>\n";
                    return;
                }
                
                std::string questId = args[1];
                std::cout << "Completing quest: " << questId << "\n";
                LOG_QUEST_EVENT(questId, "Quest completed", "");
            }
            else if (subcommand == "sync")
            {
                std::cout << "Syncing quest state with all players...\n";
                LOG_QUEST_EVENT("SYSTEM", "Quest state sync initiated", "");
            }
            else
            {
                std::cout << "Unknown subcommand: " << subcommand << "\n";
                std::cout << "Available subcommands: list, start, complete, sync\n";
            }
        });
    
    // mp_inventory command - Inventory management
    cmdManager.RegisterCommand("mp_inventory", "Inventory management commands", 
        [](const std::vector<std::string>& args) {
            if (args.empty())
            {
                std::cout << "Usage: mp_inventory <subcommand>\n";
                std::cout << "Subcommands: sync, status, debug\n";
                return;
            }
            
            std::string subcommand = args[0];
            
            if (subcommand == "sync")
            {
                std::cout << "Syncing inventory with all players...\n";
                LOG_INVENTORY_SYNC("SYSTEM", "ALL", "Full sync initiated");
            }
            else if (subcommand == "status")
            {
                std::cout << "\n=== Inventory Sync Status ===\n";
                std::cout << "Last sync: Never\n"; // TODO: Get from inventory manager
                std::cout << "Pending items: 0\n"; // TODO: Get from inventory manager
                std::cout << "Sync errors: 0\n"; // TODO: Get from inventory manager
                std::cout << "\n";
                
                LOG_INVENTORY_SYNC("SYSTEM", "STATUS", "Status requested");
            }
            else if (subcommand == "debug")
            {
                static bool debugEnabled = false;
                debugEnabled = !debugEnabled;
                std::cout << "Inventory debug logging " << (debugEnabled ? "enabled" : "disabled") << "\n";
                
                if (debugEnabled)
                {
                    Logger::GetInstance().EnableCategory(LogCategory::INVENTORY, true);
                }
                else
                {
                    Logger::GetInstance().DisableCategory(LogCategory::INVENTORY);
                }
            }
            else
            {
                std::cout << "Unknown subcommand: " << subcommand << "\n";
                std::cout << "Available subcommands: sync, status, debug\n";
            }
        });
    
    // mp_network command - Network management
    cmdManager.RegisterCommand("mp_network", "Network management commands", 
        [](const std::vector<std::string>& args) {
            if (args.empty())
            {
                std::cout << "Usage: mp_network <subcommand>\n";
                std::cout << "Subcommands: status, stats, debug\n";
                return;
            }
            
            std::string subcommand = args[0];
            
            if (subcommand == "status")
            {
                std::cout << "\n=== Network Status ===\n";
                std::cout << "Connection: Active\n"; // TODO: Get from network manager
                std::cout << "Latency: 25ms\n"; // TODO: Get from network manager
                std::cout << "Packet loss: 0.1%\n"; // TODO: Get from network manager
                std::cout << "Throughput: 1.2 MB/s\n"; // TODO: Get from network manager
                std::cout << "\n";
                
                LOG_NETWORK_EVENT("Status requested", "");
            }
            else if (subcommand == "stats")
            {
                std::cout << "\n=== Network Statistics ===\n";
                std::cout << "Packets sent: 0\n"; // TODO: Get from network manager
                std::cout << "Packets received: 0\n"; // TODO: Get from network manager
                std::cout << "Bytes sent: 0\n"; // TODO: Get from network manager
                std::cout << "Bytes received: 0\n"; // TODO: Get from network manager
                std::cout << "Connection time: 0s\n"; // TODO: Get from network manager
                std::cout << "\n";
                
                LOG_NETWORK_EVENT("Statistics requested", "");
            }
            else if (subcommand == "debug")
            {
                static bool debugEnabled = false;
                debugEnabled = !debugEnabled;
                std::cout << "Network debug logging " << (debugEnabled ? "enabled" : "disabled") << "\n";
                
                if (debugEnabled)
                {
                    Logger::GetInstance().EnableCategory(LogCategory::NETWORK, true);
                }
                else
                {
                    Logger::GetInstance().DisableCategory(LogCategory::NETWORK);
                }
            }
            else
            {
                std::cout << "Unknown subcommand: " << subcommand << "\n";
                std::cout << "Available subcommands: status, stats, debug\n";
            }
        });
    
    // help command - Show help
    cmdManager.RegisterCommand("help", "Show help for commands", 
        [](const std::vector<std::string>& args) {
            if (args.empty())
            {
                ConsoleCommandManager::GetInstance().ShowHelp();
            }
            else
            {
                ConsoleCommandManager::GetInstance().ShowCommandHelp(args[0]);
            }
        });
    
    // clear command - Clear console
    cmdManager.RegisterCommand("clear", "Clear console screen", 
        [](const std::vector<std::string>& args) {
            system("cls"); // Windows
            // system("clear"); // Linux/Mac
        });
    
    LOG_INFO_CAT(LogCategory::SYSTEM, "Mod console commands initialized");
}
