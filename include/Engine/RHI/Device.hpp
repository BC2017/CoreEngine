#pragma once

#include "Engine/RHI/RendererBackend.hpp"

#include <cstdint>
#include <string>
#include <string_view>

namespace HFEngine::RHI
{
    struct DeviceCapabilities
    {
        RendererBackend backend = RendererBackend::DirectX12;
        std::string adapterName = "No adapter selected";
        bool validationEnabled = false;
        bool supportsHardwareRayTracing = false;
        bool supportsPathTracing = false;
        std::uint32_t framesInFlight = 2;
    };

    class IShaderModule
    {
    public:
        virtual ~IShaderModule() = default;
        [[nodiscard]] virtual std::string_view DebugName() const noexcept = 0;
    };

    class IPipelineState
    {
    public:
        virtual ~IPipelineState() = default;
        [[nodiscard]] virtual std::string_view DebugName() const noexcept = 0;
    };

    class ICommandList
    {
    public:
        virtual ~ICommandList() = default;
        virtual void Begin() = 0;
        virtual void End() = 0;
    };

    class ISwapchain
    {
    public:
        virtual ~ISwapchain() = default;
        [[nodiscard]] virtual std::uint32_t Width() const noexcept = 0;
        [[nodiscard]] virtual std::uint32_t Height() const noexcept = 0;
    };

    class IDevice
    {
    public:
        virtual ~IDevice() = default;

        [[nodiscard]] virtual const DeviceCapabilities& Capabilities() const noexcept = 0;
        virtual void Shutdown() = 0;
    };
}
