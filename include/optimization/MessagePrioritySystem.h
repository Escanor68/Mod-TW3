#pragma once

#include "Common.h"
#include "networking/MessageTypes.h"
#include <vector>
#include <map>
#include <queue>
#include <chrono>
#include <functional>

namespace Optimization
{
    // Message priority levels
    enum class MessagePriority
    {
        Critical = 0,    // Combat, health, death
        High = 1,        // Movement, position updates
        Medium = 2,      // Inventory, quests
        Low = 3,         // Visual effects, sounds
        Background = 4   // Statistics, logging
    };

    // Message urgency levels
    enum class MessageUrgency
    {
        Immediate = 0,   // Must be sent immediately
        Urgent = 1,      // Should be sent within 1 frame
        Normal = 2,      // Can be sent within 3 frames
        Low = 3,         // Can be sent within 10 frames
        Batch = 4        // Can be batched with other messages
    };

    // Message classification
    struct MessageClassification
    {
        MessagePriority priority;
        MessageUrgency urgency;
        float importance;        // 0.0 to 1.0
        float timeToLive;        // Maximum time before message expires
        bool requiresAck;        // Requires acknowledgment
        bool canBeDropped;       // Can be dropped if network is congested
        bool canBeCompressed;    // Can be compressed
        size_t maxRetries;       // Maximum number of retries
        
        MessageClassification() : priority(MessagePriority::Medium), urgency(MessageUrgency::Normal),
                                 importance(0.5f), timeToLive(1.0f), requiresAck(false),
                                 canBeDropped(true), canBeCompressed(true), maxRetries(3) {}
    };

    // Prioritized message
    struct PrioritizedMessage
    {
        uint32_t messageId;
        Networking::MessageTypes type;
        std::vector<uint8_t> data;
        MessageClassification classification;
        std::chrono::high_resolution_clock::time_point timestamp;
        uint32_t retryCount;
        float age;
        
        PrioritizedMessage() : messageId(0), type(Networking::MessageTypes::TC_Invalid), 
                              retryCount(0), age(0.0f) {}
    };

    // Priority queue for messages
    class MessagePriorityQueue
    {
    public:
        MessagePriorityQueue();
        ~MessagePriorityQueue();

        // Message management
        void PushMessage(const PrioritizedMessage& message);
        PrioritizedMessage PopMessage();
        bool HasMessages() const;
        size_t GetMessageCount() const;
        void Clear();

        // Priority management
        void SetPriority(Networking::MessageTypes type, MessagePriority priority);
        void SetUrgency(Networking::MessageTypes type, MessageUrgency urgency);
        void SetClassification(Networking::MessageTypes type, const MessageClassification& classification);

        // Queue operations
        std::vector<PrioritizedMessage> PopMessages(size_t maxCount);
        std::vector<PrioritizedMessage> PopMessagesByPriority(MessagePriority priority, size_t maxCount);
        std::vector<PrioritizedMessage> PopExpiredMessages();
        
        // Statistics
        struct QueueStats
        {
            size_t totalMessages = 0;
            size_t messagesByPriority[5] = {0}; // One for each priority level
            size_t messagesByUrgency[5] = {0};  // One for each urgency level
            size_t expiredMessages = 0;
            size_t droppedMessages = 0;
            float averageQueueTime = 0.0f;
            float maxQueueTime = 0.0f;
            
            void Reset()
            {
                totalMessages = 0;
                std::fill(messagesByPriority, messagesByPriority + 5, 0);
                std::fill(messagesByUrgency, messagesByUrgency + 5, 0);
                expiredMessages = 0;
                droppedMessages = 0;
                averageQueueTime = 0.0f;
                maxQueueTime = 0.0f;
            }
        };
        
        QueueStats GetStats() const;
        void ResetStats();

    private:
        // Internal comparison for priority queue
        struct MessageComparator
        {
            bool operator()(const PrioritizedMessage& a, const PrioritizedMessage& b) const;
        };

        // Priority queue
        std::priority_queue<PrioritizedMessage, std::vector<PrioritizedMessage>, MessageComparator> m_messageQueue;
        
        // Message classifications
        std::map<Networking::MessageTypes, MessageClassification> m_messageClassifications;
        
        // Statistics
        QueueStats m_stats;
        
        // Configuration
        size_t m_maxQueueSize;
        float m_maxMessageAge;
        
        // Utility methods
        float CalculateMessageScore(const PrioritizedMessage& message) const;
        bool IsMessageExpired(const PrioritizedMessage& message) const;
        void UpdateMessageAge(PrioritizedMessage& message);
    };

    // Network traffic manager
    class NetworkTrafficManager
    {
    public:
        NetworkTrafficManager();
        ~NetworkTrafficManager();

        // Initialize traffic management
        bool Initialize();
        void Shutdown();

        // Message processing
        void ProcessMessage(const PrioritizedMessage& message);
        std::vector<PrioritizedMessage> GetMessagesToSend(size_t maxCount);
        void MarkMessageSent(uint32_t messageId);
        void MarkMessageFailed(uint32_t messageId);

        // Traffic control
        void SetBandwidthLimit(size_t bytesPerSecond);
        void SetMessageRateLimit(size_t messagesPerSecond);
        void EnableTrafficShaping(bool enable);
        void SetCongestionThreshold(float threshold);

        // Priority management
        void SetMessagePriority(Networking::MessageTypes type, MessagePriority priority);
        void SetMessageUrgency(Networking::MessageTypes type, MessageUrgency urgency);
        void SetMessageClassification(Networking::MessageTypes type, const MessageClassification& classification);

        // Statistics
        struct TrafficStats
        {
            size_t totalMessagesProcessed = 0;
            size_t messagesSent = 0;
            size_t messagesDropped = 0;
            size_t messagesRetried = 0;
            size_t bytesSent = 0;
            size_t bytesDropped = 0;
            float averageLatency = 0.0f;
            float maxLatency = 0.0f;
            float bandwidthUtilization = 0.0f;
            float congestionLevel = 0.0f;
            
            void Reset()
            {
                totalMessagesProcessed = 0;
                messagesSent = 0;
                messagesDropped = 0;
                messagesRetried = 0;
                bytesSent = 0;
                bytesDropped = 0;
                averageLatency = 0.0f;
                maxLatency = 0.0f;
                bandwidthUtilization = 0.0f;
                congestionLevel = 0.0f;
            }
        };
        
        TrafficStats GetStats() const;
        void ResetStats();
        void PrintStats() const;

        // Configuration
        void SetConfig(const std::map<Networking::MessageTypes, MessageClassification>& classifications);
        void LoadDefaultConfigurations();

    private:
        // Internal methods
        void UpdateTrafficStats();
        void CleanupExpiredMessages();
        void HandleCongestion();
        bool CanSendMessage(const PrioritizedMessage& message) const;
        void DropLowPriorityMessages();
        
        // Member variables
        bool m_initialized;
        MessagePriorityQueue m_priorityQueue;
        TrafficStats m_stats;
        
        // Traffic control
        size_t m_bandwidthLimit;
        size_t m_messageRateLimit;
        bool m_trafficShapingEnabled;
        float m_congestionThreshold;
        
        // Timing
        std::chrono::high_resolution_clock::time_point m_lastUpdateTime;
        std::chrono::high_resolution_clock::time_point m_lastBandwidthReset;
        std::chrono::high_resolution_clock::time_point m_lastRateReset;
        
        // Counters
        size_t m_bytesSentThisSecond;
        size_t m_messagesSentThisSecond;
    };

    // Message priority utilities
    namespace PriorityUtils
    {
        // Default message classifications
        MessageClassification GetDefaultClassification(Networking::MessageTypes type);
        MessagePriority GetDefaultPriority(Networking::MessageTypes type);
        MessageUrgency GetDefaultUrgency(Networking::MessageTypes type);
        
        // Message importance calculation
        float CalculateImportance(const PrioritizedMessage& message);
        float CalculateUrgency(const PrioritizedMessage& message);
        
        // Priority comparison
        bool IsHigherPriority(const PrioritizedMessage& a, const PrioritizedMessage& b);
        bool IsMoreUrgent(const PrioritizedMessage& a, const PrioritizedMessage& b);
        
        // Message filtering
        std::vector<PrioritizedMessage> FilterByPriority(const std::vector<PrioritizedMessage>& messages, 
                                                         MessagePriority priority);
        std::vector<PrioritizedMessage> FilterByUrgency(const std::vector<PrioritizedMessage>& messages, 
                                                        MessageUrgency urgency);
        std::vector<PrioritizedMessage> FilterByAge(const std::vector<PrioritizedMessage>& messages, 
                                                    float maxAge);
        
        // Message sorting
        void SortByPriority(std::vector<PrioritizedMessage>& messages);
        void SortByUrgency(std::vector<PrioritizedMessage>& messages);
        void SortByImportance(std::vector<PrioritizedMessage>& messages);
        void SortByAge(std::vector<PrioritizedMessage>& messages);
        
        // Statistics
        void PrintPriorityDistribution(const std::vector<PrioritizedMessage>& messages);
        void PrintUrgencyDistribution(const std::vector<PrioritizedMessage>& messages);
    }
}
