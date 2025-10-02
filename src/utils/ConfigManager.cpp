#include "utils/ConfigManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

// Static member initialization
ConfigManager* ConfigManager::s_instance = nullptr;

ConfigManager& ConfigManager::GetInstance()
{
    if (!s_instance)
    {
        s_instance = new ConfigManager();
    }
    return *s_instance;
}

void ConfigManager::DestroyInstance()
{
    if (s_instance)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

ConfigManager::ConfigManager()
{
    SetDefaultValues();
}

void ConfigManager::SetDefaultValues()
{
    m_config["server_name"] = "Witcher3-MP Server";
    m_config["port"] = "60000";
    m_config["max_connections"] = "100";
    m_config["debug_mode"] = "false";
    m_config["log_level"] = "INFO";
    m_config["auto_save"] = "true";
    m_config["save_interval"] = "300"; // 5 minutes
}

bool ConfigManager::LoadConfig(const std::string& filename)
{
    std::string configPath = GetConfigPath(filename);
    std::ifstream file(configPath);
    
    if (!file.is_open())
    {
        std::cout << "Config file not found: " << configPath << ", using defaults" << std::endl;
        return false;
    }
    
    std::string line;
    while (std::getline(file, line))
    {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#')
            continue;
            
        size_t pos = line.find('=');
        if (pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Remove leading/trailing whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            m_config[key] = value;
        }
    }
    
    file.close();
    std::cout << "Config loaded from: " << configPath << std::endl;
    return true;
}

bool ConfigManager::SaveConfig(const std::string& filename)
{
    std::string configPath = GetConfigPath(filename);
    std::ofstream file(configPath);
    
    if (!file.is_open())
    {
        std::cout << "Failed to create config file: " << configPath << std::endl;
        return false;
    }
    
    file << "# Witcher3-MP Server Configuration" << std::endl;
    file << "# Generated automatically" << std::endl;
    file << std::endl;
    
    for (const auto& pair : m_config)
    {
        file << pair.first << "=" << pair.second << std::endl;
    }
    
    file.close();
    std::cout << "Config saved to: " << configPath << std::endl;
    return true;
}

bool ConfigManager::ValidateConfig()
{
    // Validate port
    int port = GetIntValue("port", 0);
    if (port <= 0 || port > 65535)
    {
        std::cout << "Invalid port: " << port << std::endl;
        return false;
    }
    
    // Validate max connections
    int maxConn = GetIntValue("max_connections", 0);
    if (maxConn <= 0 || maxConn > 1000)
    {
        std::cout << "Invalid max_connections: " << maxConn << std::endl;
        return false;
    }
    
    return true;
}

void ConfigManager::PrintConfig()
{
    std::cout << "=== Server Configuration ===" << std::endl;
    for (const auto& pair : m_config)
    {
        std::cout << pair.first << " = " << pair.second << std::endl;
    }
    std::cout << "============================" << std::endl;
}

void ConfigManager::SetValue(const std::string& key, const std::string& value)
{
    m_config[key] = value;
}

std::string ConfigManager::GetValue(const std::string& key, const std::string& defaultValue)
{
    auto it = m_config.find(key);
    return (it != m_config.end()) ? it->second : defaultValue;
}

int ConfigManager::GetIntValue(const std::string& key, int defaultValue)
{
    std::string value = GetValue(key);
    if (value.empty())
        return defaultValue;
    
    try
    {
        return std::stoi(value);
    }
    catch (const std::exception&)
    {
        return defaultValue;
    }
}

float ConfigManager::GetFloatValue(const std::string& key, float defaultValue)
{
    std::string value = GetValue(key);
    if (value.empty())
        return defaultValue;
    
    try
    {
        return std::stof(value);
    }
    catch (const std::exception&)
    {
        return defaultValue;
    }
}

bool ConfigManager::GetBoolValue(const std::string& key, bool defaultValue)
{
    std::string value = GetValue(key);
    if (value.empty())
        return defaultValue;
    
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return (value == "true" || value == "1" || value == "yes");
}

uint16_t ConfigManager::GetPort() const
{
    auto it = m_config.find("port");
    if (it != m_config.end())
    {
        try
        {
            return static_cast<uint16_t>(std::stoi(it->second));
        }
        catch (const std::exception&)
        {
            return 60000;
        }
    }
    return 60000;
}

std::string ConfigManager::GetServerName() const
{
    auto it = m_config.find("server_name");
    return (it != m_config.end()) ? it->second : "Witcher3-MP Server";
}

uint32_t ConfigManager::GetMaxConnections() const
{
    auto it = m_config.find("max_connections");
    if (it != m_config.end())
    {
        try
        {
            return static_cast<uint32_t>(std::stoi(it->second));
        }
        catch (const std::exception&)
        {
            return 100;
        }
    }
    return 100;
}

bool ConfigManager::GetDebugMode() const
{
    auto it = m_config.find("debug_mode");
    if (it != m_config.end())
    {
        std::string value = it->second;
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        return (value == "true" || value == "1" || value == "yes");
    }
    return false;
}

std::string ConfigManager::GetConfigPath(const std::string& filename)
{
    return "config/" + filename;
}