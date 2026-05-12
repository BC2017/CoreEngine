#include "Engine/RHI/Pipeline.hpp"

namespace HFEngine::RHI
{
    ValidationResult ValidateShaderModuleDesc(const ShaderModuleDesc& desc)
    {
        if (desc.debugName.empty())
        {
            return { false, "Shader module debug name must not be empty" };
        }

        if (desc.entryPoint.empty())
        {
            return { false, "Shader module entry point must not be empty" };
        }

        return { true, "ok" };
    }

    ValidationResult ValidateGraphicsPipelineDesc(const GraphicsPipelineDesc& desc)
    {
        if (desc.debugName.empty())
        {
            return { false, "Graphics pipeline debug name must not be empty" };
        }

        ValidationResult shader = ValidateShaderModuleDesc(desc.vertexShader);
        if (!shader.valid)
        {
            return shader;
        }

        shader = ValidateShaderModuleDesc(desc.pixelShader);
        if (!shader.valid)
        {
            return shader;
        }

        if (desc.colorFormat == ResourceFormat::Unknown || IsDepthFormat(desc.colorFormat))
        {
            return { false, "Graphics pipeline color format must be a known color format" };
        }

        if ((desc.depthStencilState.depthTestEnabled || desc.depthStencilState.depthWriteEnabled) &&
            !IsDepthFormat(desc.depthStencilState.depthFormat))
        {
            return { false, "Depth testing or writing requires a depth format" };
        }

        for (const VertexBufferLayout& layout : desc.vertexLayouts)
        {
            if (layout.strideBytes == 0)
            {
                return { false, "Vertex buffer layout stride must be greater than zero" };
            }

            if (layout.attributes.empty())
            {
                return { false, "Vertex buffer layout must include at least one attribute" };
            }

            for (const VertexInputAttribute& attribute : layout.attributes)
            {
                if (attribute.semanticName.empty())
                {
                    return { false, "Vertex input attribute semantic name must not be empty" };
                }

                const std::uint32_t formatSize = VertexFormatSizeBytes(attribute.format);
                if (formatSize == 0)
                {
                    return { false, "Vertex input attribute format must be known" };
                }

                if (attribute.offsetBytes + formatSize > layout.strideBytes)
                {
                    return { false, "Vertex input attribute extends beyond layout stride" };
                }
            }
        }

        return { true, "ok" };
    }

    std::uint32_t VertexFormatSizeBytes(VertexFormat format) noexcept
    {
        switch (format)
        {
        case VertexFormat::Float32x2:
            return sizeof(float) * 2u;
        case VertexFormat::Float32x3:
            return sizeof(float) * 3u;
        case VertexFormat::Float32x4:
            return sizeof(float) * 4u;
        case VertexFormat::Uint16:
            return sizeof(std::uint16_t);
        case VertexFormat::Uint32:
            return sizeof(std::uint32_t);
        case VertexFormat::Unknown:
            return 0;
        }

        return 0;
    }
}
