#pragma once

#include "Common.h"

namespace Networking
{
    // Unified message types for the networking system
    enum class MessageTypes : uint32_t
    {
        // Connection messages
        ClientConnect = 1,
        ClientDisconnect = 2,
        ClientPing = 3,
        ServerPong = 4,
        
        // Player messages
        PlayerJoin = 10,
        PlayerLeave = 11,
        PlayerMove = 12,
        PlayerUpdate = 13,
        PlayerAttack = 14,
        PlayerDefend = 15,
        PlayerUseItem = 16,
        PlayerCastSpell = 17,
        
        // NPC messages
        NpcSpawn = 20,
        NpcDespawn = 21,
        NpcMove = 22,
        NpcUpdate = 23,
        NpcAttack = 24,
        NpcDeath = 25,
        
        // World messages
        WorldUpdate = 30,
        WeatherChange = 31,
        TimeUpdate = 32,
        QuestUpdate = 33,
        
        // Chat messages
        ChatMessage = 40,
        SystemMessage = 41,
        WhisperMessage = 42,
        
        // Game state messages
        GameStateSync = 50,
        SaveGame = 51,
        LoadGame = 52,
        PauseGame = 53,
        ResumeGame = 54,
        
        // Witcher3-MP specific messages (unified)
        TC_REQUEST_PLAYERDATA = 200,
        TC_UPDATE_POS = 201,
        TS_SEND_PLAYERDATA = 202,
        TC_CREATE_PLAYER = 203,
        TC_MASS_CREATE_PLAYER = 204,
        TC_DESTROY_PLAYER = 205,
        TS_NOTIFY_PLAYER_POS_CHANGE = 206,
        TC_SET_ACTOR_HEALTH = 207,
        TC_CREATE_NPC = 208,
        TS_HIT_NPC = 209,
        TS_GOT_HIT = 210,
        TC_NPC_DEAD = 211,
        TC_PLAYER_DEAD = 212,
        TS_CHAT_MESSAGE = 213,
        TC_CHAT_MESSAGE = 214,
        
        // Network optimization messages
        CompressionEnabled = 300,
        CompressionDisabled = 301,
        BatchMessage = 302,
        DeltaUpdate = 303,
        
        // Error messages
        ErrorMessage = 100,
        InvalidMessage = 101,
        ConnectionLost = 102,
        ServerFull = 103,
        VersionMismatch = 104,
        CompressionError = 105
    };
}
