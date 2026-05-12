#include "Engine/Core/Engine.hpp"
#include "Engine/Core/CommandLine.hpp"
#include "Engine/RHI/BackendFactory.hpp"
#include "Engine/RHI/RendererBackend.hpp"
#include "Engine/Core/Version.hpp"
#include "TestHarness.hpp"

#include <iostream>
#include <span>
#include <string_view>

HFENGINE_TEST_CASE("unit.core.version", "ReportsConfiguredVersion")
{
    const HFEngine::Core::Version version = HFEngine::Core::GetEngineVersion();

    HFENGINE_REQUIRE(version.major == 0);
    HFENGINE_REQUIRE(version.minor == 1);
    HFENGINE_REQUIRE(version.patch == 0);
    HFENGINE_REQUIRE(HFEngine::Core::GetEngineVersionString() == "0.1.0");
}

HFENGINE_TEST_CASE("unit.core.version", "ReportsStableEngineName")
{
    HFENGINE_REQUIRE(std::string_view(HFEngine::Core::GetEngineName()) == "HFEngine");
}

HFENGINE_TEST_CASE("unit.core.engine", "RejectsEmptyApplicationName")
{
    HFEngine::Core::EngineConfig config;
    config.applicationName.clear();

    const HFEngine::Core::StartupResult result = HFEngine::Core::ValidateEngineConfig(config);

    HFENGINE_REQUIRE(!result.success);
    HFENGINE_REQUIRE(!result.message.empty());
}

HFENGINE_TEST_CASE("unit.rhi.backend", "ParsesDirectX12Aliases")
{
    HFENGINE_REQUIRE(HFEngine::RHI::ParseRendererBackend("dx12").success);
    HFENGINE_REQUIRE(HFEngine::RHI::ParseRendererBackend("d3d12").backend ==
                     HFEngine::RHI::RendererBackend::DirectX12);
    HFENGINE_REQUIRE(HFEngine::RHI::ParseRendererBackend("DirectX12").backend ==
                     HFEngine::RHI::RendererBackend::DirectX12);
    HFENGINE_REQUIRE(std::string_view(HFEngine::RHI::ToCommandLineValue(
                         HFEngine::RHI::RendererBackend::DirectX12)) == "dx12");
}

HFENGINE_TEST_CASE("unit.rhi.backend", "ParsesVulkanAliases")
{
    HFENGINE_REQUIRE(HFEngine::RHI::ParseRendererBackend("vulkan").success);
    HFENGINE_REQUIRE(HFEngine::RHI::ParseRendererBackend("vk").backend ==
                     HFEngine::RHI::RendererBackend::Vulkan);
    HFENGINE_REQUIRE(HFEngine::RHI::ParseRendererBackend("VULKAN").backend ==
                     HFEngine::RHI::RendererBackend::Vulkan);
    HFENGINE_REQUIRE(std::string_view(HFEngine::RHI::ToCommandLineValue(
                         HFEngine::RHI::RendererBackend::Vulkan)) == "vulkan");
}

HFENGINE_TEST_CASE("unit.rhi.backend", "RejectsUnknownBackend")
{
    const HFEngine::RHI::RendererBackendParseResult parsed =
        HFEngine::RHI::ParseRendererBackend("metal");

    HFENGINE_REQUIRE(!parsed.success);
    HFENGINE_REQUIRE(std::string_view(parsed.message).find("dx12") != std::string_view::npos);
}

HFENGINE_TEST_CASE("unit.core.command_line", "DefaultsToDirectX12WithValidation")
{
    constexpr std::string_view arguments[] = { "HFEngineSandbox" };

    const HFEngine::Core::CommandLineResult result = HFEngine::Core::ParseCommandLine(arguments);

    HFENGINE_REQUIRE(result.success);
    HFENGINE_REQUIRE(result.config.rendererBackend == HFEngine::RHI::RendererBackend::DirectX12);
    HFENGINE_REQUIRE(result.config.enableValidation);
}

HFENGINE_TEST_CASE("unit.core.command_line", "ParsesRendererSeparatedValue")
{
    constexpr std::string_view arguments[] = { "HFEngineSandbox", "--renderer", "vulkan" };

    const HFEngine::Core::CommandLineResult result = HFEngine::Core::ParseCommandLine(arguments);

    HFENGINE_REQUIRE(result.success);
    HFENGINE_REQUIRE(result.config.rendererBackend == HFEngine::RHI::RendererBackend::Vulkan);
}

HFENGINE_TEST_CASE("unit.core.command_line", "ParsesRendererEqualsValue")
{
    constexpr std::string_view arguments[] = { "HFEngineSandbox", "--renderer=dx12", "--no-validation" };

    const HFEngine::Core::CommandLineResult result = HFEngine::Core::ParseCommandLine(arguments);

    HFENGINE_REQUIRE(result.success);
    HFENGINE_REQUIRE(result.config.rendererBackend == HFEngine::RHI::RendererBackend::DirectX12);
    HFENGINE_REQUIRE(!result.config.enableValidation);
}

HFENGINE_TEST_CASE("unit.core.command_line", "RejectsUnknownArguments")
{
    constexpr std::string_view arguments[] = { "HFEngineSandbox", "--unknown" };

    const HFEngine::Core::CommandLineResult result = HFEngine::Core::ParseCommandLine(arguments);

    HFENGINE_REQUIRE(!result.success);
    HFENGINE_REQUIRE(!result.message.empty());
}

HFENGINE_TEST_CASE("unit.core.command_line", "ParsesBoundedFrameCount")
{
    constexpr std::string_view arguments[] = { "HFEngineSandbox", "--frames", "3" };

    const HFEngine::Core::CommandLineResult result = HFEngine::Core::ParseCommandLine(arguments);

    HFENGINE_REQUIRE(result.success);
    HFENGINE_REQUIRE(result.config.maxFrames == 3);
}

HFENGINE_TEST_CASE("unit.rhi.capabilities", "DescribesPlannedBackendCapabilities")
{
    const HFEngine::RHI::DeviceCapabilities capabilities =
        HFEngine::RHI::GetPlannedDeviceCapabilities(HFEngine::RHI::RendererBackend::Vulkan, true);

    HFENGINE_REQUIRE(capabilities.backend == HFEngine::RHI::RendererBackend::Vulkan);
    HFENGINE_REQUIRE(capabilities.validationEnabled);
    HFENGINE_REQUIRE(capabilities.supportsHardwareRayTracing);
    HFENGINE_REQUIRE(capabilities.supportsPathTracing);
    HFENGINE_REQUIRE(capabilities.framesInFlight == 2);
    HFENGINE_REQUIRE(!capabilities.adapterName.empty());
}

int main(int argc, char** argv)
{
    return HFEngine::Testing::RunAllTests(argc, argv, std::cout);
}
