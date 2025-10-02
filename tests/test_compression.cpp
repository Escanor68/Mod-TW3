#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <string>
#include "optimization/DataCompression.h"

TEST_CASE("DataCompression - Basic Functionality", "[compression]")
{
    auto& compression = Optimization::DataCompression::GetInstance();
    
    SECTION("Initialization")
    {
        REQUIRE(compression.Initialize() == true);
    }
    
    SECTION("Zlib Compression")
    {
        std::vector<uint8_t> testData = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        std::vector<uint8_t> compressed = compression.Compress(testData, Optimization::CompressionAlgorithm::Zlib);
        
        REQUIRE(compressed.size() > 0);
        REQUIRE(compressed.size() != testData.size()); // Should be different due to header
    }
    
    SECTION("LZ4 Compression")
    {
        std::vector<uint8_t> testData = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        std::vector<uint8_t> compressed = compression.Compress(testData, Optimization::CompressionAlgorithm::LZ4);
        
        REQUIRE(compressed.size() > 0);
        REQUIRE(compressed.size() != testData.size()); // Should be different due to header
    }
    
    SECTION("Compression and Decompression Round Trip")
    {
        std::vector<uint8_t> originalData = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        
        // Compress
        std::vector<uint8_t> compressed = compression.Compress(originalData, Optimization::CompressionAlgorithm::LZ4);
        REQUIRE(compressed.size() > 0);
        
        // Decompress
        std::vector<uint8_t> decompressed = compression.Decompress(compressed);
        REQUIRE(decompressed.size() == originalData.size());
        
        // Verify data integrity
        for (size_t i = 0; i < originalData.size(); ++i)
        {
            REQUIRE(decompressed[i] == originalData[i]);
        }
    }
    
    SECTION("String Compression")
    {
        std::string testString = "The Witcher 3 Wild Hunt Next-Gen Multiplayer Mod";
        std::string compressed = compression.CompressString(testString, Optimization::CompressionAlgorithm::LZ4);
        std::string decompressed = compression.DecompressString(compressed, Optimization::CompressionAlgorithm::LZ4);
        
        REQUIRE(decompressed == testString);
    }
    
    SECTION("Statistics")
    {
        compression.ResetStats();
        auto stats = compression.GetStats();
        
        REQUIRE(stats.totalCompressions == 0);
        REQUIRE(stats.totalDecompressions == 0);
        
        // Perform some operations
        std::vector<uint8_t> testData = {1, 2, 3, 4, 5};
        compression.Compress(testData, Optimization::CompressionAlgorithm::LZ4);
        
        stats = compression.GetStats();
        REQUIRE(stats.totalCompressions > 0);
    }
}

TEST_CASE("DataCompression - Edge Cases", "[compression]")
{
    auto& compression = Optimization::DataCompression::GetInstance();
    
    SECTION("Empty Data")
    {
        std::vector<uint8_t> emptyData;
        std::vector<uint8_t> compressed = compression.Compress(emptyData);
        REQUIRE(compressed.empty());
    }
    
    SECTION("Small Data")
    {
        std::vector<uint8_t> smallData = {1, 2};
        std::vector<uint8_t> compressed = compression.Compress(smallData);
        // Should return original data if too small to compress
        REQUIRE(compressed == smallData);
    }
    
    SECTION("Large Data")
    {
        std::vector<uint8_t> largeData(10000, 42); // 10000 bytes of same value
        std::vector<uint8_t> compressed = compression.Compress(largeData, Optimization::CompressionAlgorithm::LZ4HC);
        
        REQUIRE(compressed.size() > 0);
        REQUIRE(compressed.size() < largeData.size()); // Should compress well
    }
}

TEST_CASE("DataCompression - Performance", "[compression][performance]")
{
    auto& compression = Optimization::DataCompression::GetInstance();
    
    SECTION("Compression Speed")
    {
        std::vector<uint8_t> testData(1000, 0);
        for (size_t i = 0; i < testData.size(); ++i)
        {
            testData[i] = i % 256;
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 100; ++i)
        {
            compression.Compress(testData, Optimization::CompressionAlgorithm::LZ4);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Should complete 100 compressions in reasonable time
        REQUIRE(duration.count() < 1000); // Less than 1 second
    }
}
