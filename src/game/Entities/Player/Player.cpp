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
    // Player update logic
    // This could include movement, health regeneration, etc.
}

void Player::Render()
{
    // Player rendering logic
    // This would typically be handled by the game engine
}
