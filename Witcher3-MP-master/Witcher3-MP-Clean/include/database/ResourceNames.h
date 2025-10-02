#pragma once
#include "Common.h"
#include <unordered_map>
#include <string>

namespace Database
{
    class ResourceNames
    {
    public:
        // Static methods for resource management
        static std::string GetResourceName(uint32_t id);
        static uint32_t GetResourceId(const std::string& name);
        static uint32_t FindResID(const std::string& name);
        
        // Resource database management
        static bool LoadResourceDatabase();
        static void AddResource(uint32_t id, const std::string& name);
        static bool IsValidResource(uint32_t id);
        static bool IsValidResource(const std::string& name);
        
        // Utility methods
        static void PrintAllResources();
        static size_t GetResourceCount();

    private:
        static std::unordered_map<uint32_t, std::string> s_idToName;
        static std::unordered_map<std::string, uint32_t> s_nameToId;
        static bool s_initialized;
        
        static void InitializeDefaultResources();
    };
}
