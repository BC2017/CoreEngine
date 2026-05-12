#pragma once

#include "Engine/Core/Engine.hpp"
#include "Engine/Platform/Win32Window.hpp"

#include <string>

namespace HFEngine::RHI::DX12
{
    struct TriangleRunResult
    {
        bool success = false;
        std::string message;
        std::string adapterName;
        unsigned int framesRendered = 0;
    };

    [[nodiscard]] TriangleRunResult RunTriangleSandbox(
        const Core::EngineConfig& config,
        Platform::Win32Window& window);
}
