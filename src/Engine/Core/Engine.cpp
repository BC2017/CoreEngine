#include "Engine/Core/Engine.hpp"

namespace HFEngine::Core
{
    StartupResult ValidateEngineConfig(const EngineConfig& config)
    {
        if (config.applicationName.empty())
        {
            return { false, "applicationName must not be empty" };
        }

        return { true, "ok" };
    }

    bool EngineRuntime::Initialize(const EngineConfig& config)
    {
        const StartupResult validation = ValidateEngineConfig(config);
        if (!validation.success)
        {
            return false;
        }

        applicationName_ = config.applicationName;
        validationEnabled_ = config.enableValidation;
        backend_ = config.rendererBackend;
        initialized_ = true;
        return true;
    }

    void EngineRuntime::Shutdown()
    {
        initialized_ = false;
        validationEnabled_ = false;
        backend_ = RHI::RendererBackend::DirectX12;
        applicationName_.clear();
    }

    bool EngineRuntime::IsInitialized() const noexcept
    {
        return initialized_;
    }

    std::string_view EngineRuntime::ApplicationName() const noexcept
    {
        return applicationName_;
    }

    RHI::RendererBackend EngineRuntime::Backend() const noexcept
    {
        return backend_;
    }

    bool EngineRuntime::ValidationEnabled() const noexcept
    {
        return validationEnabled_;
    }
}
