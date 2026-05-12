#include "Engine/RHI/BackendFactory.hpp"
#include "Engine/RHI/RendererBackend.hpp"
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
}

HFENGINE_TEST_CASE("gpu.rhi.vulkan", "CreatesDeviceSwapchainAndSubmitsTriangleFrame")
{
    RequireBackendOrSkip(HFEngine::RHI::RendererBackend::Vulkan);
}

int main(int argc, char** argv)
{
    return HFEngine::Testing::RunAllTests(argc, argv, std::cout);
}
