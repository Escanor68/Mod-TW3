#include "optimization/NetworkOptimizer.h"
#include "optimization/DataCompression.h"
#include <iostream>

namespace Optimization
{
    // MessageType is defined in the header file

    Networking::message<MessageType> NetworkOptimizer::CompressMessage(const Networking::message<MessageType>& originalMessage)
    {
        std::cout << "Compressing message..." << std::endl;
        // Placeholder for actual compression logic
        Networking::message<MessageType> compressedMessage = originalMessage;
        compressedMessage.body = ApplyCompression(originalMessage.body);
        compressedMessage.header.size = compressedMessage.size();
        return compressedMessage;
    }

    Networking::message<MessageType> NetworkOptimizer::DecompressMessage(const Networking::message<MessageType>& compressedMessage)
    {
        std::cout << "Decompressing message..." << std::endl;
        // Placeholder for actual decompression logic
        Networking::message<MessageType> decompressedMessage = compressedMessage;
        decompressedMessage.body = ApplyDecompression(compressedMessage.body);
        decompressedMessage.header.size = decompressedMessage.size();
        return decompressedMessage;
    }

    std::vector<uint8_t> NetworkOptimizer::ApplyCompression(const std::vector<uint8_t>& data)
    {
        // Use real compression from DataCompression.cpp
        auto& compression = DataCompression::GetInstance();
        return compression.Compress(data, CompressionAlgorithm::LZ4, CompressionLevel::Balanced);
    }

    std::vector<uint8_t> NetworkOptimizer::ApplyDecompression(const std::vector<uint8_t>& data)
    {
        // Use real decompression from DataCompression.cpp
        auto& compression = DataCompression::GetInstance();
        return compression.Decompress(data, CompressionAlgorithm::LZ4);
    }
}