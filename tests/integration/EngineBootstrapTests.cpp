#include "Engine/Core/Engine.hpp"
#include "Engine/RHI/RendererBackend.hpp"
#include "TestHarness.hpp"

#include <iostream>
#include <string_view>

HFENGINE_TEST_CASE("integration.bootstrap", "RuntimeInitializesAndShutsDown")
{
    HFEngine::Core::EngineRuntime runtime;

    HFEngine::Core::EngineConfig config;
    config.applicationName = "Bootstrap Test";

    HFENGINE_REQUIRE(runtime.Initialize(config));
    HFENGINE_REQUIRE(runtime.IsInitialized());
    HFENGINE_REQUIRE(runtime.ApplicationName() == std::string_view("Bootstrap Test"));
    HFENGINE_REQUIRE(runtime.Backend() == HFEngine::RHI::RendererBackend::DirectX12);
    HFENGINE_REQUIRE(runtime.ValidationEnabled());

    runtime.Shutdown();

    HFENGINE_REQUIRE(!runtime.IsInitialized());
    HFENGINE_REQUIRE(runtime.ApplicationName().empty());
}

HFENGINE_TEST_CASE("integration.bootstrap", "RuntimeRemainsUninitializedAfterInvalidConfig")
{
    HFEngine::Core::EngineRuntime runtime;

    HFEngine::Core::EngineConfig config;
    config.applicationName.clear();

    HFENGINE_REQUIRE(!runtime.Initialize(config));
    HFENGINE_REQUIRE(!runtime.IsInitialized());
}

HFENGINE_TEST_CASE("integration.bootstrap", "RuntimeInitializesWithDirectX12Selection")
{
    HFEngine::Core::EngineRuntime runtime;

    HFEngine::Core::EngineConfig config;
    config.applicationName = "DX12 Bootstrap Test";
    config.rendererBackend = HFEngine::RHI::RendererBackend::DirectX12;

    HFENGINE_REQUIRE(runtime.Initialize(config));
    HFENGINE_REQUIRE(runtime.Backend() == HFEngine::RHI::RendererBackend::DirectX12);
}

HFENGINE_TEST_CASE("integration.bootstrap", "RuntimeInitializesWithVulkanSelection")
{
    HFEngine::Core::EngineRuntime runtime;

    HFEngine::Core::EngineConfig config;
    config.applicationName = "Vulkan Bootstrap Test";
    config.rendererBackend = HFEngine::RHI::RendererBackend::Vulkan;
    config.enableValidation = false;

    HFENGINE_REQUIRE(runtime.Initialize(config));
    HFENGINE_REQUIRE(runtime.Backend() == HFEngine::RHI::RendererBackend::Vulkan);
    HFENGINE_REQUIRE(!runtime.ValidationEnabled());
}

int main(int argc, char** argv)
{
    return HFEngine::Testing::RunAllTests(argc, argv, std::cout);
}
