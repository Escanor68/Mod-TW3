#include "networking/NetworkLogger.h"
#include "utils/Logger.h"
#include <fstream>
#include <chrono>
#include <iomanip>

namespace Networking
{
    // Static member initialization
    NetworkLogger* NetworkLogger::s_instance = nullptr;

    NetworkLogger& NetworkLogger::GetInstance()
    {
        if (s_instance == nullptr)
        {
            s_instance = new NetworkLogger();
        }
        return *s_instance;
    }

    void NetworkLogger::DestroyInstance()
    {
        if (s_instance != nullptr)
        {
            delete s_instance;
            s_instance = nullptr;
        }
    }

    NetworkLogger::NetworkLogger()
        : m_initialized(false), m_logFile(nullptr), m_packetsSent(0), m_packetsReceived(0),
          m_bytesSent(0), m_bytesReceived(0), m_packetLoss(0.0f), m_averageLatency(0.0f)
    {
        Initialize();
    }

    NetworkLogger::~NetworkLogger()
    {
        Shutdown();
    }

    bool NetworkLogger::Initialize()
    {
        if (m_initialized)
        {
            return true;
        }

        // Create logs directory if it doesn't exist
        std::filesystem::create_directories("logs");

        // Open network log file
        m_logFile = new std::ofstream("logs/network.log", std::ios::app);
        if (!m_logFile->is_open())
        {
            LOG_ERROR_CAT(LogCategory::NETWORK, "Failed to open network log file");
            return false;
        }

        m_initialized = true;
        LOG_INFO_CAT(LogCategory::NETWORK, "Network logger initialized");
        
        // Log initialization
        LogNetworkEvent("NetworkLogger", "Initialized", "System");
        
        return true;
    }

    void NetworkLogger::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        // Log final statistics
        LogNetworkEvent("NetworkLogger", "Shutting down", "System");
        LogStatistics();
        
        if (m_logFile && m_logFile->is_open())
        {
            m_logFile->close();
            delete m_logFile;
            m_logFile = nullptr;
        }

        m_initialized = false;
        LOG_INFO_CAT(LogCategory::NETWORK, "Network logger shutdown");
    }

    void NetworkLogger::LogConnection(const std::string& clientId, const std::string& action, const std::string& details)
    {
        if (!m_initialized)
        {
            return;
        }

        std::string timestamp = GetCurrentTimestamp();
        std::string logEntry = "[" + timestamp + "] [CONNECTION] [" + clientId + "] " + action;
        
        if (!details.empty())
        {
            logEntry += " - " + details;
        }
        
        WriteToLog(logEntry);
        LOG_INFO_CAT(LogCategory::NETWORK, "Connection " + action + " - " + clientId);
    }

    void NetworkLogger::LogMessage(const std::string& messageType, const std::string& direction, 
                                  size_t size, const std::string& clientId)
    {
        if (!m_initialized)
        {
            return;
        }

        std::string timestamp = GetCurrentTimestamp();
        std::string logEntry = "[" + timestamp + "] [MESSAGE] [" + direction + "] [" + messageType + "] [" + clientId + "] " + 
                              std::to_string(size) + " bytes";
        
        WriteToLog(logEntry);
        
        // Update statistics
        if (direction == "SENT")
        {
            m_packetsSent++;
            m_bytesSent += size;
        }
        else if (direction == "RECEIVED")
        {
            m_packetsReceived++;
            m_bytesReceived += size;
        }
    }

    void NetworkLogger::LogLatency(const std::string& clientId, float latency)
    {
        if (!m_initialized)
        {
            return;
        }

        std::string timestamp = GetCurrentTimestamp();
        std::string logEntry = "[" + timestamp + "] [LATENCY] [" + clientId + "] " + 
                              std::to_string(latency) + "ms";
        
        WriteToLog(logEntry);
        
        // Update average latency
        UpdateAverageLatency(latency);
        
        LOG_DEBUG_CAT(LogCategory::NETWORK, "Latency for " + clientId + ": " + std::to_string(latency) + "ms");
    }

    void NetworkLogger::LogPacketLoss(const std::string& clientId, float packetLoss)
    {
        if (!m_initialized)
        {
            return;
        }

        std::string timestamp = GetCurrentTimestamp();
        std::string logEntry = "[" + timestamp + "] [PACKET_LOSS] [" + clientId + "] " + 
                              std::to_string(packetLoss) + "%";
        
        WriteToLog(logEntry);
        
        m_packetLoss = packetLoss;
        
        LOG_WARNING_CAT(LogCategory::NETWORK, "Packet loss for " + clientId + ": " + std::to_string(packetLoss) + "%");
    }

    void NetworkLogger::LogCompression(const std::string& messageType, size_t originalSize, 
                                      size_t compressedSize, float compressionRatio)
    {
        if (!m_initialized)
        {
            return;
        }

        std::string timestamp = GetCurrentTimestamp();
        std::string logEntry = "[" + timestamp + "] [COMPRESSION] [" + messageType + "] " + 
                              std::to_string(originalSize) + " -> " + std::to_string(compressedSize) + 
                              " bytes (" + std::to_string(compressionRatio * 100.0f) + "% ratio)";
        
        WriteToLog(logEntry);
        
        LOG_DEBUG_CAT(LogCategory::NETWORK, "Compression: " + messageType + " - " + 
                     std::to_string(compressionRatio * 100.0f) + "% ratio");
    }

    void NetworkLogger::LogNetworkEvent(const std::string& event, const std::string& details, const std::string& clientId)
    {
        if (!m_initialized)
        {
            return;
        }

        std::string timestamp = GetCurrentTimestamp();
        std::string logEntry = "[" + timestamp + "] [EVENT] [" + event + "] [" + clientId + "] " + details;
        
        WriteToLog(logEntry);
        LOG_INFO_CAT(LogCategory::NETWORK, "Network event: " + event + " - " + details);
    }

    void NetworkLogger::LogError(const std::string& error, const std::string& clientId)
    {
        if (!m_initialized)
        {
            return;
        }

        std::string timestamp = GetCurrentTimestamp();
        std::string logEntry = "[" + timestamp + "] [ERROR] [" + clientId + "] " + error;
        
        WriteToLog(logEntry);
        LOG_ERROR_CAT(LogCategory::NETWORK, "Network error: " + error);
    }

    void NetworkLogger::LogStatistics()
    {
        if (!m_initialized)
        {
            return;
        }

        std::string timestamp = GetCurrentTimestamp();
        std::string logEntry = "[" + timestamp + "] [STATISTICS] Packets sent: " + std::to_string(m_packetsSent) + 
                              ", Packets received: " + std::to_string(m_packetsReceived) + 
                              ", Bytes sent: " + std::to_string(m_bytesSent) + 
                              ", Bytes received: " + std::to_string(m_bytesReceived) + 
                              ", Packet loss: " + std::to_string(m_packetLoss) + "%" +
                              ", Average latency: " + std::to_string(m_averageLatency) + "ms";
        
        WriteToLog(logEntry);
        LOG_INFO_CAT(LogCategory::NETWORK, "Network statistics logged");
    }

    void NetworkLogger::FlushLogs()
    {
        if (m_logFile && m_logFile->is_open())
        {
            m_logFile->flush();
        }
    }

    NetworkStats NetworkLogger::GetStatistics() const
    {
        NetworkStats stats;
        stats.packetsSent = m_packetsSent;
        stats.packetsReceived = m_packetsReceived;
        stats.bytesSent = m_bytesSent;
        stats.bytesReceived = m_bytesReceived;
        stats.packetLoss = m_packetLoss;
        stats.averageLatency = m_averageLatency;
        return stats;
    }

    void NetworkLogger::ResetStatistics()
    {
        m_packetsSent = 0;
        m_packetsReceived = 0;
        m_bytesSent = 0;
        m_bytesReceived = 0;
        m_packetLoss = 0.0f;
        m_averageLatency = 0.0f;
        
        LOG_INFO_CAT(LogCategory::NETWORK, "Network statistics reset");
    }

    std::string NetworkLogger::GetCurrentTimestamp()
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

    void NetworkLogger::WriteToLog(const std::string& logEntry)
    {
        if (m_logFile && m_logFile->is_open())
        {
            *m_logFile << logEntry << std::endl;
        }
    }

    void NetworkLogger::UpdateAverageLatency(float latency)
    {
        static std::vector<float> latencyHistory;
        latencyHistory.push_back(latency);
        
        // Keep only last 100 latency measurements
        if (latencyHistory.size() > 100)
        {
            latencyHistory.erase(latencyHistory.begin());
        }
        
        // Calculate average
        float sum = 0.0f;
        for (float l : latencyHistory)
        {
            sum += l;
        }
        m_averageLatency = sum / latencyHistory.size();
    }
}
