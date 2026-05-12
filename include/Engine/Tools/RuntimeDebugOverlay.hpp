#pragma once

#include <string_view>

namespace HFEngine::Tools
{
    struct RuntimeDebugOverlayData
    {
        std::string_view backendName;
        std::string_view adapterName;
        bool validationEnabled = false;
        unsigned int framesRendered = 0;
        float frameTimeMilliseconds = 0.0f;
    };

    void DrawRuntimeDebugOverlay(const RuntimeDebugOverlayData& data);
}
