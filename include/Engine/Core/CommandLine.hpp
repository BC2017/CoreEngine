#pragma once

#include "Engine/Core/Engine.hpp"

#include <span>
#include <string>
#include <string_view>

namespace HFEngine::Core
{
    struct CommandLineResult
    {
        bool success = false;
        EngineConfig config;
        std::string message;
        bool helpRequested = false;
    };

    [[nodiscard]] CommandLineResult ParseCommandLine(std::span<const std::string_view> arguments);
    [[nodiscard]] CommandLineResult ParseCommandLine(int argc, char** argv);
    [[nodiscard]] std::string GetCommandLineUsage(std::string_view executableName);
}
