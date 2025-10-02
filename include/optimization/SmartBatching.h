#pragma once

#include "Common.h"
#include "networking/MessageTypes.h"
#include "optimization/MessagePrioritySystem.h"
#include <vector>
#include <map>
#include <queue>
#include <chrono>
#include <functional>

namespace Optimization
{
    // Batch types
    enum class BatchType
    {
        Interval = 0,        // Time-based batching (every X ms)
        Size = 1,            // Size-based batching (every X bytes)
        Priority = 2,        // Priority-based batching (group by priority)
        Mixed = 3            // Mixed batching strategy
    };

    // Batch configuration
    struct BatchConfig
    {
        BatchConfig() : type(BatchType::Interval), intervalMs(100.0f), maxSize(1024), maxMessages(50),
                       enableCompression(true), enablePriorityGrouping(true), maxWaitTime(500.0f),
                       enableAdaptiveBatching(true), minIntervalMs(16.0f), maxIntervalMs(200.0f),
                       congestionThreshold(0.8f), adaptationSpeed(0.1f) {}
        
        BatchType type;
        float intervalMs;           // Interval for time-based batching
        size_t maxSize;             // Maximum size for size-based batching
        size_t maxMessages;         // Maximum messages per batch
        bool enableCompression;     // Enable compression for batches
        bool enablePriorityGrouping; // Group messages by priority
        float maxWaitTime;          // Maximum time to wait before sending
        bool enableAdaptiveBatching; // Adaptive batching based on network conditions
        
        // Adaptive batching parameters
        float minIntervalMs;        // Minimum interval (60 FPS)
        float maxIntervalMs;        // Maximum interval
        float congestionThreshold;  // Network congestion threshold
        float adaptationSpeed;      // How fast to adapt to network conditions
    };

    // Batched message
    struct BatchedMessage
    {
        uint32_t batchId;
        std::vector<PrioritizedMessage> messages;
        std::chrono::high_resolution_clock::time_point timestamp;
        size_t totalSize;
        MessagePriority highestPriority;
        MessageUrgency highestUrgency;
        bool isCompressed;
        
        BatchedMessage() : batchId(0), totalSize(0), highestPriority(MessagePriority::Background),
                          highestUrgency(MessageUrgency::Batch), isCompressed(false) {}
    };

    // Batch statistics
    struct BatchStats
    {
        BatchStats() : totalBatches(0), totalMessages(0), compressedBatches(0), droppedBatches(0),
                      totalBytes(0), compressedBytes(0), averageBatchSize(0.0f), averageBatchTime(0.0f),
                      compressionRatio(0.0f), averageLatency(0.0f) {}
        
        uint32_t totalBatches;
        uint32_t totalMessages;
        uint32_t compressedBatches;
        uint32_t droppedBatches;
        size_t totalBytes;
        size_t compressedBytes;
        float averageBatchSize;
        float averageBatchTime;
        float compressionRatio;
        float averageLatency;
        
        void Reset()
        {
            totalBatches = 0;
            totalMessages = 0;
            compressedBatches = 0;
            droppedBatches = 0;
            totalBytes = 0;
            compressedBytes = 0;
            averageBatchSize = 0.0f;
            averageBatchTime = 0.0f;
            compressionRatio = 0.0f;
            averageLatency = 0.0f;
        }
    };

    // Smart batching system
    class SmartBatching
    {
    public:
        SmartBatching();
        ~SmartBatching();

        // Initialize batching system
        bool Initialize(const BatchConfig& config = BatchConfig());
        void Shutdown();

        // Message batching
        void AddMessage(const PrioritizedMessage& message);
        std::vector<BatchedMessage> GetReadyBatches();
        void ProcessBatches(float deltaTime);

        // Batch management
        void ForceFlush();  // Force flush all pending messages
        void ClearBatches();
        size_t GetPendingMessageCount() const;
        size_t GetPendingBatchCount() const;

        // Configuration
        void SetConfig(const BatchConfig& config);
        BatchConfig GetConfig() const;
        void SetBatchType(BatchType type);
        void SetInterval(float intervalMs);
        void SetMaxSize(size_t maxSize);
        void SetMaxMessages(size_t maxMessages);

        // Adaptive batching
        void EnableAdaptiveBatching(bool enable);
        void UpdateNetworkConditions(float congestionLevel, float latency);
        void AdjustBatchingParameters();

        // Statistics
        BatchStats GetStats() const;
        void ResetStats();
        void PrintStats() const;

        // Callbacks
        using BatchReadyCallback = std::function<void(const BatchedMessage&)>;
        using BatchDroppedCallback = std::function<void(const BatchedMessage&)>;
        
        void SetBatchReadyCallback(BatchReadyCallback callback);
        void SetBatchDroppedCallback(BatchDroppedCallback callback);

    private:
        // Internal batching methods
        void ProcessIntervalBatching(float deltaTime);
        void ProcessSizeBatching();
        void ProcessPriorityBatching();
        void ProcessMixedBatching(float deltaTime);
        
        // Batch creation
        BatchedMessage CreateBatch(const std::vector<PrioritizedMessage>& messages);
        void CompressBatch(BatchedMessage& batch);
        void UpdateBatchMetadata(BatchedMessage& batch);
        
        // Message grouping
        std::vector<std::vector<PrioritizedMessage>> GroupByPriority(const std::vector<PrioritizedMessage>& messages);
        std::vector<std::vector<PrioritizedMessage>> GroupByUrgency(const std::vector<PrioritizedMessage>& messages);
        std::vector<std::vector<PrioritizedMessage>> GroupByType(const std::vector<PrioritizedMessage>& messages);
        
        // Adaptive batching
        void CalculateOptimalInterval();
        void AdjustIntervalBasedOnCongestion(float congestionLevel);
        void AdjustIntervalBasedOnLatency(float latency);
        
        // Utility methods
        bool ShouldCreateBatch() const;
        bool IsBatchReady(const BatchedMessage& batch) const;
        void CleanupExpiredBatches();
        void UpdateStatistics(const BatchedMessage& batch);

        // Member variables
        bool m_initialized;
        BatchConfig m_config;
        BatchStats m_stats;
        
        // Pending messages
        std::vector<PrioritizedMessage> m_pendingMessages;
        std::queue<BatchedMessage> m_readyBatches;
        
        // Timing
        std::chrono::high_resolution_clock::time_point m_lastBatchTime;
        std::chrono::high_resolution_clock::time_point m_lastAdaptationTime;
        float m_accumulatedTime;
        
        // Adaptive batching
        float m_currentInterval;
        float m_networkCongestion;
        float m_networkLatency;
        bool m_adaptiveBatchingEnabled;
        
        // Callbacks
        BatchReadyCallback m_batchReadyCallback;
        BatchDroppedCallback m_batchDroppedCallback;
        
        // Batch ID counter
        uint32_t m_nextBatchId;
    };

    // Batch processor for different message types
    class BatchProcessor
    {
    public:
        BatchProcessor();
        ~BatchProcessor();

        // Initialize processor
        bool Initialize();
        void Shutdown();

        // Process different types of batches
        std::vector<uint8_t> ProcessMovementBatch(const std::vector<PrioritizedMessage>& messages);
        std::vector<uint8_t> ProcessCombatBatch(const std::vector<PrioritizedMessage>& messages);
        std::vector<uint8_t> ProcessInventoryBatch(const std::vector<PrioritizedMessage>& messages);
        std::vector<uint8_t> ProcessEffectBatch(const std::vector<PrioritizedMessage>& messages);
        std::vector<uint8_t> ProcessGenericBatch(const std::vector<PrioritizedMessage>& messages);

        // Batch optimization
        std::vector<PrioritizedMessage> OptimizeBatch(const std::vector<PrioritizedMessage>& messages);
        std::vector<PrioritizedMessage> DeduplicateMessages(const std::vector<PrioritizedMessage>& messages);
        std::vector<PrioritizedMessage> CompressSimilarMessages(const std::vector<PrioritizedMessage>& messages);

        // Configuration
        void SetCompressionEnabled(bool enabled);
        void SetDeduplicationEnabled(bool enabled);
        void SetOptimizationEnabled(bool enabled);

    private:
        // Internal processing methods
        std::vector<uint8_t> SerializeMessages(const std::vector<PrioritizedMessage>& messages);
        std::vector<PrioritizedMessage> DeserializeMessages(const std::vector<uint8_t>& data);
        
        // Message optimization
        bool AreMessagesSimilar(const PrioritizedMessage& a, const PrioritizedMessage& b);
        PrioritizedMessage MergeSimilarMessages(const PrioritizedMessage& a, const PrioritizedMessage& b);
        
        // Member variables
        bool m_initialized;
        bool m_compressionEnabled;
        bool m_deduplicationEnabled;
        bool m_optimizationEnabled;
    };

    // Batch utilities
    namespace BatchUtils
    {
        // Batch creation helpers
        BatchedMessage CreateEmptyBatch();
        BatchedMessage CreateBatchFromMessages(const std::vector<PrioritizedMessage>& messages);
        
        // Batch analysis
        float CalculateBatchEfficiency(const BatchedMessage& batch);
        float CalculateCompressionRatio(const BatchedMessage& batch);
        size_t CalculateBatchSize(const BatchedMessage& batch);
        
        // Batch optimization
        std::vector<PrioritizedMessage> SortMessagesForBatching(const std::vector<PrioritizedMessage>& messages);
        std::vector<PrioritizedMessage> FilterMessagesForBatching(const std::vector<PrioritizedMessage>& messages);
        
        // Batch validation
        bool ValidateBatch(const BatchedMessage& batch);
        bool IsBatchComplete(const BatchedMessage& batch);
        bool IsBatchExpired(const BatchedMessage& batch, float maxAge);
        
        // Statistics
        void PrintBatchAnalysis(const BatchedMessage& batch);
        void PrintBatchStatistics(const std::vector<BatchedMessage>& batches);
    }
}
