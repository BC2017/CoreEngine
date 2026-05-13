#pragma once

#include "Engine/RHI/CommandList.hpp"
#include "Engine/RHI/Pipeline.hpp"

#include <cstdint>
#include <span>
#include <string>

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
    [[nodiscard]] RHI::RenderPassDesc BuildSandboxRenderPassDesc(
        std::uint32_t width,
        std::uint32_t height,
        RHI::TextureHandle colorTarget,
        RHI::TextureHandle depthTarget);
    [[nodiscard]] bool ValidateSandboxCommandSequence(
        const RHI::DrawIndexedDesc& draw,
        std::uint32_t width,
        std::uint32_t height,
        std::string& message);
}
