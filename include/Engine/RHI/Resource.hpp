#pragma once

#include "Engine/Core/Handle.hpp"

#include <cstdint>
#include <string>

namespace HFEngine::RHI
{
    struct BufferTag;
    struct TextureTag;
    struct GraphicsPipelineTag;

    using BufferHandle = Core::Handle<BufferTag>;
    using TextureHandle = Core::Handle<TextureTag>;
    using GraphicsPipelineHandle = Core::Handle<GraphicsPipelineTag>;

    enum class ResourceFormat
    {
        Unknown,
        Rgba8Unorm,
        Bgra8Unorm,
        Rgba16Float,
        Rgba32Float,
        D24UnormS8,
        D32Float
    };

    enum class TextureDimension
    {
        Texture2D,
        Texture3D,
        TextureCube
    };

    enum class ResourceState
    {
        Undefined,
        CopySource,
        CopyDestination,
        VertexBuffer,
        IndexBuffer,
        ConstantBuffer,
        RenderTarget,
        DepthWrite,
        ShaderRead,
        UnorderedAccess,
        Present
    };

    enum class IndexFormat
    {
        Unknown,
        Uint16,
        Uint32
    };

    enum ResourceUsage : std::uint32_t
    {
        ResourceUsage_None = 0,
        ResourceUsage_CopySource = 1u << 0u,
        ResourceUsage_CopyDestination = 1u << 1u,
        ResourceUsage_VertexBuffer = 1u << 2u,
        ResourceUsage_IndexBuffer = 1u << 3u,
        ResourceUsage_ConstantBuffer = 1u << 4u,
        ResourceUsage_RenderTarget = 1u << 5u,
        ResourceUsage_DepthStencil = 1u << 6u,
        ResourceUsage_ShaderResource = 1u << 7u,
        ResourceUsage_UnorderedAccess = 1u << 8u
    };

    [[nodiscard]] constexpr ResourceUsage operator|(ResourceUsage left, ResourceUsage right) noexcept
    {
        return static_cast<ResourceUsage>(static_cast<std::uint32_t>(left) | static_cast<std::uint32_t>(right));
    }

    [[nodiscard]] constexpr bool HasUsage(ResourceUsage value, ResourceUsage flag) noexcept
    {
        return (static_cast<std::uint32_t>(value) & static_cast<std::uint32_t>(flag)) != 0u;
    }

    struct ValidationResult
    {
        bool valid = false;
        std::string message;
    };

    struct BufferDesc
    {
        std::string debugName;
        std::uint64_t sizeBytes = 0;
        ResourceUsage usage = ResourceUsage_None;
        ResourceState initialState = ResourceState::Undefined;
    };

    struct TextureDesc
    {
        std::string debugName;
        TextureDimension dimension = TextureDimension::Texture2D;
        ResourceFormat format = ResourceFormat::Unknown;
        std::uint32_t width = 0;
        std::uint32_t height = 0;
        std::uint32_t depthOrArraySize = 1;
        std::uint32_t mipLevels = 1;
        ResourceUsage usage = ResourceUsage_None;
        ResourceState initialState = ResourceState::Undefined;
    };

    struct ResourceBarrier
    {
        std::string debugName;
        ResourceState before = ResourceState::Undefined;
        ResourceState after = ResourceState::Undefined;
    };

    struct DrawIndexedDesc
    {
        std::string debugName;
        BufferHandle vertexBuffer;
        BufferHandle indexBuffer;
        std::uint32_t vertexStrideBytes = 0;
        std::uint32_t vertexCount = 0;
        IndexFormat indexFormat = IndexFormat::Unknown;
        std::uint32_t indexCount = 0;
    };

    [[nodiscard]] ValidationResult ValidateBufferDesc(const BufferDesc& desc);
    [[nodiscard]] ValidationResult ValidateTextureDesc(const TextureDesc& desc);
    [[nodiscard]] ValidationResult ValidateDrawIndexedDesc(const DrawIndexedDesc& desc);
    [[nodiscard]] std::uint32_t IndexFormatSizeBytes(IndexFormat format) noexcept;
    [[nodiscard]] bool IsDepthFormat(ResourceFormat format) noexcept;
}
