#pragma once

#include "Common.h"
#include "networking/net_message.h"
#include "networking/MessageTypes.h"
#include <vector>
#include <chrono>

namespace Optimization
{
    // Message priorities for network optimization
    enum class MessagePriority : uint8_t
    {
        Critical = 0,   // Combat, immediate player actions
        High = 1,       // Movement, health updates
        Medium = 2,     // Inventory, quest updates
        Low = 3,        // Visual FX, chat messages
        VeryLow = 4     // Background updates, telemetry
    };

    // Message flags for compression and batching
    enum MessageFlags : uint32_t
    {
        MESSAGE_FLAG_NONE = 0,
        MESSAGE_FLAG_COMPRESSED = 1 << 0,
        MESSAGE_FLAG_BATCHED = 1 << 1,
        MESSAGE_FLAG_PRIORITY = 1 << 2,
        MESSAGE_FLAG_RELIABLE = 1 << 3
    };

    // Special message types
    enum SpecialMessageTypes : uint32_t
    {
        BATCH_MESSAGE_TYPE = 0xFFFF
    };

    // Message batch structure
    struct MessageBatch
    {
        MessagePriority priority;
        uint64_t timestamp;
        std::vector<Networking::message<Networking::MessageTypes>> messages;
        
        MessageBatch() : priority(MessagePriority::Medium), timestamp(0) {}
    };

    // Network statistics
    struct NetworkStats
    {
        size_t totalMessagesSent = 0;
        size_t totalMessagesReceived = 0;
        size_t totalBytesSent = 0;
        size_t totalBytesReceived = 0;
        size_t totalBatchesSent = 0;
        float compressionRatio = 0.0f;
        float averageLatency = 0.0f;
        
        void UpdateCompressionRatio(size_t originalSize, size_t compressedSize)
        {
            if (originalSize > 0)
            {
                compressionRatio = static_cast<float>(compressedSize) / static_cast<float>(originalSize);
            }
        }
    };

    // Main network optimization class
    class NetworkOptimizer
    {
    public:
        NetworkOptimizer();
        virtual ~NetworkOptimizer();

        // Initialization
        bool Initialize();
        void Shutdown();

        // Message compression/decompression
        Networking::message<Networking::MessageTypes> CompressMessage(const Networking::message<Networking::MessageTypes>& originalMessage);
        Networking::message<Networking::MessageTypes> DecompressMessage(const Networking::message<Networking::MessageTypes>& compressedMessage);

        // Message batching
        void AddMessageToBatch(const Networking::message<Networking::MessageTypes>& message, MessagePriority priority);
        void FlushBatches();
        void ProcessIncomingMessage(const Networking::message<Networking::MessageTypes>& message);

        // Message prioritization
        MessagePriority GetMessagePriority(Networking::MessageTypes messageType);
        void OptimizeMessage(Networking::message<Networking::MessageTypes>& message);

        // Configuration
        void SetCompressionEnabled(bool enabled);
        void SetBatchingEnabled(bool enabled);
        void SetPriorityEnabled(bool enabled);
        void SetMaxBatchSize(size_t maxSize);
        void SetBatchTimeout(uint32_t timeoutMs);

        // Statistics
        NetworkStats GetStats() const;
        void ResetStats();
        void PrintStats() const;

        // Legacy methods for backward compatibility
        std::vector<uint8_t> ApplyCompression(const std::vector<uint8_t>& data);
        std::vector<uint8_t> ApplyDecompression(const std::vector<uint8_t>& data);

    private:
        // Internal methods
        void ProcessPriorityQueue(std::vector<Networking::message<Networking::MessageTypes>>& queue, MessagePriority priority);
        MessageBatch CompressBatch(const MessageBatch& batch);
        void SendBatch(const MessageBatch& batch);
        void SendMessageImmediate(const Networking::message<Networking::MessageTypes>& message);
        
        void ProcessBatchedMessage(const Networking::message<Networking::MessageTypes>& message);
        void ProcessSingleMessage(const Networking::message<Networking::MessageTypes>& message);
        std::vector<Networking::message<Networking::MessageTypes>> ExtractMessagesFromBatch(const Networking::message<Networking::MessageTypes>& batchMessage);
        
        bool ShouldFlushBatch();
        uint64_t GetCurrentTimeMs();

        // Message queues for batching
        std::vector<Networking::message<Networking::MessageTypes>> m_highPriorityQueue;
        std::vector<Networking::message<Networking::MessageTypes>> m_mediumPriorityQueue;
        std::vector<Networking::message<Networking::MessageTypes>> m_lowPriorityQueue;

        // Pending batches
        std::vector<MessageBatch> m_pendingBatches;

        // Configuration
        bool m_initialized;
        bool m_compressionEnabled;
        bool m_batchingEnabled;
        bool m_priorityEnabled;
        size_t m_maxBatchSize;
        uint32_t m_batchTimeout;
        uint64_t m_lastBatchTime;

        // Statistics
        NetworkStats m_stats;
    };
}
