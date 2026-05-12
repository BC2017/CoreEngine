#pragma once

#include "Engine/RHI/Device.hpp"
#include "Engine/RHI/RendererBackend.hpp"

namespace HFEngine::RHI
{
    struct BackendAvailability
    {
        RendererBackend backend = RendererBackend::DirectX12;
        bool compiled = false;
        bool runtimeAvailable = false;
        const char* reason = "Backend implementation has not been added yet";
    };

    [[nodiscard]] BackendAvailability QueryBackendAvailability(RendererBackend backend) noexcept;
    [[nodiscard]] DeviceCapabilities GetPlannedDeviceCapabilities(RendererBackend backend, bool validationEnabled);
}
