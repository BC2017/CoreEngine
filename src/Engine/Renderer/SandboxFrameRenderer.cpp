#include "Engine/Renderer/SandboxFrameRenderer.hpp"

#include "Engine/Platform/Win32Window.hpp"
#include "Engine/RHI/DX12/DX12SandboxRenderer.hpp"
#include "Engine/RHI/Vulkan/VulkanSandboxRenderer.hpp"

namespace HFEngine::Renderer
{
    const wchar_t* SandboxWindowTitle(RHI::RendererBackend backend) noexcept
    {
        switch (backend)
        {
        case RHI::RendererBackend::DirectX12:
            return L"HFEngine - DirectX 12 Mesh";
        case RHI::RendererBackend::Vulkan:
            return L"HFEngine - Vulkan Mesh";
        }

        return L"HFEngine";
    }

    SandboxFrameRenderResult RunSandboxFrameRenderer(const Core::EngineConfig& config)
    {
        SandboxFrameRenderResult result;
        result.backend = config.rendererBackend;

        Platform::Win32Window window;
        Platform::WindowDesc windowDesc;
        windowDesc.title = SandboxWindowTitle(config.rendererBackend);

        if (!window.Create(windowDesc))
        {
            result.message = "Failed to create Win32 window";
            return result;
        }

        switch (config.rendererBackend)
        {
        case RHI::RendererBackend::DirectX12:
        {
            const RHI::DX12::SandboxRenderResult backendResult =
                RHI::DX12::RunSandboxRenderer(config, window);
            result.success = backendResult.success;
            result.message = backendResult.message;
            result.adapterName = backendResult.adapterName;
            result.framesRendered = backendResult.framesRendered;
            return result;
        }
        case RHI::RendererBackend::Vulkan:
        {
            const RHI::Vulkan::SandboxRenderResult backendResult =
                RHI::Vulkan::RunSandboxRenderer(config, window);
            result.success = backendResult.success;
            result.message = backendResult.message;
            result.adapterName = backendResult.adapterName;
            result.framesRendered = backendResult.framesRendered;
            return result;
        }
        }

        result.message = "Selected renderer is not implemented";
        return result;
    }
}
