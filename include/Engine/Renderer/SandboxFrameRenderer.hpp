#pragma once

#include "Engine/Core/Engine.hpp"
#include "Engine/RHI/RendererBackend.hpp"
#include "Engine/Scene/Scene.hpp"

#include <string>

namespace HFEngine::Renderer
{
    struct SandboxFrameRenderResult
    {
        bool success = false;
        std::string message;
        RHI::RendererBackend backend = RHI::RendererBackend::DirectX12;
        std::string adapterName;
        unsigned int framesRendered = 0;
    };

    [[nodiscard]] const wchar_t* SandboxWindowTitle(RHI::RendererBackend backend) noexcept;
    [[nodiscard]] Scene::SceneRenderData BuildSandboxSceneRenderData();
    [[nodiscard]] SandboxFrameRenderResult RunSandboxFrameRenderer(const Core::EngineConfig& config);
}
