#include "optimization/MessagePrioritySystem.h"
#include "utils/Logger.h"
#include <algorithm>
#include <chrono>
#include <queue>

namespace Optimization
{
    // Concrete implementation of MessagePrioritySystem
    class MessagePrioritySystemImpl : public MessagePrioritySystem
    {
    public:
        MessagePrioritySystemImpl() : m_maxQueueSize(1000), m_priorityThreshold(0.3f)
        {
            LOG_INFO_CAT(LogCategory::NETWORK, "MessagePrioritySystemImpl created");
            ResetStats();
        }

        virtual ~MessagePrioritySystemImpl()
        {
            LOG_INFO_CAT(LogCategory::NETWORK, "MessagePrioritySystemImpl destroyed");
        }

        void AddMessage(const PrioritizedMessage& message) override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            // Calculate priority if not set
            PrioritizedMessage msg = message;
            if (msg.priority < 0.0f)
            {
                msg.priority = CalculatePriority(msg.messageType, msg.timestamp, msg.size);
            }

            // Add to priority queue
            m_messageQueue.push(msg);
            m_stats.totalMessages++;

            // Check if queue is full
            if (m_messageQueue.size() > m_maxQueueSize)
            {
                DropLowPriorityMessages();
            }
        }

        std::vector<PrioritizedMessage> GetMessagesByPriority(uint32_t count) override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            std::vector<PrioritizedMessage> messages;
            messages.reserve(count);

            // Create a temporary queue to maintain order
            std::priority_queue<PrioritizedMessage> tempQueue = m_messageQueue;
            
            for (uint32_t i = 0; i < count && !tempQueue.empty(); i++)
            {
                messages.push_back(tempQueue.top());
                tempQueue.pop();
            }

            return messages;
        }

        std::vector<PrioritizedMessage> GetMessagesByType(MessageType type, uint32_t count) override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            std::vector<PrioritizedMessage> messages;
            messages.reserve(count);

            // Create a temporary queue to maintain order
            std::priority_queue<PrioritizedMessage> tempQueue = m_messageQueue;
            
            while (!tempQueue.empty() && messages.size() < count)
            {
                const auto& msg = tempQueue.top();
                if (msg.messageType == type)
                {
                    messages.push_back(msg);
                }
                tempQueue.pop();
            }

            return messages;
        }

        std::vector<PrioritizedMessage> GetMessagesAbovePriority(float priority, uint32_t count) override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            std::vector<PrioritizedMessage> messages;
            messages.reserve(count);

            // Create a temporary queue to maintain order
            std::priority_queue<PrioritizedMessage> tempQueue = m_messageQueue;
            
            while (!tempQueue.empty() && messages.size() < count)
            {
                const auto& msg = tempQueue.top();
                if (msg.priority >= priority)
                {
                    messages.push_back(msg);
                }
                tempQueue.pop();
            }

            return messages;
        }

        void RemoveMessage(uint32_t messageId) override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            // Create a new queue without the specified message
            std::priority_queue<PrioritizedMessage> newQueue;
            
            while (!m_messageQueue.empty())
            {
                const auto& msg = m_messageQueue.top();
                if (msg.id != messageId)
                {
                    newQueue.push(msg);
                }
                else
                {
                    m_stats.droppedMessages++;
                }
                m_messageQueue.pop();
            }
            
            m_messageQueue = newQueue;
        }

        void ClearExpiredMessages(uint32_t maxAge) override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::priority_queue<PrioritizedMessage> newQueue;
            
            while (!m_messageQueue.empty())
            {
                const auto& msg = m_messageQueue.top();
                auto age = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - msg.timestamp).count();
                
                if (age <= maxAge)
                {
                    newQueue.push(msg);
                }
                else
                {
                    m_stats.expiredMessages++;
                }
                m_messageQueue.pop();
            }
            
            m_messageQueue = newQueue;
        }

        void UpdateNetworkConditions(const NetworkConditions& conditions) override
        {
            m_networkConditions = conditions;
            LOG_DEBUG_CAT(LogCategory::NETWORK, "Network conditions updated: latency=" + 
                         std::to_string(conditions.latency) + "ms, packetLoss=" + 
                         std::to_string(conditions.packetLoss * 100.0f) + "%");
        }

        void SetMaxQueueSize(uint32_t size) override
        {
            m_maxQueueSize = size;
            LOG_INFO_CAT(LogCategory::NETWORK, "Max queue size set to: " + std::to_string(size));
        }

        void SetPriorityThreshold(float threshold) override
        {
            m_priorityThreshold = threshold;
            LOG_INFO_CAT(LogCategory::NETWORK, "Priority threshold set to: " + std::to_string(threshold));
        }

        uint32_t GetQueueSize() const override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return static_cast<uint32_t>(m_messageQueue.size());
        }

        bool IsQueueFull() const override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_messageQueue.size() >= m_maxQueueSize;
        }

        void ResetStats() override
        {
            m_stats = MessagePriorityStats();
        }

        MessagePriorityStats GetStats() const override
        {
            return m_stats;
        }

        float CalculatePriority(MessageType messageType, 
                               std::chrono::high_resolution_clock::time_point timestamp, 
                               size_t size) override
        {
            float priority = 0.0f;
            
            // Base priority from message type
            switch (messageType)
            {
                case MessageType::ClientConnect:
                case MessageType::ClientDisconnect:
                    priority = 1.0f; // Highest priority
                    break;
                case MessageType::PlayerAttack:
                case MessageType::PlayerDefend:
                    priority = 0.9f; // High priority
                    break;
                case MessageType::PlayerMove:
                    priority = 0.7f; // Medium-high priority
                    break;
                case MessageType::PlayerUpdate:
                    priority = 0.5f; // Medium priority
                    break;
                case MessageType::ChatMessage:
                    priority = 0.3f; // Low priority
                    break;
                default:
                    priority = 0.5f; // Default medium priority
                    break;
            }

            // Adjust priority based on age
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto age = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - timestamp).count();
            
            if (age > 100) // 100ms old
            {
                priority *= 0.8f; // Reduce priority for old messages
            }
            else if (age > 50) // 50ms old
            {
                priority *= 0.9f; // Slightly reduce priority
            }

            // Adjust priority based on size
            if (size > 1024) // Large message
            {
                priority *= 0.9f; // Slightly reduce priority for large messages
            }
            else if (size < 64) // Small message
            {
                priority *= 1.1f; // Increase priority for small messages
            }

            // Adjust priority based on network conditions
            if (m_networkConditions.latency > 100) // High latency
            {
                priority *= 1.1f; // Increase priority to compensate
            }
            else if (m_networkConditions.packetLoss > 0.05f) // High packet loss
            {
                priority *= 1.2f; // Increase priority significantly
            }

            return std::max(0.0f, std::min(1.0f, priority));
        }

        void DropLowPriorityMessages() override
        {
            std::priority_queue<PrioritizedMessage> newQueue;
            uint32_t droppedCount = 0;
            
            while (!m_messageQueue.empty())
            {
                const auto& msg = m_messageQueue.top();
                if (msg.priority >= m_priorityThreshold)
                {
                    newQueue.push(msg);
                }
                else
                {
                    droppedCount++;
                }
                m_messageQueue.pop();
            }
            
            m_messageQueue = newQueue;
            m_stats.droppedMessages += droppedCount;
            
            LOG_DEBUG_CAT(LogCategory::NETWORK, "Dropped " + std::to_string(droppedCount) + " low priority messages");
        }

        std::vector<PrioritizedMessage> GetTopMessages(uint32_t count) override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            std::vector<PrioritizedMessage> messages;
            messages.reserve(count);

            // Create a temporary queue to maintain order
            std::priority_queue<PrioritizedMessage> tempQueue = m_messageQueue;
            
            for (uint32_t i = 0; i < count && !tempQueue.empty(); i++)
            {
                messages.push_back(tempQueue.top());
                tempQueue.pop();
            }

            return messages;
        }

        void ProcessMessages(std::function<void(const PrioritizedMessage&)> processor) override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            while (!m_messageQueue.empty())
            {
                const auto& msg = m_messageQueue.top();
                processor(msg);
                m_messageQueue.pop();
                m_stats.messagesSent++;
            }
        }

        void ProcessMessagesByPriority(std::function<void(const PrioritizedMessage&)> processor, 
                                      float minPriority) override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            std::priority_queue<PrioritizedMessage> newQueue;
            
            while (!m_messageQueue.empty())
            {
                const auto& msg = m_messageQueue.top();
                if (msg.priority >= minPriority)
                {
                    processor(msg);
                    m_stats.messagesSent++;
                }
                else
                {
                    newQueue.push(msg);
                }
                m_messageQueue.pop();
            }
            
            m_messageQueue = newQueue;
        }

        void ProcessMessagesByType(MessageType type, 
                                  std::function<void(const PrioritizedMessage&)> processor) override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            std::priority_queue<PrioritizedMessage> newQueue;
            
            while (!m_messageQueue.empty())
            {
                const auto& msg = m_messageQueue.top();
                if (msg.messageType == type)
                {
                    processor(msg);
                    m_stats.messagesSent++;
                }
                else
                {
                    newQueue.push(msg);
                }
                m_messageQueue.pop();
            }
            
            m_messageQueue = newQueue;
        }

        void ProcessMessagesWithLimit(std::function<void(const PrioritizedMessage&)> processor, 
                                     uint32_t maxCount) override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            uint32_t processed = 0;
            std::priority_queue<PrioritizedMessage> newQueue;
            
            while (!m_messageQueue.empty() && processed < maxCount)
            {
                const auto& msg = m_messageQueue.top();
                processor(msg);
                m_messageQueue.pop();
                m_stats.messagesSent++;
                processed++;
            }
            
            // Put remaining messages back
            while (!m_messageQueue.empty())
            {
                newQueue.push(m_messageQueue.top());
                m_messageQueue.pop();
            }
            
            m_messageQueue = newQueue;
        }

        void ClearQueue() override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            uint32_t clearedCount = static_cast<uint32_t>(m_messageQueue.size());
            m_messageQueue = std::priority_queue<PrioritizedMessage>();
            m_stats.droppedMessages += clearedCount;
            
            LOG_INFO_CAT(LogCategory::NETWORK, "Cleared " + std::to_string(clearedCount) + " messages from queue");
        }

        std::vector<PrioritizedMessage> GetMessagesInRange(float minPriority, float maxPriority) override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            std::vector<PrioritizedMessage> messages;
            std::priority_queue<PrioritizedMessage> tempQueue = m_messageQueue;
            
            while (!tempQueue.empty())
            {
                const auto& msg = tempQueue.top();
                if (msg.priority >= minPriority && msg.priority <= maxPriority)
                {
                    messages.push_back(msg);
                }
                tempQueue.pop();
            }
            
            return messages;
        }

        void SetMessagePriority(uint32_t messageId, float priority) override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            std::priority_queue<PrioritizedMessage> newQueue;
            bool found = false;
            
            while (!m_messageQueue.empty())
            {
                auto msg = m_messageQueue.top();
                if (msg.id == messageId)
                {
                    msg.priority = priority;
                    found = true;
                }
                newQueue.push(msg);
                m_messageQueue.pop();
            }
            
            m_messageQueue = newQueue;
            
            if (found)
            {
                LOG_DEBUG_CAT(LogCategory::NETWORK, "Updated priority for message " + std::to_string(messageId) + " to " + std::to_string(priority));
            }
        }

        float GetAveragePriority() const override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            if (m_messageQueue.empty())
            {
                return 0.0f;
            }
            
            float totalPriority = 0.0f;
            std::priority_queue<PrioritizedMessage> tempQueue = m_messageQueue;
            
            while (!tempQueue.empty())
            {
                totalPriority += tempQueue.top().priority;
                tempQueue.pop();
            }
            
            return totalPriority / static_cast<float>(m_messageQueue.size());
        }

        uint32_t GetMessageCountByType(MessageType type) const override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            uint32_t count = 0;
            std::priority_queue<PrioritizedMessage> tempQueue = m_messageQueue;
            
            while (!tempQueue.empty())
            {
                if (tempQueue.top().messageType == type)
                {
                    count++;
                }
                tempQueue.pop();
            }
            
            return count;
        }

        uint32_t GetMessageCountAbovePriority(float priority) const override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            uint32_t count = 0;
            std::priority_queue<PrioritizedMessage> tempQueue = m_messageQueue;
            
            while (!tempQueue.empty())
            {
                if (tempQueue.top().priority >= priority)
                {
                    count++;
                }
                tempQueue.pop();
            }
            
            return count;
        }

    private:
        mutable std::mutex m_mutex;
        std::priority_queue<PrioritizedMessage> m_messageQueue;
        uint32_t m_maxQueueSize;
        float m_priorityThreshold;
        NetworkConditions m_networkConditions;
        MessagePriorityStats m_stats;
    };

    // Factory function to create MessagePrioritySystem instance
    std::unique_ptr<MessagePrioritySystem> CreateMessagePrioritySystem()
    {
        return std::make_unique<MessagePrioritySystemImpl>();
    }
}
