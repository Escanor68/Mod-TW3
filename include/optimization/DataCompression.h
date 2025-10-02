#pragma once

#include "Common.h"
#include <vector>
#include <memory>
#include <string>

namespace Optimization
{
    // Compression algorithms
    enum class CompressionAlgorithm
    {
        None = 0,
        Zlib = 1,
        LZ4 = 2,
        LZ4HC = 3,  // High compression
        Gzip = 4
    };

    // Compression levels
    enum class CompressionLevel
    {
        Fast = 1,
        Balanced = 2,
        High = 3,
        Maximum = 4
    };

    // Compression statistics
    struct CompressionStats
    {
        size_t originalSize = 0;
        size_t compressedSize = 0;
        float compressionRatio = 0.0f;
        float compressionTime = 0.0f;
        size_t totalCompressions = 0;
        size_t totalDecompressions = 0;
        
        void UpdateStats(size_t orig, size_t comp, float time)
        {
            originalSize += orig;
            compressedSize += comp;
            compressionRatio = (float)compressedSize / (float)originalSize;
            compressionTime += time;
        }
    };

    // Data compression interface
    class DataCompression
    {
    public:
        static DataCompression& GetInstance();
        static void DestroyInstance();

        // Initialize compression system
        bool Initialize();
        void Shutdown();

        // Compression methods
        std::vector<uint8_t> Compress(const std::vector<uint8_t>& data, 
                                     CompressionAlgorithm algorithm = CompressionAlgorithm::LZ4,
                                     CompressionLevel level = CompressionLevel::Balanced);
        
        std::vector<uint8_t> Decompress(const std::vector<uint8_t>& compressedData,
                                       CompressionAlgorithm algorithm = CompressionAlgorithm::LZ4);

        // String compression (for JSON, text data)
        std::string CompressString(const std::string& data,
                                  CompressionAlgorithm algorithm = CompressionAlgorithm::LZ4,
                                  CompressionLevel level = CompressionLevel::Balanced);
        
        std::string DecompressString(const std::string& compressedData,
                                    CompressionAlgorithm algorithm = CompressionAlgorithm::LZ4);

        // Smart compression (auto-selects best algorithm)
        std::vector<uint8_t> SmartCompress(const std::vector<uint8_t>& data);
        std::vector<uint8_t> SmartDecompress(const std::vector<uint8_t>& compressedData);

        // Compression for specific data types
        std::vector<uint8_t> CompressPlayerData(const std::vector<uint8_t>& playerData);
        std::vector<uint8_t> CompressWorldData(const std::vector<uint8_t>& worldData);
        std::vector<uint8_t> CompressInventoryData(const std::vector<uint8_t>& inventoryData);
        std::vector<uint8_t> CompressCombatData(const std::vector<uint8_t>& combatData);

        // Batch compression (for multiple small packets)
        std::vector<uint8_t> CompressBatch(const std::vector<std::vector<uint8_t>>& packets);
        std::vector<std::vector<uint8_t>> DecompressBatch(const std::vector<uint8_t>& compressedBatch);

        // Compression utilities
        bool IsCompressible(const std::vector<uint8_t>& data, size_t minSize = 64);
        CompressionAlgorithm GetBestAlgorithm(const std::vector<uint8_t>& data);
        float GetCompressionRatio(const std::vector<uint8_t>& original, const std::vector<uint8_t>& compressed);

        // Statistics
        CompressionStats GetStats() const;
        void ResetStats();
        void PrintStats() const;

        // Configuration
        void SetDefaultAlgorithm(CompressionAlgorithm algorithm);
        void SetDefaultLevel(CompressionLevel level);
        void SetMinCompressionSize(size_t minSize);
        void SetMaxCompressionTime(float maxTime);

    private:
        DataCompression();
        ~DataCompression();
        DataCompression(const DataCompression&) = delete;
        DataCompression& operator=(const DataCompression&) = delete;

        // Internal compression methods
        std::vector<uint8_t> CompressZlib(const std::vector<uint8_t>& data, int level);
        std::vector<uint8_t> DecompressZlib(const std::vector<uint8_t>& compressedData);
        
        std::vector<uint8_t> CompressLZ4(const std::vector<uint8_t>& data, int level);
        std::vector<uint8_t> DecompressLZ4(const std::vector<uint8_t>& compressedData);
        
        std::vector<uint8_t> CompressLZ4HC(const std::vector<uint8_t>& data, int level);
        std::vector<uint8_t> DecompressLZ4HC(const std::vector<uint8_t>& compressedData);

        // Utility methods
        int GetCompressionLevel(CompressionLevel level);
        bool IsDataCompressed(const std::vector<uint8_t>& data);
        CompressionAlgorithm DetectAlgorithm(const std::vector<uint8_t>& data);

        // Member variables
        bool m_initialized;
        CompressionAlgorithm m_defaultAlgorithm;
        CompressionLevel m_defaultLevel;
        size_t m_minCompressionSize;
        float m_maxCompressionTime;
        CompressionStats m_stats;
        
        static DataCompression* s_instance;
    };

    // Compression helper functions
    namespace CompressionUtils
    {
        // Quick compression for small data
        std::vector<uint8_t> QuickCompress(const std::vector<uint8_t>& data);
        std::vector<uint8_t> QuickDecompress(const std::vector<uint8_t>& compressedData);

        // High compression for large data
        std::vector<uint8_t> HighCompress(const std::vector<uint8_t>& data);
        std::vector<uint8_t> HighDecompress(const std::vector<uint8_t>& compressedData);

        // Compression for specific game data
        std::vector<uint8_t> CompressPositionData(const std::vector<Vector4F>& positions);
        std::vector<Vector4F> DecompressPositionData(const std::vector<uint8_t>& compressedData);

        std::vector<uint8_t> CompressHealthData(const std::vector<float>& healthValues);
        std::vector<float> DecompressHealthData(const std::vector<uint8_t>& compressedData);

        // Delta compression (for incremental updates)
        std::vector<uint8_t> CompressDelta(const std::vector<uint8_t>& current, const std::vector<uint8_t>& previous);
        std::vector<uint8_t> DecompressDelta(const std::vector<uint8_t>& delta, const std::vector<uint8_t>& previous);
    }
}
