#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <memory>

namespace Networking
{
    // Network statistics structure
    struct NetworkStats
    {
        size_t packetsSent = 0;
        size_t packetsReceived = 0;
        size_t bytesSent = 0;
        size_t bytesReceived = 0;
        float packetLoss = 0.0f;
        float averageLatency = 0.0f;
    };

    // Network logger for Witcher3-MP
    class NetworkLogger
    {
    public:
        static NetworkLogger& GetInstance();
        static void DestroyInstance();

        // Initialize/Shutdown
        bool Initialize();
        void Shutdown();

        // Logging methods
        void LogConnection(const std::string& clientId, const std::string& action, const std::string& details = "");
        void LogMessage(const std::string& messageType, const std::string& direction, size_t size, const std::string& clientId);
        void LogLatency(const std::string& clientId, float latency);
        void LogPacketLoss(const std::string& clientId, float packetLoss);
        void LogCompression(const std::string& messageType, size_t originalSize, size_t compressedSize, float compressionRatio);
        void LogNetworkEvent(const std::string& event, const std::string& details, const std::string& clientId);
        void LogError(const std::string& error, const std::string& clientId);

        // Statistics
        void LogStatistics();
        NetworkStats GetStatistics() const;
        void ResetStatistics();

        // Log management
        void FlushLogs();

    private:
        NetworkLogger() = default;
        ~NetworkLogger();
        NetworkLogger(const NetworkLogger&) = delete;
        NetworkLogger& operator=(const NetworkLogger&) = delete;

        std::string GetCurrentTimestamp();
        void WriteToLog(const std::string& logEntry);
        void UpdateAverageLatency(float latency);

        bool m_initialized;
        std::ofstream* m_logFile;
        
        // Statistics
        size_t m_packetsSent;
        size_t m_packetsReceived;
        size_t m_bytesSent;
        size_t m_bytesReceived;
        float m_packetLoss;
        float m_averageLatency;
        
        static NetworkLogger* s_instance;
    };

    // Convenience macros for network logging
    #define LOG_NETWORK_CONNECTION(clientId, action, details) NetworkLogger::GetInstance().LogConnection(clientId, action, details)
    #define LOG_NETWORK_MESSAGE(type, direction, size, clientId) NetworkLogger::GetInstance().LogMessage(type, direction, size, clientId)
    #define LOG_NETWORK_LATENCY(clientId, latency) NetworkLogger::GetInstance().LogLatency(clientId, latency)
    #define LOG_NETWORK_PACKET_LOSS(clientId, loss) NetworkLogger::GetInstance().LogPacketLoss(clientId, loss)
    #define LOG_NETWORK_COMPRESSION(type, orig, comp, ratio) NetworkLogger::GetInstance().LogCompression(type, orig, comp, ratio)
    #define LOG_NETWORK_EVENT(event, details, clientId) NetworkLogger::GetInstance().LogNetworkEvent(event, details, clientId)
    #define LOG_NETWORK_ERROR(error, clientId) NetworkLogger::GetInstance().LogError(error, clientId)
}
