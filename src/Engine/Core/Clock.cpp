#include "Engine/Core/Clock.hpp"

namespace HFEngine::Core
{
    FixedStepClock::FixedStepClock(double fixedDeltaSeconds) noexcept
        : fixedDeltaSeconds_(fixedDeltaSeconds)
    {
    }

    FrameTime FixedStepClock::Tick() noexcept
    {
        totalSeconds_ += fixedDeltaSeconds_;
        ++frameIndex_;
        return { fixedDeltaSeconds_, totalSeconds_, frameIndex_ };
    }

    void FixedStepClock::Reset() noexcept
    {
        totalSeconds_ = 0.0;
        frameIndex_ = 0;
    }
}
