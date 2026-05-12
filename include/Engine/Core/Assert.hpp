#pragma once

#include <source_location>
#include <string>
#include <string_view>

namespace HFEngine::Core
{
    struct CheckResult
    {
        bool passed = false;
        std::string message;
        std::source_location location;
    };

    [[nodiscard]] CheckResult Check(
        bool condition,
        std::string_view expression,
        std::string_view message = {},
        std::source_location location = std::source_location::current());
}
