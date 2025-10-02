#include "optimization/DataCompression.h"
#include "utils/Logger.h"
#include <chrono>
#include <algorithm>
#include <cstring>

// Note: In a real implementation, you would link against zlib and LZ4 libraries
// For now, we'll implement placeholder compression methods

namespace Optimization
{
    // Static member initialization
    DataCompression* DataCompression::s_instance = nullptr;

    DataCompression& DataCompression::GetInstance()
    {
        if (s_instance == nullptr)
        {
            s_instance = new DataCompression();
        }
        return *s_instance;
    }

    void DataCompression::DestroyInstance()
    {
        if (s_instance != nullptr)
        {
            delete s_instance;
            s_instance = nullptr;
        }
    }

    DataCompression::DataCompression()
        : m_initialized(false), m_defaultAlgorithm(CompressionAlgorithm::LZ4),
          m_defaultLevel(CompressionLevel::Balanced), m_minCompressionSize(64),
          m_maxCompressionTime(0.01f) // 10ms max
    {
        LOG_INFO("DataCompression created");
    }

    DataCompression::~DataCompression()
    {
        Shutdown();
        LOG_INFO("DataCompression destroyed");
    }

    bool DataCompression::Initialize()
    {
        if (m_initialized)
        {
            return true;
        }

        LOG_INFO("Initializing data compression system...");

        // Initialize compression libraries
        // In a real implementation, this would initialize zlib and LZ4
        
        m_initialized = true;
        LOG_INFO("Data compression system initialized");
        return true;
    }

    void DataCompression::Shutdown()
    {
        if (!m_initialized)
        {
            return;
        }

        LOG_INFO("Shutting down data compression system...");
        
        // Cleanup compression libraries
        // In a real implementation, this would cleanup zlib and LZ4
        
        m_initialized = false;
        LOG_INFO("Data compression system shutdown complete");
    }

    std::vector<uint8_t> DataCompression::Compress(const std::vector<uint8_t>& data, 
                                                   CompressionAlgorithm algorithm, 
                                                   CompressionLevel level)
    {
        if (!m_initialized || data.empty())
        {
            return data;
        }

        if (!IsCompressible(data, m_minCompressionSize))
        {
            return data;
        }

        auto startTime = std::chrono::high_resolution_clock::now();

        std::vector<uint8_t> compressedData;
        
        switch (algorithm)
        {
            case CompressionAlgorithm::Zlib:
                compressedData = CompressZlib(data, GetCompressionLevel(level));
                break;
            case CompressionAlgorithm::LZ4:
                compressedData = CompressLZ4(data, GetCompressionLevel(level));
                break;
            case CompressionAlgorithm::LZ4HC:
                compressedData = CompressLZ4HC(data, GetCompressionLevel(level));
                break;
            default:
                compressedData = data;
                break;
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        float compressionTime = std::chrono::duration<float>(endTime - startTime).count();

        // Update statistics
        m_stats.UpdateStats(data.size(), compressedData.size(), compressionTime);
        m_stats.totalCompressions++;

        // Check if compression was beneficial
        if (compressedData.size() >= data.size())
        {
            LOG_DEBUG("Compression not beneficial, returning original data");
            return data;
        }

        // Add compression header
        std::vector<uint8_t> result;
        result.reserve(compressedData.size() + 8);
        
        // Header: [algorithm(1)] [level(1)] [original_size(4)] [compressed_size(4)]
        result.push_back(static_cast<uint8_t>(algorithm));
        result.push_back(static_cast<uint8_t>(level));
        
        uint32_t originalSize = static_cast<uint32_t>(data.size());
        uint32_t compressedSize = static_cast<uint32_t>(compressedData.size());
        
        result.insert(result.end(), reinterpret_cast<uint8_t*>(&originalSize), 
                     reinterpret_cast<uint8_t*>(&originalSize) + 4);
        result.insert(result.end(), reinterpret_cast<uint8_t*>(&compressedSize), 
                     reinterpret_cast<uint8_t*>(&compressedSize) + 4);
        
        result.insert(result.end(), compressedData.begin(), compressedData.end());

        LOG_DEBUG("Compressed " + std::to_string(data.size()) + " bytes to " + 
                 std::to_string(result.size()) + " bytes (" + 
                 std::to_string(GetCompressionRatio(data, result) * 100.0f) + "% ratio)");

        return result;
    }

    std::vector<uint8_t> DataCompression::Decompress(const std::vector<uint8_t>& compressedData,
                                                     CompressionAlgorithm algorithm)
    {
        if (!m_initialized || compressedData.empty())
        {
            return compressedData;
        }

        // Check if data has compression header
        if (compressedData.size() < 8)
        {
            return compressedData;
        }

        auto startTime = std::chrono::high_resolution_clock::now();

        // Read header
        CompressionAlgorithm detectedAlgorithm = static_cast<CompressionAlgorithm>(compressedData[0]);
        CompressionLevel level = static_cast<CompressionLevel>(compressedData[1]);
        
        uint32_t originalSize, compressedSize;
        std::memcpy(&originalSize, &compressedData[2], 4);
        std::memcpy(&compressedSize, &compressedData[6], 4);

        // Extract compressed data
        std::vector<uint8_t> dataToDecompress(compressedData.begin() + 8, compressedData.end());

        std::vector<uint8_t> decompressedData;
        
        switch (detectedAlgorithm)
        {
            case CompressionAlgorithm::Zlib:
                decompressedData = DecompressZlib(dataToDecompress);
                break;
            case CompressionAlgorithm::LZ4:
                decompressedData = DecompressLZ4(dataToDecompress);
                break;
            case CompressionAlgorithm::LZ4HC:
                decompressedData = DecompressLZ4HC(dataToDecompress);
                break;
            default:
                decompressedData = compressedData;
                break;
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        float decompressionTime = std::chrono::duration<float>(endTime - startTime).count();

        // Update statistics
        m_stats.compressionTime += decompressionTime;
        m_stats.totalDecompressions++;

        LOG_DEBUG("Decompressed " + std::to_string(compressedData.size()) + " bytes to " + 
                 std::to_string(decompressedData.size()) + " bytes");

        return decompressedData;
    }

    std::string DataCompression::CompressString(const std::string& data,
                                                CompressionAlgorithm algorithm,
                                                CompressionLevel level)
    {
        std::vector<uint8_t> dataVec(data.begin(), data.end());
        std::vector<uint8_t> compressed = Compress(dataVec, algorithm, level);
        return std::string(compressed.begin(), compressed.end());
    }

    std::string DataCompression::DecompressString(const std::string& compressedData,
                                                  CompressionAlgorithm algorithm)
    {
        std::vector<uint8_t> compressedVec(compressedData.begin(), compressedData.end());
        std::vector<uint8_t> decompressed = Decompress(compressedVec, algorithm);
        return std::string(decompressed.begin(), decompressed.end());
    }

    std::vector<uint8_t> DataCompression::SmartCompress(const std::vector<uint8_t>& data)
    {
        if (!m_initialized || data.empty())
        {
            return data;
        }

        CompressionAlgorithm bestAlgorithm = GetBestAlgorithm(data);
        return Compress(data, bestAlgorithm, m_defaultLevel);
    }

    std::vector<uint8_t> DataCompression::SmartDecompress(const std::vector<uint8_t>& compressedData)
    {
        if (!m_initialized || compressedData.empty())
        {
            return compressedData;
        }

        CompressionAlgorithm algorithm = DetectAlgorithm(compressedData);
        return Decompress(compressedData, algorithm);
    }

    std::vector<uint8_t> DataCompression::CompressPlayerData(const std::vector<uint8_t>& playerData)
    {
        // Player data is typically small and changes frequently
        // Use LZ4 for fast compression
        return Compress(playerData, CompressionAlgorithm::LZ4, CompressionLevel::Fast);
    }

    std::vector<uint8_t> DataCompression::CompressWorldData(const std::vector<uint8_t>& worldData)
    {
        // World data is large and changes infrequently
        // Use LZ4HC for high compression
        return Compress(worldData, CompressionAlgorithm::LZ4HC, CompressionLevel::High);
    }

    std::vector<uint8_t> DataCompression::CompressInventoryData(const std::vector<uint8_t>& inventoryData)
    {
        // Inventory data is medium-sized and changes moderately
        // Use balanced compression
        return Compress(inventoryData, CompressionAlgorithm::LZ4, CompressionLevel::Balanced);
    }

    std::vector<uint8_t> DataCompression::CompressCombatData(const std::vector<uint8_t>& combatData)
    {
        // Combat data is small but needs fast compression
        // Use LZ4 for speed
        return Compress(combatData, CompressionAlgorithm::LZ4, CompressionLevel::Fast);
    }

    std::vector<uint8_t> DataCompression::CompressBatch(const std::vector<std::vector<uint8_t>>& packets)
    {
        if (packets.empty())
        {
            return {};
        }

        // Combine all packets into one large packet
        std::vector<uint8_t> combinedData;
        size_t totalSize = 0;
        
        for (const auto& packet : packets)
        {
            totalSize += packet.size() + 4; // +4 for size header
        }
        
        combinedData.reserve(totalSize);
        
        for (const auto& packet : packets)
        {
            uint32_t packetSize = static_cast<uint32_t>(packet.size());
            combinedData.insert(combinedData.end(), 
                               reinterpret_cast<uint8_t*>(&packetSize), 
                               reinterpret_cast<uint8_t*>(&packetSize) + 4);
            combinedData.insert(combinedData.end(), packet.begin(), packet.end());
        }

        // Compress the combined data
        return Compress(combinedData, CompressionAlgorithm::LZ4HC, CompressionLevel::High);
    }

    std::vector<std::vector<uint8_t>> DataCompression::DecompressBatch(const std::vector<uint8_t>& compressedBatch)
    {
        std::vector<std::vector<uint8_t>> packets;
        
        // Decompress the batch
        std::vector<uint8_t> decompressedData = Decompress(compressedBatch);
        
        if (decompressedData.empty())
        {
            return packets;
        }

        // Extract individual packets
        size_t offset = 0;
        while (offset < decompressedData.size())
        {
            if (offset + 4 > decompressedData.size())
            {
                break;
            }

            uint32_t packetSize;
            std::memcpy(&packetSize, &decompressedData[offset], 4);
            offset += 4;

            if (offset + packetSize > decompressedData.size())
            {
                break;
            }

            std::vector<uint8_t> packet(decompressedData.begin() + offset, 
                                       decompressedData.begin() + offset + packetSize);
            packets.push_back(packet);
            offset += packetSize;
        }

        return packets;
    }

    bool DataCompression::IsCompressible(const std::vector<uint8_t>& data, size_t minSize)
    {
        return data.size() >= minSize;
    }

    CompressionAlgorithm DataCompression::GetBestAlgorithm(const std::vector<uint8_t>& data)
    {
        // Simple heuristic for algorithm selection
        if (data.size() < 1024)
        {
            return CompressionAlgorithm::LZ4; // Fast for small data
        }
        else if (data.size() < 10240)
        {
            return CompressionAlgorithm::LZ4; // Balanced for medium data
        }
        else
        {
            return CompressionAlgorithm::LZ4HC; // High compression for large data
        }
    }

    float DataCompression::GetCompressionRatio(const std::vector<uint8_t>& original, 
                                              const std::vector<uint8_t>& compressed)
    {
        if (original.empty())
        {
            return 1.0f;
        }
        return static_cast<float>(compressed.size()) / static_cast<float>(original.size());
    }

    CompressionStats DataCompression::GetStats() const
    {
        return m_stats;
    }

    void DataCompression::ResetStats()
    {
        m_stats = CompressionStats();
    }

    void DataCompression::PrintStats() const
    {
        LOG_INFO("=== Compression Statistics ===");
        LOG_INFO("Total compressions: " + std::to_string(m_stats.totalCompressions));
        LOG_INFO("Total decompressions: " + std::to_string(m_stats.totalDecompressions));
        LOG_INFO("Original size: " + std::to_string(m_stats.originalSize) + " bytes");
        LOG_INFO("Compressed size: " + std::to_string(m_stats.compressedSize) + " bytes");
        LOG_INFO("Compression ratio: " + std::to_string(m_stats.compressionRatio * 100.0f) + "%");
        LOG_INFO("Total compression time: " + std::to_string(m_stats.compressionTime) + "s");
        LOG_INFO("==============================");
    }

    void DataCompression::SetDefaultAlgorithm(CompressionAlgorithm algorithm)
    {
        m_defaultAlgorithm = algorithm;
    }

    void DataCompression::SetDefaultLevel(CompressionLevel level)
    {
        m_defaultLevel = level;
    }

    void DataCompression::SetMinCompressionSize(size_t minSize)
    {
        m_minCompressionSize = minSize;
    }

    void DataCompression::SetMaxCompressionTime(float maxTime)
    {
        m_maxCompressionTime = maxTime;
    }

    // Internal compression methods (real implementations)
    std::vector<uint8_t> DataCompression::CompressZlib(const std::vector<uint8_t>& data, int level)
    {
        if (data.empty())
        {
            return data;
        }

        // Simple RLE (Run-Length Encoding) implementation for zlib placeholder
        std::vector<uint8_t> compressed;
        compressed.reserve(data.size());

        for (size_t i = 0; i < data.size(); )
        {
            uint8_t current = data[i];
            size_t count = 1;

            // Count consecutive identical bytes
            while (i + count < data.size() && data[i + count] == current && count < 255)
            {
                count++;
            }

            if (count > 3) // Only compress runs of 4 or more
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

        LOG_DEBUG("Zlib compression: " + std::to_string(data.size()) + " -> " + std::to_string(compressed.size()) + " bytes");
        return compressed;
    }

    std::vector<uint8_t> DataCompression::DecompressZlib(const std::vector<uint8_t>& compressedData)
    {
        if (compressedData.empty())
        {
            return compressedData;
        }

        std::vector<uint8_t> decompressed;
        decompressed.reserve(compressedData.size() * 2); // Estimate

        for (size_t i = 0; i < compressedData.size(); )
        {
            if (compressedData[i] == 0xFF && i + 2 < compressedData.size())
            {
                // Decompress run-length encoded data
                uint8_t count = compressedData[i + 1];
                uint8_t value = compressedData[i + 2];
                
                for (uint8_t j = 0; j < count; j++)
                {
                    decompressed.push_back(value);
                }
                
                i += 3;
            }
            else
            {
                decompressed.push_back(compressedData[i]);
                i++;
            }
        }

        LOG_DEBUG("Zlib decompression: " + std::to_string(compressedData.size()) + " -> " + std::to_string(decompressed.size()) + " bytes");
        return decompressed;
    }

    std::vector<uint8_t> DataCompression::CompressLZ4(const std::vector<uint8_t>& data, int level)
    {
        if (data.empty())
        {
            return data;
        }

        // Simple LZ77-like compression implementation
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

            if (matchLength >= 3) // Only compress matches of 3 or more
            {
                compressed.push_back(0x80 | static_cast<uint8_t>(matchLength - 3)); // High bit + length
                compressed.push_back(static_cast<uint8_t>(matchDistance));
                i += matchLength;
            }
            else
            {
                compressed.push_back(data[i]);
                i++;
            }
        }

        LOG_DEBUG("LZ4 compression: " + std::to_string(data.size()) + " -> " + std::to_string(compressed.size()) + " bytes");
        return compressed;
    }

    std::vector<uint8_t> DataCompression::DecompressLZ4(const std::vector<uint8_t>& compressedData)
    {
        if (compressedData.empty())
        {
            return compressedData;
        }

        std::vector<uint8_t> decompressed;
        decompressed.reserve(compressedData.size() * 2); // Estimate

        for (size_t i = 0; i < compressedData.size(); )
        {
            if (compressedData[i] & 0x80) // High bit set = compressed
            {
                uint8_t length = (compressedData[i] & 0x7F) + 3;
                if (i + 1 < compressedData.size())
                {
                    uint8_t distance = compressedData[i + 1];
                    
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
                        // Invalid distance, copy as literal
                        decompressed.push_back(compressedData[i]);
                    }
                }
                else
                {
                    decompressed.push_back(compressedData[i]);
                }
                
                i += 2;
            }
            else
            {
                decompressed.push_back(compressedData[i]);
                i++;
            }
        }

        LOG_DEBUG("LZ4 decompression: " + std::to_string(compressedData.size()) + " -> " + std::to_string(decompressed.size()) + " bytes");
        return decompressed;
    }

    std::vector<uint8_t> DataCompression::CompressLZ4HC(const std::vector<uint8_t>& data, int level)
    {
        // LZ4HC is a high-compression variant of LZ4
        // For now, use the same implementation as LZ4 but with better parameters
        return CompressLZ4(data, level);
    }

    std::vector<uint8_t> DataCompression::DecompressLZ4HC(const std::vector<uint8_t>& compressedData)
    {
        // LZ4HC uses the same decompression as LZ4
        return DecompressLZ4(compressedData);
    }

    int DataCompression::GetCompressionLevel(CompressionLevel level)
    {
        switch (level)
        {
            case CompressionLevel::Fast: return 1;
            case CompressionLevel::Balanced: return 3;
            case CompressionLevel::High: return 6;
            case CompressionLevel::Maximum: return 9;
            default: return 3;
        }
    }

    bool DataCompression::IsDataCompressed(const std::vector<uint8_t>& data)
    {
        return data.size() >= 8 && data[0] < 10; // Check for compression header
    }

    CompressionAlgorithm DataCompression::DetectAlgorithm(const std::vector<uint8_t>& data)
    {
        if (data.size() < 8)
        {
            return CompressionAlgorithm::None;
        }
        return static_cast<CompressionAlgorithm>(data[0]);
    }

    // Compression utility functions
    namespace CompressionUtils
    {
        std::vector<uint8_t> QuickCompress(const std::vector<uint8_t>& data)
        {
            auto& compression = DataCompression::GetInstance();
            return compression.Compress(data, CompressionAlgorithm::LZ4, CompressionLevel::Fast);
        }

        std::vector<uint8_t> QuickDecompress(const std::vector<uint8_t>& compressedData)
        {
            auto& compression = DataCompression::GetInstance();
            return compression.Decompress(compressedData, CompressionAlgorithm::LZ4);
        }

        std::vector<uint8_t> HighCompress(const std::vector<uint8_t>& data)
        {
            auto& compression = DataCompression::GetInstance();
            return compression.Compress(data, CompressionAlgorithm::LZ4HC, CompressionLevel::Maximum);
        }

        std::vector<uint8_t> HighDecompress(const std::vector<uint8_t>& compressedData)
        {
            auto& compression = DataCompression::GetInstance();
            return compression.Decompress(compressedData, CompressionAlgorithm::LZ4HC);
        }

        std::vector<uint8_t> CompressPositionData(const std::vector<Vector4F>& positions)
        {
            // Convert positions to byte array
            std::vector<uint8_t> data;
            data.reserve(positions.size() * sizeof(Vector4F));
            
            for (const auto& pos : positions)
            {
                const uint8_t* posBytes = reinterpret_cast<const uint8_t*>(&pos);
                data.insert(data.end(), posBytes, posBytes + sizeof(Vector4F));
            }
            
            return QuickCompress(data);
        }

        std::vector<Vector4F> DecompressPositionData(const std::vector<uint8_t>& compressedData)
        {
            std::vector<uint8_t> decompressed = QuickDecompress(compressedData);
            std::vector<Vector4F> positions;
            
            size_t posCount = decompressed.size() / sizeof(Vector4F);
            positions.reserve(posCount);
            
            for (size_t i = 0; i < posCount; ++i)
            {
                Vector4F pos;
                std::memcpy(&pos, &decompressed[i * sizeof(Vector4F)], sizeof(Vector4F));
                positions.push_back(pos);
            }
            
            return positions;
        }

        std::vector<uint8_t> CompressHealthData(const std::vector<float>& healthValues)
        {
            std::vector<uint8_t> data;
            data.reserve(healthValues.size() * sizeof(float));
            
            for (float health : healthValues)
            {
                const uint8_t* healthBytes = reinterpret_cast<const uint8_t*>(&health);
                data.insert(data.end(), healthBytes, healthBytes + sizeof(float));
            }
            
            return QuickCompress(data);
        }

        std::vector<float> DecompressHealthData(const std::vector<uint8_t>& compressedData)
        {
            std::vector<uint8_t> decompressed = QuickDecompress(compressedData);
            std::vector<float> healthValues;
            
            size_t healthCount = decompressed.size() / sizeof(float);
            healthValues.reserve(healthCount);
            
            for (size_t i = 0; i < healthCount; ++i)
            {
                float health;
                std::memcpy(&health, &decompressed[i * sizeof(float)], sizeof(float));
                healthValues.push_back(health);
            }
            
            return healthValues;
        }

        std::vector<uint8_t> CompressDelta(const std::vector<uint8_t>& current, const std::vector<uint8_t>& previous)
        {
            if (current.size() != previous.size())
            {
                return current; // Can't do delta compression
            }
            
            std::vector<uint8_t> delta;
            delta.reserve(current.size());
            
            for (size_t i = 0; i < current.size(); ++i)
            {
                delta.push_back(current[i] ^ previous[i]); // XOR for delta
            }
            
            return QuickCompress(delta);
        }

        std::vector<uint8_t> DecompressDelta(const std::vector<uint8_t>& delta, const std::vector<uint8_t>& previous)
        {
            std::vector<uint8_t> decompressedDelta = QuickDecompress(delta);
            
            if (decompressedDelta.size() != previous.size())
            {
                return previous; // Can't reconstruct
            }
            
            std::vector<uint8_t> current;
            current.reserve(previous.size());
            
            for (size_t i = 0; i < previous.size(); ++i)
            {
                current.push_back(decompressedDelta[i] ^ previous[i]); // XOR to reconstruct
            }
            
            return current;
        }
    }
}
