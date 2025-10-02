#include "game/Entities/Npc/Npc.h"
#include "game/Entities/Player/Player.h" // For Vector4F definition
#include <iostream>

Npc::Npc(uint32_t id, uint32_t resourceId, const Vector4F& position, float health)
    : m_id(id), m_resourceId(resourceId), m_position(position), m_health(health), m_maxHealth(health)
{
    std::cout << "NPC created with ID: " << id << ", Resource ID: " << resourceId << std::endl;
}

void Npc::Update()
{
    // NPC update logic
    // This could include AI behavior, health regeneration, etc.
}

void Npc::Render()
{
    // NPC rendering logic
    // This would typically be handled by the game engine
}
