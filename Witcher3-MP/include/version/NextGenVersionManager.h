#pragma once

#include "Common.h"
#include "version/DynamicVersionManager.h"
#include "utils/Logger.h"

namespace Version
{
    class NextGenVersionManager : public DynamicVersionManager
    {
    public:
        NextGenVersionManager(const std::string& currentVersion);
        virtual ~NextGenVersionManager() = default;
        
        bool CheckVersion() override;
        std::string GetVersion() override;
        bool UpdateVersion() override;
        
        // Additional methods
        void AnnounceNextGenFeature(const std::string& featureName);
    };
}