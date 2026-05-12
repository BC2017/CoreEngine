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
            false,
            false,
            "Concrete Vulkan backend implementation is scheduled after the DirectX 12 triangle path"
        };
    }

    DeviceCapabilities GetPlannedDeviceCapabilities(RendererBackend backend, bool validationEnabled)
    {
        DeviceCapabilities capabilities;
        capabilities.backend = backend;
        capabilities.validationEnabled = validationEnabled;
        capabilities.adapterName = backend == RendererBackend::DirectX12
            ? "DX12 backend enumerates the active adapter at runtime"
            : "RTX 4060 minimum target; adapter enumeration pending backend implementation";
        capabilities.supportsHardwareRayTracing = true;
        capabilities.supportsPathTracing = true;
        capabilities.framesInFlight = 2;
        return capabilities;
    }
}
