#include "optimization/NetworkOptimizer.h"
#include "optimization/DataCompression.h"
#include "utils/Logger.h"
#include <iostream>
#include <chrono>

namespace Optimization
{
    // MessageType is defined in the header file

    Networking::message<MessageType> NetworkOptimizer::CompressMessage(const Networking::message<MessageType>& originalMessage)
    {
        LOG_DEBUG_CAT(LogCategory::NETWORK, "Compressing message of type " + std::to_string(static_cast<int>(originalMessage.header.id)));
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        Networking::message<MessageType> compressedMessage = originalMessage;
        compressedMessage.body = ApplyCompression(originalMessage.body);
        compressedMessage.header.size = compressedMessage.size();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto compressionTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        
        LOG_DEBUG_CAT(LogCategory::NETWORK, "Message compressed in " + std::to_string(compressionTime) + "μs");
        
        return compressedMessage;
    }

    Networking::message<MessageType> NetworkOptimizer::DecompressMessage(const Networking::message<MessageType>& compressedMessage)
    {
        LOG_DEBUG_CAT(LogCategory::NETWORK, "Decompressing message of type " + std::to_string(static_cast<int>(compressedMessage.header.id)));
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        Networking::message<MessageType> decompressedMessage = compressedMessage;
        decompressedMessage.body = ApplyDecompression(compressedMessage.body);
        decompressedMessage.header.size = decompressedMessage.size();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto decompressionTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
        
        LOG_DEBUG_CAT(LogCategory::NETWORK, "Message decompressed in " + std::to_string(decompressionTime) + "μs");
        
        return decompressedMessage;
    }

    std::vector<uint8_t> NetworkOptimizer::ApplyCompression(const std::vector<uint8_t>& data)
    {
        if (data.empty())
        {
            return data;
        }
        
        // Use real compression from DataCompression.cpp
        auto& compression = DataCompression::GetInstance();
        
        // Choose compression algorithm based on data size
        CompressionAlgorithm algorithm = CompressionAlgorithm::LZ4;
        CompressionLevel level = CompressionLevel::Balanced;
        
        if (data.size() < 1024)
        {
            algorithm = CompressionAlgorithm::LZ4;
            level = CompressionLevel::Fast;
        }
        else if (data.size() < 10240)
        {
            algorithm = CompressionAlgorithm::LZ4;
            level = CompressionLevel::Balanced;
        }
        else
        {
            algorithm = CompressionAlgorithm::LZ4HC;
            level = CompressionLevel::High;
        }
        
        auto compressed = compression.Compress(data, algorithm, level);
        
        // Log compression statistics
        if (compressed.size() < data.size())
        {
            float ratio = (float)compressed.size() / (float)data.size();
            LOG_DEBUG_CAT(LogCategory::NETWORK, "Compression ratio: " + std::to_string(ratio * 100.0f) + "%");
        }
        
        return compressed;
    }

    std::vector<uint8_t> NetworkOptimizer::ApplyDecompression(const std::vector<uint8_t>& data)
    {
        if (data.empty())
        {
            return data;
        }
        
        // Use real decompression from DataCompression.cpp
        auto& compression = DataCompression::GetInstance();
        
        // Detect algorithm from compressed data
        CompressionAlgorithm algorithm = CompressionAlgorithm::LZ4;
        if (data.size() >= 8)
        {
            algorithm = static_cast<CompressionAlgorithm>(data[0]);
        }
        
        return compression.Decompress(data, algorithm);
    }

    // Batch compression for multiple messages
    std::vector<uint8_t> NetworkOptimizer::CompressBatch(const std::vector<Networking::message<MessageType>>& messages)
    {
        if (messages.empty())
        {
            return {};
        }
        
        LOG_DEBUG_CAT(LogCategory::NETWORK, "Compressing batch of " + std::to_string(messages.size()) + " messages");
        
        auto& compression = DataCompression::GetInstance();
        
        // Convert messages to byte vectors
        std::vector<std::vector<uint8_t>> packets;
        for (const auto& msg : messages)
        {
            std::vector<uint8_t> packet;
            packet.reserve(sizeof(Networking::message_header<MessageType>) + msg.body.size());
            
            // Add header
            const uint8_t* headerBytes = reinterpret_cast<const uint8_t*>(&msg.header);
            packet.insert(packet.end(), headerBytes, headerBytes + sizeof(Networking::message_header<MessageType>));
            
            // Add body
            packet.insert(packet.end(), msg.body.begin(), msg.body.end());
            
            packets.push_back(packet);
        }
        
        return compression.CompressBatch(packets);
    }

    std::vector<Networking::message<MessageType>> NetworkOptimizer::DecompressBatch(const std::vector<uint8_t>& compressedBatch)
    {
        if (compressedBatch.empty())
        {
            return {};
        }
        
        LOG_DEBUG_CAT(LogCategory::NETWORK, "Decompressing batch");
        
        auto& compression = DataCompression::GetInstance();
        auto packets = compression.DecompressBatch(compressedBatch);
        
        std::vector<Networking::message<MessageType>> messages;
        for (const auto& packet : packets)
        {
            if (packet.size() < sizeof(Networking::message_header<MessageType>))
            {
                continue;
            }
            
            Networking::message<MessageType> msg;
            
            // Extract header
            std::memcpy(&msg.header, packet.data(), sizeof(Networking::message_header<MessageType>));
            
            // Extract body
            msg.body.assign(packet.begin() + sizeof(Networking::message_header<MessageType>), packet.end());
            
            messages.push_back(msg);
        }
        
        return messages;
    }

    // Smart compression based on message type
    std::vector<uint8_t> NetworkOptimizer::SmartCompress(const std::vector<uint8_t>& data, MessageType messageType)
    {
        if (data.empty())
        {
            return data;
        }
        
        auto& compression = DataCompression::GetInstance();
        
        // Choose compression based on message type
        switch (messageType)
        {
            case MessageType::PlayerMove:
            case MessageType::TC_UPDATE_POS:
                // Position data - use fast compression
                return compression.Compress(data, CompressionAlgorithm::LZ4, CompressionLevel::Fast);
                
            case MessageType::WorldUpdate:
            case MessageType::GameStateSync:
                // Large data - use high compression
                return compression.Compress(data, CompressionAlgorithm::LZ4HC, CompressionLevel::High);
                
            case MessageType::ChatMessage:
            case MessageType::TC_CHAT_MESSAGE:
                // Text data - use balanced compression
                return compression.Compress(data, CompressionAlgorithm::LZ4, CompressionLevel::Balanced);
                
            default:
                // Default compression
                return compression.Compress(data, CompressionAlgorithm::LZ4, CompressionLevel::Balanced);
        }
    }
}