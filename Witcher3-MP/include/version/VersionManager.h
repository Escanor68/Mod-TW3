#pragma once

#include "Common.h"
#include "utils/Logger.h"

namespace Version
{
    class VersionManager
    {
    public:
        VersionManager(const std::string& currentVersion);
        virtual ~VersionManager() = default;
        
        virtual bool CheckVersion() = 0;
        virtual std::string GetVersion() = 0;
        virtual bool UpdateVersion() = 0;
        
        // Additional methods
        std::string GetCurrentVersion() const;
        bool IsCompatible(const std::string& otherVersion) const;
        
    protected:
        std::string m_currentVersion;
    };
}