#include "optimization/SmartBatching.h"
#include "utils/Logger.h"
#include <algorithm>
#include <chrono>

namespace Optimization
{
    // Concrete implementation of SmartBatching
    class SmartBatchingImpl : public SmartBatching
    {
    public:
        SmartBatchingImpl() : m_batchSize(1024), m_maxBatchTime(16), m_compressionEnabled(true)
        {
            LOG_INFO_CAT(LogCategory::NETWORK, "SmartBatchingImpl created");
            ResetStats();
        }

        virtual ~SmartBatchingImpl()
        {
            LOG_INFO_CAT(LogCategory::NETWORK, "SmartBatchingImpl destroyed");
        }

        void AddMessage(const BatchedMessage& message) override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            
            // Add message to current batch
            m_currentBatch.messages.push_back(message);
            m_currentBatch.totalSize += message.size;
            m_currentBatch.priority = std::max(m_currentBatch.priority, message.priority);

            // Check if batch should be sent
            if (ShouldSendBatch())
            {
                ProcessBatch();
            }

            m_stats.totalMessages++;
        }

        void ProcessBatch() override
        {
            if (m_currentBatch.messages.empty())
            {
                return;
            }

            auto startTime = std::chrono::high_resolution_clock::now();

            // Sort messages by priority
            std::sort(m_currentBatch.messages.begin(), m_currentBatch.messages.end(),
                [](const BatchedMessage& a, const BatchedMessage& b) {
                    return a.priority > b.priority;
                });

            // Compress batch if enabled
            std::vector<uint8_t> compressedData;
            if (m_compressionEnabled)
            {
                compressedData = CompressBatch(m_currentBatch);
            }
            else
            {
                compressedData = SerializeBatch(m_currentBatch);
            }

            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

            // Update statistics
            m_stats.totalBatches++;
            m_stats.totalBytes += m_currentBatch.totalSize;
            m_stats.compressedBytes += compressedData.size();
            m_stats.compressionTime += duration;

            if (compressedData.size() < m_currentBatch.totalSize)
            {
                m_stats.compressedBatches++;
            }

            // Send batch
            SendBatch(compressedData);

            // Clear current batch
            m_currentBatch = BatchedMessageGroup();
        }

        void SetBatchSize(size_t size) override
        {
            m_batchSize = size;
            LOG_INFO_CAT(LogCategory::NETWORK, "Batch size set to: " + std::to_string(size));
        }

        void SetMaxBatchTime(uint32_t milliseconds) override
        {
            m_maxBatchTime = milliseconds;
            LOG_INFO_CAT(LogCategory::NETWORK, "Max batch time set to: " + std::to_string(milliseconds) + "ms");
        }

        void SetCompressionEnabled(bool enabled) override
        {
            m_compressionEnabled = enabled;
            LOG_INFO_CAT(LogCategory::NETWORK, "Batch compression " + (enabled ? "enabled" : "disabled"));
        }

        BatchedMessageGroup GetCurrentBatch() const override
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_currentBatch;
        }

        BatchedMessageGroup CreateBatch(const std::vector<BatchedMessage>& messages) override
        {
            BatchedMessageGroup batch;
            batch.messages = messages;
            batch.totalSize = 0;
            batch.priority = 0;

            for (const auto& msg : messages)
            {
                batch.totalSize += msg.size;
                batch.priority = std::max(batch.priority, msg.priority);
            }

            return batch;
        }

        std::vector<uint8_t> SerializeBatch(const BatchedMessageGroup& batch) override
        {
            std::vector<uint8_t> data;
            data.reserve(batch.totalSize + 1024); // Reserve extra space for headers

            // Serialize batch header
            uint32_t messageCount = static_cast<uint32_t>(batch.messages.size());
            data.insert(data.end(), reinterpret_cast<const uint8_t*>(&messageCount), 
                       reinterpret_cast<const uint8_t*>(&messageCount) + sizeof(uint32_t));
            
            data.insert(data.end(), reinterpret_cast<const uint8_t*>(&batch.totalSize), 
                       reinterpret_cast<const uint8_t*>(&batch.totalSize) + sizeof(uint32_t));
            
            data.insert(data.end(), reinterpret_cast<const uint8_t*>(&batch.priority), 
                       reinterpret_cast<const uint8_t*>(&batch.priority) + sizeof(uint32_t));

            // Serialize messages
            for (const auto& msg : batch.messages)
            {
                // Message header
                data.insert(data.end(), reinterpret_cast<const uint8_t*>(&msg.id), 
                           reinterpret_cast<const uint8_t*>(&msg.id) + sizeof(uint32_t));
                
                data.insert(data.end(), reinterpret_cast<const uint8_t*>(&msg.size), 
                           reinterpret_cast<const uint8_t*>(&msg.size) + sizeof(uint32_t));
                
                data.insert(data.end(), reinterpret_cast<const uint8_t*>(&msg.priority), 
                           reinterpret_cast<const uint8_t*>(&msg.priority) + sizeof(uint32_t));

                // Message data
                data.insert(data.end(), msg.data.begin(), msg.data.end());
            }

            return data;
        }

        BatchedMessageGroup DeserializeBatch(const std::vector<uint8_t>& data) override
        {
            BatchedMessageGroup batch;
            size_t offset = 0;

            if (data.size() < 12) // Minimum header size
            {
                LOG_ERROR_CAT(LogCategory::NETWORK, "Invalid batch data: too small");
                return batch;
            }

            // Deserialize batch header
            uint32_t messageCount = *reinterpret_cast<const uint32_t*>(data.data() + offset);
            offset += sizeof(uint32_t);
            
            batch.totalSize = *reinterpret_cast<const uint32_t*>(data.data() + offset);
            offset += sizeof(uint32_t);
            
            batch.priority = *reinterpret_cast<const uint32_t*>(data.data() + offset);
            offset += sizeof(uint32_t);

            // Deserialize messages
            batch.messages.reserve(messageCount);
            for (uint32_t i = 0; i < messageCount; i++)
            {
                if (offset + 12 > data.size()) // Message header size
                {
                    LOG_ERROR_CAT(LogCategory::NETWORK, "Invalid batch data: incomplete message header");
                    break;
                }

                BatchedMessage msg;
                msg.id = *reinterpret_cast<const uint32_t*>(data.data() + offset);
                offset += sizeof(uint32_t);
                
                msg.size = *reinterpret_cast<const uint32_t*>(data.data() + offset);
                offset += sizeof(uint32_t);
                
                msg.priority = *reinterpret_cast<const uint32_t*>(data.data() + offset);
                offset += sizeof(uint32_t);

                if (offset + msg.size > data.size())
                {
                    LOG_ERROR_CAT(LogCategory::NETWORK, "Invalid batch data: incomplete message data");
                    break;
                }

                msg.data.assign(data.begin() + offset, data.begin() + offset + msg.size);
                offset += msg.size;

                batch.messages.push_back(msg);
            }

            return batch;
        }

        BatchedMessageGroup OptimizeBatch(const BatchedMessageGroup& batch) override
        {
            BatchedMessageGroup optimizedBatch = batch;

            // Sort by priority (highest first)
            std::sort(optimizedBatch.messages.begin(), optimizedBatch.messages.end(),
                [](const BatchedMessage& a, const BatchedMessage& b) {
                    return a.priority > b.priority;
                });

            // Remove duplicate messages (same ID and data)
            optimizedBatch.messages.erase(
                std::unique(optimizedBatch.messages.begin(), optimizedBatch.messages.end(),
                    [](const BatchedMessage& a, const BatchedMessage& b) {
                        return a.id == b.id && a.data == b.data;
                    }),
                optimizedBatch.messages.end()
            );

            // Recalculate total size
            optimizedBatch.totalSize = 0;
            for (const auto& msg : optimizedBatch.messages)
            {
                optimizedBatch.totalSize += msg.size;
            }

            return optimizedBatch;
        }

        BatchedMessageGroup MergeBatches(const std::vector<BatchedMessageGroup>& batches) override
        {
            BatchedMessageGroup mergedBatch;
            mergedBatch.priority = 0;
            mergedBatch.totalSize = 0;

            for (const auto& batch : batches)
            {
                mergedBatch.messages.insert(mergedBatch.messages.end(), 
                                          batch.messages.begin(), batch.messages.end());
                mergedBatch.totalSize += batch.totalSize;
                mergedBatch.priority = std::max(mergedBatch.priority, batch.priority);
            }

            return mergedBatch;
        }

        void SplitBatch(const BatchedMessageGroup& batch, size_t maxSize, 
                       std::vector<BatchedMessageGroup>& outputBatches) override
        {
            outputBatches.clear();
            BatchedMessageGroup currentBatch;
            currentBatch.priority = batch.priority;

            for (const auto& msg : batch.messages)
            {
                if (currentBatch.totalSize + msg.size > maxSize && !currentBatch.messages.empty())
                {
                    outputBatches.push_back(currentBatch);
                    currentBatch = BatchedMessageGroup();
                    currentBatch.priority = batch.priority;
                }

                currentBatch.messages.push_back(msg);
                currentBatch.totalSize += msg.size;
            }

            if (!currentBatch.messages.empty())
            {
                outputBatches.push_back(currentBatch);
            }
        }

        BatchedMessageGroup FilterBatch(const BatchedMessageGroup& batch, 
                                       std::function<bool(const BatchedMessage&)> filter) override
        {
            BatchedMessageGroup filteredBatch;
            filteredBatch.priority = batch.priority;
            filteredBatch.totalSize = 0;

            for (const auto& msg : batch.messages)
            {
                if (filter(msg))
                {
                    filteredBatch.messages.push_back(msg);
                    filteredBatch.totalSize += msg.size;
                }
            }

            return filteredBatch;
        }

        BatchedMessageGroup SortBatch(const BatchedMessageGroup& batch, 
                                     std::function<bool(const BatchedMessage&, const BatchedMessage&)> comparator) override
        {
            BatchedMessageGroup sortedBatch = batch;
            std::sort(sortedBatch.messages.begin(), sortedBatch.messages.end(), comparator);
            return sortedBatch;
        }

        BatchedMessageGroup CompressBatch(const BatchedMessageGroup& batch) override
        {
            if (!m_compressionEnabled)
            {
                return batch;
            }

            auto startTime = std::chrono::high_resolution_clock::now();

            // Serialize batch
            std::vector<uint8_t> serializedData = SerializeBatch(batch);
            
            // Compress data
            auto& compression = DataCompression::GetInstance();
            std::vector<uint8_t> compressedData = compression.Compress(serializedData, 
                                                                      CompressionAlgorithm::LZ4, 
                                                                      CompressionLevel::Balanced);

            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

            // Update compression statistics
            m_stats.compressionTime += duration;
            if (compressedData.size() < serializedData.size())
            {
                m_stats.compressedBatches++;
            }

            // Create compressed batch
            BatchedMessageGroup compressedBatch;
            compressedBatch.messages = batch.messages;
            compressedBatch.totalSize = compressedData.size();
            compressedBatch.priority = batch.priority;
            compressedBatch.compressed = true;
            compressedBatch.compressedData = compressedData;

            return compressedBatch;
        }

        BatchedMessageGroup DecompressBatch(const BatchedMessageGroup& batch) override
        {
            if (!batch.compressed || batch.compressedData.empty())
            {
                return batch;
            }

            auto startTime = std::chrono::high_resolution_clock::now();

            // Decompress data
            auto& compression = DataCompression::GetInstance();
            std::vector<uint8_t> decompressedData = compression.Decompress(batch.compressedData, 
                                                                          CompressionAlgorithm::LZ4);

            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

            // Update decompression statistics
            m_stats.decompressionTime += duration;

            // Deserialize batch
            BatchedMessageGroup decompressedBatch = DeserializeBatch(decompressedData);
            decompressedBatch.compressed = false;

            return decompressedBatch;
        }

        BatchedMessageGroup CreatePriorityBatch(const std::vector<BatchedMessage>& messages, 
                                               uint32_t priority) override
        {
            BatchedMessageGroup batch;
            batch.priority = priority;
            batch.totalSize = 0;

            for (const auto& msg : messages)
            {
                if (msg.priority >= priority)
                {
                    batch.messages.push_back(msg);
                    batch.totalSize += msg.size;
                }
            }

            return batch;
        }

        BatchedMessageGroup CreateSizeBatch(const std::vector<BatchedMessage>& messages, 
                                           size_t maxSize) override
        {
            BatchedMessageGroup batch;
            batch.totalSize = 0;

            for (const auto& msg : messages)
            {
                if (batch.totalSize + msg.size <= maxSize)
                {
                    batch.messages.push_back(msg);
                    batch.totalSize += msg.size;
                    batch.priority = std::max(batch.priority, msg.priority);
                }
            }

            return batch;
        }

        BatchedMessageGroup CreateTimeBatch(const std::vector<BatchedMessage>& messages, 
                                           uint32_t maxAge) override
        {
            auto currentTime = std::chrono::high_resolution_clock::now();
            BatchedMessageGroup batch;
            batch.totalSize = 0;

            for (const auto& msg : messages)
            {
                auto messageAge = std::chrono::duration_cast<std::chrono::milliseconds>(
                    currentTime - msg.timestamp).count();
                
                if (messageAge <= maxAge)
                {
                    batch.messages.push_back(msg);
                    batch.totalSize += msg.size;
                    batch.priority = std::max(batch.priority, msg.priority);
                }
            }

            return batch;
        }

        void ResetStats() override
        {
            m_stats = BatchedMessageStats();
        }

        BatchedMessageStats GetStats() const override
        {
            return m_stats;
        }

        float GetEfficiency() const override
        {
            if (m_stats.totalBatches == 0)
            {
                return 0.0f;
            }

            float compressionRatio = static_cast<float>(m_stats.compressedBytes) / static_cast<float>(m_stats.totalBytes);
            float batchUtilization = static_cast<float>(m_stats.totalMessages) / static_cast<float>(m_stats.totalBatches);
            float droppedRatio = static_cast<float>(m_stats.droppedBatches) / static_cast<float>(m_stats.totalBatches);

            float efficiency = compressionRatio * batchUtilization * (1.0f - droppedRatio);
            return std::max(0.0f, std::min(1.0f, efficiency));
        }

        size_t CalculateBatchSize(const BatchedMessageGroup& batch) override
        {
            return batch.totalSize;
        }

        uint32_t CalculateBatchPriority(const BatchedMessageGroup& batch) override
        {
            return batch.priority;
        }

        bool ShouldSendBatch() const override
        {
            return m_currentBatch.totalSize >= m_batchSize || 
                   m_currentBatch.messages.size() >= 10; // Max 10 messages per batch
        }

        void SendBatch(const std::vector<uint8_t>& data) override
        {
            // This would send the batch data over the network
            // For now, just log the action
            LOG_DEBUG_CAT(LogCategory::NETWORK, "Sending batch of " + std::to_string(data.size()) + " bytes");
        }

    private:
        mutable std::mutex m_mutex;
        BatchedMessageGroup m_currentBatch;
        size_t m_batchSize;
        uint32_t m_maxBatchTime;
        bool m_compressionEnabled;
        BatchedMessageStats m_stats;
    };

    // Factory function to create SmartBatching instance
    std::unique_ptr<SmartBatching> CreateSmartBatching()
    {
        return std::make_unique<SmartBatchingImpl>();
    }
}
