#pragma once

// Standard C++ includes
#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <deque>
#include <queue>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <chrono>
#include <atomic>
#include <random>
#include <exception>
#include <stdexcept>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iomanip>

// Windows specific includes
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include "WindowsConfig.h"
#endif

// ASIO includes
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

// Common type definitions
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;
using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

// Common aliases
template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

// Common macros
#define UNUSED(x) ((void)(x))

// Common constants
constexpr uint16 DEFAULT_PORT = 60000;
constexpr uint32 MAX_MESSAGE_SIZE = 1024;
constexpr uint32 MAX_CONNECTIONS = 100;

// Common enums
enum class MessageType : uint8
{
    ServerAccept,
    ServerDeny,
    ServerPing,
    MessageAll,
    ServerMessage,
    ClientMessage,
    PlayerJoin,
    PlayerLeave,
    PlayerMove,
    PlayerAction,
    GameState,
    ChatMessage,
    SystemMessage
};

// Common structures
struct NetworkMessage
{
    MessageType type;
    uint32 size;
    std::vector<uint8> data;
    
    NetworkMessage() : type(MessageType::ServerAccept), size(0) {}
    NetworkMessage(MessageType t, uint32 s) : type(t), size(s) {}
};

// Common utility functions
inline std::string GetCurrentTimeString()
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    struct tm timeinfo;
    localtime_s(&timeinfo, &time_t);
    ss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

inline void LogMessage(const std::string& message)
{
    std::cout << "[" << GetCurrentTimeString() << "] " << message << std::endl;
}

// Definition of Vector4F
struct Vector4F
{
    float x, y, z, w;
    
    Vector4F() : x(0), y(0), z(0), w(0) {}
    Vector4F(float x, float y, float z, float w = 0) : x(x), y(y), z(z), w(w) {}
    
    bool null() const { return x == 0 && y == 0 && z == 0 && w == 0; }
};

// Common error handling
class NetworkException : public std::runtime_error
{
public:
    NetworkException(const std::string& message) : std::runtime_error(message) {}
};

// Common validation
inline bool IsValidPort(uint16 port)
{
    return port > 0 && port < 65536;
}

inline bool IsValidMessageSize(uint32 size)
{
    return size > 0 && size <= MAX_MESSAGE_SIZE;
}
