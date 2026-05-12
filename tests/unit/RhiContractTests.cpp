#include "Engine/RHI/Resource.hpp"
#include "TestHarness.hpp"

HFENGINE_TEST_CASE("unit.rhi.resources", "ValidatesBufferDescriptor")
{
    HFEngine::RHI::BufferDesc desc;
    desc.debugName = "triangle vertices";
    desc.sizeBytes = 256;
    desc.usage = HFEngine::RHI::ResourceUsage_VertexBuffer | HFEngine::RHI::ResourceUsage_CopyDestination;
    desc.initialState = HFEngine::RHI::ResourceState::CopyDestination;

    const HFEngine::RHI::ValidationResult result = HFEngine::RHI::ValidateBufferDesc(desc);

    HFENGINE_REQUIRE(result.valid);
}

HFENGINE_TEST_CASE("unit.rhi.resources", "RejectsEmptyBuffer")
{
    HFEngine::RHI::BufferDesc desc;
    desc.usage = HFEngine::RHI::ResourceUsage_VertexBuffer;

    const HFEngine::RHI::ValidationResult result = HFEngine::RHI::ValidateBufferDesc(desc);

    HFENGINE_REQUIRE(!result.valid);
}

HFENGINE_TEST_CASE("unit.rhi.resources", "ValidatesRenderTargetTexture")
{
    HFEngine::RHI::TextureDesc desc;
    desc.debugName = "swapchain color";
    desc.format = HFEngine::RHI::ResourceFormat::Bgra8Unorm;
    desc.width = 1280;
    desc.height = 720;
    desc.usage = HFEngine::RHI::ResourceUsage_RenderTarget;
    desc.initialState = HFEngine::RHI::ResourceState::RenderTarget;

    const HFEngine::RHI::ValidationResult result = HFEngine::RHI::ValidateTextureDesc(desc);

    HFENGINE_REQUIRE(result.valid);
}

HFENGINE_TEST_CASE("unit.rhi.resources", "RejectsDepthUsageWithoutDepthFormat")
{
    HFEngine::RHI::TextureDesc desc;
    desc.format = HFEngine::RHI::ResourceFormat::Rgba8Unorm;
    desc.width = 16;
    desc.height = 16;
    desc.usage = HFEngine::RHI::ResourceUsage_DepthStencil;

    const HFEngine::RHI::ValidationResult result = HFEngine::RHI::ValidateTextureDesc(desc);

    HFENGINE_REQUIRE(!result.valid);
}
