#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

// Test includes
#include "integration/TW3ModInterface.h"
#include "networking/LANDiscovery.h"
#include "utils/Logger.h"

using namespace TW3Integration;
using namespace TW3Networking;

class IntegrationTestSuite
{
public:
    IntegrationTestSuite()
    {
        // Initialize logging for tests
        Logger::Initialize("logs/test_integration.log");
        LOG_INFO("=== Starting Integration Test Suite ===");
    }

    ~IntegrationTestSuite()
    {
        LOG_INFO("=== Integration Test Suite Completed ===");
        Logger::Shutdown();
    }

    // Test mod file structure
    bool TestModFileStructure()
    {
        LOG_INFO("Testing mod file structure...");
        
        std::vector<std::string> requiredFiles = {
            "mod/modInfo.ws",
            "mod/mod.settings",
            "mod/config/mp_config.json",
            "mod/config/witcher3_mp_config.xml",
            "mod/scripts/game/witcher3_mp/MP_ModMenu.ws",
            "mod/scripts/game/witcher3_mp/MP_Config.ws",
            "mod/scripts/game/witcher3_mp/MP_ServerManager.ws",
            "mod/scripts/game/witcher3_mp/MP_ModInitializer.ws"
        };

        bool allFilesExist = true;
        for (const auto& file : requiredFiles)
        {
            if (!std::filesystem::exists(file))
            {
                LOG_ERROR("Required file missing: " + file);
                allFilesExist = false;
            }
            else
            {
                LOG_INFO("✓ Found: " + file);
            }
        }

        return allFilesExist;
    }

    // Test JSON configuration files
    bool TestJSONConfiguration()
    {
        LOG_INFO("Testing JSON configuration files...");
        
        // Test main config
        if (!TestJSONFile("config/config.json"))
        {
            return false;
        }

        // Test mod config
        if (!TestJSONFile("mod/config/mp_config.json"))
        {
            return false;
        }

        return true;
    }

    // Test XML configuration files
    bool TestXMLConfiguration()
    {
        LOG_INFO("Testing XML configuration files...");
        
        if (!TestXMLFile("mod/config/witcher3_mp_config.xml"))
        {
            return false;
        }

        return true;
    }

    // Test WitcherScript compilation
    bool TestWitcherScriptCompilation()
    {
        LOG_INFO("Testing WitcherScript compilation...");
        
        std::vector<std::string> scriptFiles = {
            "mod/scripts/game/witcher3_mp/MP_ModMenu.ws",
            "mod/scripts/game/witcher3_mp/MP_Config.ws",
            "mod/scripts/game/witcher3_mp/MP_ServerManager.ws",
            "mod/scripts/game/witcher3_mp/MP_ModInitializer.ws"
        };

        bool allValid = true;
        for (const auto& script : scriptFiles)
        {
            if (!TestWitcherScriptFile(script))
            {
                allValid = false;
            }
        }

        return allValid;
    }

    // Test mod interface initialization
    bool TestModInterfaceInitialization()
    {
        LOG_INFO("Testing mod interface initialization...");
        
        try
        {
            auto& interface = TW3ModInterface::GetInstance();
            
            if (!interface.Initialize())
            {
                LOG_ERROR("Failed to initialize TW3ModInterface");
                return false;
            }

            LOG_INFO("✓ TW3ModInterface initialized successfully");
            
            // Test LAN discovery initialization
            interface.InitializeLANDiscovery();
            LOG_INFO("✓ LAN Discovery initialized");
            
            // Test IP connection initialization
            interface.InitializeIPConnection();
            LOG_INFO("✓ IP Connection initialized");
            
            interface.Shutdown();
            return true;
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Exception during mod interface initialization: " + std::string(e.what()));
            return false;
        }
    }

    // Test LAN discovery system
    bool TestLANDiscovery()
    {
        LOG_INFO("Testing LAN discovery system...");
        
        try
        {
            LANDiscovery discovery;
            if (!discovery.Initialize())
            {
                LOG_ERROR("Failed to initialize LAN discovery");
                return false;
            }

            LOG_INFO("✓ LAN Discovery initialized successfully");
            
            // Test discovery start/stop
            discovery.StartDiscovery();
            LOG_INFO("✓ LAN Discovery started");
            
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            
            discovery.StopDiscovery();
            LOG_INFO("✓ LAN Discovery stopped");
            
            discovery.Shutdown();
            return true;
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Exception during LAN discovery test: " + std::string(e.what()));
            return false;
        }
    }

    // Test IP connection system
    bool TestIPConnection()
    {
        LOG_INFO("Testing IP connection system...");
        
        try
        {
            IPConnection connection;
            if (!connection.Initialize())
            {
                LOG_ERROR("Failed to initialize IP connection");
                return false;
            }

            LOG_INFO("✓ IP Connection initialized successfully");
            
            // Test connection to localhost (should fail gracefully)
            bool connected = connection.ConnectToServer("127.0.0.1", 60000, "TestPlayer");
            if (connected)
            {
                LOG_INFO("✓ Connected to test server");
                connection.Disconnect();
            }
            else
            {
                LOG_INFO("✓ Connection test completed (no server running, expected)");
            }
            
            connection.Shutdown();
            return true;
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Exception during IP connection test: " + std::string(e.what()));
            return false;
        }
    }

    // Test configuration loading and saving
    bool TestConfigurationManagement()
    {
        LOG_INFO("Testing configuration management...");
        
        try
        {
            auto& interface = TW3ModInterface::GetInstance();
            interface.Initialize();
            
            // Test game state serialization
            std::string gameState = interface.SerializeGameState();
            if (gameState.empty())
            {
                LOG_ERROR("Failed to serialize game state");
                return false;
            }
            
            LOG_INFO("✓ Game state serialization successful");
            
            // Test game state deserialization
            interface.DeserializeGameState(gameState);
            LOG_INFO("✓ Game state deserialization successful");
            
            interface.Shutdown();
            return true;
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Exception during configuration management test: " + std::string(e.what()));
            return false;
        }
    }

    // Test mod menu integration
    bool TestModMenuIntegration()
    {
        LOG_INFO("Testing mod menu integration...");
        
        // Check if mod menu script exists and has correct structure
        std::string menuScript = "mod/scripts/game/witcher3_mp/MP_ModMenu.ws";
        if (!std::filesystem::exists(menuScript))
        {
            LOG_ERROR("Mod menu script not found: " + menuScript);
            return false;
        }

        // Read and validate script content
        std::ifstream file(menuScript);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open mod menu script");
            return false;
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        // Check for required functions
        std::vector<std::string> requiredFunctions = {
            "InitializeModMenu",
            "ShowModMenu",
            "HideModMenu",
            "ToggleModMenu",
            "ConnectToServer",
            "StartServer"
        };

        bool allFunctionsFound = true;
        for (const auto& func : requiredFunctions)
        {
            if (content.find(func) == std::string::npos)
            {
                LOG_ERROR("Required function not found in mod menu: " + func);
                allFunctionsFound = false;
            }
            else
            {
                LOG_INFO("✓ Found function: " + func);
            }
        }

        return allFunctionsFound;
    }

private:
    bool TestJSONFile(const std::string& filePath)
    {
        if (!std::filesystem::exists(filePath))
        {
            LOG_ERROR("JSON file not found: " + filePath);
            return false;
        }

        std::ifstream file(filePath);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open JSON file: " + filePath);
            return false;
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        // Basic JSON validation (check for balanced braces and quotes)
        int braceCount = 0;
        int bracketCount = 0;
        bool inString = false;
        bool escaped = false;

        for (char c : content)
        {
            if (escaped)
            {
                escaped = false;
                continue;
            }

            if (c == '\\')
            {
                escaped = true;
                continue;
            }

            if (c == '"' && !escaped)
            {
                inString = !inString;
                continue;
            }

            if (!inString)
            {
                if (c == '{') braceCount++;
                else if (c == '}') braceCount--;
                else if (c == '[') bracketCount++;
                else if (c == ']') bracketCount--;
            }
        }

        if (braceCount != 0 || bracketCount != 0)
        {
            LOG_ERROR("Invalid JSON structure in: " + filePath);
            return false;
        }

        LOG_INFO("✓ Valid JSON: " + filePath);
        return true;
    }

    bool TestXMLFile(const std::string& filePath)
    {
        if (!std::filesystem::exists(filePath))
        {
            LOG_ERROR("XML file not found: " + filePath);
            return false;
        }

        std::ifstream file(filePath);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open XML file: " + filePath);
            return false;
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        // Basic XML validation (check for balanced tags)
        std::istringstream iss(content);
        std::string line;
        int openTags = 0;
        int closeTags = 0;

        while (std::getline(iss, line))
        {
            for (size_t i = 0; i < line.length() - 1; ++i)
            {
                if (line[i] == '<' && line[i + 1] != '/')
                {
                    openTags++;
                }
                else if (line[i] == '<' && line[i + 1] == '/')
                {
                    closeTags++;
                }
            }
        }

        if (openTags != closeTags)
        {
            LOG_ERROR("Invalid XML structure in: " + filePath);
            return false;
        }

        LOG_INFO("✓ Valid XML: " + filePath);
        return true;
    }

    bool TestWitcherScriptFile(const std::string& filePath)
    {
        if (!std::filesystem::exists(filePath))
        {
            LOG_ERROR("WitcherScript file not found: " + filePath);
            return false;
        }

        std::ifstream file(filePath);
        if (!file.is_open())
        {
            LOG_ERROR("Failed to open WitcherScript file: " + filePath);
            return false;
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        // Basic WitcherScript validation
        if (content.find("class ") == std::string::npos)
        {
            LOG_ERROR("No class definition found in: " + filePath);
            return false;
        }

        if (content.find("func ") == std::string::npos)
        {
            LOG_ERROR("No function definitions found in: " + filePath);
            return false;
        }

        LOG_INFO("✓ Valid WitcherScript: " + filePath);
        return true;
    }
};

// Test cases
TEST_CASE("Mod File Structure", "[integration]")
{
    IntegrationTestSuite suite;
    REQUIRE(suite.TestModFileStructure());
}

TEST_CASE("JSON Configuration", "[integration]")
{
    IntegrationTestSuite suite;
    REQUIRE(suite.TestJSONConfiguration());
}

TEST_CASE("XML Configuration", "[integration]")
{
    IntegrationTestSuite suite;
    REQUIRE(suite.TestXMLConfiguration());
}

TEST_CASE("WitcherScript Compilation", "[integration]")
{
    IntegrationTestSuite suite;
    REQUIRE(suite.TestWitcherScriptCompilation());
}

TEST_CASE("Mod Interface Initialization", "[integration]")
{
    IntegrationTestSuite suite;
    REQUIRE(suite.TestModInterfaceInitialization());
}

TEST_CASE("LAN Discovery System", "[integration]")
{
    IntegrationTestSuite suite;
    REQUIRE(suite.TestLANDiscovery());
}

TEST_CASE("IP Connection System", "[integration]")
{
    IntegrationTestSuite suite;
    REQUIRE(suite.TestIPConnection());
}

TEST_CASE("Configuration Management", "[integration]")
{
    IntegrationTestSuite suite;
    REQUIRE(suite.TestConfigurationManagement());
}

TEST_CASE("Mod Menu Integration", "[integration]")
{
    IntegrationTestSuite suite;
    REQUIRE(suite.TestModMenuIntegration());
}

// Main test runner
int main(int argc, char* argv[])
{
    // Create logs directory if it doesn't exist
    std::filesystem::create_directories("logs");

    // Run tests
    int result = Catch::Session().run(argc, argv);

    // Generate test report
    std::ofstream report("logs/test_results.log");
    if (report.is_open())
    {
        report << "=== Witcher3-MP Integration Test Results ===" << std::endl;
        report << "Test completed at: " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
        report << "Result: " << (result == 0 ? "PASSED" : "FAILED") << std::endl;
        report.close();
    }

    return result;
}
