#include "Engine/RHI/Resource.hpp"

namespace HFEngine::RHI
{
    ValidationResult ValidateBufferDesc(const BufferDesc& desc)
    {
        if (desc.sizeBytes == 0)
        {
            return { false, "Buffer size must be greater than zero" };
        }

        if (desc.usage == ResourceUsage_None)
        {
            return { false, "Buffer usage must include at least one usage flag" };
        }

        if (HasUsage(desc.usage, ResourceUsage_RenderTarget) || HasUsage(desc.usage, ResourceUsage_DepthStencil))
        {
            return { false, "Buffer usage cannot include texture-only attachment flags" };
        }

        return { true, "ok" };
    }

    ValidationResult ValidateTextureDesc(const TextureDesc& desc)
    {
        if (desc.width == 0 || desc.height == 0 || desc.depthOrArraySize == 0)
        {
            return { false, "Texture dimensions must be greater than zero" };
        }

        if (desc.mipLevels == 0)
        {
            return { false, "Texture mipLevels must be greater than zero" };
        }

        if (desc.format == ResourceFormat::Unknown)
        {
            return { false, "Texture format must be known" };
        }

        if (desc.usage == ResourceUsage_None)
        {
            return { false, "Texture usage must include at least one usage flag" };
        }

        if (HasUsage(desc.usage, ResourceUsage_DepthStencil) && !IsDepthFormat(desc.format))
        {
            return { false, "Depth-stencil usage requires a depth format" };
        }

        return { true, "ok" };
    }

    bool IsDepthFormat(ResourceFormat format) noexcept
    {
        return format == ResourceFormat::D24UnormS8 || format == ResourceFormat::D32Float;
    }
}
