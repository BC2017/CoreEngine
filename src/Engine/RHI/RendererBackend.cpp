#include "Engine/RHI/RendererBackend.hpp"

#include <array>

namespace HFEngine::RHI
{
    namespace
    {
        [[nodiscard]] char ToLowerAscii(char value) noexcept
        {
            if (value >= 'A' && value <= 'Z')
            {
                return static_cast<char>(value - 'A' + 'a');
            }

            return value;
        }

        [[nodiscard]] bool EqualsIgnoreCase(std::string_view left, std::string_view right) noexcept
        {
            if (left.size() != right.size())
            {
                return false;
            }

            for (std::size_t index = 0; index < left.size(); ++index)
            {
                if (ToLowerAscii(left[index]) != ToLowerAscii(right[index]))
                {
                    return false;
                }
            }

            return true;
        }
    }

    RendererBackendParseResult ParseRendererBackend(std::string_view text) noexcept
    {
        if (EqualsIgnoreCase(text, "dx12") ||
            EqualsIgnoreCase(text, "d3d12") ||
            EqualsIgnoreCase(text, "directx12") ||
            EqualsIgnoreCase(text, "directx"))
        {
            return { true, RendererBackend::DirectX12, "ok" };
        }

        if (EqualsIgnoreCase(text, "vulkan") || EqualsIgnoreCase(text, "vk"))
        {
            return { true, RendererBackend::Vulkan, "ok" };
        }

        return { false, RendererBackend::DirectX12, "Unknown renderer backend. Expected dx12 or vulkan." };
    }

    const char* ToString(RendererBackend backend) noexcept
    {
        switch (backend)
        {
        case RendererBackend::DirectX12:
            return "DirectX 12";
        case RendererBackend::Vulkan:
            return "Vulkan";
        }

        return "Unknown";
    }

    const char* ToCommandLineValue(RendererBackend backend) noexcept
    {
        switch (backend)
        {
        case RendererBackend::DirectX12:
            return "dx12";
        case RendererBackend::Vulkan:
            return "vulkan";
        }

        return "unknown";
    }
}
