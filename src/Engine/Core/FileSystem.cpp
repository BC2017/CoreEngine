#include "Engine/Core/FileSystem.hpp"

namespace HFEngine::Core
{
    void VirtualFileSystem::Mount(std::string rootName, std::filesystem::path path)
    {
        roots_[std::move(rootName)] = std::filesystem::weakly_canonical(path);
    }

    bool VirtualFileSystem::IsMounted(std::string_view rootName) const
    {
        return roots_.contains(std::string(rootName));
    }

    std::optional<std::filesystem::path> VirtualFileSystem::Resolve(
        std::string_view rootName,
        std::filesystem::path relativePath) const
    {
        const auto iterator = roots_.find(std::string(rootName));
        if (iterator == roots_.end() || relativePath.is_absolute())
        {
            return std::nullopt;
        }

        return (iterator->second / relativePath).lexically_normal();
    }
}
