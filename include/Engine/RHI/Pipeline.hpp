#pragma once

#include "Engine/RHI/Resource.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace HFEngine::RHI
{
    enum class ShaderStage : std::uint32_t
    {
        Vertex = 1u << 0u,
        Pixel = 1u << 1u,
        Compute = 1u << 2u,
        RayGeneration = 1u << 3u,
        Miss = 1u << 4u,
        ClosestHit = 1u << 5u
    };

    enum class VertexFormat
    {
        Unknown,
        Float32x2,
        Float32x3,
        Float32x4,
        Uint16,
        Uint32
    };

    enum class PrimitiveTopology
    {
        TriangleList,
        LineList
    };

    enum class CullMode
    {
        None,
        Front,
        Back
    };

    enum class FrontFace
    {
        Clockwise,
        CounterClockwise
    };

    enum class CompareOp
    {
        Always,
        Less,
        LessEqual
    };

    struct ShaderModuleDesc
    {
        std::string debugName;
        ShaderStage stage = ShaderStage::Vertex;
        std::string entryPoint = "main";
    };

    struct VertexInputAttribute
    {
        std::string semanticName;
        std::uint32_t semanticIndex = 0;
        std::uint32_t location = 0;
        VertexFormat format = VertexFormat::Unknown;
        std::uint32_t offsetBytes = 0;
    };

    struct VertexBufferLayout
    {
        std::uint32_t binding = 0;
        std::uint32_t strideBytes = 0;
        std::vector<VertexInputAttribute> attributes;
    };

    struct RasterStateDesc
    {
        CullMode cullMode = CullMode::Back;
        FrontFace frontFace = FrontFace::Clockwise;
        bool depthClipEnabled = true;
    };

    struct DepthStencilStateDesc
    {
        bool depthTestEnabled = false;
        bool depthWriteEnabled = false;
        CompareOp depthCompare = CompareOp::LessEqual;
        ResourceFormat depthFormat = ResourceFormat::Unknown;
    };

    struct GraphicsPipelineDesc
    {
        std::string debugName;
        ShaderModuleDesc vertexShader;
        ShaderModuleDesc pixelShader;
        PrimitiveTopology topology = PrimitiveTopology::TriangleList;
        ResourceFormat colorFormat = ResourceFormat::Bgra8Unorm;
        RasterStateDesc rasterState;
        DepthStencilStateDesc depthStencilState;
        std::vector<VertexBufferLayout> vertexLayouts;
    };

    [[nodiscard]] ValidationResult ValidateShaderModuleDesc(const ShaderModuleDesc& desc);
    [[nodiscard]] ValidationResult ValidateGraphicsPipelineDesc(const GraphicsPipelineDesc& desc);
    [[nodiscard]] std::uint32_t VertexFormatSizeBytes(VertexFormat format) noexcept;
}
