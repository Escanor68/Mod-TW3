#pragma once

#include "Common.h"

class Npc
{
public:
    Npc(uint32_t id, uint32_t resourceId, const Vector4F& position, float health);
    ~Npc() = default;
    
    // Getters
    uint32_t GetID() const { return m_id; }
    uint32_t GetResourceId() const { return m_resourceId; }
    Vector4F GetPosition() const { return m_position; }
    float GetHealth() const { return m_health; }
    float GetMaxHealth() const { return m_maxHealth; }
    
    // Setters
    void SetHealth(float health) { m_health = health; }
    void SetMaxHealth(float maxHealth) { m_maxHealth = maxHealth; }
    void SetPosition(const Vector4F& position) { m_position = position; }
    
    // Game methods
    void Update();
    void Render();
    
private:
    uint32_t m_id;
    uint32_t m_resourceId;
    Vector4F m_position;
    float m_health;
    float m_maxHealth;
};
