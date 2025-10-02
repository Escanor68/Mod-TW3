#include "game/Entities/Player/Player.h"
#include <iostream>

Player::Player(uint32_t id, const Vector4F& position, float health, uint8_t characterId, 
               std::shared_ptr<Networking::connection<Networking::MessageTypes>> ownerClient)
    : m_id(id), m_position(position), m_health(health), m_maxHealth(health), 
      m_characterId(characterId), ownerClient(ownerClient), m_ownerClient(ownerClient), characterId(characterId)
{
    std::cout << "Player created with ID: " << id << std::endl;
}

void Player::Update()
{
    // Update player position based on input
    if (m_inputManager)
    {
        Vector4F input = m_inputManager->GetMovementInput();
        m_position.x += input.x * m_speed * 0.016f; // Assuming 60 FPS
        m_position.y += input.y * m_speed * 0.016f;
        m_position.z += input.z * m_speed * 0.016f;
    }

    // Update animation state
    if (m_animationManager)
    {
        m_animationManager->Update(0.016f);
    }

    // Update combat state
    if (m_combatState != CombatState::Idle)
    {
        UpdateCombatState();
    }

    // Update network sync
    if (m_networkSync)
    {
        m_networkSync->UpdatePosition(m_position);
    }
}

void Player::Render()
{
    // Render player model
    if (m_model)
    {
        m_model->SetPosition(m_position);
        m_model->SetRotation(m_rotation);
        m_model->Render();
    }

    // Render health bar
    if (m_healthBar)
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
}
