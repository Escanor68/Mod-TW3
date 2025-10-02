#pragma once

#include "Common.h"
#include "networking/MessageTypes.h"
#include "optimization/DataCompression.h"
#include "optimization/MovementPrediction.h"
#include "optimization/MessagePrioritySystem.h"
#include "optimization/SmartBatching.h"
#include <vector>
#include <map>
#include <queue>
#include <chrono>
#include <functional>

namespace Optimization
{
    // Network protocol version
    constexpr uint32_t PROTOCOL_VERSION = 1;
    constexpr uint32_t MAX_PACKET_SIZE = 1024 * 1024; // 1MB
    constexpr uint32_t MAX_MESSAGES_PER_PACKET = 100;

    // Packet types
    enum class PacketType : uint8_t
    {
        Data = 0,
        Acknowledgment = 1,
        Heartbeat = 2,
        Compression = 3,
        Batch = 4,
        Control = 5
    };

    // Packet header
    struct PacketHeader
    {
        uint32_t magic;           // Magic number for packet identification
        uint32_t version;         // Protocol version
        uint32_t packetId;        // Unique packet identifier
        uint32_t sequenceNumber;  // Sequence number for ordering
        uint32_t timestamp;       // Packet timestamp
        PacketType type;          // Packet type
        uint16_t flags;           // Packet flags
        uint32_t dataSize;        // Size of packet data
        uint32_t checksum;        // Packet checksum
        
        PacketHeader() : magic(0xDEADBEEF), version(PROTOCOL_VERSION), packetId(0), 
                        sequenceNumber(0), timestamp(0), type(PacketType::Data), 
                        flags(0), dataSize(0), checksum(0) {}
    };

    // Packet flags
    enum class PacketFlags : uint16_t
    {
        None = 0,
        Compressed = 1 << 0,
        Encrypted = 1 << 1,
        Reliable = 1 << 2,
        Ordered = 1 << 3,
        Fragmented = 1 << 4,
        LastFragment = 1 << 5,
        RequiresAck = 1 << 6,
        HighPriority = 1 << 7,
        Batch = 1 << 8
    };

    // Network packet
    struct NetworkPacket
    {
        PacketHeader header;
        std::vector<uint8_t> data;
        std::chrono::high_resolution_clock::time_point sendTime;
        std::chrono::high_resolution_clock::time_point ackTime;
        uint32_t retryCount;
        bool isAcknowledged;
        
        NetworkPacket() : retryCount(0), isAcknowledged(false) {}
    };

    // Network statistics
    struct NetworkStats
    {
        uint32_t packetsSent = 0;
        uint32_t packetsReceived = 0;
        uint32_t packetsLost = 0;
        uint32_t packetsRetransmitted = 0;
        uint32_t bytesSent = 0;
        uint32_t bytesReceived = 0;
        float averageLatency = 0.0f;
        float maxLatency = 0.0f;
        float packetLossRate = 0.0f;
        float throughput = 0.0f;
        uint32_t compressionRatio = 0;
        uint32_t batchEfficiency = 0;
        
        void Reset()
        {
            packetsSent = 0;
            packetsReceived = 0;
            packetsLost = 0;
            packetsRetransmitted = 0;
            bytesSent = 0;
            bytesReceived = 0;
            averageLatency = 0.0f;
            maxLatency = 0.0f;
            packetLossRate = 0.0f;
            throughput = 0.0f;
            compressionRatio = 0;
            batchEfficiency = 0;
        }
    };

    // Optimized network protocol
    class OptimizedNetworkProtocol
    {
    public:
        OptimizedNetworkProtocol();
        ~OptimizedNetworkProtocol();

        // Initialize protocol
        bool Initialize();
        void Shutdown();

        // Message processing
        void SendMessage(const PrioritizedMessage& message);
        void SendMessages(const std::vector<PrioritizedMessage>& messages);
        std::vector<PrioritizedMessage> ProcessReceivedPacket(const NetworkPacket& packet);
        
        // Packet management
        std::vector<NetworkPacket> GetPacketsToSend();
        void MarkPacketSent(uint32_t packetId);
        void MarkPacketAcknowledged(uint32_t packetId);
        void MarkPacketLost(uint32_t packetId);
        
        // Protocol features
        void EnableCompression(bool enable);
        void EnableBatching(bool enable);
        void EnablePrediction(bool enable);
        void EnablePrioritySystem(bool enable);
        
        // Configuration
        void SetCompressionLevel(CompressionLevel level);
        void SetBatchConfig(const BatchConfig& config);
        void SetPriorityConfig(const std::map<Networking::MessageTypes, MessageClassification>& classifications);
        void SetPredictionConfig(const PredictionConfig& config);
        
        // Network optimization
        void UpdateNetworkConditions(float latency, float packetLoss, float bandwidth);
        void OptimizeForLatency();
        void OptimizeForThroughput();
        void OptimizeForReliability();
        
        // Statistics
        NetworkStats GetStats() const;
        void ResetStats();
        void PrintStats() const;
        
        // Callbacks
        using MessageReceivedCallback = std::function<void(const PrioritizedMessage&)>;
        using PacketSentCallback = std::function<void(const NetworkPacket&)>;
        using PacketLostCallback = std::function<void(uint32_t packetId)>;
        
        void SetMessageReceivedCallback(MessageReceivedCallback callback);
        void SetPacketSentCallback(PacketSentCallback callback);
        void SetPacketLostCallback(PacketLostCallback callback);

    private:
        // Internal processing methods
        NetworkPacket CreatePacket(const std::vector<PrioritizedMessage>& messages);
        std::vector<NetworkPacket> CreatePackets(const std::vector<PrioritizedMessage>& messages);
        std::vector<PrioritizedMessage> ExtractMessages(const NetworkPacket& packet);
        
        // Packet optimization
        void OptimizePacket(NetworkPacket& packet);
        void CompressPacket(NetworkPacket& packet);
        void BatchMessages(std::vector<PrioritizedMessage>& messages);
        void PrioritizeMessages(std::vector<PrioritizedMessage>& messages);
        
        // Packet validation
        bool ValidatePacket(const NetworkPacket& packet) const;
        uint32_t CalculateChecksum(const std::vector<uint8_t>& data) const;
        bool VerifyChecksum(const NetworkPacket& packet) const;
        
        // Retransmission
        void HandleRetransmission();
        void ScheduleRetransmission(const NetworkPacket& packet);
        void CancelRetransmission(uint32_t packetId);
        
        // Fragmentation
        std::vector<NetworkPacket> FragmentPacket(const NetworkPacket& packet);
        NetworkPacket DefragmentPacket(const std::vector<NetworkPacket>& fragments);
        
        // Member variables
        bool m_initialized;
        NetworkStats m_stats;
        
        // Protocol components
        std::unique_ptr<DataCompression> m_compression;
        std::unique_ptr<SmartBatching> m_batching;
        std::unique_ptr<NetworkTrafficManager> m_trafficManager;
        std::unique_ptr<MovementPredictionManager> m_predictionManager;
        
        // Packet management
        std::queue<NetworkPacket> m_sendQueue;
        std::map<uint32_t, NetworkPacket> m_pendingPackets;
        std::map<uint32_t, std::vector<NetworkPacket>> m_fragments;
        uint32_t m_nextPacketId;
        uint32_t m_nextSequenceNumber;
        
        // Configuration
        bool m_compressionEnabled;
        bool m_batchingEnabled;
        bool m_predictionEnabled;
        bool m_priorityEnabled;
        
        CompressionLevel m_compressionLevel;
        BatchConfig m_batchConfig;
        PredictionConfig m_predictionConfig;
        
        // Network conditions
        float m_currentLatency;
        float m_currentPacketLoss;
        float m_currentBandwidth;
        
        // Timing
        std::chrono::high_resolution_clock::time_point m_lastUpdateTime;
        std::chrono::high_resolution_clock::time_point m_lastStatsUpdate;
        
        // Callbacks
        MessageReceivedCallback m_messageReceivedCallback;
        PacketSentCallback m_packetSentCallback;
        PacketLostCallback m_packetLostCallback;
    };

    // Network protocol utilities
    namespace ProtocolUtils
    {
        // Packet creation
        NetworkPacket CreateDataPacket(const std::vector<uint8_t>& data, uint32_t packetId);
        NetworkPacket CreateAckPacket(uint32_t ackedPacketId, uint32_t packetId);
        NetworkPacket CreateHeartbeatPacket(uint32_t packetId);
        
        // Packet serialization
        std::vector<uint8_t> SerializePacket(const NetworkPacket& packet);
        NetworkPacket DeserializePacket(const std::vector<uint8_t>& data);
        
        // Packet validation
        bool IsValidPacket(const NetworkPacket& packet);
        bool IsPacketExpired(const NetworkPacket& packet, float maxAge);
        
        // Packet optimization
        void OptimizePacketSize(NetworkPacket& packet);
        void OptimizePacketOrder(std::vector<NetworkPacket>& packets);
        
        // Statistics
        void PrintPacketAnalysis(const NetworkPacket& packet);
        void PrintProtocolStatistics(const NetworkStats& stats);
    }
}
