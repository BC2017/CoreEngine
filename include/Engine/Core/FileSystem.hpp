#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace HFEngine::Core
{
    class VirtualFileSystem
    {
    public:
        void Mount(std::string rootName, std::filesystem::path path);

        [[nodiscard]] bool IsMounted(std::string_view rootName) const;
        [[nodiscard]] std::optional<std::filesystem::path> Resolve(
            std::string_view rootName,
            std::filesystem::path relativePath) const;

    private:
        std::unordered_map<std::string, std::filesystem::path> roots_;
    };
}
