#include "optimization/DynamicNetworkOptimizer.h"
#include "optimization/DataCompression.h"
#include "utils/Logger.h"
#include <chrono>
#include <algorithm>

namespace Optimization
{
    DynamicNetworkOptimizer::DynamicNetworkOptimizer() 
        : m_compressionEnabled(true), m_adaptiveMode(true)
    {
        LOG_INFO_CAT(LogCategory::NETWORK, "DynamicNetworkOptimizer created");
        ResetStats();
    }

    DynamicNetworkOptimizer::~DynamicNetworkOptimizer()
    {
        LOG_INFO_CAT(LogCategory::NETWORK, "DynamicNetworkOptimizer destroyed");
    }

    Networking::message<MessageType> DynamicNetworkOptimizer::CompressMessage(const Networking::message<MessageType>& originalMessage)
    {
        if (!m_compressionEnabled)
        {
            return originalMessage;
        }

        auto startTime = std::chrono::high_resolution_clock::now();

        // Analyze message characteristics for dynamic optimization
        MessageCharacteristics characteristics = AnalyzeMessage(originalMessage);
        
        // Choose compression algorithm based on characteristics
        CompressionAlgorithm algorithm = ChooseCompressionAlgorithm(characteristics);
        CompressionLevel level = ChooseCompressionLevel(characteristics);

        // Convert message to bytes
        std::vector<uint8_t> messageData = SerializeMessage(originalMessage);
        
        // Compress the data
        auto& compression = DataCompression::GetInstance();
        std::vector<uint8_t> compressedData = compression.Compress(messageData, algorithm, level);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

        // Update statistics
        UpdateCompressionStats(messageData.size(), compressedData.size(), duration);

        // Create compressed message
        Networking::message<MessageType> compressedMessage;
        compressedMessage.header.id = originalMessage.header.id;
        compressedMessage.header.size = static_cast<uint32_t>(compressedData.size());
        compressedMessage.body = compressedData;

        return compressedMessage;
    }

    Networking::message<MessageType> DynamicNetworkOptimizer::DecompressMessage(const Networking::message<MessageType>& compressedMessage)
    {
        if (!m_compressionEnabled)
        {
            return compressedMessage;
        }

        auto startTime = std::chrono::high_resolution_clock::now();

        // Determine compression algorithm from message metadata or use default
        CompressionAlgorithm algorithm = CompressionAlgorithm::LZ4; // Default
        
        // Decompress the data
        auto& compression = DataCompression::GetInstance();
        std::vector<uint8_t> decompressedData = compression.Decompress(compressedMessage.body, algorithm);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();

        // Update statistics
        UpdateDecompressionStats(compressedMessage.body.size(), decompressedData.size(), duration);

        // Create decompressed message
        Networking::message<MessageType> decompressedMessage;
        decompressedMessage.header.id = compressedMessage.header.id;
        decompressedMessage.header.size = static_cast<uint32_t>(decompressedData.size());
        decompressedMessage.body = decompressedData;

        return decompressedMessage;
    }

    std::vector<uint8_t> DynamicNetworkOptimizer::ApplyDynamicCompression(const std::vector<uint8_t>& data)
    {
        if (data.empty())
        {
            return data;
        }

        // Analyze data characteristics
        DataCharacteristics characteristics = AnalyzeData(data);
        
        // Choose compression based on characteristics
        if (characteristics.entropy < 0.5f) // Low entropy - use RLE
        {
            return CompressRLE(data);
        }
        else if (characteristics.repetition > 0.3f) // High repetition - use LZ77
        {
            return CompressLZ77(data);
        }
        else // Default - use LZ4
        {
            auto& compression = DataCompression::GetInstance();
            return compression.Compress(data, CompressionAlgorithm::LZ4, CompressionLevel::Balanced);
        }
    }

    std::vector<uint8_t> DynamicNetworkOptimizer::ApplyDynamicDecompression(const std::vector<uint8_t>& data)
    {
        if (data.empty())
        {
            return data;
        }

        // Try to determine compression method from data format
        if (IsRLECompressed(data))
        {
            return DecompressRLE(data);
        }
        else if (IsLZ77Compressed(data))
        {
            return DecompressLZ77(data);
        }
        else // Default - use LZ4
        {
            auto& compression = DataCompression::GetInstance();
            return compression.Decompress(data, CompressionAlgorithm::LZ4);
        }
    }

    DynamicNetworkOptimizer::MessageCharacteristics DynamicNetworkOptimizer::AnalyzeMessage(const Networking::message<MessageType>& message)
    {
        MessageCharacteristics characteristics;
        
        characteristics.size = message.body.size();
        characteristics.type = message.header.id;
        
        // Analyze data characteristics
        DataCharacteristics dataChar = AnalyzeData(message.body);
        characteristics.entropy = dataChar.entropy;
        characteristics.repetition = dataChar.repetition;
        characteristics.compressibility = dataChar.compressibility;
        
        return characteristics;
    }

    DynamicNetworkOptimizer::DataCharacteristics DynamicNetworkOptimizer::AnalyzeData(const std::vector<uint8_t>& data)
    {
        DataCharacteristics characteristics;
        
        if (data.empty())
        {
            return characteristics;
        }

        // Calculate entropy
        std::array<size_t, 256> byteCounts = {0};
        for (uint8_t byte : data)
        {
            byteCounts[byte]++;
        }

        float entropy = 0.0f;
        for (size_t count : byteCounts)
        {
            if (count > 0)
            {
                float probability = static_cast<float>(count) / static_cast<float>(data.size());
                entropy -= probability * log2f(probability);
            }
        }
        characteristics.entropy = entropy / 8.0f; // Normalize to 0-1

        // Calculate repetition ratio
        size_t repetitions = 0;
        for (size_t i = 1; i < data.size(); i++)
        {
            if (data[i] == data[i-1])
            {
                repetitions++;
            }
        }
        characteristics.repetition = static_cast<float>(repetitions) / static_cast<float>(data.size());

        // Estimate compressibility
        characteristics.compressibility = 1.0f - (entropy / 8.0f);

        return characteristics;
    }

    CompressionAlgorithm DynamicNetworkOptimizer::ChooseCompressionAlgorithm(const MessageCharacteristics& characteristics)
    {
        if (characteristics.size < 64) // Small messages - use fast compression
        {
            return CompressionAlgorithm::LZ4;
        }
        else if (characteristics.entropy < 0.3f) // Low entropy - use RLE
        {
            return CompressionAlgorithm::ZLIB;
        }
        else if (characteristics.repetition > 0.4f) // High repetition - use LZ4HC
        {
            return CompressionAlgorithm::LZ4HC;
        }
        else // Default - use LZ4
        {
            return CompressionAlgorithm::LZ4;
        }
    }

    CompressionLevel DynamicNetworkOptimizer::ChooseCompressionLevel(const MessageCharacteristics& characteristics)
    {
        if (characteristics.size < 128) // Small messages - use fast compression
        {
            return CompressionLevel::Fast;
        }
        else if (characteristics.compressibility > 0.7f) // Highly compressible - use high compression
        {
            return CompressionLevel::High;
        }
        else // Default - use balanced compression
        {
            return CompressionLevel::Balanced;
        }
    }

    void DynamicNetworkOptimizer::UpdateCompressionStats(size_t originalSize, size_t compressedSize, long long duration)
    {
        m_stats.totalOriginalSize += originalSize;
        m_stats.totalCompressedSize += compressedSize;
        m_stats.totalCompressionTime += duration;
        m_stats.compressionEvents++;

        // Update adaptive parameters
        if (m_adaptiveMode)
        {
            float compressionRatio = static_cast<float>(compressedSize) / static_cast<float>(originalSize);
            if (compressionRatio > 0.8f) // Poor compression
            {
                m_adaptiveThresholds.entropyThreshold += 0.01f;
            }
            else if (compressionRatio < 0.3f) // Good compression
            {
                m_adaptiveThresholds.entropyThreshold -= 0.01f;
            }
        }
    }

    void DynamicNetworkOptimizer::UpdateDecompressionStats(size_t compressedSize, size_t decompressedSize, long long duration)
    {
        m_stats.totalDecompressedSize += decompressedSize;
        m_stats.totalDecompressionTime += duration;
        m_stats.decompressionEvents++;
    }

    std::vector<uint8_t> DynamicNetworkOptimizer::SerializeMessage(const Networking::message<MessageType>& msg)
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

    // RLE compression implementation
    std::vector<uint8_t> DynamicNetworkOptimizer::CompressRLE(const std::vector<uint8_t>& data)
    {
        std::vector<uint8_t> compressed;
        compressed.reserve(data.size());

        for (size_t i = 0; i < data.size(); )
        {
            uint8_t current = data[i];
            size_t count = 1;

            while (i + count < data.size() && data[i + count] == current && count < 255)
            {
                count++;
            }

            if (count > 3)
            {
                compressed.push_back(0xFF); // Escape character
                compressed.push_back(static_cast<uint8_t>(count));
                compressed.push_back(current);
            }
            else
            {
                for (size_t j = 0; j < count; j++)
                {
                    compressed.push_back(current);
                }
            }

            i += count;
        }

        return compressed;
    }

    std::vector<uint8_t> DynamicNetworkOptimizer::DecompressRLE(const std::vector<uint8_t>& data)
    {
        std::vector<uint8_t> decompressed;
        decompressed.reserve(data.size() * 2);

        for (size_t i = 0; i < data.size(); )
        {
            if (data[i] == 0xFF && i + 2 < data.size())
            {
                uint8_t count = data[i + 1];
                uint8_t value = data[i + 2];
                
                for (uint8_t j = 0; j < count; j++)
                {
                    decompressed.push_back(value);
                }
                
                i += 3;
            }
            else
            {
                decompressed.push_back(data[i]);
                i++;
            }
        }

        return decompressed;
    }

    // LZ77 compression implementation
    std::vector<uint8_t> DynamicNetworkOptimizer::CompressLZ77(const std::vector<uint8_t>& data)
    {
        std::vector<uint8_t> compressed;
        compressed.reserve(data.size());

        for (size_t i = 0; i < data.size(); )
        {
            size_t matchLength = 0;
            size_t matchDistance = 0;

            // Look for matches in the previous 255 bytes
            for (size_t j = (i > 255) ? i - 255 : 0; j < i; j++)
            {
                size_t len = 0;
                while (i + len < data.size() && j + len < i && data[i + len] == data[j + len] && len < 15)
                {
                    len++;
                }

                if (len > matchLength)
                {
                    matchLength = len;
                    matchDistance = i - j;
                }
            }

            if (matchLength >= 3)
            {
                compressed.push_back(0x80 | static_cast<uint8_t>(matchLength - 3));
                compressed.push_back(static_cast<uint8_t>(matchDistance));
                i += matchLength;
            }
            else
            {
                compressed.push_back(data[i]);
                i++;
            }
        }

        return compressed;
    }

    std::vector<uint8_t> DynamicNetworkOptimizer::DecompressLZ77(const std::vector<uint8_t>& data)
    {
        std::vector<uint8_t> decompressed;
        decompressed.reserve(data.size() * 2);

        for (size_t i = 0; i < data.size(); )
        {
            if (data[i] & 0x80)
            {
                uint8_t length = (data[i] & 0x7F) + 3;
                if (i + 1 < data.size())
                {
                    uint8_t distance = data[i + 1];
                    
                    if (distance <= decompressed.size())
                    {
                        size_t start = decompressed.size() - distance;
                        for (uint8_t j = 0; j < length; j++)
                        {
                            decompressed.push_back(decompressed[start + (j % distance)]);
                        }
                    }
                    else
                    {
                        decompressed.push_back(data[i]);
                    }
                }
                else
                {
                    decompressed.push_back(data[i]);
                }
                
                i += 2;
            }
            else
            {
                decompressed.push_back(data[i]);
                i++;
            }
        }

        return decompressed;
    }

    bool DynamicNetworkOptimizer::IsRLECompressed(const std::vector<uint8_t>& data)
    {
        // Simple heuristic: look for RLE escape sequences
        for (size_t i = 0; i < data.size() - 2; i++)
        {
            if (data[i] == 0xFF && data[i + 1] > 3)
            {
                return true;
            }
        }
        return false;
    }

    bool DynamicNetworkOptimizer::IsLZ77Compressed(const std::vector<uint8_t>& data)
    {
        // Simple heuristic: look for LZ77 high bit patterns
        for (size_t i = 0; i < data.size() - 1; i++)
        {
            if (data[i] & 0x80)
            {
                return true;
            }
        }
        return false;
    }

    void DynamicNetworkOptimizer::ResetStats()
    {
        m_stats = DynamicCompressionStats();
        m_adaptiveThresholds = AdaptiveThresholds();
    }

    DynamicCompressionStats DynamicNetworkOptimizer::GetStats() const
    {
        return m_stats;
    }

    void DynamicNetworkOptimizer::SetCompressionEnabled(bool enabled)
    {
        m_compressionEnabled = enabled;
        LOG_INFO_CAT(LogCategory::NETWORK, "Dynamic compression " + (enabled ? "enabled" : "disabled"));
    }

    void DynamicNetworkOptimizer::SetAdaptiveMode(bool enabled)
    {
        m_adaptiveMode = enabled;
        LOG_INFO_CAT(LogCategory::NETWORK, "Adaptive mode " + (enabled ? "enabled" : "disabled"));
    }
}