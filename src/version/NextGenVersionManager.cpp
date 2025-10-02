#include "version/NextGenVersionManager.h"
#include <iostream>

namespace Version
{
    NextGenVersionManager::NextGenVersionManager(const std::string& currentVersion)
        : DynamicVersionManager(currentVersion)
    {
        std::cout << "NextGenVersionManager initialized with version: " << currentVersion << std::endl;
    }

    void NextGenVersionManager::AnnounceNextGenFeature(const std::string& featureName)
    {
        std::cout << "Next-Gen feature announced: " << featureName << std::endl;
    }

    bool NextGenVersionManager::CheckVersion()
    {
        // Next-gen specific version checking
        return DynamicVersionManager::CheckVersion();
    }

    std::string NextGenVersionManager::GetVersion()
    {
        return DynamicVersionManager::GetVersion();
    }

    bool NextGenVersionManager::UpdateVersion()
    {
        // Next-gen specific update logic
        return DynamicVersionManager::UpdateVersion();
    }
}