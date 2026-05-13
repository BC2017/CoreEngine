#include "Engine/Renderer/SandboxMesh.hpp"

#include <array>
#include <cstddef>

namespace HFEngine::Renderer
{
    namespace
    {
        constexpr std::array<SandboxMeshVertex, 24> CubeVertices{
            SandboxMeshVertex{ { -0.5f, -0.5f, -0.5f }, { 0.92f, 0.18f, 0.16f, 1.0f } },
            SandboxMeshVertex{ { 0.5f, -0.5f, -0.5f }, { 0.92f, 0.18f, 0.16f, 1.0f } },
            SandboxMeshVertex{ { 0.5f, 0.5f, -0.5f }, { 0.92f, 0.18f, 0.16f, 1.0f } },
            SandboxMeshVertex{ { -0.5f, 0.5f, -0.5f }, { 0.92f, 0.18f, 0.16f, 1.0f } },
            SandboxMeshVertex{ { 0.5f, -0.5f, -0.5f }, { 0.10f, 0.58f, 0.92f, 1.0f } },
            SandboxMeshVertex{ { 0.5f, -0.5f, 0.5f }, { 0.10f, 0.58f, 0.92f, 1.0f } },
            SandboxMeshVertex{ { 0.5f, 0.5f, 0.5f }, { 0.10f, 0.58f, 0.92f, 1.0f } },
            SandboxMeshVertex{ { 0.5f, 0.5f, -0.5f }, { 0.10f, 0.58f, 0.92f, 1.0f } },
            SandboxMeshVertex{ { -0.5f, 0.5f, -0.5f }, { 0.18f, 0.72f, 0.28f, 1.0f } },
            SandboxMeshVertex{ { 0.5f, 0.5f, -0.5f }, { 0.18f, 0.72f, 0.28f, 1.0f } },
            SandboxMeshVertex{ { 0.5f, 0.5f, 0.5f }, { 0.18f, 0.72f, 0.28f, 1.0f } },
            SandboxMeshVertex{ { -0.5f, 0.5f, 0.5f }, { 0.18f, 0.72f, 0.28f, 1.0f } },
            SandboxMeshVertex{ { -0.5f, -0.5f, 0.5f }, { 0.82f, 0.66f, 0.18f, 1.0f } },
            SandboxMeshVertex{ { -0.5f, -0.5f, -0.5f }, { 0.82f, 0.66f, 0.18f, 1.0f } },
            SandboxMeshVertex{ { -0.5f, 0.5f, -0.5f }, { 0.82f, 0.66f, 0.18f, 1.0f } },
            SandboxMeshVertex{ { -0.5f, 0.5f, 0.5f }, { 0.82f, 0.66f, 0.18f, 1.0f } },
            SandboxMeshVertex{ { -0.5f, -0.5f, 0.5f }, { 0.78f, 0.30f, 0.92f, 1.0f } },
            SandboxMeshVertex{ { 0.5f, -0.5f, 0.5f }, { 0.78f, 0.30f, 0.92f, 1.0f } },
            SandboxMeshVertex{ { 0.5f, -0.5f, -0.5f }, { 0.78f, 0.30f, 0.92f, 1.0f } },
            SandboxMeshVertex{ { -0.5f, -0.5f, -0.5f }, { 0.78f, 0.30f, 0.92f, 1.0f } },
            SandboxMeshVertex{ { 0.5f, -0.5f, 0.5f }, { 0.14f, 0.82f, 0.78f, 1.0f } },
            SandboxMeshVertex{ { -0.5f, -0.5f, 0.5f }, { 0.14f, 0.82f, 0.78f, 1.0f } },
            SandboxMeshVertex{ { -0.5f, 0.5f, 0.5f }, { 0.14f, 0.82f, 0.78f, 1.0f } },
            SandboxMeshVertex{ { 0.5f, 0.5f, 0.5f }, { 0.14f, 0.82f, 0.78f, 1.0f } },
        };

        constexpr std::array<std::uint16_t, 36> CubeIndices{
            0, 1, 2, 0, 2, 3,
            4, 5, 6, 4, 6, 7,
            8, 9, 10, 8, 10, 11,
            12, 13, 14, 12, 14, 15,
            16, 17, 18, 16, 18, 19,
            20, 21, 22, 20, 22, 23,
        };
    }

    std::span<const SandboxMeshVertex> SandboxCubeVertices() noexcept
    {
        return CubeVertices;
    }

    std::span<const std::uint16_t> SandboxCubeIndices() noexcept
    {
        return CubeIndices;
    }

    RHI::GraphicsPipelineDesc BuildSandboxMeshPipelineDesc()
    {
        RHI::GraphicsPipelineDesc desc;
        desc.debugName = "sandbox mesh pipeline";
        desc.vertexShader.debugName = "sandbox mesh vertex shader";
        desc.vertexShader.stage = RHI::ShaderStage::Vertex;
        desc.vertexShader.entryPoint = "VSMain";
        desc.pixelShader.debugName = "sandbox mesh pixel shader";
        desc.pixelShader.stage = RHI::ShaderStage::Pixel;
        desc.pixelShader.entryPoint = "PSMain";
        desc.topology = RHI::PrimitiveTopology::TriangleList;
        desc.colorFormat = RHI::ResourceFormat::Bgra8Unorm;
        desc.rasterState.cullMode = RHI::CullMode::None;
        desc.rasterState.frontFace = RHI::FrontFace::Clockwise;
        desc.depthStencilState.depthTestEnabled = true;
        desc.depthStencilState.depthWriteEnabled = true;
        desc.depthStencilState.depthCompare = RHI::CompareOp::LessEqual;
        desc.depthStencilState.depthFormat = RHI::ResourceFormat::D32Float;

        RHI::VertexBufferLayout layout;
        layout.binding = 0;
        layout.strideBytes = sizeof(SandboxMeshVertex);
        layout.attributes.push_back({
            "POSITION",
            0,
            0,
            RHI::VertexFormat::Float32x3,
            static_cast<std::uint32_t>(offsetof(SandboxMeshVertex, position)),
        });
        layout.attributes.push_back({
            "COLOR",
            0,
            1,
            RHI::VertexFormat::Float32x4,
            static_cast<std::uint32_t>(offsetof(SandboxMeshVertex, color)),
        });
        desc.vertexLayouts.push_back(layout);
        return desc;
    }

    RHI::DrawIndexedDesc BuildSandboxMeshDrawDesc(RHI::BufferHandle vertexBuffer, RHI::BufferHandle indexBuffer)
    {
        RHI::DrawIndexedDesc desc;
        desc.debugName = "sandbox cube draw";
        desc.vertexBuffer = vertexBuffer;
        desc.indexBuffer = indexBuffer;
        desc.vertexStrideBytes = sizeof(SandboxMeshVertex);
        desc.vertexCount = static_cast<std::uint32_t>(CubeVertices.size());
        desc.indexFormat = RHI::IndexFormat::Uint16;
        desc.indexCount = static_cast<std::uint32_t>(CubeIndices.size());
        return desc;
    }
}
