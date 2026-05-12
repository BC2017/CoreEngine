#include "Engine/Core/CommandLine.hpp"

#include <vector>

namespace HFEngine::Core
{
    namespace
    {
        constexpr std::string_view RendererPrefix = "--renderer=";

        bool StartsWith(std::string_view value, std::string_view prefix) noexcept
        {
            return value.size() >= prefix.size() && value.substr(0, prefix.size()) == prefix;
        }
    }

    CommandLineResult ParseCommandLine(std::span<const std::string_view> arguments)
    {
        CommandLineResult result;
        result.success = true;
        result.config.applicationName = "HFEngine Sandbox";

        for (std::size_t index = 1; index < arguments.size(); ++index)
        {
            const std::string_view argument = arguments[index];

            if (argument == "--help" || argument == "-h")
            {
                result.helpRequested = true;
                result.message = "help requested";
                return result;
            }

            if (argument == "--renderer")
            {
                if (index + 1 >= arguments.size())
                {
                    result.success = false;
                    result.message = "--renderer requires a value: dx12 or vulkan";
                    return result;
                }

                const RHI::RendererBackendParseResult parsed = RHI::ParseRendererBackend(arguments[++index]);
                if (!parsed.success)
                {
                    result.success = false;
                    result.message = parsed.message;
                    return result;
                }

                result.config.rendererBackend = parsed.backend;
                continue;
            }

            if (StartsWith(argument, RendererPrefix))
            {
                const RHI::RendererBackendParseResult parsed =
                    RHI::ParseRendererBackend(argument.substr(RendererPrefix.size()));
                if (!parsed.success)
                {
                    result.success = false;
                    result.message = parsed.message;
                    return result;
                }

                result.config.rendererBackend = parsed.backend;
                continue;
            }

            if (argument == "--no-validation")
            {
                result.config.enableValidation = false;
                continue;
            }

            result.success = false;
            result.message = "Unknown argument: ";
            result.message += argument;
            return result;
        }

        return result;
    }

    CommandLineResult ParseCommandLine(int argc, char** argv)
    {
        std::vector<std::string_view> arguments;
        arguments.reserve(static_cast<std::size_t>(argc));

        for (int index = 0; index < argc; ++index)
        {
            arguments.push_back(argv[index] != nullptr ? std::string_view(argv[index]) : std::string_view());
        }

        return ParseCommandLine(std::span<const std::string_view>(arguments.data(), arguments.size()));
    }

    std::string GetCommandLineUsage(std::string_view executableName)
    {
        std::string usage;
        usage += "Usage: ";
        usage += executableName.empty() ? "HFEngineSandbox" : executableName;
        usage += " [--renderer dx12|vulkan] [--no-validation]\n";
        usage += "Backend switching is launch-time only for the first milestone.\n";
        return usage;
    }
}
