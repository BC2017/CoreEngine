#pragma once

#include "Engine/RHI/RendererBackend.hpp"

#include <cstdint>
#include <string>
#include <string_view>

namespace HFEngine::Core
{
    struct EngineConfig
    {
        std::string applicationName = "HFEngine Application";
        bool enableValidation = true;
        RHI::RendererBackend rendererBackend = RHI::RendererBackend::DirectX12;
        std::uint32_t maxFrames = 0;
    };

    struct StartupResult
    {
        bool success = false;
        std::string message;
    };

    [[nodiscard]] StartupResult ValidateEngineConfig(const EngineConfig& config);

    class EngineRuntime
    {
    public:
        [[nodiscard]] bool Initialize(const EngineConfig& config);
        void Shutdown();

        [[nodiscard]] bool IsInitialized() const noexcept;
        [[nodiscard]] std::string_view ApplicationName() const noexcept;
        [[nodiscard]] RHI::RendererBackend Backend() const noexcept;
        [[nodiscard]] bool ValidationEnabled() const noexcept;

    private:
        bool initialized_ = false;
        bool validationEnabled_ = false;
        RHI::RendererBackend backend_ = RHI::RendererBackend::DirectX12;
        std::string applicationName_;
    };
}
