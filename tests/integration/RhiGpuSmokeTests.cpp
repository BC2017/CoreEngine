#include "Engine/RHI/BackendFactory.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/RHI/RendererBackend.hpp"
#include "Engine/Renderer/SandboxFrameRenderer.hpp"
#include "TestHarness.hpp"

#include <iostream>
#include <string>

namespace
{
    void RequireBackendOrSkip(HFEngine::RHI::RendererBackend backend)
    {
        const HFEngine::RHI::BackendAvailability availability =
            HFEngine::RHI::QueryBackendAvailability(backend);

        if (!availability.compiled || !availability.runtimeAvailable)
        {
            std::string message = HFEngine::RHI::ToString(backend);
            message += " GPU backend unavailable: ";
            message += availability.reason;
            HFENGINE_SKIP(message);
        }
    }
}

HFENGINE_TEST_CASE("gpu.rhi.dx12", "CreatesDeviceSwapchainAndSubmitsIndexedMeshFrame")
{
    RequireBackendOrSkip(HFEngine::RHI::RendererBackend::DirectX12);

    HFEngine::Core::EngineConfig config;
    config.applicationName = "DX12 Smoke Test";
    config.rendererBackend = HFEngine::RHI::RendererBackend::DirectX12;
    config.enableValidation = false;
    config.maxFrames = 1;

    const HFEngine::Renderer::SandboxFrameRenderResult result =
        HFEngine::Renderer::RunSandboxFrameRenderer(config);

    HFENGINE_REQUIRE(result.success);
    HFENGINE_REQUIRE(result.backend == HFEngine::RHI::RendererBackend::DirectX12);
    HFENGINE_REQUIRE(result.framesRendered == 1);
    HFENGINE_REQUIRE(!result.adapterName.empty());
}

HFENGINE_TEST_CASE("gpu.rhi.vulkan", "CreatesDeviceSwapchainAndSubmitsIndexedMeshFrame")
{
    RequireBackendOrSkip(HFEngine::RHI::RendererBackend::Vulkan);

    HFEngine::Core::EngineConfig config;
    config.applicationName = "Vulkan Smoke Test";
    config.rendererBackend = HFEngine::RHI::RendererBackend::Vulkan;
    config.enableValidation = false;
    config.maxFrames = 1;

    const HFEngine::Renderer::SandboxFrameRenderResult result =
        HFEngine::Renderer::RunSandboxFrameRenderer(config);

    HFENGINE_REQUIRE(result.success);
    HFENGINE_REQUIRE(result.backend == HFEngine::RHI::RendererBackend::Vulkan);
    HFENGINE_REQUIRE(result.framesRendered == 1);
    HFENGINE_REQUIRE(!result.adapterName.empty());
}

int main(int argc, char** argv)
{
    return HFEngine::Testing::RunAllTests(argc, argv, std::cout);
}
