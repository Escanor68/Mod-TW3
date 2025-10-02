#include "optimization/NetworkOptimizer.h"
#include "optimization/DataCompression.h"
#include "utils/Logger.h"
#include <algorithm>
#include <chrono>

namespace Optimization
{
    NetworkOptimizer::NetworkOptimizer()
        : m_initialized(false), m_compressionEnabled(true), m_batchingEnabled(true),
          m_priorityEnabled(true), m_maxBatchSize(1024), m_batchTimeout(50) // 50ms
    {
        LOG_INFO("NetworkOptimizer created");
    }

    NetworkOptimizer::~NetworkOptimizer()
    {
        Shutdown();
        LOG_INFO("NetworkOptimizer destroyed");
    }

    bool NetworkOptimizer::Initialize()
    {
        if (m_initialized)
        {
            return true;
        }

        LOG_INFO("Initializing NetworkOptimizer...");

        // Initialize compression system
        auto& compression = DataCompression::GetInstance();
        if (!compression.Initialize())
        {
            LOG_ERROR("Failed to initialize compression system");
            return false;
        }

        // Initialize message queues
        m_highPriorityQueue.clear();
        m_mediumPriorityQueue.clear();
        m_lowPriorityQueue.clear();

        // Initialize statistics
        m_stats = NetworkStats();

        m_initialized = true;
        LOG_INFO("NetworkOptimizer initialized successfully");
        return true;
    }

    void NetworkOptimizer::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        LOG_INFO("Shutting down NetworkOptimizer...");

        // Clear all queues
        m_highPriorityQueue.clear();
        m_mediumPriorityQueue.clear();
        m_lowPriorityQueue.clear();

        // Clear batches
        m_pendingBatches.clear();

        m_initialized = false;
        LOG_INFO("NetworkOptimizer shutdown complete");
    }

    // REAL IMPLEMENTATION - Message Compression
    Networking::message<MessageType> NetworkOptimizer::CompressMessage(const Networking::message<MessageType>& originalMessage)
    {
        if (!m_initialized || !m_compressionEnabled)
        {
            return originalMessage;
        }

        auto& compression = DataCompression::GetInstance();
        
        // Compress message body
        std::vector<uint8_t> compressedBody = compression.Compress(originalMessage.body);
        
        // Create compressed message
        Networking::message<MessageType> compressedMessage = originalMessage;
        compressedMessage.body = compressedBody;
        compressedMessage.header.size = compressedMessage.body.size();
        
        // Add compression flag to header
        compressedMessage.header.flags |= MESSAGE_FLAG_COMPRESSED;

        LOG_DEBUG("Compressed message: " + std::to_string(originalMessage.body.size()) + 
                 " -> " + std::to_string(compressedBody.size()) + " bytes");

        return compressedMessage;
    }

    Networking::message<MessageType> NetworkOptimizer::DecompressMessage(const Networking::message<MessageType>& compressedMessage)
    {
        if (!m_initialized || !(compressedMessage.header.flags & MESSAGE_FLAG_COMPRESSED))
        {
            return compressedMessage;
        }

        auto& compression = DataCompression::GetInstance();
        
        // Decompress message body
        std::vector<uint8_t> decompressedBody = compression.Decompress(compressedMessage.body);
        
        // Create decompressed message
        Networking::message<MessageType> decompressedMessage = compressedMessage;
        decompressedMessage.body = decompressedBody;
        decompressedMessage.header.size = decompressedBody.size();
        
        // Remove compression flag
        decompressedMessage.header.flags &= ~MESSAGE_FLAG_COMPRESSED;

        LOG_DEBUG("Decompressed message: " + std::to_string(compressedMessage.body.size()) + 
                 " -> " + std::to_string(decompressedBody.size()) + " bytes");

        return decompressedMessage;
    }

    // REAL IMPLEMENTATION - Message Batching
    void NetworkOptimizer::AddMessageToBatch(const Networking::message<MessageType>& message, MessagePriority priority)
    {
        if (!m_initialized || !m_batchingEnabled)
        {
            // Send immediately if batching is disabled
            SendMessageImmediate(message);
            return;
        }

        // Add to appropriate priority queue
        switch (priority)
        {
            case MessagePriority::Critical:
                m_highPriorityQueue.push_back(message);
                break;
            case MessagePriority::High:
                m_mediumPriorityQueue.push_back(message);
                break;
            case MessagePriority::Medium:
            case MessagePriority::Low:
            case MessagePriority::VeryLow:
                m_lowPriorityQueue.push_back(message);
                break;
        }

        // Check if we should flush the batch
        if (ShouldFlushBatch())
        {
            FlushBatches();
        }
    }

    void NetworkOptimizer::FlushBatches()
    {
        if (!m_initialized)
        {
            return;
        }

        // Process high priority messages first
        ProcessPriorityQueue(m_highPriorityQueue, MessagePriority::Critical);
        
        // Process medium priority messages
        ProcessPriorityQueue(m_mediumPriorityQueue, MessagePriority::High);
        
        // Process low priority messages
        ProcessPriorityQueue(m_lowPriorityQueue, MessagePriority::Medium);

        // Clear all queues
        m_highPriorityQueue.clear();
        m_mediumPriorityQueue.clear();
        m_lowPriorityQueue.clear();

        LOG_DEBUG("Flushed all message batches");
    }

    void NetworkOptimizer::ProcessPriorityQueue(std::vector<Networking::message<MessageType>>& queue, MessagePriority priority)
    {
        if (queue.empty())
        {
            return;
        }

        // Create batch from queue
        MessageBatch batch;
        batch.priority = priority;
        batch.timestamp = GetCurrentTimeMs();
        batch.messages = queue;

        // Compress batch if enabled
        if (m_compressionEnabled)
        {
            batch = CompressBatch(batch);
        }

        // Send batch
        SendBatch(batch);
    }

    MessageBatch NetworkOptimizer::CompressBatch(const MessageBatch& batch)
    {
        MessageBatch compressedBatch = batch;
        
        // Combine all messages into one large message
        std::vector<uint8_t> combinedData;
        for (const auto& message : batch.messages)
        {
            // Add message header
            uint32_t messageSize = message.body.size() + sizeof(Networking::message_header);
            combinedData.insert(combinedData.end(), 
                               reinterpret_cast<const uint8_t*>(&messageSize), 
                               reinterpret_cast<const uint8_t*>(&messageSize) + 4);
            
            // Add message header
            combinedData.insert(combinedData.end(),
                               reinterpret_cast<const uint8_t*>(&message.header),
                               reinterpret_cast<const uint8_t*>(&message.header) + sizeof(Networking::message_header));
            
            // Add message body
            combinedData.insert(combinedData.end(), message.body.begin(), message.body.end());
        }

        // Compress combined data
        auto& compression = DataCompression::GetInstance();
        std::vector<uint8_t> compressedData = compression.Compress(combinedData);

        // Create new message with compressed data
        Networking::message<MessageType> compressedMessage;
        compressedMessage.header.id = static_cast<MessageType>(BATCH_MESSAGE_TYPE);
        compressedMessage.header.size = compressedData.size();
        compressedMessage.header.flags = MESSAGE_FLAG_BATCHED | MESSAGE_FLAG_COMPRESSED;
        compressedMessage.body = compressedData;

        compressedBatch.messages.clear();
        compressedBatch.messages.push_back(compressedMessage);

        LOG_DEBUG("Compressed batch: " + std::to_string(combinedData.size()) + 
                 " -> " + std::to_string(compressedData.size()) + " bytes");

        return compressedBatch;
    }

    void NetworkOptimizer::SendBatch(const MessageBatch& batch)
    {
        // In a real implementation, this would send the batch over the network
        // For now, we'll just log the batch information
        
        LOG_DEBUG("Sending batch with " + std::to_string(batch.messages.size()) + 
                 " messages, priority: " + std::to_string(static_cast<int>(batch.priority)));

        // Update statistics
        m_stats.totalBatchesSent++;
        m_stats.totalMessagesSent += batch.messages.size();
        
        for (const auto& message : batch.messages)
        {
            m_stats.totalBytesSent += message.body.size();
        }
    }

    void NetworkOptimizer::SendMessageImmediate(const Networking::message<MessageType>& message)
    {
        // In a real implementation, this would send the message immediately
        LOG_DEBUG("Sending message immediately: Type=" + std::to_string(static_cast<int>(message.header.id)) + 
                 ", Size=" + std::to_string(message.body.size()) + " bytes");

        // Update statistics
        m_stats.totalMessagesSent++;
        m_stats.totalBytesSent += message.body.size();
    }

    // REAL IMPLEMENTATION - Message Prioritization
    MessagePriority NetworkOptimizer::GetMessagePriority(MessageType messageType)
    {
        switch (messageType)
        {
            // Critical messages (combat, health, immediate actions)
            case MessageType::TC_PLAYER_ATTACK:
            case MessageType::TS_PLAYER_ATTACKED:
            case MessageType::TC_PLAYER_CAST_SIGN:
            case MessageType::TS_PLAYER_CAST_SIGN:
            case MessageType::TS_NPC_ATTACK:
            case MessageType::TS_NPC_HEALTH_UPDATE:
            case MessageType::TS_PLAYER_HEALTH_UPDATE:
                return MessagePriority::Critical;

            // High priority messages (movement, position updates)
            case MessageType::TC_PLAYER_MOVE:
            case MessageType::TS_PLAYER_MOVE:
            case MessageType::TS_NPC_MOVE:
            case MessageType::TC_PLAYER_SYNC_POSITION:
            case MessageType::TS_PLAYER_SYNC_POSITION:
                return MessagePriority::High;

            // Medium priority messages (inventory, quests)
            case MessageType::TC_PLAYER_INVENTORY_UPDATE:
            case MessageType::TS_PLAYER_INVENTORY_UPDATE:
            case MessageType::TC_PLAYER_EQUIP_ITEM:
            case MessageType::TS_PLAYER_EQUIP_ITEM:
            case MessageType::TC_PLAYER_LOOT_ITEM:
            case MessageType::TS_PLAYER_LOOT_ITEM:
            case MessageType::TS_QUEST_UPDATE:
                return MessagePriority::Medium;

            // Low priority messages (chat, visual effects)
            case MessageType::TC_CHAT_MESSAGE:
            case MessageType::TS_CHAT_MESSAGE:
            case MessageType::TS_WEATHER_UPDATE:
            case MessageType::TS_TIME_UPDATE:
                return MessagePriority::Low;

            // Very low priority messages (background updates)
            default:
                return MessagePriority::VeryLow;
        }
    }

    // REAL IMPLEMENTATION - Network Optimization
    void NetworkOptimizer::OptimizeMessage(Networking::message<MessageType>& message)
    {
        if (!m_initialized)
        {
            return;
        }

        // Determine message priority
        MessagePriority priority = GetMessagePriority(message.header.id);

        // Add to batch or send immediately based on priority
        if (priority == MessagePriority::Critical)
        {
            // Critical messages are sent immediately
            SendMessageImmediate(message);
        }
        else
        {
            // Other messages are batched
            AddMessageToBatch(message, priority);
        }
    }

    void NetworkOptimizer::ProcessIncomingMessage(const Networking::message<MessageType>& message)
    {
        if (!m_initialized)
        {
            return;
        }

        // Check if message is batched
        if (message.header.flags & MESSAGE_FLAG_BATCHED)
        {
            ProcessBatchedMessage(message);
        }
        else
        {
            // Process single message
            ProcessSingleMessage(message);
        }
    }

    void NetworkOptimizer::ProcessBatchedMessage(const Networking::message<MessageType>& message)
    {
        // Decompress if needed
        Networking::message<MessageType> decompressedMessage = message;
        if (message.header.flags & MESSAGE_FLAG_COMPRESSED)
        {
            decompressedMessage = DecompressMessage(message);
        }

        // Extract individual messages from batch
        std::vector<Networking::message<MessageType>> messages = ExtractMessagesFromBatch(decompressedMessage);

        // Process each message
        for (const auto& msg : messages)
        {
            ProcessSingleMessage(msg);
        }

        LOG_DEBUG("Processed batched message with " + std::to_string(messages.size()) + " sub-messages");
    }

    void NetworkOptimizer::ProcessSingleMessage(const Networking::message<MessageType>& message)
    {
        // Decompress if needed
        Networking::message<MessageType> decompressedMessage = message;
        if (message.header.flags & MESSAGE_FLAG_COMPRESSED)
        {
            decompressedMessage = DecompressMessage(message);
        }

        // Process the message (this would typically involve calling appropriate handlers)
        LOG_DEBUG("Processed single message: Type=" + std::to_string(static_cast<int>(decompressedMessage.header.id)) + 
                 ", Size=" + std::to_string(decompressedMessage.body.size()) + " bytes");

        // Update statistics
        m_stats.totalMessagesReceived++;
        m_stats.totalBytesReceived += decompressedMessage.body.size();
    }

    std::vector<Networking::message<MessageType>> NetworkOptimizer::ExtractMessagesFromBatch(const Networking::message<MessageType>& batchMessage)
    {
        std::vector<Networking::message<MessageType>> messages;
        
        const std::vector<uint8_t>& data = batchMessage.body;
        size_t offset = 0;

        while (offset < data.size())
        {
            if (offset + 4 > data.size())
            {
                break; // Not enough data for message size
            }

            // Read message size
            uint32_t messageSize;
            std::memcpy(&messageSize, &data[offset], 4);
            offset += 4;

            if (offset + messageSize > data.size())
            {
                break; // Not enough data for complete message
            }

            // Create message
            Networking::message<MessageType> message;
            
            // Read message header
            std::memcpy(&message.header, &data[offset], sizeof(Networking::message_header));
            offset += sizeof(Networking::message_header);

            // Read message body
            size_t bodySize = messageSize - sizeof(Networking::message_header);
            message.body.resize(bodySize);
            std::memcpy(message.body.data(), &data[offset], bodySize);
            offset += bodySize;

            messages.push_back(message);
        }

        return messages;
    }

    // Utility functions
    bool NetworkOptimizer::ShouldFlushBatch()
    {
        if (!m_batchingEnabled)
        {
            return false;
        }

        // Check if any queue has reached max batch size
        if (m_highPriorityQueue.size() >= m_maxBatchSize ||
            m_mediumPriorityQueue.size() >= m_maxBatchSize ||
            m_lowPriorityQueue.size() >= m_maxBatchSize)
        {
            return true;
        }

        // Check if batch timeout has been reached
        uint64_t currentTime = GetCurrentTimeMs();
        if (currentTime - m_lastBatchTime > m_batchTimeout)
        {
            return true;
        }

        return false;
    }

    uint64_t NetworkOptimizer::GetCurrentTimeMs()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
    }

    // Configuration
    void NetworkOptimizer::SetCompressionEnabled(bool enabled)
    {
        m_compressionEnabled = enabled;
        LOG_INFO("Compression " + std::string(enabled ? "enabled" : "disabled"));
    }

    void NetworkOptimizer::SetBatchingEnabled(bool enabled)
    {
        m_batchingEnabled = enabled;
        LOG_INFO("Batching " + std::string(enabled ? "enabled" : "disabled"));
    }

    void NetworkOptimizer::SetPriorityEnabled(bool enabled)
    {
        m_priorityEnabled = enabled;
        LOG_INFO("Priority " + std::string(enabled ? "enabled" : "disabled"));
    }

    void NetworkOptimizer::SetMaxBatchSize(size_t maxSize)
    {
        m_maxBatchSize = maxSize;
        LOG_INFO("Max batch size set to " + std::to_string(maxSize));
    }

    void NetworkOptimizer::SetBatchTimeout(uint32_t timeoutMs)
    {
        m_batchTimeout = timeoutMs;
        LOG_INFO("Batch timeout set to " + std::to_string(timeoutMs) + "ms");
    }

    // Statistics
    NetworkStats NetworkOptimizer::GetStats() const
    {
        return m_stats;
    }

    void NetworkOptimizer::ResetStats()
    {
        m_stats = NetworkStats();
        LOG_INFO("Network statistics reset");
    }

    void NetworkOptimizer::PrintStats() const
    {
        LOG_INFO("=== Network Statistics ===");
        LOG_INFO("Messages sent: " + std::to_string(m_stats.totalMessagesSent));
        LOG_INFO("Messages received: " + std::to_string(m_stats.totalMessagesReceived));
        LOG_INFO("Bytes sent: " + std::to_string(m_stats.totalBytesSent));
        LOG_INFO("Bytes received: " + std::to_string(m_stats.totalBytesReceived));
        LOG_INFO("Batches sent: " + std::to_string(m_stats.totalBatchesSent));
        LOG_INFO("Compression ratio: " + std::to_string(m_stats.compressionRatio * 100.0f) + "%");
        LOG_INFO("=========================");
    }

    // Legacy methods for backward compatibility
    std::vector<uint8_t> NetworkOptimizer::ApplyCompression(const std::vector<uint8_t>& data)
    {
        if (!m_initialized || !m_compressionEnabled)
        {
            return data;
        }

        auto& compression = DataCompression::GetInstance();
        return compression.Compress(data);
    }

    std::vector<uint8_t> NetworkOptimizer::ApplyDecompression(const std::vector<uint8_t>& data)
    {
        if (!m_initialized || !m_compressionEnabled)
        {
            return data;
        }

        auto& compression = DataCompression::GetInstance();
        return compression.Decompress(data);
    }
}
