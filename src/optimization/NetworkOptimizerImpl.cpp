#include "optimization/NetworkOptimizer.h"
#include "optimization/DataCompression.h"
#include "utils/Logger.h"
#include <chrono>

namespace Optimization
{
    // Concrete implementation of NetworkOptimizer
    class NetworkOptimizerImpl : public NetworkOptimizer
    {
    public:
        NetworkOptimizerImpl() : m_compressionEnabled(true)
        {
            LOG_INFO_CAT(LogCategory::NETWORK, "NetworkOptimizerImpl created");
        }

        virtual ~NetworkOptimizerImpl()
        {
            LOG_INFO_CAT(LogCategory::NETWORK, "NetworkOptimizerImpl destroyed");
        }

        Networking::message<MessageType> CompressMessage(const Networking::message<MessageType>& originalMessage) override
        {
            if (!m_compressionEnabled)
            {
                return originalMessage;
            }

            auto startTime = std::chrono::high_resolution_clock::now();

            // Convert message to bytes
            std::vector<uint8_t> messageData = SerializeMessage(originalMessage);
            
            // Compress the data
            auto& compression = DataCompression::GetInstance();
            std::vector<uint8_t> compressedData = compression.Compress(messageData, CompressionAlgorithm::LZ4, CompressionLevel::Balanced);

            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

            // Log compression stats
            float compressionRatio = static_cast<float>(compressedData.size()) / static_cast<float>(messageData.size());
            LOG_DEBUG_CAT(LogCategory::NETWORK, "Message compressed: " + std::to_string(messageData.size()) + " -> " + 
                         std::to_string(compressedData.size()) + " bytes (ratio: " + std::to_string(compressionRatio) + ")");

            // Create compressed message
            Networking::message<MessageType> compressedMessage;
            compressedMessage.header.id = originalMessage.header.id;
            compressedMessage.header.size = static_cast<uint32_t>(compressedData.size());
            compressedMessage.body = compressedData;

            return compressedMessage;
        }

        Networking::message<MessageType> DecompressMessage(const Networking::message<MessageType>& compressedMessage) override
        {
            if (!m_compressionEnabled)
            {
                return compressedMessage;
            }

            auto startTime = std::chrono::high_resolution_clock::now();

            // Decompress the data
            auto& compression = DataCompression::GetInstance();
            std::vector<uint8_t> decompressedData = compression.Decompress(compressedMessage.body, CompressionAlgorithm::LZ4);

            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

            LOG_DEBUG_CAT(LogCategory::NETWORK, "Message decompressed: " + std::to_string(compressedMessage.body.size()) + " -> " + 
                         std::to_string(decompressedData.size()) + " bytes");

            // Create decompressed message
            Networking::message<MessageType> decompressedMessage;
            decompressedMessage.header.id = compressedMessage.header.id;
            decompressedMessage.header.size = static_cast<uint32_t>(decompressedData.size());
            decompressedMessage.body = decompressedData;

            return decompressedMessage;
        }

        void SetCompressionEnabled(bool enabled)
        {
            m_compressionEnabled = enabled;
            LOG_INFO_CAT(LogCategory::NETWORK, "Compression " + (enabled ? "enabled" : "disabled"));
        }

        bool IsCompressionEnabled() const
        {
            return m_compressionEnabled;
        }

        // Additional utility methods
        std::vector<uint8_t> CompressBatch(const std::vector<Networking::message<MessageType>>& messages)
        {
            if (messages.empty())
            {
                return {};
            }

            // Serialize all messages into a single buffer
            std::vector<uint8_t> batchData;
            for (const auto& msg : messages)
            {
                auto msgData = SerializeMessage(msg);
                batchData.insert(batchData.end(), msgData.begin(), msgData.end());
            }

            // Compress the batch
            auto& compression = DataCompression::GetInstance();
            return compression.Compress(batchData, CompressionAlgorithm::LZ4, CompressionLevel::High);
        }

        std::vector<Networking::message<MessageType>> DecompressBatch(const std::vector<uint8_t>& compressedBatch)
        {
            if (compressedBatch.empty())
            {
                return {};
            }

            // Decompress the batch
            auto& compression = DataCompression::GetInstance();
            std::vector<uint8_t> decompressedBatch = compression.Decompress(compressedBatch, CompressionAlgorithm::LZ4);

            // Deserialize messages from the batch
            std::vector<Networking::message<MessageType>> messages;
            size_t offset = 0;
            while (offset < decompressedBatch.size())
            {
                Networking::message<MessageType> msg;
                if (DeserializeMessage(decompressedBatch, offset, msg))
                {
                    messages.push_back(msg);
                }
                else
                {
                    LOG_ERROR_CAT(LogCategory::NETWORK, "Failed to deserialize message from batch");
                    break;
                }
            }

            return messages;
        }

        std::vector<uint8_t> SmartCompress(const std::vector<uint8_t>& data, MessageType messageType)
        {
            if (data.empty())
            {
                return data;
            }

            auto& compression = DataCompression::GetInstance();

            // Choose compression based on message type
            switch (messageType)
            {
                case MessageType::ClientConnect:
                case MessageType::ClientDisconnect:
                    // Connection messages - use fast compression
                    return compression.Compress(data, CompressionAlgorithm::LZ4, CompressionLevel::Fast);

                case MessageType::ClientMessage:
                    // Regular messages - use balanced compression
                    return compression.Compress(data, CompressionAlgorithm::LZ4, CompressionLevel::Balanced);

                default:
                    // Default compression
                    return compression.Compress(data, CompressionAlgorithm::LZ4, CompressionLevel::Balanced);
            }
        }

    private:
        std::vector<uint8_t> SerializeMessage(const Networking::message<MessageType>& msg)
        {
            std::vector<uint8_t> data;
            
            // Serialize header
            data.push_back(static_cast<uint8_t>(msg.header.id));
            data.push_back(static_cast<uint8_t>(msg.header.id >> 8));
            data.push_back(static_cast<uint8_t>(msg.header.id >> 16));
            data.push_back(static_cast<uint8_t>(msg.header.id >> 24));
            
            data.push_back(static_cast<uint8_t>(msg.header.size));
            data.push_back(static_cast<uint8_t>(msg.header.size >> 8));
            data.push_back(static_cast<uint8_t>(msg.header.size >> 16));
            data.push_back(static_cast<uint8_t>(msg.header.size >> 24));

            // Serialize body
            data.insert(data.end(), msg.body.begin(), msg.body.end());

            return data;
        }

        bool DeserializeMessage(const std::vector<uint8_t>& data, size_t& offset, Networking::message<MessageType>& msg)
        {
            if (offset + 8 > data.size())
            {
                return false;
            }

            // Deserialize header
            msg.header.id = static_cast<MessageType>(data[offset] | (data[offset + 1] << 8) | 
                                                   (data[offset + 2] << 16) | (data[offset + 3] << 24));
            offset += 4;

            msg.header.size = data[offset] | (data[offset + 1] << 8) | 
                            (data[offset + 2] << 16) | (data[offset + 3] << 24);
            offset += 4;

            // Deserialize body
            if (offset + msg.header.size > data.size())
            {
                return false;
            }

            msg.body.assign(data.begin() + offset, data.begin() + offset + msg.header.size);
            offset += msg.header.size;

            return true;
        }

        bool m_compressionEnabled;
    };

    // Factory function to create NetworkOptimizer instance
    std::unique_ptr<NetworkOptimizer> CreateNetworkOptimizer()
    {
        return std::make_unique<NetworkOptimizerImpl>();
    }
}
