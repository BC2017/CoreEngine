#pragma once

#include "Engine/RHI/Pipeline.hpp"

#include <cstdint>
#include <span>

namespace HFEngine::Renderer
{
    struct SandboxMeshVertex
    {
        float position[3];
        float color[4];
    };

    [[nodiscard]] std::span<const SandboxMeshVertex> SandboxCubeVertices() noexcept;
    [[nodiscard]] std::span<const std::uint16_t> SandboxCubeIndices() noexcept;
    [[nodiscard]] RHI::GraphicsPipelineDesc BuildSandboxMeshPipelineDesc();
    [[nodiscard]] RHI::DrawIndexedDesc BuildSandboxMeshDrawDesc(
        RHI::BufferHandle vertexBuffer,
        RHI::BufferHandle indexBuffer);
}
