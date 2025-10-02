#include "optimization/SmartBatching.h"
#include "optimization/DataCompression.h"
#include "utils/Logger.h"
#include <algorithm>
#include <numeric>

namespace Optimization
{
    // SmartBatching implementation
    SmartBatching::SmartBatching()
        : m_initialized(false), m_accumulatedTime(0.0f), m_currentInterval(100.0f),
          m_networkCongestion(0.0f), m_networkLatency(0.0f), m_adaptiveBatchingEnabled(true),
          m_nextBatchId(1)
    {
        m_lastBatchTime = std::chrono::high_resolution_clock::now();
        m_lastAdaptationTime = m_lastBatchTime;
        
        LOG_INFO("Smart batching system created");
    }

    SmartBatching::~SmartBatching()
    {
        Shutdown();
        LOG_INFO("Smart batching system destroyed");
    }

    bool SmartBatching::Initialize(const BatchConfig& config)
    {
        if (m_initialized)
        {
            return true;
        }

        LOG_INFO("Initializing smart batching system...");

        m_config = config;
        m_currentInterval = config.intervalMs;
        m_initialized = true;
        
        LOG_INFO("Smart batching system initialized (type: " + 
                std::to_string(static_cast<int>(config.type)) + 
                ", interval: " + std::to_string(config.intervalMs) + "ms)");
        return true;
    }

    void SmartBatching::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        LOG_INFO("Shutting down smart batching system...");
        
        // Force flush all pending messages
        ForceFlush();
        
        // Clear all data
        ClearBatches();
        m_pendingMessages.clear();
        
        m_initialized = false;
        LOG_INFO("Smart batching system shutdown complete");
    }

    void SmartBatching::AddMessage(const PrioritizedMessage& message)
    {
        if (!m_initialized)
        {
            return;
        }

        m_pendingMessages.push_back(message);
        
        // Check if we should create a batch immediately
        if (ShouldCreateBatch())
        {
            ProcessBatches(0.0f);
        }

        LOG_DEBUG("Added message to batching queue (ID: " + std::to_string(message.messageId) + 
                 ", pending: " + std::to_string(m_pendingMessages.size()) + ")");
    }

    std::vector<BatchedMessage> SmartBatching::GetReadyBatches()
    {
        std::vector<BatchedMessage> readyBatches;
        
        while (!m_readyBatches.empty())
        {
            readyBatches.push_back(m_readyBatches.front());
            m_readyBatches.pop();
        }
        
        return readyBatches;
    }

    void SmartBatching::ProcessBatches(float deltaTime)
    {
        if (!m_initialized || m_pendingMessages.empty())
        {
            return;
        }

        m_accumulatedTime += deltaTime;

        // Process based on batch type
        switch (m_config.type)
        {
            case BatchType::Interval:
                ProcessIntervalBatching(deltaTime);
                break;
            case BatchType::Size:
                ProcessSizeBatching();
                break;
            case BatchType::Priority:
                ProcessPriorityBatching();
                break;
            case BatchType::Mixed:
                ProcessMixedBatching(deltaTime);
                break;
        }

        // Clean up expired batches
        CleanupExpiredBatches();
    }

    void SmartBatching::ForceFlush()
    {
        if (!m_initialized || m_pendingMessages.empty())
        {
            return;
        }

        LOG_INFO("Force flushing " + std::to_string(m_pendingMessages.size()) + " pending messages");

        // Create batch from all pending messages
        BatchedMessage batch = CreateBatch(m_pendingMessages);
        m_readyBatches.push(batch);
        
        // Clear pending messages
        m_pendingMessages.clear();
        
        // Call callback
        if (m_batchReadyCallback)
        {
            m_batchReadyCallback(batch);
        }
    }

    void SmartBatching::ClearBatches()
    {
        while (!m_readyBatches.empty())
        {
            m_readyBatches.pop();
        }
        m_pendingMessages.clear();
    }

    size_t SmartBatching::GetPendingMessageCount() const
    {
        return m_pendingMessages.size();
    }

    size_t SmartBatching::GetPendingBatchCount() const
    {
        return m_readyBatches.size();
    }

    void SmartBatching::SetConfig(const BatchConfig& config)
    {
        m_config = config;
        m_currentInterval = config.intervalMs;
    }

    BatchConfig SmartBatching::GetConfig() const
    {
        return m_config;
    }

    void SmartBatching::SetBatchType(BatchType type)
    {
        m_config.type = type;
    }

    void SmartBatching::SetInterval(float intervalMs)
    {
        m_config.intervalMs = intervalMs;
        m_currentInterval = intervalMs;
    }

    void SmartBatching::SetMaxSize(size_t maxSize)
    {
        m_config.maxSize = maxSize;
    }

    void SmartBatching::SetMaxMessages(size_t maxMessages)
    {
        m_config.maxMessages = maxMessages;
    }

    void SmartBatching::EnableAdaptiveBatching(bool enable)
    {
        m_adaptiveBatchingEnabled = enable;
    }

    void SmartBatching::UpdateNetworkConditions(float congestionLevel, float latency)
    {
        m_networkCongestion = congestionLevel;
        m_networkLatency = latency;
        
        if (m_adaptiveBatchingEnabled)
        {
            AdjustBatchingParameters();
        }
    }

    void SmartBatching::AdjustBatchingParameters()
    {
        auto now = std::chrono::high_resolution_clock::now();
        float timeSinceLastAdaptation = std::chrono::duration<float>(now - m_lastAdaptationTime).count();
        
        if (timeSinceLastAdaptation < 1.0f) // Only adapt once per second
        {
            return;
        }
        
        CalculateOptimalInterval();
        m_lastAdaptationTime = now;
    }

    BatchStats SmartBatching::GetStats() const
    {
        return m_stats;
    }

    void SmartBatching::ResetStats()
    {
        m_stats.Reset();
    }

    void SmartBatching::PrintStats() const
    {
        LOG_INFO("=== Smart Batching Statistics ===");
        LOG_INFO("Total batches: " + std::to_string(m_stats.totalBatches));
        LOG_INFO("Total messages: " + std::to_string(m_stats.totalMessages));
        LOG_INFO("Compressed batches: " + std::to_string(m_stats.compressedBatches));
        LOG_INFO("Dropped batches: " + std::to_string(m_stats.droppedBatches));
        LOG_INFO("Total bytes: " + std::to_string(m_stats.totalBytes));
        LOG_INFO("Compressed bytes: " + std::to_string(m_stats.compressedBytes));
        LOG_INFO("Average batch size: " + std::to_string(m_stats.averageBatchSize) + " bytes");
        LOG_INFO("Average batch time: " + std::to_string(m_stats.averageBatchTime) + "ms");
        LOG_INFO("Compression ratio: " + std::to_string(m_stats.compressionRatio * 100.0f) + "%");
        LOG_INFO("Average latency: " + std::to_string(m_stats.averageLatency) + "ms");
        LOG_INFO("================================");
    }

    void SmartBatching::SetBatchReadyCallback(BatchReadyCallback callback)
    {
        m_batchReadyCallback = callback;
    }

    void SmartBatching::SetBatchDroppedCallback(BatchDroppedCallback callback)
    {
        m_batchDroppedCallback = callback;
    }

    // Internal batching methods
    void SmartBatching::ProcessIntervalBatching(float deltaTime)
    {
        if (m_accumulatedTime >= m_currentInterval)
        {
            if (!m_pendingMessages.empty())
            {
                BatchedMessage batch = CreateBatch(m_pendingMessages);
                m_readyBatches.push(batch);
                
                // Call callback
                if (m_batchReadyCallback)
                {
                    m_batchReadyCallback(batch);
                }
                
                m_pendingMessages.clear();
            }
            
            m_accumulatedTime = 0.0f;
            m_lastBatchTime = std::chrono::high_resolution_clock::now();
        }
    }

    void SmartBatching::ProcessSizeBatching()
    {
        size_t currentSize = 0;
        std::vector<PrioritizedMessage> batchMessages;
        
        for (auto it = m_pendingMessages.begin(); it != m_pendingMessages.end();)
        {
            if (currentSize + it->data.size() <= m_config.maxSize && 
                batchMessages.size() < m_config.maxMessages)
            {
                currentSize += it->data.size();
                batchMessages.push_back(*it);
                it = m_pendingMessages.erase(it);
            }
            else
            {
                break;
            }
        }
        
        if (!batchMessages.empty())
        {
            BatchedMessage batch = CreateBatch(batchMessages);
            m_readyBatches.push(batch);
            
            if (m_batchReadyCallback)
            {
                m_batchReadyCallback(batch);
            }
        }
    }

    void SmartBatching::ProcessPriorityBatching()
    {
        if (m_pendingMessages.empty())
        {
            return;
        }

        // Group messages by priority
        std::vector<std::vector<PrioritizedMessage>> priorityGroups = GroupByPriority(m_pendingMessages);
        
        // Create batches for each priority group
        for (const auto& group : priorityGroups)
        {
            if (!group.empty())
            {
                BatchedMessage batch = CreateBatch(group);
                m_readyBatches.push(batch);
                
                if (m_batchReadyCallback)
                {
                    m_batchReadyCallback(batch);
                }
            }
        }
        
        m_pendingMessages.clear();
    }

    void SmartBatching::ProcessMixedBatching(float deltaTime)
    {
        // Combine interval and size-based batching
        if (m_accumulatedTime >= m_currentInterval || 
            std::accumulate(m_pendingMessages.begin(), m_pendingMessages.end(), 0, 
                          [](size_t sum, const PrioritizedMessage& msg) { return sum + msg.data.size(); }) >= m_config.maxSize)
        {
            ProcessSizeBatching();
            m_accumulatedTime = 0.0f;
        }
    }

    BatchedMessage SmartBatching::CreateBatch(const std::vector<PrioritizedMessage>& messages)
    {
        BatchedMessage batch;
        batch.batchId = m_nextBatchId++;
        batch.messages = messages;
        batch.timestamp = std::chrono::high_resolution_clock::now();
        
        // Calculate total size
        batch.totalSize = 0;
        for (const auto& message : messages)
        {
            batch.totalSize += message.data.size();
        }
        
        // Find highest priority and urgency
        batch.highestPriority = MessagePriority::Background;
        batch.highestUrgency = MessageUrgency::Batch;
        
        for (const auto& message : messages)
        {
            if (static_cast<int>(message.classification.priority) < static_cast<int>(batch.highestPriority))
            {
                batch.highestPriority = message.classification.priority;
            }
            if (static_cast<int>(message.classification.urgency) < static_cast<int>(batch.highestUrgency))
            {
                batch.highestUrgency = message.classification.urgency;
            }
        }
        
        // Compress if enabled
        if (m_config.enableCompression)
        {
            CompressBatch(batch);
        }
        
        // Update statistics
        UpdateStatistics(batch);
        
        LOG_DEBUG("Created batch " + std::to_string(batch.batchId) + 
                 " with " + std::to_string(messages.size()) + " messages (" + 
                 std::to_string(batch.totalSize) + " bytes)");
        
        return batch;
    }

    void SmartBatching::CompressBatch(BatchedMessage& batch)
    {
        if (!m_config.enableCompression)
        {
            return;
        }

        // Serialize all messages into a single data block
        std::vector<uint8_t> batchData;
        for (const auto& message : batch.messages)
        {
            // Add message size header
            uint32_t messageSize = static_cast<uint32_t>(message.data.size());
            batchData.insert(batchData.end(), 
                           reinterpret_cast<uint8_t*>(&messageSize), 
                           reinterpret_cast<uint8_t*>(&messageSize) + 4);
            
            // Add message data
            batchData.insert(batchData.end(), message.data.begin(), message.data.end());
        }
        
        // Compress the batch data
        auto& compression = DataCompression::GetInstance();
        std::vector<uint8_t> compressedData = compression.Compress(batchData);
        
        if (compressedData.size() < batchData.size())
        {
            // Replace messages with compressed data
            batch.messages.clear();
            PrioritizedMessage compressedMessage;
            compressedMessage.messageId = batch.batchId;
            compressedMessage.type = Networking::MessageTypes::TC_BatchData;
            compressedMessage.data = compressedData;
            compressedMessage.classification.priority = batch.highestPriority;
            compressedMessage.classification.urgency = batch.highestUrgency;
            batch.messages.push_back(compressedMessage);
            
            batch.isCompressed = true;
            batch.totalSize = compressedData.size();
            
            LOG_DEBUG("Compressed batch " + std::to_string(batch.batchId) + 
                     " from " + std::to_string(batchData.size()) + " to " + 
                     std::to_string(compressedData.size()) + " bytes");
        }
    }

    void SmartBatching::UpdateBatchMetadata(BatchedMessage& batch)
    {
        // Update batch metadata based on messages
        batch.totalSize = 0;
        batch.highestPriority = MessagePriority::Background;
        batch.highestUrgency = MessageUrgency::Batch;
        
        for (const auto& message : batch.messages)
        {
            batch.totalSize += message.data.size();
            
            if (static_cast<int>(message.classification.priority) < static_cast<int>(batch.highestPriority))
            {
                batch.highestPriority = message.classification.priority;
            }
            if (static_cast<int>(message.classification.urgency) < static_cast<int>(batch.highestUrgency))
            {
                batch.highestUrgency = message.classification.urgency;
            }
        }
    }

    std::vector<std::vector<PrioritizedMessage>> SmartBatching::GroupByPriority(const std::vector<PrioritizedMessage>& messages)
    {
        std::map<MessagePriority, std::vector<PrioritizedMessage>> groups;
        
        for (const auto& message : messages)
        {
            groups[message.classification.priority].push_back(message);
        }
        
        std::vector<std::vector<PrioritizedMessage>> result;
        for (const auto& pair : groups)
        {
            result.push_back(pair.second);
        }
        
        return result;
    }

    std::vector<std::vector<PrioritizedMessage>> SmartBatching::GroupByUrgency(const std::vector<PrioritizedMessage>& messages)
    {
        std::map<MessageUrgency, std::vector<PrioritizedMessage>> groups;
        
        for (const auto& message : messages)
        {
            groups[message.classification.urgency].push_back(message);
        }
        
        std::vector<std::vector<PrioritizedMessage>> result;
        for (const auto& pair : groups)
        {
            result.push_back(pair.second);
        }
        
        return result;
    }

    std::vector<std::vector<PrioritizedMessage>> SmartBatching::GroupByType(const std::vector<PrioritizedMessage>& messages)
    {
        std::map<Networking::MessageTypes, std::vector<PrioritizedMessage>> groups;
        
        for (const auto& message : messages)
        {
            groups[message.type].push_back(message);
        }
        
        std::vector<std::vector<PrioritizedMessage>> result;
        for (const auto& pair : groups)
        {
            result.push_back(pair.second);
        }
        
        return result;
    }

    void SmartBatching::CalculateOptimalInterval()
    {
        float baseInterval = m_config.intervalMs;
        
        // Adjust based on congestion
        AdjustIntervalBasedOnCongestion(m_networkCongestion);
        
        // Adjust based on latency
        AdjustIntervalBasedOnLatency(m_networkLatency);
        
        // Clamp to valid range
        m_currentInterval = std::max(m_config.minIntervalMs, 
                                   std::min(m_config.maxIntervalMs, m_currentInterval));
        
        LOG_DEBUG("Adjusted batching interval to " + std::to_string(m_currentInterval) + "ms");
    }

    void SmartBatching::AdjustIntervalBasedOnCongestion(float congestionLevel)
    {
        if (congestionLevel > m_config.congestionThreshold)
        {
            // Increase interval to reduce network load
            m_currentInterval *= (1.0f + m_config.adaptationSpeed);
        }
        else
        {
            // Decrease interval to improve responsiveness
            m_currentInterval *= (1.0f - m_config.adaptationSpeed);
        }
    }

    void SmartBatching::AdjustIntervalBasedOnLatency(float latency)
    {
        if (latency > 100.0f) // High latency
        {
            // Increase interval to reduce packet overhead
            m_currentInterval *= (1.0f + m_config.adaptationSpeed * 0.5f);
        }
        else if (latency < 20.0f) // Low latency
        {
            // Decrease interval to improve responsiveness
            m_currentInterval *= (1.0f - m_config.adaptationSpeed * 0.5f);
        }
    }

    bool SmartBatching::ShouldCreateBatch() const
    {
        if (m_pendingMessages.empty())
        {
            return false;
        }
        
        switch (m_config.type)
        {
            case BatchType::Interval:
                return m_accumulatedTime >= m_currentInterval;
            case BatchType::Size:
                return std::accumulate(m_pendingMessages.begin(), m_pendingMessages.end(), 0,
                                     [](size_t sum, const PrioritizedMessage& msg) { return sum + msg.data.size(); }) >= m_config.maxSize;
            case BatchType::Priority:
                return true; // Always create batch for priority-based
            case BatchType::Mixed:
                return m_accumulatedTime >= m_currentInterval || 
                       std::accumulate(m_pendingMessages.begin(), m_pendingMessages.end(), 0,
                                     [](size_t sum, const PrioritizedMessage& msg) { return sum + msg.data.size(); }) >= m_config.maxSize;
        }
        
        return false;
    }

    bool SmartBatching::IsBatchReady(const BatchedMessage& batch) const
    {
        auto now = std::chrono::high_resolution_clock::now();
        float age = std::chrono::duration<float>(now - batch.timestamp).count() * 1000.0f; // Convert to ms
        
        return age >= m_config.intervalMs || batch.totalSize >= m_config.maxSize;
    }

    void SmartBatching::CleanupExpiredBatches()
    {
        // Remove expired batches from ready queue
        std::queue<BatchedMessage> validBatches;
        
        while (!m_readyBatches.empty())
        {
            BatchedMessage batch = m_readyBatches.front();
            m_readyBatches.pop();
            
            auto now = std::chrono::high_resolution_clock::now();
            float age = std::chrono::duration<float>(now - batch.timestamp).count() * 1000.0f;
            
            if (age <= m_config.maxWaitTime)
            {
                validBatches.push(batch);
            }
            else
            {
                m_stats.droppedBatches++;
                if (m_batchDroppedCallback)
                {
                    m_batchDroppedCallback(batch);
                }
            }
        }
        
        m_readyBatches = validBatches;
    }

    void SmartBatching::UpdateStatistics(const BatchedMessage& batch)
    {
        m_stats.totalBatches++;
        m_stats.totalMessages += batch.messages.size();
        m_stats.totalBytes += batch.totalSize;
        
        if (batch.isCompressed)
        {
            m_stats.compressedBatches++;
            m_stats.compressedBytes += batch.totalSize;
        }
        
        // Update averages
        m_stats.averageBatchSize = static_cast<float>(m_stats.totalBytes) / static_cast<float>(m_stats.totalBatches);
        m_stats.averageBatchTime = m_currentInterval;
        
        if (m_stats.compressedBytes > 0)
        {
            m_stats.compressionRatio = static_cast<float>(m_stats.compressedBytes) / static_cast<float>(m_stats.totalBytes);
        }
    }

    // BatchProcessor implementation
    BatchProcessor::BatchProcessor()
        : m_initialized(false), m_compressionEnabled(true), m_deduplicationEnabled(true), m_optimizationEnabled(true)
    {
        LOG_INFO("Batch processor created");
    }

    BatchProcessor::~BatchProcessor()
    {
        Shutdown();
        LOG_INFO("Batch processor destroyed");
    }

    bool BatchProcessor::Initialize()
    {
        if (m_initialized)
        {
            return true;
        }

        LOG_INFO("Initializing batch processor...");
        m_initialized = true;
        LOG_INFO("Batch processor initialized");
        return true;
    }

    void BatchProcessor::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        LOG_INFO("Shutting down batch processor...");
        m_initialized = false;
        LOG_INFO("Batch processor shutdown complete");
    }

    std::vector<uint8_t> BatchProcessor::ProcessMovementBatch(const std::vector<PrioritizedMessage>& messages)
    {
        // Optimize movement messages
        std::vector<PrioritizedMessage> optimizedMessages = messages;
        if (m_optimizationEnabled)
        {
            optimizedMessages = OptimizeBatch(messages);
        }
        
        return SerializeMessages(optimizedMessages);
    }

    std::vector<uint8_t> BatchProcessor::ProcessCombatBatch(const std::vector<PrioritizedMessage>& messages)
    {
        // Combat messages need immediate processing, no optimization
        return SerializeMessages(messages);
    }

    std::vector<uint8_t> BatchProcessor::ProcessInventoryBatch(const std::vector<PrioritizedMessage>& messages)
    {
        // Deduplicate inventory messages
        std::vector<PrioritizedMessage> processedMessages = messages;
        if (m_deduplicationEnabled)
        {
            processedMessages = DeduplicateMessages(messages);
        }
        
        return SerializeMessages(processedMessages);
    }

    std::vector<uint8_t> BatchProcessor::ProcessEffectBatch(const std::vector<PrioritizedMessage>& messages)
    {
        // Compress similar effect messages
        std::vector<PrioritizedMessage> processedMessages = messages;
        if (m_optimizationEnabled)
        {
            processedMessages = CompressSimilarMessages(messages);
        }
        
        return SerializeMessages(processedMessages);
    }

    std::vector<uint8_t> BatchProcessor::ProcessGenericBatch(const std::vector<PrioritizedMessage>& messages)
    {
        return SerializeMessages(messages);
    }

    std::vector<PrioritizedMessage> BatchProcessor::OptimizeBatch(const std::vector<PrioritizedMessage>& messages)
    {
        if (!m_optimizationEnabled)
        {
            return messages;
        }

        std::vector<PrioritizedMessage> optimized = messages;
        
        // Sort by priority and type
        std::sort(optimized.begin(), optimized.end(), [](const PrioritizedMessage& a, const PrioritizedMessage& b) {
            if (a.classification.priority != b.classification.priority)
            {
                return static_cast<int>(a.classification.priority) < static_cast<int>(b.classification.priority);
            }
            return a.type < b.type;
        });
        
        return optimized;
    }

    std::vector<PrioritizedMessage> BatchProcessor::DeduplicateMessages(const std::vector<PrioritizedMessage>& messages)
    {
        if (!m_deduplicationEnabled)
        {
            return messages;
        }

        std::vector<PrioritizedMessage> deduplicated;
        std::map<uint32_t, PrioritizedMessage> messageMap;
        
        for (const auto& message : messages)
        {
            // Use message ID as key for deduplication
            if (messageMap.find(message.messageId) == messageMap.end())
            {
                messageMap[message.messageId] = message;
                deduplicated.push_back(message);
            }
        }
        
        return deduplicated;
    }

    std::vector<PrioritizedMessage> BatchProcessor::CompressSimilarMessages(const std::vector<PrioritizedMessage>& messages)
    {
        if (!m_optimizationEnabled)
        {
            return messages;
        }

        std::vector<PrioritizedMessage> compressed;
        
        for (size_t i = 0; i < messages.size(); ++i)
        {
            bool merged = false;
            
            for (size_t j = 0; j < compressed.size(); ++j)
            {
                if (AreMessagesSimilar(messages[i], compressed[j]))
                {
                    compressed[j] = MergeSimilarMessages(compressed[j], messages[i]);
                    merged = true;
                    break;
                }
            }
            
            if (!merged)
            {
                compressed.push_back(messages[i]);
            }
        }
        
        return compressed;
    }

    void BatchProcessor::SetCompressionEnabled(bool enabled)
    {
        m_compressionEnabled = enabled;
    }

    void BatchProcessor::SetDeduplicationEnabled(bool enabled)
    {
        m_deduplicationEnabled = enabled;
    }

    void BatchProcessor::SetOptimizationEnabled(bool enabled)
    {
        m_optimizationEnabled = enabled;
    }

    std::vector<uint8_t> BatchProcessor::SerializeMessages(const std::vector<PrioritizedMessage>& messages)
    {
        std::vector<uint8_t> data;
        
        // Add message count
        uint32_t messageCount = static_cast<uint32_t>(messages.size());
        data.insert(data.end(), reinterpret_cast<uint8_t*>(&messageCount), 
                   reinterpret_cast<uint8_t*>(&messageCount) + 4);
        
        // Serialize each message
        for (const auto& message : messages)
        {
            // Message ID
            data.insert(data.end(), reinterpret_cast<const uint8_t*>(&message.messageId), 
                       reinterpret_cast<const uint8_t*>(&message.messageId) + 4);
            
            // Message type
            uint32_t messageType = static_cast<uint32_t>(message.type);
            data.insert(data.end(), reinterpret_cast<uint8_t*>(&messageType), 
                       reinterpret_cast<uint8_t*>(&messageType) + 4);
            
            // Data size
            uint32_t dataSize = static_cast<uint32_t>(message.data.size());
            data.insert(data.end(), reinterpret_cast<uint8_t*>(&dataSize), 
                       reinterpret_cast<uint8_t*>(&dataSize) + 4);
            
            // Data
            data.insert(data.end(), message.data.begin(), message.data.end());
        }
        
        return data;
    }

    std::vector<PrioritizedMessage> BatchProcessor::DeserializeMessages(const std::vector<uint8_t>& data)
    {
        std::vector<PrioritizedMessage> messages;
        
        if (data.size() < 4)
        {
            return messages;
        }
        
        size_t offset = 0;
        
        // Read message count
        uint32_t messageCount;
        std::memcpy(&messageCount, &data[offset], 4);
        offset += 4;
        
        messages.reserve(messageCount);
        
        // Deserialize each message
        for (uint32_t i = 0; i < messageCount; ++i)
        {
            if (offset + 12 > data.size()) // ID + type + dataSize
            {
                break;
            }
            
            PrioritizedMessage message;
            
            // Message ID
            std::memcpy(&message.messageId, &data[offset], 4);
            offset += 4;
            
            // Message type
            uint32_t messageType;
            std::memcpy(&messageType, &data[offset], 4);
            offset += 4;
            message.type = static_cast<Networking::MessageTypes>(messageType);
            
            // Data size
            uint32_t dataSize;
            std::memcpy(&dataSize, &data[offset], 4);
            offset += 4;
            
            if (offset + dataSize > data.size())
            {
                break;
            }
            
            // Data
            message.data.assign(data.begin() + offset, data.begin() + offset + dataSize);
            offset += dataSize;
            
            messages.push_back(message);
        }
        
        return messages;
    }

    bool BatchProcessor::AreMessagesSimilar(const PrioritizedMessage& a, const PrioritizedMessage& b)
    {
        // Messages are similar if they have the same type and similar data
        return a.type == b.type && a.data.size() == b.data.size() && 
               std::equal(a.data.begin(), a.data.end(), b.data.begin());
    }

    PrioritizedMessage BatchProcessor::MergeSimilarMessages(const PrioritizedMessage& a, const PrioritizedMessage& b)
    {
        // For now, just return the first message
        // In a real implementation, this would merge the data appropriately
        return a;
    }

    // Batch utilities implementation
    namespace BatchUtils
    {
        BatchedMessage CreateEmptyBatch()
        {
            BatchedMessage batch;
            batch.batchId = 0;
            batch.timestamp = std::chrono::high_resolution_clock::now();
            batch.totalSize = 0;
            batch.highestPriority = MessagePriority::Background;
            batch.highestUrgency = MessageUrgency::Batch;
            batch.isCompressed = false;
            return batch;
        }

        BatchedMessage CreateBatchFromMessages(const std::vector<PrioritizedMessage>& messages)
        {
            BatchedMessage batch = CreateEmptyBatch();
            batch.messages = messages;
            
            // Calculate metadata
            batch.totalSize = 0;
            for (const auto& message : messages)
            {
                batch.totalSize += message.data.size();
                
                if (static_cast<int>(message.classification.priority) < static_cast<int>(batch.highestPriority))
                {
                    batch.highestPriority = message.classification.priority;
                }
                if (static_cast<int>(message.classification.urgency) < static_cast<int>(batch.highestUrgency))
                {
                    batch.highestUrgency = message.classification.urgency;
                }
            }
            
            return batch;
        }

        float CalculateBatchEfficiency(const BatchedMessage& batch)
        {
            if (batch.messages.empty())
            {
                return 0.0f;
            }
            
            // Efficiency based on compression ratio and message count
            float compressionRatio = batch.isCompressed ? 0.5f : 1.0f;
            float messageCountFactor = std::min(static_cast<float>(batch.messages.size()) / 10.0f, 1.0f);
            
            return compressionRatio * messageCountFactor;
        }

        float CalculateCompressionRatio(const BatchedMessage& batch)
        {
            if (!batch.isCompressed)
            {
                return 1.0f;
            }
            
            // This would need to be calculated based on original vs compressed size
            return 0.5f; // Placeholder
        }

        size_t CalculateBatchSize(const BatchedMessage& batch)
        {
            return batch.totalSize;
        }

        std::vector<PrioritizedMessage> SortMessagesForBatching(const std::vector<PrioritizedMessage>& messages)
        {
            std::vector<PrioritizedMessage> sorted = messages;
            
            std::sort(sorted.begin(), sorted.end(), [](const PrioritizedMessage& a, const PrioritizedMessage& b) {
                // Sort by priority first, then by urgency, then by type
                if (a.classification.priority != b.classification.priority)
                {
                    return static_cast<int>(a.classification.priority) < static_cast<int>(b.classification.priority);
                }
                if (a.classification.urgency != b.classification.urgency)
                {
                    return static_cast<int>(a.classification.urgency) < static_cast<int>(b.classification.urgency);
                }
                return a.type < b.type;
            });
            
            return sorted;
        }

        std::vector<PrioritizedMessage> FilterMessagesForBatching(const std::vector<PrioritizedMessage>& messages)
        {
            std::vector<PrioritizedMessage> filtered;
            
            for (const auto& message : messages)
            {
                // Filter out messages that shouldn't be batched
                if (message.classification.urgency == MessageUrgency::Immediate)
                {
                    continue; // Don't batch immediate messages
                }
                
                filtered.push_back(message);
            }
            
            return filtered;
        }

        bool ValidateBatch(const BatchedMessage& batch)
        {
            return !batch.messages.empty() && batch.totalSize > 0;
        }

        bool IsBatchComplete(const BatchedMessage& batch)
        {
            return !batch.messages.empty();
        }

        bool IsBatchExpired(const BatchedMessage& batch, float maxAge)
        {
            auto now = std::chrono::high_resolution_clock::now();
            float age = std::chrono::duration<float>(now - batch.timestamp).count() * 1000.0f; // Convert to ms
            return age > maxAge;
        }

        void PrintBatchAnalysis(const BatchedMessage& batch)
        {
            LOG_INFO("=== Batch Analysis ===");
            LOG_INFO("Batch ID: " + std::to_string(batch.batchId));
            LOG_INFO("Message count: " + std::to_string(batch.messages.size()));
            LOG_INFO("Total size: " + std::to_string(batch.totalSize) + " bytes");
            LOG_INFO("Highest priority: " + std::to_string(static_cast<int>(batch.highestPriority)));
            LOG_INFO("Highest urgency: " + std::to_string(static_cast<int>(batch.highestUrgency)));
            LOG_INFO("Compressed: " + (batch.isCompressed ? "Yes" : "No"));
            LOG_INFO("Efficiency: " + std::to_string(CalculateBatchEfficiency(batch) * 100.0f) + "%");
            LOG_INFO("===================");
        }

        void PrintBatchStatistics(const std::vector<BatchedMessage>& batches)
        {
            if (batches.empty())
            {
                LOG_INFO("No batches to analyze");
                return;
            }
            
            size_t totalMessages = 0;
            size_t totalBytes = 0;
            int compressedBatches = 0;
            
            for (const auto& batch : batches)
            {
                totalMessages += batch.messages.size();
                totalBytes += batch.totalSize;
                if (batch.isCompressed)
                {
                    compressedBatches++;
                }
            }
            
            LOG_INFO("=== Batch Statistics ===");
            LOG_INFO("Total batches: " + std::to_string(batches.size()));
            LOG_INFO("Total messages: " + std::to_string(totalMessages));
            LOG_INFO("Total bytes: " + std::to_string(totalBytes));
            LOG_INFO("Compressed batches: " + std::to_string(compressedBatches));
            LOG_INFO("Average messages per batch: " + std::to_string(static_cast<float>(totalMessages) / batches.size()));
            LOG_INFO("Average bytes per batch: " + std::to_string(static_cast<float>(totalBytes) / batches.size()));
            LOG_INFO("=========================");
        }
    }
}
