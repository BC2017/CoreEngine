#include "Engine/RHI/BackendFactory.hpp"

namespace HFEngine::RHI
{
    BackendAvailability QueryBackendAvailability(RendererBackend backend) noexcept
    {
        return {
            backend,
            false,
            false,
            "Concrete DX12/Vulkan backend implementation is scheduled for the dual-backend triangle milestone"
        };
    }

    DeviceCapabilities GetPlannedDeviceCapabilities(RendererBackend backend, bool validationEnabled)
    {
        DeviceCapabilities capabilities;
        capabilities.backend = backend;
        capabilities.validationEnabled = validationEnabled;
        capabilities.adapterName = "RTX 4060 minimum target; adapter enumeration pending backend implementation";
        capabilities.supportsHardwareRayTracing = true;
        capabilities.supportsPathTracing = true;
        capabilities.framesInFlight = 2;
        return capabilities;
    }
}
