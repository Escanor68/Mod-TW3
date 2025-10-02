#pragma once
#include "Common.h"
#include "integration/REDkitBridge.h"
#include "optimization/DataCompression.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace TW3Integration
{
    // Asset types
    enum class AssetType
    {
        Unknown = 0,
        Weapon = 1,
        Armor = 2,
        Item = 3,
        NPC = 4,
        Quest = 5,
        Bundle = 6,
        Texture = 7,
        Model = 8,
        Sound = 9,
        Script = 10
    };

    // Asset data structure
    struct AssetData
    {
        std::string name;
        AssetType type;
        std::vector<uint8_t> data;
        size_t originalSize;
        size_t compressedSize;
        bool isCompressed;
        std::string filePath;
        uint32_t checksum;
        uint64_t timestamp;
    };

    // Bundle information
    struct BundleInfo
    {
        std::string name;
        std::string path;
        std::vector<std::string> assets;
        size_t totalSize;
        bool isLoaded;
        uint32_t version;
    };

    // Asset loader class for TW3 Next-Gen
    class AssetLoader
    {
    public:
        // Constructor/Destructor
        AssetLoader();
        ~AssetLoader();

        // Initialization
        bool Initialize();
        void Shutdown();

        // Bundle loading
        bool LoadBundle(const std::string& bundlePath);
        bool UnloadBundle(const std::string& bundleName);
        bool IsBundleLoaded(const std::string& bundleName) const;
        std::vector<std::string> GetLoadedBundles() const;

        // Asset loading from REDkit
        bool LoadWeaponData(const std::string& weaponType, AssetData& assetData);
        bool LoadArmorData(const std::string& armorType, AssetData& assetData);
        bool LoadItemData(const std::string& itemType, AssetData& assetData);
        bool LoadNPCData(const std::string& npcType, AssetData& assetData);
        bool LoadQuestData(const std::string& questType, AssetData& assetData);

        // Asset loading from file system
        bool LoadAssetFromFile(const std::string& filePath, AssetData& assetData);
        bool SaveAssetToFile(const std::string& filePath, const AssetData& assetData);

        // Asset management
        bool RegisterAsset(const AssetData& assetData);
        bool UnregisterAsset(const std::string& assetName);
        bool IsAssetLoaded(const std::string& assetName) const;
        AssetData GetAsset(const std::string& assetName) const;
        std::vector<std::string> GetLoadedAssets() const;

        // Asset search and filtering
        std::vector<std::string> FindAssetsByType(AssetType type) const;
        std::vector<std::string> FindAssetsByPattern(const std::string& pattern) const;
        std::vector<std::string> FindAssetsInBundle(const std::string& bundleName) const;

        // Compression and optimization
        bool CompressAsset(AssetData& assetData, CompressionAlgorithm algorithm = CompressionAlgorithm::LZ4);
        bool DecompressAsset(AssetData& assetData);
        bool OptimizeAsset(AssetData& assetData);

        // Validation and integrity
        bool ValidateAsset(const AssetData& assetData) const;
        uint32_t CalculateChecksum(const std::vector<uint8_t>& data) const;
        bool VerifyAssetIntegrity(const AssetData& assetData) const;

        // Statistics and monitoring
        size_t GetTotalLoadedAssets() const;
        size_t GetTotalLoadedSize() const;
        size_t GetTotalCompressedSize() const;
        float GetCompressionRatio() const;
        std::string GetAssetStatistics() const;

        // Callbacks for asset events
        using AssetLoadedCallback = std::function<void(const std::string& assetName, const AssetData& assetData)>;
        using AssetUnloadedCallback = std::function<void(const std::string& assetName)>;
        using BundleLoadedCallback = std::function<void(const std::string& bundleName, const BundleInfo& bundleInfo)>;

        void SetAssetLoadedCallback(AssetLoadedCallback callback);
        void SetAssetUnloadedCallback(AssetUnloadedCallback callback);
        void SetBundleLoadedCallback(BundleLoadedCallback callback);

        // Utility functions
        AssetType GetAssetTypeFromExtension(const std::string& filePath) const;
        AssetType GetAssetTypeFromName(const std::string& assetName) const;
        std::string GetAssetExtension(AssetType type) const;
        bool IsValidAssetFile(const std::string& filePath) const;

        // Error handling
        std::string GetLastError() const;
        void ClearError();

    private:
        // State
        bool m_initialized;
        std::string m_lastError;

        // Asset storage
        std::unordered_map<std::string, AssetData> m_loadedAssets;
        std::unordered_map<std::string, BundleInfo> m_loadedBundles;

        // REDkit bridge
        REDkitBridge& m_redkitBridge;

        // Compression system
        DataCompression& m_compression;

        // Callbacks
        AssetLoadedCallback m_assetLoadedCallback;
        AssetUnloadedCallback m_assetUnloadedCallback;
        BundleLoadedCallback m_bundleLoadedCallback;

        // Internal methods
        bool LoadBundleInternal(const std::string& bundlePath, BundleInfo& bundleInfo);
        bool ParseBundleFile(const std::string& bundlePath, BundleInfo& bundleInfo);
        bool ExtractAssetFromBundle(const std::string& bundleName, const std::string& assetName, AssetData& assetData);
        bool LoadAssetFromREDkit(const std::string& assetName, AssetType type, AssetData& assetData);
        bool LoadAssetFromFileSystem(const std::string& filePath, AssetData& assetData);

        // Validation methods
        bool ValidateBundleFile(const std::string& bundlePath) const;
        bool ValidateAssetData(const AssetData& assetData) const;
        bool CheckAssetDependencies(const AssetData& assetData) const;

        // Error handling
        void SetError(const std::string& error);
        void ClearError();

        // File system utilities
        bool CreateDirectoryIfNotExists(const std::string& path) const;
        bool FileExists(const std::string& filePath) const;
        size_t GetFileSize(const std::string& filePath) const;
        std::string GetFileName(const std::string& filePath) const;
        std::string GetFileExtension(const std::string& filePath) const;

        // Constants
        static const std::vector<std::string> SUPPORTED_BUNDLE_EXTENSIONS;
        static const std::vector<std::string> SUPPORTED_ASSET_EXTENSIONS;
        static const size_t MAX_ASSET_SIZE;
        static const size_t MAX_BUNDLE_SIZE;
    };
}
