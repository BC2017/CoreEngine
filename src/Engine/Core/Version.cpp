#include "Engine/Core/Version.hpp"

#include <sstream>

namespace HFEngine::Core
{
    Version GetEngineVersion() noexcept
    {
        return {};
    }

    std::string GetEngineVersionString()
    {
        const Version version = GetEngineVersion();

        std::ostringstream stream;
        stream << version.major << '.' << version.minor << '.' << version.patch;
        return stream.str();
    }

    const char* GetEngineName() noexcept
    {
        return "HFEngine";
    }
}

