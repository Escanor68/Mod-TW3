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
    // Update AI behavior
    if (m_aiController)
    {
        m_aiController->Update(0.016f);
    }

    // Update movement
    if (m_movementController)
    {
        m_movementController->Update(0.016f);
        m_position = m_movementController->GetPosition();
    }

    // Update animation
    if (m_animationManager)
    {
        m_animationManager->Update(0.016f);
    }

    // Update combat AI
    if (m_combatAI)
    {
        m_combatAI->Update(0.016f);
    }

    // Update network sync
    if (m_networkSync)
    {
        m_networkSync->UpdatePosition(m_position);
        m_networkSync->UpdateState(m_state);
    }
}

void Npc::Render()
{
    // Render NPC model
    if (m_model)
    {
        m_model->SetPosition(m_position);
        m_model->SetRotation(m_rotation);
        m_model->Render();
    }

    // Render health bar if in combat
    if (m_healthBar && m_state == NpcState::Combat)
    {
        m_healthBar->SetValue(m_health / m_maxHealth);
        m_healthBar->Render();
    }

    // Render name tag
    if (m_nameTag)
    {
        m_nameTag->SetText(m_name);
        m_nameTag->SetPosition(Vector4F{m_position.x, m_position.y + 2.0f, m_position.z, 1.0f});
        m_nameTag->Render();
    }

    // Render AI debug info if enabled
    if (m_debugRenderer && m_showDebugInfo)
    {
        m_debugRenderer->RenderAIPath(m_aiController->GetPath());
        m_debugRenderer->RenderAITarget(m_aiController->GetTarget());
    }
}
