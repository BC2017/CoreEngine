#pragma once

#include <string>

namespace HFEngine::Core
{
    struct Version
    {
        int major = HFENGINE_VERSION_MAJOR;
        int minor = HFENGINE_VERSION_MINOR;
        int patch = HFENGINE_VERSION_PATCH;
    };

    [[nodiscard]] Version GetEngineVersion() noexcept;
    [[nodiscard]] std::string GetEngineVersionString();
    [[nodiscard]] const char* GetEngineName() noexcept;
}

