#include "database/ResourceNames.h"
#include <iostream>
#include <fstream>
#include <algorithm>

// Static member initialization
std::unordered_map<uint32_t, std::string> Database::ResourceNames::s_idToName;
std::unordered_map<std::string, uint32_t> Database::ResourceNames::s_nameToId;
bool Database::ResourceNames::s_initialized = false;

std::string Database::ResourceNames::GetResourceName(uint32_t id)
{
    if (!s_initialized)
    {
        InitializeDefaultResources();
    }
    
    auto it = s_idToName.find(id);
    return (it != s_idToName.end()) ? it->second : "Unknown";
}

uint32_t Database::ResourceNames::GetResourceId(const std::string& name)
{
    if (!s_initialized)
    {
        InitializeDefaultResources();
    }
    
    auto it = s_nameToId.find(name);
    return (it != s_nameToId.end()) ? it->second : 0;
}

uint32_t Database::ResourceNames::FindResID(const std::string& name)
{
    return GetResourceId(name);
}

bool Database::ResourceNames::LoadResourceDatabase()
{
    // Try to load from file first
    std::ifstream file("config/resource_database.txt");
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            if (line.empty() || line[0] == '#')
                continue;
                
            size_t pos = line.find('=');
            if (pos != std::string::npos)
            {
                std::string name = line.substr(0, pos);
                std::string idStr = line.substr(pos + 1);
                
                // Remove whitespace
                name.erase(0, name.find_first_not_of(" \t"));
                name.erase(name.find_last_not_of(" \t") + 1);
                idStr.erase(0, idStr.find_first_not_of(" \t"));
                idStr.erase(idStr.find_last_not_of(" \t") + 1);
                
                try
                {
                    uint32_t id = static_cast<uint32_t>(std::stoul(idStr));
                    AddResource(id, name);
                }
                catch (const std::exception&)
                {
                    std::cout << "Invalid resource ID: " << idStr << std::endl;
                }
            }
        }
        file.close();
        std::cout << "Resource database loaded from file" << std::endl;
        return true;
    }
    
    // Fallback to default resources
    InitializeDefaultResources();
    return false;
}

void Database::ResourceNames::AddResource(uint32_t id, const std::string& name)
{
    s_idToName[id] = name;
    s_nameToId[name] = id;
}

bool Database::ResourceNames::IsValidResource(uint32_t id)
{
    if (!s_initialized)
    {
        InitializeDefaultResources();
    }
    
    return s_idToName.find(id) != s_idToName.end();
}

bool Database::ResourceNames::IsValidResource(const std::string& name)
{
    if (!s_initialized)
    {
        InitializeDefaultResources();
    }
    
    return s_nameToId.find(name) != s_nameToId.end();
}

void Database::ResourceNames::PrintAllResources()
{
    if (!s_initialized)
    {
        InitializeDefaultResources();
    }
    
    std::cout << "=== Resource Database ===" << std::endl;
    for (const auto& pair : s_idToName)
    {
        std::cout << "ID: " << pair.first << " -> Name: " << pair.second << std::endl;
    }
    std::cout << "Total resources: " << s_idToName.size() << std::endl;
    std::cout << "========================" << std::endl;
}

size_t Database::ResourceNames::GetResourceCount()
{
    if (!s_initialized)
    {
        InitializeDefaultResources();
    }
    
    return s_idToName.size();
}

void Database::ResourceNames::InitializeDefaultResources()
{
    if (s_initialized)
        return;
    
    // Add some default Witcher 3 resources
    AddResource(1, "Geralt");
    AddResource(2, "Yennefer");
    AddResource(3, "Triss");
    AddResource(4, "Ciri");
    AddResource(5, "Dandelion");
    AddResource(6, "Zoltan");
    AddResource(7, "Vesemir");
    AddResource(8, "Lambert");
    AddResource(9, "Eskel");
    AddResource(10, "Keira");
    
    // Add some monster resources
    AddResource(100, "Drowner");
    AddResource(101, "Ghoul");
    AddResource(102, "Wraith");
    AddResource(103, "Leshen");
    AddResource(104, "Griffin");
    AddResource(105, "Wyvern");
    AddResource(106, "Basilisk");
    AddResource(107, "Cockatrice");
    AddResource(108, "Golem");
    AddResource(109, "Elemental");
    
    // Add some item resources
    AddResource(200, "Silver_Sword");
    AddResource(201, "Steel_Sword");
    AddResource(202, "Crossbow");
    AddResource(203, "Bomb");
    AddResource(204, "Potion");
    AddResource(205, "Oil");
    AddResource(206, "Decoction");
    AddResource(207, "Mutagen");
    AddResource(208, "Rune");
    AddResource(209, "Glyph");
    
    // Add some NPC resources (as used in Main.cpp)
    AddResource(242, "Giant");
    AddResource(243, "Troll");
    AddResource(244, "Dragon");
    
    s_initialized = true;
    std::cout << "Default resource database initialized with " << s_idToName.size() << " resources" << std::endl;
}