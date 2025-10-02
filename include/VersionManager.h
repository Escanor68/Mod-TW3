#pragma once
#include "Common.h"
#include <string>

namespace Version
{
    class VersionManager
    {
    public:
        VersionManager(const std::string& currentVersion);
        ~VersionManager() = default;

        std::string GetCurrentVersion() const;
        bool IsCompatible(const std::string& otherVersion) const;

    private:
        std::string m_currentVersion;
    };
}
