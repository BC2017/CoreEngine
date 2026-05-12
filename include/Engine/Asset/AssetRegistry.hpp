#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace HFEngine::Asset
{
    enum class AssetType
    {
        Unknown,
        Mesh,
        Material,
        Texture,
        Shader,
        Scene,
        Audio
    };

    struct AssetId
    {
        std::uint64_t value = 0;

        [[nodiscard]] bool IsValid() const noexcept;
        [[nodiscard]] friend bool operator==(AssetId left, AssetId right) noexcept = default;
    };

    struct AssetRecord
    {
        AssetId id;
        AssetType type = AssetType::Unknown;
        std::string sourcePath;
        std::string cookedPath;
        std::uint64_t sourceHash = 0;
        std::vector<AssetId> dependencies;
    };

    struct CookedAssetHeader
    {
        char magic[4] = { 'H', 'F', 'A', 'C' };
        std::uint32_t version = 1;
        AssetType type = AssetType::Unknown;
        AssetId id;
        std::uint64_t sourceHash = 0;
        std::uint32_t dependencyCount = 0;
    };

    [[nodiscard]] std::uint64_t HashString(std::string_view value) noexcept;
    [[nodiscard]] AssetId MakeAssetId(AssetType type, std::string_view normalizedSourcePath) noexcept;
    [[nodiscard]] const char* ToString(AssetType type) noexcept;

    class AssetRegistry
    {
    public:
        [[nodiscard]] AssetRecord RegisterSource(
            AssetType type,
            std::string sourcePath,
            std::string cookedPath = {});

        [[nodiscard]] bool AddDependency(AssetId asset, AssetId dependency);
        [[nodiscard]] const AssetRecord* Find(AssetId id) const;
        [[nodiscard]] std::size_t Count() const noexcept;

    private:
        std::unordered_map<std::uint64_t, AssetRecord> records_;
    };

    [[nodiscard]] CookedAssetHeader MakeCookedHeader(const AssetRecord& record) noexcept;
}
