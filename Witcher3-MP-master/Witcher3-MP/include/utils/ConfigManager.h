#pragma once

#include "Common.h"
#include <unordered_map>
#include <string>

class ConfigManager
{
public:
    // Singleton pattern
    static ConfigManager& GetInstance();
    static void DestroyInstance();
    
    // Configuration loading/saving
    bool LoadConfig(const std::string& filename = "config.json");
    bool SaveConfig(const std::string& filename = "config.json");
    bool ValidateConfig();
    void PrintConfig();
    
    // Value access
    void SetValue(const std::string& key, const std::string& value);
    std::string GetValue(const std::string& key, const std::string& defaultValue = "");
    
    // Typed value access
    int GetIntValue(const std::string& key, int defaultValue = 0);
    float GetFloatValue(const std::string& key, float defaultValue = 0.0f);
    bool GetBoolValue(const std::string& key, bool defaultValue = false);
    
    // Specific getters for common config values
    uint16_t GetPort() const;
    std::string GetServerName() const;
    uint32_t GetMaxConnections() const;
    bool GetDebugMode() const;

private:
    ConfigManager();
    ~ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    void SetDefaultValues();
    std::string GetConfigPath(const std::string& filename);

    std::unordered_map<std::string, std::string> m_config;
    static ConfigManager* s_instance;
};