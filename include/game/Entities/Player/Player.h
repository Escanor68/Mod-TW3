#pragma once

#include "Common.h"
#include "networking/net_connection.h"
#include "networking/net_message.h"
#include "networking/MessageTypes.h"

// Forward declarations
namespace Networking {
    template<typename T>
    class connection;
}

class Player
{
public:
    Player(uint32_t id, const Vector4F& position, float health, uint8_t characterId, 
           std::shared_ptr<Networking::connection<Networking::MessageTypes>> ownerClient);
    ~Player() = default;
    
    // Getters
    uint32_t GetID() const { return m_id; }
    Vector4F GetPosition() const { return m_position; }
    float GetHealth() const { return m_health; }
    float GetMaxHealth() const { return m_maxHealth; }
    uint8_t GetCharacterId() const { return m_characterId; }
    std::shared_ptr<Networking::connection<Networking::MessageTypes>> GetOwnerClient() const { return m_ownerClient; }
    
    // Setters
    void SetHealth(float health) { m_health = health; }
    void SetMaxHealth(float maxHealth) { m_maxHealth = maxHealth; }
    void SetPosition(const Vector4F& position) { m_position = position; }
    void UpdatePosition(const Vector4F& newPosition) { m_position = newPosition; }
    
    // Game methods
    void Update();
    void Render();
    
    // Public members for easy access (as used in Main.cpp)
    std::shared_ptr<Networking::connection<Networking::MessageTypes>> ownerClient;
    uint8_t characterId;

private:
    uint32_t m_id;
    Vector4F m_position;
    float m_health;
    float m_maxHealth;
    uint8_t m_characterId;
    std::shared_ptr<Networking::connection<Networking::MessageTypes>> m_ownerClient;
};