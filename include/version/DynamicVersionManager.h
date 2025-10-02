#pragma once

#include "Common.h"
#include "VersionManager.h"
#include "utils/Logger.h"

namespace Version
{
    class DynamicVersionManager : public VersionManager
    {
    public:
        DynamicVersionManager(const std::string& currentVersion);
        virtual ~DynamicVersionManager() = default;
        
        bool CheckVersion() override;
        std::string GetVersion() override;
        bool UpdateVersion() override;
        
        // Additional methods
        void AddCompatibleVersion(const std::string& compatibleVersion);
        void RemoveCompatibleVersion(const std::string& compatibleVersion);
        
    protected:
        std::vector<std::string> m_compatibleVersions;
    };
}