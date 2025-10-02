#include "utils/ConsoleCommands.h"
#include "utils/Logger.h"
#include <sstream>
#include <algorithm>
#include <iostream>

// Static member initialization
ConsoleCommandManager* ConsoleCommandManager::s_instance = nullptr;

ConsoleCommandManager& ConsoleCommandManager::GetInstance()
{
    if (s_instance == nullptr)
    {
        s_instance = new ConsoleCommandManager();
    }
    return *s_instance;
}

void ConsoleCommandManager::DestroyInstance()
{
    if (s_instance != nullptr)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

void ConsoleCommandManager::RegisterCommand(const std::string& name, const std::string& description,
                                           std::function<void(const std::vector<std::string>&)> handler)
{
    m_commands[name] = std::make_unique<ConsoleCommand>(name, description, handler);
    LOG_DEBUG_CAT(LogCategory::SYSTEM, "Console command registered: " + name);
}

bool ConsoleCommandManager::ExecuteCommand(const std::string& input)
{
    std::vector<std::string> tokens = ParseCommand(input);
    if (tokens.empty())
        return false;
    
    std::string command = tokens[0];
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    
    return ExecuteCommand(command, args);
}

bool ConsoleCommandManager::ExecuteCommand(const std::string& command, const std::vector<std::string>& args)
{
    auto it = m_commands.find(command);
    if (it == m_commands.end())
    {
        LOG_WARNING_CAT(LogCategory::SYSTEM, "Unknown command: " + command);
        return false;
    }
    
    try
    {
        it->second->handler(args);
        return true;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR_CAT(LogCategory::SYSTEM, "Error executing command '" + command + "': " + e.what());
        return false;
    }
}

void ConsoleCommandManager::ShowHelp()
{
    std::cout << "\n=== Witcher3-MP Console Commands ===\n";
    std::cout << "Type 'help <command>' for detailed help on a specific command.\n\n";
    
    for (const auto& pair : m_commands)
    {
        std::cout << "  " << std::setw(20) << std::left << pair.first 
                  << " - " << pair.second->description << "\n";
    }
    std::cout << "\n";
}

void ConsoleCommandManager::ShowCommandHelp(const std::string& command)
{
    auto it = m_commands.find(command);
    if (it == m_commands.end())
    {
        std::cout << "Command not found: " << command << "\n";
        return;
    }
    
    std::cout << "\n=== Help for command: " << command << " ===\n";
    std::cout << "Description: " << it->second->description << "\n";
    
    // Add specific help for known commands
    if (command == "mp_log")
    {
        std::cout << "\nUsage: mp_log <subcommand> [options]\n";
        std::cout << "Subcommands:\n";
        std::cout << "  flush          - Flush all buffered logs to file\n";
        std::cout << "  level <level>  - Set log level (debug, info, warning, error, critical)\n";
        std::cout << "  enable <cat>   - Enable logging for category\n";
        std::cout << "  disable <cat>  - Disable logging for category\n";
        std::cout << "  performance    - Toggle performance logging\n";
        std::cout << "  status         - Show current logging status\n";
        std::cout << "\nCategories: general, network, combat, quest, inventory, dialog, player, system\n";
    }
    else if (command == "mp_status")
    {
        std::cout << "\nUsage: mp_status\n";
        std::cout << "Shows current multiplayer session status including:\n";
        std::cout << "  - Connected players\n";
        std::cout << "  - Active quests\n";
        std::cout << "  - Network statistics\n";
        std::cout << "  - Performance metrics\n";
    }
    else if (command == "mp_quest")
    {
        std::cout << "\nUsage: mp_quest <subcommand> [options]\n";
        std::cout << "Subcommands:\n";
        std::cout << "  list           - List all active quests\n";
        std::cout << "  start <id>     - Start a quest\n";
        std::cout << "  complete <id>  - Complete a quest\n";
        std::cout << "  sync           - Sync quest state with all players\n";
    }
    else if (command == "mp_inventory")
    {
        std::cout << "\nUsage: mp_inventory <subcommand> [options]\n";
        std::cout << "Subcommands:\n";
        std::cout << "  sync           - Sync inventory with all players\n";
        std::cout << "  status         - Show inventory sync status\n";
        std::cout << "  debug          - Enable/disable inventory debug logging\n";
    }
    
    std::cout << "\n";
}

std::vector<std::string> ConsoleCommandManager::GetAvailableCommands()
{
    std::vector<std::string> commands;
    for (const auto& pair : m_commands)
    {
        commands.push_back(pair.first);
    }
    return commands;
}

std::vector<std::string> ConsoleCommandManager::ParseCommand(const std::string& input)
{
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    
    while (ss >> token)
    {
        // Remove quotes if present
        if (token.front() == '"' && token.back() == '"')
        {
            token = token.substr(1, token.length() - 2);
        }
        tokens.push_back(token);
    }
    
    return tokens;
}
