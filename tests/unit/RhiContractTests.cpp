#include "Engine/RHI/Pipeline.hpp"
#include "Engine/RHI/Resource.hpp"
#include "Engine/Renderer/SandboxMesh.hpp"
#include "TestHarness.hpp"

#include <cstddef>

namespace
{
    struct TestVertex
    {
        float position[3];
        float color[4];
    };

    HFEngine::RHI::GraphicsPipelineDesc MakeValidPipelineDesc()
    {
        HFEngine::RHI::GraphicsPipelineDesc desc;
        desc.debugName = "mesh pipeline";
        desc.vertexShader.debugName = "mesh vertex shader";
        desc.vertexShader.stage = HFEngine::RHI::ShaderStage::Vertex;
        desc.vertexShader.entryPoint = "VSMain";
        desc.pixelShader.debugName = "mesh pixel shader";
        desc.pixelShader.stage = HFEngine::RHI::ShaderStage::Pixel;
        desc.pixelShader.entryPoint = "PSMain";
        desc.colorFormat = HFEngine::RHI::ResourceFormat::Bgra8Unorm;
        desc.depthStencilState.depthTestEnabled = true;
        desc.depthStencilState.depthWriteEnabled = true;
        desc.depthStencilState.depthFormat = HFEngine::RHI::ResourceFormat::D32Float;

        HFEngine::RHI::VertexBufferLayout layout;
        layout.strideBytes = sizeof(TestVertex);
        layout.attributes.push_back({
            "POSITION",
            0,
            0,
            HFEngine::RHI::VertexFormat::Float32x3,
            static_cast<std::uint32_t>(offsetof(TestVertex, position)),
        });
        layout.attributes.push_back({
            "COLOR",
            0,
            1,
            HFEngine::RHI::VertexFormat::Float32x4,
            static_cast<std::uint32_t>(offsetof(TestVertex, color)),
        });
        desc.vertexLayouts.push_back(layout);
        return desc;
    }
}

HFENGINE_TEST_CASE("unit.rhi.resources", "ValidatesBufferDescriptor")
{
    HFEngine::RHI::BufferDesc desc;
    desc.debugName = "mesh vertices";
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

HFENGINE_TEST_CASE("unit.rhi.resources", "ValidatesIndexedDrawDescriptor")
{
    HFEngine::RHI::DrawIndexedDesc desc;
    desc.debugName = "cube draw";
    desc.vertexBuffer = { 1, 1 };
    desc.indexBuffer = { 2, 1 };
    desc.vertexStrideBytes = sizeof(TestVertex);
    desc.vertexCount = 24;
    desc.indexFormat = HFEngine::RHI::IndexFormat::Uint16;
    desc.indexCount = 36;

    const HFEngine::RHI::ValidationResult result = HFEngine::RHI::ValidateDrawIndexedDesc(desc);

    HFENGINE_REQUIRE(result.valid);
}

HFENGINE_TEST_CASE("unit.rhi.resources", "RejectsDrawWithoutValidBuffers")
{
    HFEngine::RHI::DrawIndexedDesc desc;
    desc.debugName = "invalid draw";
    desc.vertexStrideBytes = sizeof(TestVertex);
    desc.vertexCount = 24;
    desc.indexFormat = HFEngine::RHI::IndexFormat::Uint16;
    desc.indexCount = 36;

    const HFEngine::RHI::ValidationResult result = HFEngine::RHI::ValidateDrawIndexedDesc(desc);

    HFENGINE_REQUIRE(!result.valid);
}

HFENGINE_TEST_CASE("unit.rhi.pipeline", "ValidatesMeshPipelineDescriptor")
{
    const HFEngine::RHI::GraphicsPipelineDesc desc = MakeValidPipelineDesc();

    const HFEngine::RHI::ValidationResult result = HFEngine::RHI::ValidateGraphicsPipelineDesc(desc);

    HFENGINE_REQUIRE(result.valid);
}

HFENGINE_TEST_CASE("unit.rhi.pipeline", "RejectsMissingShaderEntryPoint")
{
    HFEngine::RHI::GraphicsPipelineDesc desc = MakeValidPipelineDesc();
    desc.vertexShader.entryPoint.clear();

    const HFEngine::RHI::ValidationResult result = HFEngine::RHI::ValidateGraphicsPipelineDesc(desc);

    HFENGINE_REQUIRE(!result.valid);
}

HFENGINE_TEST_CASE("unit.renderer.sandboxmesh", "BuildsValidSharedMeshContracts")
{
    const HFEngine::RHI::GraphicsPipelineDesc pipeline = HFEngine::Renderer::BuildSandboxMeshPipelineDesc();
    const HFEngine::RHI::DrawIndexedDesc draw =
        HFEngine::Renderer::BuildSandboxMeshDrawDesc({ 1, 1 }, { 2, 1 });

    HFENGINE_REQUIRE(HFEngine::RHI::ValidateGraphicsPipelineDesc(pipeline).valid);
    HFENGINE_REQUIRE(HFEngine::RHI::ValidateDrawIndexedDesc(draw).valid);
    HFENGINE_REQUIRE(HFEngine::Renderer::SandboxCubeVertices().size() == draw.vertexCount);
    HFENGINE_REQUIRE(HFEngine::Renderer::SandboxCubeIndices().size() == draw.indexCount);
}

HFENGINE_TEST_CASE("unit.rhi.pipeline", "RejectsAttributePastVertexStride")
{
    HFEngine::RHI::GraphicsPipelineDesc desc = MakeValidPipelineDesc();
    desc.vertexLayouts[0].strideBytes = sizeof(float) * 3u;

    const HFEngine::RHI::ValidationResult result = HFEngine::RHI::ValidateGraphicsPipelineDesc(desc);

    HFENGINE_REQUIRE(!result.valid);
}

HFENGINE_TEST_CASE("unit.rhi.pipeline", "RejectsDepthTestWithoutDepthFormat")
{
    HFEngine::RHI::GraphicsPipelineDesc desc = MakeValidPipelineDesc();
    desc.depthStencilState.depthFormat = HFEngine::RHI::ResourceFormat::Unknown;

    const HFEngine::RHI::ValidationResult result = HFEngine::RHI::ValidateGraphicsPipelineDesc(desc);

    HFENGINE_REQUIRE(!result.valid);
}
