#pragma once

#include <cstdint>

namespace HFEngine::Core
{
    struct FrameTime
    {
        double deltaSeconds = 0.0;
        double totalSeconds = 0.0;
        std::uint64_t frameIndex = 0;
    };

    class FixedStepClock
    {
    public:
        explicit FixedStepClock(double fixedDeltaSeconds = 1.0 / 60.0) noexcept;

        [[nodiscard]] FrameTime Tick() noexcept;
        void Reset() noexcept;

    private:
        double fixedDeltaSeconds_ = 1.0 / 60.0;
        double totalSeconds_ = 0.0;
        std::uint64_t frameIndex_ = 0;
    };
}
