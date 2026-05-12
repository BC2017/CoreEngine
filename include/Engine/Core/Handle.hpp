#pragma once

#include <cstdint>

namespace HFEngine::Core
{
    template <typename Tag>
    struct Handle
    {
        std::uint32_t index = 0;
        std::uint32_t generation = 0;

        [[nodiscard]] constexpr bool IsValid() const noexcept
        {
            return generation != 0;
        }

        [[nodiscard]] friend constexpr bool operator==(Handle left, Handle right) noexcept
        {
            return left.index == right.index && left.generation == right.generation;
        }
    };
}
