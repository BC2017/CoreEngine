#include "Engine/Core/Assert.hpp"

namespace HFEngine::Core
{
    CheckResult Check(
        bool condition,
        std::string_view expression,
        std::string_view message,
        std::source_location location)
    {
        CheckResult result;
        result.passed = condition;
        result.location = location;

        if (!condition)
        {
            result.message = "Check failed: ";
            result.message += expression;
            if (!message.empty())
            {
                result.message += " - ";
                result.message += message;
            }
        }

        return result;
    }
}
