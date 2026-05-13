#pragma once

#include "Engine/Math/LinearAlgebra.hpp"

#include <cstdint>

namespace HFEngine::Renderer
{
    struct CameraDesc
    {
        Math::Vec3 position{1.8f, 1.2f, -3.0f};
        Math::Vec3 target{0.0f, 0.0f, 0.0f};
        Math::Vec3 up{0.0f, 1.0f, 0.0f};
        float verticalFovRadians = Math::Pi / 3.0f;
        float nearPlane = 0.1f;
        float farPlane = 50.0f;
    };

    struct CameraMatrices
    {
        Math::Mat4 view;
        Math::Mat4 projection;
        Math::Mat4 viewProjection;
    };

    [[nodiscard]] CameraMatrices BuildCameraMatrices(const CameraDesc& desc, float aspectRatio);
    [[nodiscard]] Math::Mat4 BuildSandboxViewProjection(std::uint32_t width, std::uint32_t height);
}
