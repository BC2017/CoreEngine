#include "Engine/Asset/AssetRegistry.hpp"

#include <utility>

namespace HFEngine::Asset
{
    bool AssetId::IsValid() const noexcept
    {
        return value != 0;
    }

    std::uint64_t HashString(std::string_view value) noexcept
    {
        std::uint64_t hash = 14695981039346656037ull;
        for (const char character : value)
        {
            hash ^= static_cast<unsigned char>(character);
            hash *= 1099511628211ull;
        }

        return hash;
    }

    AssetId MakeAssetId(AssetType type, std::string_view normalizedSourcePath) noexcept
    {
        std::uint64_t hash = HashString(normalizedSourcePath);
        hash ^= static_cast<std::uint64_t>(type) + 0x9e3779b97f4a7c15ull + (hash << 6u) + (hash >> 2u);
        return { hash == 0 ? 1 : hash };
    }

    const char* ToString(AssetType type) noexcept
    {
        switch (type)
        {
        case AssetType::Unknown:
            return "Unknown";
        case AssetType::Mesh:
            return "Mesh";
        case AssetType::Material:
            return "Material";
        case AssetType::Texture:
            return "Texture";
        case AssetType::Shader:
            return "Shader";
        case AssetType::Scene:
            return "Scene";
        case AssetType::Audio:
            return "Audio";
        }

        return "Unknown";
    }

    AssetRecord AssetRegistry::RegisterSource(AssetType type, std::string sourcePath, std::string cookedPath)
    {
        AssetRecord record;
        record.type = type;
        record.sourceHash = HashString(sourcePath);
        record.id = MakeAssetId(type, sourcePath);
        record.sourcePath = std::move(sourcePath);
        record.cookedPath = std::move(cookedPath);

        records_[record.id.value] = record;
        return record;
    }

    bool AssetRegistry::AddDependency(AssetId asset, AssetId dependency)
    {
        auto assetIterator = records_.find(asset.value);
        if (assetIterator == records_.end() || records_.find(dependency.value) == records_.end())
        {
            return false;
        }

        assetIterator->second.dependencies.push_back(dependency);
        return true;
    }

    const AssetRecord* AssetRegistry::Find(AssetId id) const
    {
        const auto iterator = records_.find(id.value);
        if (iterator == records_.end())
        {
            return nullptr;
        }

        return &iterator->second;
    }

    std::size_t AssetRegistry::Count() const noexcept
    {
        return records_.size();
    }

    CookedAssetHeader MakeCookedHeader(const AssetRecord& record) noexcept
    {
        CookedAssetHeader header;
        header.type = record.type;
        header.id = record.id;
        header.sourceHash = record.sourceHash;
        header.dependencyCount = static_cast<std::uint32_t>(record.dependencies.size());
        return header;
    }
}
