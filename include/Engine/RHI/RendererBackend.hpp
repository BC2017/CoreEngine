#pragma once

#include <string_view>

namespace HFEngine::RHI
{
    enum class RendererBackend
    {
        DirectX12,
        Vulkan
    };

    struct RendererBackendParseResult
    {
        bool success = false;
        RendererBackend backend = RendererBackend::DirectX12;
        const char* message = "";
    };

    [[nodiscard]] RendererBackendParseResult ParseRendererBackend(std::string_view text) noexcept;
    [[nodiscard]] const char* ToString(RendererBackend backend) noexcept;
    [[nodiscard]] const char* ToCommandLineValue(RendererBackend backend) noexcept;
}
