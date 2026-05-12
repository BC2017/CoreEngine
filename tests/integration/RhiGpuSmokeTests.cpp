#include "Engine/RHI/BackendFactory.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Platform/Win32Window.hpp"
#include "Engine/RHI/DX12/DX12TriangleRenderer.hpp"
#include "Engine/RHI/RendererBackend.hpp"
#include "Engine/RHI/Vulkan/VulkanTriangleRenderer.hpp"
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

HFENGINE_TEST_CASE("gpu.rhi.dx12", "CreatesDeviceSwapchainAndSubmitsTriangleFrame")
{
    RequireBackendOrSkip(HFEngine::RHI::RendererBackend::DirectX12);

    HFEngine::Platform::Win32Window window;
    HFEngine::Platform::WindowDesc desc;
    desc.title = L"HFEngine DX12 Smoke Test";
    desc.width = 320;
    desc.height = 240;

    HFENGINE_REQUIRE(window.Create(desc));

    HFEngine::Core::EngineConfig config;
    config.applicationName = "DX12 Smoke Test";
    config.rendererBackend = HFEngine::RHI::RendererBackend::DirectX12;
    config.enableValidation = false;
    config.maxFrames = 1;

    const HFEngine::RHI::DX12::TriangleRunResult result =
        HFEngine::RHI::DX12::RunTriangleSandbox(config, window);

    HFENGINE_REQUIRE(result.success);
    HFENGINE_REQUIRE(result.framesRendered == 1);
    HFENGINE_REQUIRE(!result.adapterName.empty());
}

HFENGINE_TEST_CASE("gpu.rhi.vulkan", "CreatesDeviceSwapchainAndSubmitsTriangleFrame")
{
    RequireBackendOrSkip(HFEngine::RHI::RendererBackend::Vulkan);

    HFEngine::Platform::Win32Window window;
    HFEngine::Platform::WindowDesc desc;
    desc.title = L"HFEngine Vulkan Smoke Test";
    desc.width = 320;
    desc.height = 240;

    HFENGINE_REQUIRE(window.Create(desc));

    HFEngine::Core::EngineConfig config;
    config.applicationName = "Vulkan Smoke Test";
    config.rendererBackend = HFEngine::RHI::RendererBackend::Vulkan;
    config.enableValidation = false;
    config.maxFrames = 1;

    const HFEngine::RHI::Vulkan::TriangleRunResult result =
        HFEngine::RHI::Vulkan::RunTriangleSandbox(config, window);

    HFENGINE_REQUIRE(result.success);
    HFENGINE_REQUIRE(result.framesRendered == 1);
    HFENGINE_REQUIRE(!result.adapterName.empty());
}

int main(int argc, char** argv)
{
    return HFEngine::Testing::RunAllTests(argc, argv, std::cout);
}
