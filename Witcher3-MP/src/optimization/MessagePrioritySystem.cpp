#include "optimization/MessagePrioritySystem.h"
#include "utils/Logger.h"
#include <algorithm>
#include <cmath>

namespace Optimization
{
    // MessagePriorityQueue implementation
    MessagePriorityQueue::MessagePriorityQueue()
        : m_maxQueueSize(1000), m_maxMessageAge(5.0f)
    {
        LOG_INFO("Message priority queue created");
    }

    MessagePriorityQueue::~MessagePriorityQueue()
    {
        Clear();
        LOG_INFO("Message priority queue destroyed");
    }

    void MessagePriorityQueue::PushMessage(const PrioritizedMessage& message)
    {
        if (m_messageQueue.size() >= m_maxQueueSize)
        {
            LOG_WARNING("Message queue is full, dropping oldest message");
            m_stats.droppedMessages++;
            return;
        }

        PrioritizedMessage messageCopy = message;
        messageCopy.timestamp = std::chrono::high_resolution_clock::now();
        messageCopy.age = 0.0f;

        m_messageQueue.push(messageCopy);
        m_stats.totalMessages++;
        m_stats.messagesByPriority[static_cast<int>(messageCopy.classification.priority)]++;
        m_stats.messagesByUrgency[static_cast<int>(messageCopy.classification.urgency)]++;

        LOG_DEBUG("Pushed message ID " + std::to_string(messageCopy.messageId) + 
                 " with priority " + std::to_string(static_cast<int>(messageCopy.classification.priority)));
    }

    PrioritizedMessage MessagePriorityQueue::PopMessage()
    {
        if (m_messageQueue.empty())
        {
            return PrioritizedMessage();
        }

        PrioritizedMessage message = m_messageQueue.top();
        m_messageQueue.pop();

        // Update age
        auto now = std::chrono::high_resolution_clock::now();
        message.age = std::chrono::duration<float>(now - message.timestamp).count();

        // Update statistics
        m_stats.averageQueueTime = (m_stats.averageQueueTime + message.age) / 2.0f;
        m_stats.maxQueueTime = std::max(m_stats.maxQueueTime, message.age);

        LOG_DEBUG("Popped message ID " + std::to_string(message.messageId) + 
                 " (age: " + std::to_string(message.age) + "s)");

        return message;
    }

    bool MessagePriorityQueue::HasMessages() const
    {
        return !m_messageQueue.empty();
    }

    size_t MessagePriorityQueue::GetMessageCount() const
    {
        return m_messageQueue.size();
    }

    void MessagePriorityQueue::Clear()
    {
        while (!m_messageQueue.empty())
        {
            m_messageQueue.pop();
        }
        m_stats.Reset();
    }

    void MessagePriorityQueue::SetPriority(Networking::MessageTypes type, MessagePriority priority)
    {
        auto it = m_messageClassifications.find(type);
        if (it != m_messageClassifications.end())
        {
            it->second.priority = priority;
        }
        else
        {
            MessageClassification classification = PriorityUtils::GetDefaultClassification(type);
            classification.priority = priority;
            m_messageClassifications[type] = classification;
        }
    }

    void MessagePriorityQueue::SetUrgency(Networking::MessageTypes type, MessageUrgency urgency)
    {
        auto it = m_messageClassifications.find(type);
        if (it != m_messageClassifications.end())
        {
            it->second.urgency = urgency;
        }
        else
        {
            MessageClassification classification = PriorityUtils::GetDefaultClassification(type);
            classification.urgency = urgency;
            m_messageClassifications[type] = classification;
        }
    }

    void MessagePriorityQueue::SetClassification(Networking::MessageTypes type, const MessageClassification& classification)
    {
        m_messageClassifications[type] = classification;
    }

    std::vector<PrioritizedMessage> MessagePriorityQueue::PopMessages(size_t maxCount)
    {
        std::vector<PrioritizedMessage> messages;
        messages.reserve(maxCount);

        for (size_t i = 0; i < maxCount && !m_messageQueue.empty(); ++i)
        {
            messages.push_back(PopMessage());
        }

        return messages;
    }

    std::vector<PrioritizedMessage> MessagePriorityQueue::PopMessagesByPriority(MessagePriority priority, size_t maxCount)
    {
        std::vector<PrioritizedMessage> messages;
        std::vector<PrioritizedMessage> tempMessages;

        // Pop all messages and filter by priority
        while (!m_messageQueue.empty())
        {
            PrioritizedMessage message = PopMessage();
            if (message.classification.priority == priority)
            {
                messages.push_back(message);
                if (messages.size() >= maxCount)
                {
                    break;
                }
            }
            else
            {
                tempMessages.push_back(message);
            }
        }

        // Push back non-matching messages
        for (const auto& message : tempMessages)
        {
            m_messageQueue.push(message);
        }

        return messages;
    }

    std::vector<PrioritizedMessage> MessagePriorityQueue::PopExpiredMessages()
    {
        std::vector<PrioritizedMessage> expiredMessages;
        std::vector<PrioritizedMessage> validMessages;

        while (!m_messageQueue.empty())
        {
            PrioritizedMessage message = PopMessage();
            if (IsMessageExpired(message))
            {
                expiredMessages.push_back(message);
                m_stats.expiredMessages++;
            }
            else
            {
                validMessages.push_back(message);
            }
        }

        // Push back valid messages
        for (const auto& message : validMessages)
        {
            m_messageQueue.push(message);
        }

        return expiredMessages;
    }

    MessagePriorityQueue::QueueStats MessagePriorityQueue::GetStats() const
    {
        return m_stats;
    }

    void MessagePriorityQueue::ResetStats()
    {
        m_stats.Reset();
    }

    // MessageComparator implementation
    bool MessagePriorityQueue::MessageComparator::operator()(const PrioritizedMessage& a, const PrioritizedMessage& b) const
    {
        // First compare by priority (lower number = higher priority)
        if (a.classification.priority != b.classification.priority)
        {
            return static_cast<int>(a.classification.priority) > static_cast<int>(b.classification.priority);
        }

        // Then compare by urgency (lower number = higher urgency)
        if (a.classification.urgency != b.classification.urgency)
        {
            return static_cast<int>(a.classification.urgency) > static_cast<int>(b.classification.urgency);
        }

        // Then compare by importance (higher importance = higher priority)
        if (std::abs(a.classification.importance - b.classification.importance) > 0.01f)
        {
            return a.classification.importance < b.classification.importance;
        }

        // Finally compare by age (older messages have higher priority)
        return a.age < b.age;
    }

    float MessagePriorityQueue::CalculateMessageScore(const PrioritizedMessage& message) const
    {
        float priorityScore = 1.0f - (static_cast<float>(message.classification.priority) / 4.0f);
        float urgencyScore = 1.0f - (static_cast<float>(message.classification.urgency) / 4.0f);
        float importanceScore = message.classification.importance;
        float ageScore = std::min(message.age / m_maxMessageAge, 1.0f);

        return (priorityScore * 0.4f) + (urgencyScore * 0.3f) + (importanceScore * 0.2f) + (ageScore * 0.1f);
    }

    bool MessagePriorityQueue::IsMessageExpired(const PrioritizedMessage& message) const
    {
        return message.age > message.classification.timeToLive;
    }

    void MessagePriorityQueue::UpdateMessageAge(PrioritizedMessage& message)
    {
        auto now = std::chrono::high_resolution_clock::now();
        message.age = std::chrono::duration<float>(now - message.timestamp).count();
    }

    // NetworkTrafficManager implementation
    NetworkTrafficManager::NetworkTrafficManager()
        : m_initialized(false), m_bandwidthLimit(1024 * 1024), // 1MB/s
          m_messageRateLimit(1000), m_trafficShapingEnabled(true), m_congestionThreshold(0.8f),
          m_bytesSentThisSecond(0), m_messagesSentThisSecond(0)
    {
        m_lastUpdateTime = std::chrono::high_resolution_clock::now();
        m_lastBandwidthReset = m_lastUpdateTime;
        m_lastRateReset = m_lastUpdateTime;
        
        LOG_INFO("Network traffic manager created");
    }

    NetworkTrafficManager::~NetworkTrafficManager()
    {
        Shutdown();
        LOG_INFO("Network traffic manager destroyed");
    }

    bool NetworkTrafficManager::Initialize()
    {
        if (m_initialized)
        {
            return true;
        }

        LOG_INFO("Initializing network traffic manager...");

        // Load default configurations
        LoadDefaultConfigurations();

        m_initialized = true;
        LOG_INFO("Network traffic manager initialized");
        return true;
    }

    void NetworkTrafficManager::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        LOG_INFO("Shutting down network traffic manager...");
        
        m_priorityQueue.Clear();
        m_initialized = false;
        LOG_INFO("Network traffic manager shutdown complete");
    }

    void NetworkTrafficManager::ProcessMessage(const PrioritizedMessage& message)
    {
        if (!m_initialized)
        {
            return;
        }

        m_stats.totalMessagesProcessed++;

        // Check if message can be sent
        if (!CanSendMessage(message))
        {
            m_stats.messagesDropped++;
            m_stats.bytesDropped += message.data.size();
            LOG_DEBUG("Message " + std::to_string(message.messageId) + " dropped due to traffic limits");
            return;
        }

        // Add to priority queue
        m_priorityQueue.PushMessage(message);

        LOG_DEBUG("Processed message " + std::to_string(message.messageId) + 
                 " (priority: " + std::to_string(static_cast<int>(message.classification.priority)) + ")");
    }

    std::vector<PrioritizedMessage> NetworkTrafficManager::GetMessagesToSend(size_t maxCount)
    {
        if (!m_initialized)
        {
            return {};
        }

        std::vector<PrioritizedMessage> messages = m_priorityQueue.PopMessages(maxCount);
        
        // Update statistics
        for (const auto& message : messages)
        {
            m_stats.messagesSent++;
            m_stats.bytesSent += message.data.size();
            m_bytesSentThisSecond += message.data.size();
            m_messagesSentThisSecond++;
        }

        // Update traffic stats
        UpdateTrafficStats();

        return messages;
    }

    void NetworkTrafficManager::MarkMessageSent(uint32_t messageId)
    {
        // Message was successfully sent
        LOG_DEBUG("Message " + std::to_string(messageId) + " sent successfully");
    }

    void NetworkTrafficManager::MarkMessageFailed(uint32_t messageId)
    {
        // Message failed to send, might need retry
        m_stats.messagesRetried++;
        LOG_DEBUG("Message " + std::to_string(messageId) + " failed to send");
    }

    void NetworkTrafficManager::SetBandwidthLimit(size_t bytesPerSecond)
    {
        m_bandwidthLimit = bytesPerSecond;
        LOG_INFO("Bandwidth limit set to " + std::to_string(bytesPerSecond) + " bytes/second");
    }

    void NetworkTrafficManager::SetMessageRateLimit(size_t messagesPerSecond)
    {
        m_messageRateLimit = messagesPerSecond;
        LOG_INFO("Message rate limit set to " + std::to_string(messagesPerSecond) + " messages/second");
    }

    void NetworkTrafficManager::EnableTrafficShaping(bool enable)
    {
        m_trafficShapingEnabled = enable;
        LOG_INFO("Traffic shaping " + (enable ? "enabled" : "disabled"));
    }

    void NetworkTrafficManager::SetCongestionThreshold(float threshold)
    {
        m_congestionThreshold = threshold;
        LOG_INFO("Congestion threshold set to " + std::to_string(threshold));
    }

    void NetworkTrafficManager::SetMessagePriority(Networking::MessageTypes type, MessagePriority priority)
    {
        m_priorityQueue.SetPriority(type, priority);
    }

    void NetworkTrafficManager::SetMessageUrgency(Networking::MessageTypes type, MessageUrgency urgency)
    {
        m_priorityQueue.SetUrgency(type, urgency);
    }

    void NetworkTrafficManager::SetMessageClassification(Networking::MessageTypes type, const MessageClassification& classification)
    {
        m_priorityQueue.SetClassification(type, classification);
    }

    NetworkTrafficManager::TrafficStats NetworkTrafficManager::GetStats() const
    {
        return m_stats;
    }

    void NetworkTrafficManager::ResetStats()
    {
        m_stats.Reset();
    }

    void NetworkTrafficManager::PrintStats() const
    {
        LOG_INFO("=== Network Traffic Statistics ===");
        LOG_INFO("Total messages processed: " + std::to_string(m_stats.totalMessagesProcessed));
        LOG_INFO("Messages sent: " + std::to_string(m_stats.messagesSent));
        LOG_INFO("Messages dropped: " + std::to_string(m_stats.messagesDropped));
        LOG_INFO("Messages retried: " + std::to_string(m_stats.messagesRetried));
        LOG_INFO("Bytes sent: " + std::to_string(m_stats.bytesSent));
        LOG_INFO("Bytes dropped: " + std::to_string(m_stats.bytesDropped));
        LOG_INFO("Average latency: " + std::to_string(m_stats.averageLatency) + "ms");
        LOG_INFO("Max latency: " + std::to_string(m_stats.maxLatency) + "ms");
        LOG_INFO("Bandwidth utilization: " + std::to_string(m_stats.bandwidthUtilization * 100.0f) + "%");
        LOG_INFO("Congestion level: " + std::to_string(m_stats.congestionLevel * 100.0f) + "%");
        LOG_INFO("=================================");
    }

    void NetworkTrafficManager::SetConfig(const std::map<Networking::MessageTypes, MessageClassification>& classifications)
    {
        for (const auto& pair : classifications)
        {
            m_priorityQueue.SetClassification(pair.first, pair.second);
        }
    }

    void NetworkTrafficManager::LoadDefaultConfigurations()
    {
        // Combat messages - Critical priority
        MessageClassification combatClass;
        combatClass.priority = MessagePriority::Critical;
        combatClass.urgency = MessageUrgency::Immediate;
        combatClass.importance = 1.0f;
        combatClass.timeToLive = 0.1f;
        combatClass.requiresAck = true;
        combatClass.canBeDropped = false;
        combatClass.canBeCompressed = false;
        combatClass.maxRetries = 5;
        
        m_priorityQueue.SetClassification(Networking::MessageTypes::TC_PlayerAttack, combatClass);
        m_priorityQueue.SetClassification(Networking::MessageTypes::TC_PlayerDamage, combatClass);
        m_priorityQueue.SetClassification(Networking::MessageTypes::TC_PlayerDeath, combatClass);
        m_priorityQueue.SetClassification(Networking::MessageTypes::TC_PlayerHealth, combatClass);

        // Movement messages - High priority
        MessageClassification movementClass;
        movementClass.priority = MessagePriority::High;
        movementClass.urgency = MessageUrgency::Urgent;
        movementClass.importance = 0.8f;
        movementClass.timeToLive = 0.5f;
        movementClass.requiresAck = false;
        movementClass.canBeDropped = true;
        movementClass.canBeCompressed = true;
        movementClass.maxRetries = 2;
        
        m_priorityQueue.SetClassification(Networking::MessageTypes::TC_PlayerMove, movementClass);
        m_priorityQueue.SetClassification(Networking::MessageTypes::TC_PlayerPosition, movementClass);
        m_priorityQueue.SetClassification(Networking::MessageTypes::TC_PlayerRotation, movementClass);

        // Inventory messages - Medium priority
        MessageClassification inventoryClass;
        inventoryClass.priority = MessagePriority::Medium;
        inventoryClass.urgency = MessageUrgency::Normal;
        inventoryClass.importance = 0.6f;
        inventoryClass.timeToLive = 2.0f;
        inventoryClass.requiresAck = true;
        inventoryClass.canBeDropped = true;
        inventoryClass.canBeCompressed = true;
        inventoryClass.maxRetries = 3;
        
        m_priorityQueue.SetClassification(Networking::MessageTypes::TC_PlayerInventory, inventoryClass);
        m_priorityQueue.SetClassification(Networking::MessageTypes::TC_PlayerEquipment, inventoryClass);
        m_priorityQueue.SetClassification(Networking::MessageTypes::TC_PlayerLoot, inventoryClass);

        // Visual effects - Low priority
        MessageClassification effectsClass;
        effectsClass.priority = MessagePriority::Low;
        effectsClass.urgency = MessageUrgency::Low;
        effectsClass.importance = 0.3f;
        effectsClass.timeToLive = 5.0f;
        effectsClass.requiresAck = false;
        effectsClass.canBeDropped = true;
        effectsClass.canBeCompressed = true;
        effectsClass.maxRetries = 1;
        
        m_priorityQueue.SetClassification(Networking::MessageTypes::TC_PlayerEffect, effectsClass);
        m_priorityQueue.SetClassification(Networking::MessageTypes::TC_PlayerAnimation, effectsClass);
        m_priorityQueue.SetClassification(Networking::MessageTypes::TC_PlayerSound, effectsClass);

        LOG_INFO("Default message classifications loaded");
    }

    void NetworkTrafficManager::UpdateTrafficStats()
    {
        auto now = std::chrono::high_resolution_clock::now();
        
        // Reset bandwidth counter every second
        if (std::chrono::duration<float>(now - m_lastBandwidthReset).count() >= 1.0f)
        {
            m_stats.bandwidthUtilization = static_cast<float>(m_bytesSentThisSecond) / static_cast<float>(m_bandwidthLimit);
            m_bytesSentThisSecond = 0;
            m_lastBandwidthReset = now;
        }
        
        // Reset rate counter every second
        if (std::chrono::duration<float>(now - m_lastRateReset).count() >= 1.0f)
        {
            m_messagesSentThisSecond = 0;
            m_lastRateReset = now;
        }
        
        // Calculate congestion level
        m_stats.congestionLevel = std::min(m_stats.bandwidthUtilization, 1.0f);
    }

    void NetworkTrafficManager::CleanupExpiredMessages()
    {
        std::vector<PrioritizedMessage> expiredMessages = m_priorityQueue.PopExpiredMessages();
        m_stats.messagesDropped += expiredMessages.size();
        
        for (const auto& message : expiredMessages)
        {
            m_stats.bytesDropped += message.data.size();
        }
    }

    void NetworkTrafficManager::HandleCongestion()
    {
        if (m_stats.congestionLevel > m_congestionThreshold)
        {
            DropLowPriorityMessages();
        }
    }

    bool NetworkTrafficManager::CanSendMessage(const PrioritizedMessage& message) const
    {
        if (!m_trafficShapingEnabled)
        {
            return true;
        }

        // Check bandwidth limit
        if (m_bytesSentThisSecond + message.data.size() > m_bandwidthLimit)
        {
            return false;
        }

        // Check rate limit
        if (m_messagesSentThisSecond >= m_messageRateLimit)
        {
            return false;
        }

        return true;
    }

    void NetworkTrafficManager::DropLowPriorityMessages()
    {
        // This would implement dropping low priority messages during congestion
        // For now, it's a placeholder
        LOG_DEBUG("Dropping low priority messages due to congestion");
    }

    // Priority utilities implementation
    namespace PriorityUtils
    {
        MessageClassification GetDefaultClassification(Networking::MessageTypes type)
        {
            MessageClassification classification;
            
            // Set default values based on message type
            switch (type)
            {
                case Networking::MessageTypes::TC_PlayerAttack:
                case Networking::MessageTypes::TC_PlayerDamage:
                case Networking::MessageTypes::TC_PlayerDeath:
                case Networking::MessageTypes::TC_PlayerHealth:
                    classification.priority = MessagePriority::Critical;
                    classification.urgency = MessageUrgency::Immediate;
                    classification.importance = 1.0f;
                    classification.timeToLive = 0.1f;
                    classification.requiresAck = true;
                    classification.canBeDropped = false;
                    break;
                    
                case Networking::MessageTypes::TC_PlayerMove:
                case Networking::MessageTypes::TC_PlayerPosition:
                case Networking::MessageTypes::TC_PlayerRotation:
                    classification.priority = MessagePriority::High;
                    classification.urgency = MessageUrgency::Urgent;
                    classification.importance = 0.8f;
                    classification.timeToLive = 0.5f;
                    classification.requiresAck = false;
                    classification.canBeDropped = true;
                    break;
                    
                case Networking::MessageTypes::TC_PlayerInventory:
                case Networking::MessageTypes::TC_PlayerEquipment:
                case Networking::MessageTypes::TC_PlayerLoot:
                    classification.priority = MessagePriority::Medium;
                    classification.urgency = MessageUrgency::Normal;
                    classification.importance = 0.6f;
                    classification.timeToLive = 2.0f;
                    classification.requiresAck = true;
                    classification.canBeDropped = true;
                    break;
                    
                default:
                    classification.priority = MessagePriority::Low;
                    classification.urgency = MessageUrgency::Low;
                    classification.importance = 0.3f;
                    classification.timeToLive = 5.0f;
                    classification.requiresAck = false;
                    classification.canBeDropped = true;
                    break;
            }
            
            return classification;
        }

        MessagePriority GetDefaultPriority(Networking::MessageTypes type)
        {
            return GetDefaultClassification(type).priority;
        }

        MessageUrgency GetDefaultUrgency(Networking::MessageTypes type)
        {
            return GetDefaultClassification(type).urgency;
        }

        float CalculateImportance(const PrioritizedMessage& message)
        {
            return message.classification.importance;
        }

        float CalculateUrgency(const PrioritizedMessage& message)
        {
            float urgencyScore = 1.0f - (static_cast<float>(message.classification.urgency) / 4.0f);
            float ageFactor = std::min(message.age / message.classification.timeToLive, 1.0f);
            return urgencyScore + ageFactor;
        }

        bool IsHigherPriority(const PrioritizedMessage& a, const PrioritizedMessage& b)
        {
            if (a.classification.priority != b.classification.priority)
            {
                return static_cast<int>(a.classification.priority) < static_cast<int>(b.classification.priority);
            }
            return a.classification.importance > b.classification.importance;
        }

        bool IsMoreUrgent(const PrioritizedMessage& a, const PrioritizedMessage& b)
        {
            if (a.classification.urgency != b.classification.urgency)
            {
                return static_cast<int>(a.classification.urgency) < static_cast<int>(b.classification.urgency);
            }
            return a.age > b.age;
        }

        std::vector<PrioritizedMessage> FilterByPriority(const std::vector<PrioritizedMessage>& messages, 
                                                         MessagePriority priority)
        {
            std::vector<PrioritizedMessage> filtered;
            for (const auto& message : messages)
            {
                if (message.classification.priority == priority)
                {
                    filtered.push_back(message);
                }
            }
            return filtered;
        }

        std::vector<PrioritizedMessage> FilterByUrgency(const std::vector<PrioritizedMessage>& messages, 
                                                        MessageUrgency urgency)
        {
            std::vector<PrioritizedMessage> filtered;
            for (const auto& message : messages)
            {
                if (message.classification.urgency == urgency)
                {
                    filtered.push_back(message);
                }
            }
            return filtered;
        }

        std::vector<PrioritizedMessage> FilterByAge(const std::vector<PrioritizedMessage>& messages, 
                                                    float maxAge)
        {
            std::vector<PrioritizedMessage> filtered;
            for (const auto& message : messages)
            {
                if (message.age <= maxAge)
                {
                    filtered.push_back(message);
                }
            }
            return filtered;
        }

        void SortByPriority(std::vector<PrioritizedMessage>& messages)
        {
            std::sort(messages.begin(), messages.end(), [](const PrioritizedMessage& a, const PrioritizedMessage& b) {
                return static_cast<int>(a.classification.priority) < static_cast<int>(b.classification.priority);
            });
        }

        void SortByUrgency(std::vector<PrioritizedMessage>& messages)
        {
            std::sort(messages.begin(), messages.end(), [](const PrioritizedMessage& a, const PrioritizedMessage& b) {
                return static_cast<int>(a.classification.urgency) < static_cast<int>(b.classification.urgency);
            });
        }

        void SortByImportance(std::vector<PrioritizedMessage>& messages)
        {
            std::sort(messages.begin(), messages.end(), [](const PrioritizedMessage& a, const PrioritizedMessage& b) {
                return a.classification.importance > b.classification.importance;
            });
        }

        void SortByAge(std::vector<PrioritizedMessage>& messages)
        {
            std::sort(messages.begin(), messages.end(), [](const PrioritizedMessage& a, const PrioritizedMessage& b) {
                return a.age > b.age;
            });
        }

        void PrintPriorityDistribution(const std::vector<PrioritizedMessage>& messages)
        {
            int counts[5] = {0};
            for (const auto& message : messages)
            {
                counts[static_cast<int>(message.classification.priority)]++;
            }
            
            LOG_INFO("Priority distribution:");
            LOG_INFO("  Critical: " + std::to_string(counts[0]));
            LOG_INFO("  High: " + std::to_string(counts[1]));
            LOG_INFO("  Medium: " + std::to_string(counts[2]));
            LOG_INFO("  Low: " + std::to_string(counts[3]));
            LOG_INFO("  Background: " + std::to_string(counts[4]));
        }

        void PrintUrgencyDistribution(const std::vector<PrioritizedMessage>& messages)
        {
            int counts[5] = {0};
            for (const auto& message : messages)
            {
                counts[static_cast<int>(message.classification.urgency)]++;
            }
            
            LOG_INFO("Urgency distribution:");
            LOG_INFO("  Immediate: " + std::to_string(counts[0]));
            LOG_INFO("  Urgent: " + std::to_string(counts[1]));
            LOG_INFO("  Normal: " + std::to_string(counts[2]));
            LOG_INFO("  Low: " + std::to_string(counts[3]));
            LOG_INFO("  Batch: " + std::to_string(counts[4]));
        }
    }
}
