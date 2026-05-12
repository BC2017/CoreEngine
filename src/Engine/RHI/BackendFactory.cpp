#include "Engine/RHI/BackendFactory.hpp"

namespace HFEngine::RHI
{
    BackendAvailability QueryBackendAvailability(RendererBackend backend) noexcept
    {
        if (backend == RendererBackend::DirectX12)
        {
            return {
                backend,
                true,
                true,
                "DirectX 12 triangle backend is available"
            };
        }

        return {
            backend,
            true,
            true,
            "Vulkan triangle backend is available"
        };
    }

    DeviceCapabilities GetPlannedDeviceCapabilities(RendererBackend backend, bool validationEnabled)
    {
        DeviceCapabilities capabilities;
        capabilities.backend = backend;
        capabilities.validationEnabled = validationEnabled;
        capabilities.adapterName = backend == RendererBackend::DirectX12
            ? "DX12 backend enumerates the active adapter at runtime"
            : "Vulkan backend enumerates the active adapter at runtime";
        capabilities.supportsHardwareRayTracing = true;
        capabilities.supportsPathTracing = true;
        capabilities.framesInFlight = 2;
        return capabilities;
    }
}
