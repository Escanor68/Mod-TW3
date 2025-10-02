#pragma once

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <memory>

// Console command structure
struct ConsoleCommand
{
    std::string name;
    std::string description;
    std::function<void(const std::vector<std::string>&)> handler;
    
    ConsoleCommand(const std::string& cmdName, const std::string& desc, 
                   std::function<void(const std::vector<std::string>&)> handlerFunc)
        : name(cmdName), description(desc), handler(handlerFunc) {}
};

class ConsoleCommandManager
{
public:
    static ConsoleCommandManager& GetInstance();
    static void DestroyInstance();
    
    // Command registration
    void RegisterCommand(const std::string& name, const std::string& description,
                        std::function<void(const std::vector<std::string>&)> handler);
    
    // Command execution
    bool ExecuteCommand(const std::string& input);
    bool ExecuteCommand(const std::string& command, const std::vector<std::string>& args);
    
    // Help system
    void ShowHelp();
    void ShowCommandHelp(const std::string& command);
    std::vector<std::string> GetAvailableCommands();
    
    // Command parsing
    std::vector<std::string> ParseCommand(const std::string& input);
    
private:
    ConsoleCommandManager() = default;
    ~ConsoleCommandManager() = default;
    ConsoleCommandManager(const ConsoleCommandManager&) = delete;
    ConsoleCommandManager& operator=(const ConsoleCommandManager&) = delete;
    
    std::map<std::string, std::unique_ptr<ConsoleCommand>> m_commands;
    static ConsoleCommandManager* s_instance;
};

// Convenience macro for command registration
#define REGISTER_CONSOLE_COMMAND(name, desc, handler) \
    ConsoleCommandManager::GetInstance().RegisterCommand(name, desc, handler)
