#include "Engine/Core/CommandLine.hpp"
#include "Engine/Core/Engine.hpp"
#include "Engine/Core/Version.hpp"
#include "Engine/Platform/Win32Window.hpp"
#include "Engine/RHI/BackendFactory.hpp"
#include "Engine/RHI/DX12/DX12TriangleRenderer.hpp"
#include "Engine/RHI/RendererBackend.hpp"
#include "Engine/RHI/Vulkan/VulkanTriangleRenderer.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    std::cout << HFEngine::Core::GetEngineName() << ' '
              << HFEngine::Core::GetEngineVersionString() << '\n';

    const HFEngine::Core::CommandLineResult commandLine = HFEngine::Core::ParseCommandLine(argc, argv);
    if (commandLine.helpRequested)
    {
        std::cout << HFEngine::Core::GetCommandLineUsage(argc > 0 ? argv[0] : "HFEngineSandbox");
        return 0;
    }

    if (!commandLine.success)
    {
        std::cerr << commandLine.message << '\n';
        std::cerr << HFEngine::Core::GetCommandLineUsage(argc > 0 ? argv[0] : "HFEngineSandbox");
        return 2;
    }

    HFEngine::Core::EngineRuntime runtime;
    if (!runtime.Initialize(commandLine.config))
    {
        std::cerr << "Failed to initialize engine runtime\n";
        return 1;
    }

    const HFEngine::RHI::DeviceCapabilities plannedCapabilities =
        HFEngine::RHI::GetPlannedDeviceCapabilities(runtime.Backend(), runtime.ValidationEnabled());
    const HFEngine::RHI::BackendAvailability availability =
        HFEngine::RHI::QueryBackendAvailability(runtime.Backend());

    std::cout << "Runtime initialized for: " << runtime.ApplicationName() << '\n';
    std::cout << "Selected renderer: " << HFEngine::RHI::ToString(runtime.Backend()) << '\n';
    std::cout << "Validation: " << (runtime.ValidationEnabled() ? "enabled" : "disabled") << '\n';
    std::cout << "Adapter target: " << plannedCapabilities.adapterName << '\n';
    std::cout << "Ray tracing target: "
              << (plannedCapabilities.supportsHardwareRayTracing ? "enabled" : "disabled") << '\n';
    std::cout << "Path tracing target: "
              << (plannedCapabilities.supportsPathTracing ? "enabled" : "disabled") << '\n';
    std::cout << "Backend implementation status: "
              << (availability.runtimeAvailable ? "available" : availability.reason) << '\n';
    std::cout << "Backend switching: launch-time via --renderer dx12|vulkan\n";
    std::cout << "Current visible milestone: render the same indexed cube mesh through both backends\n";
    std::cout << "Runtime/debug tooling planned before full editor: ImGui backend/status overlay\n";

    if (runtime.Backend() == HFEngine::RHI::RendererBackend::DirectX12)
    {
        HFEngine::Platform::Win32Window window;
        HFEngine::Platform::WindowDesc windowDesc;
        windowDesc.title = L"HFEngine - DirectX 12 Mesh";

        if (!window.Create(windowDesc))
        {
            std::cerr << "Failed to create Win32 window\n";
            runtime.Shutdown();
            return 1;
        }

        const HFEngine::RHI::DX12::TriangleRunResult triangle =
            HFEngine::RHI::DX12::RunTriangleSandbox(commandLine.config, window);
        if (!triangle.success)
        {
            std::cerr << "DX12 mesh render failed: " << triangle.message << '\n';
            runtime.Shutdown();
            return 1;
        }

        std::cout << "DX12 adapter: " << triangle.adapterName << '\n';
        std::cout << "DX12 frames rendered: " << triangle.framesRendered << '\n';
    }
    else if (runtime.Backend() == HFEngine::RHI::RendererBackend::Vulkan)
    {
        HFEngine::Platform::Win32Window window;
        HFEngine::Platform::WindowDesc windowDesc;
        windowDesc.title = L"HFEngine - Vulkan Mesh";

        if (!window.Create(windowDesc))
        {
            std::cerr << "Failed to create Win32 window\n";
            runtime.Shutdown();
            return 1;
        }

        const HFEngine::RHI::Vulkan::TriangleRunResult triangle =
            HFEngine::RHI::Vulkan::RunTriangleSandbox(commandLine.config, window);
        if (!triangle.success)
        {
            std::cerr << "Vulkan mesh render failed: " << triangle.message << '\n';
            runtime.Shutdown();
            return 1;
        }

        std::cout << "Vulkan adapter: " << triangle.adapterName << '\n';
        std::cout << "Vulkan frames rendered: " << triangle.framesRendered << '\n';
    }
    else
    {
        std::cerr << "Selected renderer is not implemented.\n";
        runtime.Shutdown();
        return 2;
    }

    runtime.Shutdown();
    std::cout << "Runtime shutdown complete\n";

    return 0;
}
