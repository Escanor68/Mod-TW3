#include "version/DynamicVersionManager.h"
#include <iostream>
#include <algorithm>

namespace Version
{
    DynamicVersionManager::DynamicVersionManager(const std::string& currentVersion)
        : VersionManager(currentVersion)
    {
        std::cout << "DynamicVersionManager initialized with version: " << currentVersion << std::endl;
    }

    void DynamicVersionManager::AddCompatibleVersion(const std::string& compatibleVersion)
    {
        m_compatibleVersions.push_back(compatibleVersion);
        std::cout << "Added compatible version: " << compatibleVersion << std::endl;
    }

    void DynamicVersionManager::RemoveCompatibleVersion(const std::string& compatibleVersion)
    {
        auto it = std::find(m_compatibleVersions.begin(), m_compatibleVersions.end(), compatibleVersion);
        if (it != m_compatibleVersions.end())
        {
            m_compatibleVersions.erase(it);
            std::cout << "Removed compatible version: " << compatibleVersion << std::endl;
        }
    }

    bool DynamicVersionManager::CheckVersion()
    {
        // Check if current version is compatible
        return std::find(m_compatibleVersions.begin(), m_compatibleVersions.end(), m_currentVersion) != m_compatibleVersions.end();
    }

    std::string DynamicVersionManager::GetVersion()
    {
        return m_currentVersion;
    }

    bool DynamicVersionManager::UpdateVersion()
    {
        // Dynamic version update logic
        return false;
    }
}